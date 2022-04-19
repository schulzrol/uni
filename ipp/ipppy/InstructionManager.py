from dataclasses import dataclass, field
from typing import Callable, Dict, Optional, Sequence, Union
from xml.etree.ElementTree import ElementTree, Element
import sys

import ipppy.Errors as ippE

class Argument():
    def __init__(self, order: int, type: str, value: str) -> None:
        if order < 1:
            raise ippE.UnsupportedXMLError(f"Negative order {order}")

        self.order = order
        self.type = type.lower()
        self.value = value

    def get_typed_value(self):
        if self.type == 'string':
            return self.value

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

        raise ippE.UnknownTypeError(self.type)

    @classmethod
    def from_et(cls, e: Element):
        return cls(int(e.tag[-1]), e.attrib['type'], e.text)
    
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
    
    def execute(self, ctx):
        raise ippE.NotYetImplementedError(f"execute() method for {self.opcode}") 

    def before_execute(self, ctx):
        raise ippE.NotYetImplementedError(f"before_execute() method for {self.opcode}") 

    def run(self, ctx):
        self.before_execute(ctx)
        self.execute(ctx)

    def __str__(self) -> str:
        return f"{self.order}: {self.opcode} {self.args}"

    def __repr__(self) -> str:
       return str(self)

    def __lt__(self, other):
        return self.order < other.order

class NullaryInstruction(Instruction):
    def before_execute(self, ctx):
        if not len(self.args) == 0:
            raise ippE.InvalidArgumentCountError(self.opcode, 0, len(self.args))

class UnaryInstruction(Instruction):
    def before_execute(self, ctx):
        if not len(self.args) == 1:
            raise ippE.InvalidArgumentCountError(self.opcode, 1, len(self.args))

class BinaryInstruction(Instruction):
    def before_execute(self, ctx):
        if not len(self.args) == 2:
            raise ippE.InvalidArgumentCountError(self.opcode, 2, len(self.args))

class TernaryInstruction(Instruction):
    def before_execute(self, ctx):
        if not len(self.args) == 3:
            raise ippE.InvalidArgumentCountError(self.opcode, 3, len(self.args))

# instruction set
# create subclasses to handle each instruction
# each instruction has a procedure that takes a context and manipulates it
class MOVEInstruction(BinaryInstruction):
    def execute(self, ctx):
        # get values from arguments
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].get_typed_value()
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        ctx.save_to_variable(varname, symb1_value)

class CREATEFRAMEInstruction(NullaryInstruction):
    def execute(self, ctx):
        ctx.fm.init_frame('TF')
    
class PUSHFRAMEInstruction(NullaryInstruction):
    def execute(self, ctx):
        if ctx.fm.is_defined('TF'):
            ctx.fm.move_to('TF', 'LF')
        else:
            raise ippE.NoFrameToPush()

class POPFRAMEInstruction(NullaryInstruction):
    def execute(self, ctx):
        if ctx.fm.is_defined('LF'):
            ctx.fm.move_to('LF', 'TF')
        else:
            raise ippE.NoFrameToPop()
        
class DEFVARInstruction(UnaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        if ctx.fm.is_var_defined(varname):
            raise ippE.VariableAlreadyDefinedError(varname)
        ctx.fm.define_var(varname)

class CALLInstruction(UnaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'label':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        label = self.args[0].value
        if not ctx.lm.is_defined(label):
            raise ippE.JumpToUndefinedLabelError(label)
        # push return address
        ctx.call_stack.append(ctx.ip)
        # get label address
        ctx.ip = ctx.lm.get_label_address(label)

class RETURNInstruction(NullaryInstruction):
    def execute(self, ctx):
        if len(ctx.call_stack) > 0:
            ctx.ip = ctx.call_stack.pop()
        else:
            raise ippE.NowhereToReturnError()

class PUSHSInstruction(UnaryInstruction):
    def execute(self, ctx):
        varname = self.args[0]
        # get value from frame manager
        value = ctx.get_symbol_value_or_error(varname)
        ctx.data_stack.append(value)

class POPSInstruction(UnaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        if len(ctx.data_stack) > 0:
            data = ctx.data_stack.pop()
            varname = self.args[0].value
            # save data to variable in frame manager
            ctx.save_to_variable(varname, data)
        else:
            raise ippE.NoDataToPopError()

class ADDInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = symb1_value + symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "integers")
        # save result to variable
        ctx.save_to_variable(varname, result)

class SUBInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = symb1_value - symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "integers")
        # save result to variable
        ctx.save_to_variable(varname, result)
    
class MULInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = symb1_value * symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "integers")
        # save result to variable
        ctx.save_to_variable(varname, result)

class IDIVInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if symb2_value == 0:
            raise ippE.DivisionByZeroError()

        try:
            result = symb1_value // symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "integers")
        # save result to variable
        ctx.save_to_variable(varname, result)

# TODO comparator instructions to account for variables
class LTInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if symb1_value is None or symb2_value is None:
            raise ippE.NilComparisonError()

        try:
            result = symb1_value < symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "same types on both sides")
        # save result to variable
        ctx.save_to_variable(varname, result)

class GTInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if symb1_value is None or symb2_value is None:
            raise ippE.NilComparisonError()
        try:
            result = symb1_value > symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "same types on both sides")
        # save result to variable
        ctx.save_to_variable(varname, result)

class EQInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = symb1_value == symb2_value
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "same types on both sides")
        # save result to variable
        ctx.save_to_variable(varname, result)

class ANDInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        result = symb1_value and symb2_value
        # save result to variable
        ctx.save_to_variable(varname, result)

class ORInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        result = symb1_value or symb2_value
        # save result to variable
        ctx.save_to_variable(varname, result)

class NOTInstruction(BinaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        result = not symb1_value
        # save result to variable
        ctx.save_to_variable(varname, result)

class INT2CHARInstruction(BinaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        try:
            result = chr(symb1_value)
        except: 
            #TODO define exception errors 58
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "int")
        # save result to variable
        ctx.save_to_variable(varname, result)
    
class STRI2INTInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2 = self.args[2]
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = ord(symb1_value[symb2_value])
        except:
            #TODO define exception errors 58
            raise ippE.Error(58, "string bad")
            #raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string")

        # save result to variable
        ctx.save_to_variable(varname, result)

class READInstruction(BinaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        try:
            result = ctx.input_handler.readline().strip()
            type = Argument.type_from_value(result)
            arg = Argument(type, result)
        except:
            ctx.save_to_variable(varname, None)
        # save result to variable
        ctx.save_to_variable(varname, result)
    
class WRITEInstruction(UnaryInstruction):
    def execute(self, ctx):
        symb1 = self.args[0]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        if isinstance(symb1_value, bool):
            symb1_value = str(symb1_value).lower()
        print('' if symb1_value is None else symb1_value, end="")
    
# TODO add type checking for arguments using parent class or typed value
class CONCATInstruction(TernaryInstruction):
    def execute(self, ctx):
        if not self.args[0].type == 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        # TODO: use get_symbol_value_or_error instead of this
        symb1 = self.args[1]
        symb2 = self.args[2]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if not (isinstance(symb1_value, str) and isinstance(symb2_value, str)):
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string")
        try:
            result = symb1_value + symb2_value
        except Exception as e:
            raise ippE.Error(58, "string bad")
            #raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "strings")
        # save result to variable
        ctx.save_to_variable(varname, result)

class STRLENInstruction(BinaryInstruction):
    def execute(self, ctx):
        if self.args[0].type != 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        if not isinstance(symb1_value, str):
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string")
        try:
            result = len(symb1_value)
        except:
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string")
        # save result to variable
        ctx.save_to_variable(varname, result)

class GETCHARInstruction(TernaryInstruction):
    def execute(self, ctx):
        if self.args[0].type != 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb2 = self.args[2]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        try:
            result = symb1_value[symb2_value]
        except:
            #TODO define exception errors 58
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string and int in bounds")
        # save result to variable
        ctx.save_to_variable(varname, result)
    
class SETCHARInstruction(TernaryInstruction):
    def execute(self, ctx):
        if self.args[0].type != 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1]
        symb2 = self.args[2]
        symb3 = self.args[3]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        symb3_value = ctx.get_symbol_value_or_error(symb3)
        if not isinstance(symb3_value, str):
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string of one character")

        if len(symb3_value) != 1:
            raise ippE.BadValueError(self.opcode, "not given string of one character")

        try:
            # TODO check if symb3 is a valid character
            symb1_value[symb2_value] = symb3_value
            result = symb1_value
        except:
            #TODO define exception errors 58
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, "string, int in bounds and char")
        # save result to variable
        ctx.save_to_variable(varname, result)
    
