from dataclasses import dataclass, field
from typing import Callable, Dict, Optional, Sequence, Union
from xml.etree.ElementTree import ElementTree, Element
import sys

class Argument():
    def __init__(self, order: int, type: str, value: str) -> None:
        self.order = order
        self.type = type
        self.value = value

    # unescape XML values for use in Python
    def get_typed_value(self):
        if self.type == 'string':
            return self.value.replace('&lt;', '<').replace('&gt;', '>').replace('&amp;', '&')

        if self.type == 'int':
            return int(self.value)
        
        if self.type == 'label':
            return self.value
        
        if self.type == 'bool':
            return self.value.lower() == 'true'

        if self.type == 'nil':
            return None

        # name of the var
        if self.type == 'var':
            return self.value

        raise Exception(f"Unknown type {self.type}")

    @classmethod
    def from_et(cls, e: Element):
        return Argument(int(e.tag[-1]), e.attrib['type'], e.text)
    
    @staticmethod
    def type_from_value(value: Optional[str]) -> str:
        if value is None:
            return "nil"

        try:
            if int(value, 0):
                return 'int'
        except ValueError:
            pass
        
        if value.lower() in {'true', 'false'}:
            return 'bool'
        
        if len(value.strip()) == 0:
            return 'nil'
        
        return 'string'

    
    def __str__(self) -> str:
        return f"{self.order}: {self.type} {self.value}"

    def __repr__(self) -> str:
        return str(self)
    
    def __lt__(self, other):
        return self.order < other.order


class Instruction():
    def __init__(self, order: int, opcode: str, args: Sequence[Argument] = None) -> None:
        self.order = order
        self.opcode = opcode
        self.args = args or list()

    @classmethod
    def from_et(cls, e: Element):
        args = sorted([Argument.from_et(arg) for arg in [a for a in e.iter() if a != e]])
        return cls(int(e.attrib['order']), e.attrib['opcode'], args)
    
    def execute(self, context):
        raise Exception("Undefined instruction execution")

    def __str__(self) -> str:
        return f"{self.order}: {self.opcode} {self.args}"

    def __repr__(self) -> str:
       return str(self)

    def __lt__(self, other):
        return self.order < other.order

# instruction set
# create subclasses to handle each instruction
# each instruction has a procedure that takes a context and manipulates it
class MOVEInstruction(Instruction):
    def execute(self, context):
        # get values from arguments
        varname = self.args[0].get_typed_value()
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {"value": symb1_value})

class CREATEFRAMEInstruction(Instruction):
    def execute(self, context):
        context.fm.init_frame('TF')
    
class PUSHFRAMEInstruction(Instruction):
    def execute(self, context):
        if context.fm.is_defined('TF'):
            context.fm.move_to('TF', 'LF')
        else:
            raise Exception("No frame to push")

class POPFRAMEInstruction(Instruction):
    def execute(self, context):
        if context.fm.is_defined('LF'):
            context.fm.move_to('LF', 'TF')
        else:
            raise Exception("No frame to pop")
        
class DEFVARInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        context.fm.set(context.fm.frame_name_from_var(varname), varname, None)

class CALLInstruction(Instruction):
    def execute(self, context):
        label = self.args[0].value
        if not context.lm.is_defined(label):
            raise Exception(f"Label {label} not defined")
        # push return address
        context.call_stack.append(context.ip)
        # get label address
        context.ip = context.lm.get_label_address(label)

class RETURNInstruction(Instruction):
    def execute(self, context):
        if len(context.call_stack) > 0:
            context.ip = context.call_stack.pop()
        else:
            raise Exception("No call stack to return from")

class PUSHSInstruction(Instruction):
    def execute(self, context):
        symb1 = self.args[0]
        # get value from frame manager
        value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        context.data_stack.append(value)

class POPSInstruction(Instruction):
    def execute(self, context):
        if len(context.data_stack) > 0:
            data = context.data_stack.pop()
            symb1 = self.args[0]
            # save data to variable in frame manager
            context.fm.set(context.fm.frame_name_from_var(symb1.value), symb1.value, {'value': data})
        else:
            raise Exception("No data stack to pop")

class ADDInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value + symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {"value": result})

class SUBInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value - symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {"value": result})
    
class MULInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value * symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

class IDIVInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value // symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

# TODO comparator instructions to account for variables
class LTInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value < symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

class GTInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value > symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

class EQInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value == symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

class ANDInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value and symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {'value': result})

class ORInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        result = symb1_value or symb2_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class NOTInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        result = not symb1_value
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class INT2CHARInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        try:
            result = chr(symb1_value)
        except: 
            #TODO define exception errors 58
            raise Exception("Invalid integer")
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)
    
class STRI2INTInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        symb2 = self.args[2]
        symb2_value = context.fm.get_var_value_or_default(symb2.value, symb2.get_typed_value())
        try:
            result = ord(symb1_value[symb2_value])
        except:
            #TODO define exception errors 58
            raise Exception("Invalid string")
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class READInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        try:
            result = context.input_handler.readline().strip()
            type = Argument.type_from_value(result)
        except:
            #TODO define exception errors 58
            raise Exception("Invalid input")
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, {"value": result})
    
class WRITEInstruction(Instruction):
    def execute(self, context):
        symb1 = self.args[0]
        symb1_value = context.fm.get_var_value_or_default(symb1.value, symb1.get_typed_value())
        print(symb1_value, end="")
    
