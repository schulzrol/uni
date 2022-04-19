from typing import Dict, Any, Sequence, TextIO

from .LabelManager import LabelManager
from .FrameManager import FrameManager
from .InstructionManager import Argument, InstructionManager, Instruction

import ipppy.Errors as ippE

class Context():
    def __init__(self,
                 label_manager: LabelManager,
                 frame_manager: FrameManager,
                 instruction_manager: InstructionManager,
                 input_handler: TextIO) -> None:
        self.lm = label_manager
        self.fm = frame_manager
        self.im = instruction_manager
        self.data_stack = list()
        self.ip = 0
        self.call_stack = list()
        self.exit_code = 0
        self.input_handler = input_handler
        self.started = False

    def get_current_instruction(self, safe: bool = False) -> Instruction:
        if self.ip == -1:
            raise Exception("Exit called. No more instructions to execute.")
        return self.im.get_instruction(self.ip)

    def start(self) -> None:
        self.started = True

    def advance(self) -> bool:
        # exit called
        if self.ip < 0:
            return False
        
        # advance
        if self.started:
            self.ip += 1
        else:
            self.start()

        # detect bounds
        if self.ip >= self.im.get_size():
            return False

        return True
    
    # return string representation of current state of context
    def as_breakpoint(self) -> str:
        return "BREAK: ip: {}, current instruction: {}, call stack: {}, data stack: {}" \
               .format(self.ip, self.get_current_instruction(), self.call_stack, self.data_stack)

    def get_symbol_value_or_error(self, symbol: Argument) -> Any:
        if symbol.type.lower() == 'var' and not self.fm.is_var_defined(symbol.value):
            raise ippE.UndefinedVariableError(symbol.value)

        return self.fm.get_var_value_or_default(symbol.value, symbol.get_typed_value())

    def save_to_variable(self, varname: str, value: Any) -> None:
        self.fm.set_var_value(varname, value)