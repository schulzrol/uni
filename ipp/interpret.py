from ipppy.InstructionManager import InstructionManager, Instruction
from ipppy.LabelManager import LabelManager
from ipppy.FrameManager import FrameManager
from ipppy.Context import Context
from ipppy.Errors import Error, GenericError, BadXMLError

import xml.etree.ElementTree as ET
import sys
import argparse
from typing import Any, Dict, Sequence

# [x] read input
# [ ] context includes label->ip map, ip, instructions, (basically State)
# [ ] implement templates for instructions -> supported instructions map (key should also be typed -> lambdas for reuse)
# [ ] first pass (gather labels)
# [ ] second pass (interpret)

def handle_argv(argv: Sequence[str]):
    parser = argparse.ArgumentParser(description="Skript načte XML reprezentaci programu a tento program s využitím vstupu dle parametrů příkazové řádky interpretuje a generuje výstup. Musí být definován alespoň jeden argument [source] nebo [input].")
    parser.add_argument("--source", nargs=argparse.OPTIONAL, help="Soubor obsahující XML reprezentaci programu. Standardní vstup pokud nepoužito.")
    parser.add_argument("--input", nargs=argparse.OPTIONAL, help="Soubor obsahující vstup programu. Standardní vstup pokud nepoužito.")

    args = parser.parse_args(argv)

    # atleast one filename must be specified
    if not (args.source or args.input):
        # TODO handle noargs
        print("err obe stdin")
        exit(1)

    try:
        source_handler = open(args.source, 'r') if args.source else sys.stdin
        input_handler = open(args.input, 'r') if args.input else sys.stdin
    except (FileNotFoundError, IOError) as e:
        # TODO handle files
        print(e.strerror)
        exit(1)
    
    return source_handler, input_handler
    

def main(argv) -> int:
    source_handler, input_handler = handle_argv(argv[1:])
    try:
        source_xml = ET.parse(source_handler).getroot()
    except:
        return BadXMLError("").exit_code

    if source_handler != sys.stdin:
        source_handler.close()

    try:
        im = InstructionManager.from_et(source_xml)
        lm = LabelManager.from_instructions(im.instructions)
    except Error as e:
        print(e.message)
        return e.exit_code
    except:
        return BadXMLError("").exit_code

    fm = FrameManager([("GF", True), ("LF", True), ("TF", False)])
    ctx = Context(lm, fm, im, input_handler)
    
    # TODO handle exit
    while ctx.advance():
        current_instruction = ctx.get_current_instruction()
        try:
            #print(current_instruction)
            current_instruction.run(ctx)
        except (Error) as e:
            print(e.message)
            return e.exit_code
        except Exception as e:
            return GenericError().exit_code

    if input_handler != sys.stdin:
        input_handler.close()

    return ctx.exit_code

if __name__ == "__main__":
    exit(main(sys.argv))