# TODO add type checking for arguments using parent class or typed value
class CONCATInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        symb2 = context.fm.get_var_value_or_default(self.args[2].value, self.args[2].get_typed_value())
        result = symb1 + symb2
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class STRLENInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        result = len(symb1)
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class GETCHARInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        symb2 = context.fm.get_var_value_or_default(self.args[2].value, self.args[2].get_typed_value())
        try:
            result = symb1[symb2]
        except:
            #TODO define exception errors 58
            raise Exception("Invalid string")
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)
    
class SETCHARInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        symb2 = context.fm.get_var_value_or_default(self.args[2].value, self.args[2].get_typed_value())
        symb3 = context.fm.get_var_value_or_default(self.args[3].value, self.args[3].get_typed_value())
        try:
            # TODO check if symb3 is a valid character
            symb1[symb2] = symb3
            result = symb1
        except:
            #TODO define exception errors 58
            raise Exception("Invalid string")
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)
    
class TYPEInstruction(Instruction):
    def execute(self, context):
        varname = self.args[0].value
        symb1 = self.args[1].get_typed_value()
        result = Argument.type_from_value(symb1)
        # save result to variable
        context.fm.set(context.fm.frame_name_from_var(varname), varname, result)

class LABELInstruction(Instruction):
    def execute(self, context):
        # define label with address
        context.lm.define_label(self.args[0].value, {'address': context.ip})

class JUMPInstruction(Instruction):
    def execute(self, context):
        # jump to label
        context.ip = context.fm.get_label_address(self.args[0].value)

class JUMPIFEQInstruction(Instruction):
    def execute(self, context):
        # jump to label if symb1 == symb2
        label = self.args[0].get_typed_value()
        symb1_value = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        symb2_value = context.fm.get_var_value_or_default(self.args[2].value, self.args[2].get_typed_value())
        if symb1_value == symb2_value:
            context.ip = context.fm.get_label_address(label)

class JUMPIFNEQInstruction(Instruction):
    def execute(self, context):
        # jump to label if symb1 != symb2
        label = self.args[0].get_typed_value()
        symb1_value = context.fm.get_var_value_or_default(self.args[1].value, self.args[1].get_typed_value())
        symb2_value = context.fm.get_var_value_or_default(self.args[2].value, self.args[2].get_typed_value())
        if symb1_value != symb2_value:
            context.ip = context.lm.get_label_address(label)

class EXITInstruction(Instruction):
    def execute(self, context):
        symb1 = self.args[0].get_typed_value()
        if not isinstance(symb1, int) or symb1 < 0 or symb1 >= 49:
            #TODO define exception errors 57
            raise Exception("Invalid exit code")

        context.ip = -1
        context.exit_code = self.args[0].get_typed_value()

class DPRINTInstruction(Instruction):
    def execute(self, context):
        symb1 = self.args[0].get_typed_value()
        print(symb1, file=sys.stderr)
    
class BREAKInstruction(Instruction):
    def execute(self, context):
        print(context.as_breakpoint(), file=sys.stderr)
    

class InstructionFactory():
    instruction_set = {
        'MOVE': MOVEInstruction,
        'CREATEFRAME': CREATEFRAMEInstruction,
        'PUSHFRAME': PUSHFRAMEInstruction,
        'POPFRAME': POPFRAMEInstruction,
        'DEFVAR': DEFVARInstruction,
        'CALL': CALLInstruction,
        'RETURN': RETURNInstruction,
        'PUSHS': PUSHSInstruction,
        'POPS': POPSInstruction,
        'ADD': ADDInstruction,
        'SUB': SUBInstruction,
        'MUL': MULInstruction,
        'IDIV': IDIVInstruction,
        'LT': LTInstruction,
        'GT': GTInstruction,
        'EQ': EQInstruction,
        'AND': ANDInstruction,
        'OR': ORInstruction,
        'NOT': NOTInstruction,
        'INT2CHAR': INT2CHARInstruction,
        'STRI2INT': STRI2INTInstruction,
        'READ': READInstruction,
        'WRITE': WRITEInstruction,
        'CONCAT': CONCATInstruction,
        'STRLEN': STRLENInstruction,
        'GETCHAR': GETCHARInstruction,
        'SETCHAR': SETCHARInstruction,
        'TYPE': TYPEInstruction,
        'LABEL': LABELInstruction,
        'JUMP': JUMPInstruction,
        'JUMPIFEQ': JUMPIFEQInstruction,
        'JUMPIFNEQ': JUMPIFNEQInstruction,
        'EXIT': EXITInstruction,
        'DPRINT': DPRINTInstruction,
        'BREAK': BREAKInstruction
    }

    @classmethod
    def class_from_opcode(cls, opcode) -> Optional[Instruction]:
        return cls.instruction_set[opcode] if opcode in cls.instruction_set else None


class InstructionManager():
    opcode_procedures: Dict[str, Instruction] = dict()

    def __init__(self, instructions: Sequence[Instruction]) -> None:
        self.instructions = instructions

    def get_instruction(self, order: int) -> Instruction:
        return self.instructions[order]

    def advance_ip(self) -> None:
        if not self.ip_jump:
            self.ip += 1
    
    def get_lambda(self):
        pass

    def get_size(self):
        return len(self.instructions)

    @classmethod
    def from_et(cls, program: ElementTree):
        instructions = sorted([InstructionFactory.class_from_opcode(e.attrib['opcode']).from_et(e) for e in program.iter('instruction')])
        return cls(instructions)