class TYPEInstruction(BinaryInstruction):
    def execute(self, ctx):
        if self.args[0].type != 'var':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        varname = self.args[0].value
        symb1 = self.args[1].get_typed_value()
        result = Argument.type_from_value(symb1)
        # save result to variable
        ctx.save_to_variable(varname, result)

class LABELInstruction(UnaryInstruction):
    def execute(self, ctx):
        # define label with address
        if self.args[0].type != 'label':
            raise ippE.InvalidInstructionArgumentTypesError(self.opcode, 'var', self.args[0].type)
        ctx.lm.define_label(self.args[0].value, {'address': ctx.ip})

class JUMPInstruction(UnaryInstruction):
    def execute(self, ctx):
        # jump to label
        ctx.ip = ctx.fm.get_label_address(self.args[0].value)

class JUMPIFEQInstruction(TernaryInstruction):
    def execute(self, ctx):
        # jump to label if symb1 == symb2
        label = self.args[0].get_typed_value()
        symb1 = self.args[1]
        symb2 = self.args[2]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if symb1_value == symb2_value:
            ctx.ip = ctx.fm.get_label_address(label)

class JUMPIFNEQInstruction(TernaryInstruction):
    def execute(self, ctx):
        # jump to label if symb1 != symb2
        label = self.args[0].get_typed_value()
        symb1 = self.args[1]
        symb2 = self.args[2]
        symb1_value = ctx.get_symbol_value_or_error(symb1)
        symb2_value = ctx.get_symbol_value_or_error(symb2)
        if symb1_value != symb2_value:
            ctx.ip = ctx.lm.get_label_address(label)

class EXITInstruction(UnaryInstruction):
    def execute(self, ctx):
        symb1 = self.args[0].get_typed_value()
        if not isinstance(symb1, int) or symb1 < 0 or symb1 >= 49:
            #TODO define exception errors 57
            raise ippE.InvalidExitCodeError(symb1)

        ctx.ip = -1
        ctx.exit_code = self.args[0].get_typed_value()

class DPRINTInstruction(UnaryInstruction):
    def execute(self, ctx):
        symb1 = self.args[0].get_typed_value()
        print(symb1, file=sys.stderr)
    
class BREAKInstruction(NullaryInstruction):
    def execute(self, ctx):
        print(ctx.as_breakpoint(), file=sys.stderr)


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
    def class_from_opcode(cls, opcode: str) -> Union[Instruction, str]:
        return cls.instruction_set[opcode.upper()] if opcode in cls.instruction_set else opcode


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
        for uin in [uin for uin in instructions if isinstance(uin, str)]:
            raise ippE.UnsupportedInstructionError(uin)

        # check if order of instructions is valid
        # order is valid if instruction order is consecutive and there are no gaps
        zipped = list(zip(instructions, range(1, len(instructions), )))
        if any(uin.order != order for uin, order in zipped):
            raise ippE.UnsupportedXMLError('invalid order of instructions')

        return cls(instructions)