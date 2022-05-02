# define subclasses of Exception for each interpreter error type

# base error class for all ipppy errors with exit codes
class Error(Exception):
    def __init__(self, exit_code: int, message: str) -> None:
        self.exit_code = exit_code
        self.message = message

class NotYetImplementedError(Error):
    def __init__(self, message: str) -> None:
        super().__init__(32, f'Not yet implemented: {message}')
    
class GenericError(Error):
    def __init__(self) -> None:
        super().__init__(56, "Something went wrong")

class UnsupportedInstructionError(Error):
    def __init__(self, instruction: str) -> None:
        super().__init__(52, f'Unsupported instruction: {instruction}')
    
class UndefinedFrameError(Error):
    def __init__(self, frame_name: str) -> None:
        super().__init__(55, f'Frame {frame_name} is not defined')

class UndefinedVariableError(Error):
    def __init__(self, varname: str) -> None:
        super().__init__(54, f'Variable {varname} is not defined')

class UnknownTypeError(Error):
    def __init__(self, type_name: str) -> None:
        super().__init__(52, f'Unknown type: {type_name}')

class NoFrameToPush(Error):
    def __init__(self) -> None:
        super().__init__(55, "No frame to push")

class NoFrameToPop(Error):
    def __init__(self) -> None:
        super().__init__(55, "No frame to pop")

class JumpToUndefinedLabelError(Error):
    def __init__(self, label: str) -> None:
        super().__init__(52, f'Jump to undefined label: {label}')

class NowhereToReturnError(Error):
    def __init__(self) -> None:
        super().__init__(56, "Call stack empty. Nowhere to return")

class NoDataToPopError(Error):
    def __init__(self) -> None:
        super().__init__(56, "Data stack empty. No data to pop")

class InvalidInstructionArgumentTypesError(Error):
    def __init__(self, opcode: str, expected: str, actual: str = None) -> None:
        super().__init__(53, f'Invalid argument types for {opcode}: expected {expected}' + f" but got {actual}" if actual is not None else "")

class InvalidInputError(Error):
    def __init__(self, message: str) -> None:
        super().__init__(58, f'Invalid input: {message}')
    
class InvalidExitCodeError(Error):
    def __init__(self, exit_code: int) -> None:
        super().__init__(57, f'Invalid exit code: {exit_code}')

class VariableAlreadyDefinedError(Error):
    def __init__(self, varname: str) -> None:
        super().__init__(52, f'Variable {varname} is already defined')

class DivisionByZeroError(Error):
    def __init__(self) -> None:
        super().__init__(57, "Division by zero")

class NilComparisonError(Error):
    def __init__(self) -> None:
        super().__init__(53, "Nil comparison")
    
class BadXMLError(Error):
    def __init__(self, message: str) -> None:
        super().__init__(31, f'Bad XML: {message}')

class UnsupportedXMLError(Error):
    def __init__(self, message: str) -> None:
        super().__init__(32, f'Unsupported XML: {message}')

class BadValueError(Error):
    def __init__(self, opcode: str, message: str) -> None:
        super().__init__(57, f'Bad value for operand of {opcode}: {message}')

class InvalidArgumentCountError(Error):
    def __init__(self, opcode: str, expected: int, actual: int) -> None:
        super().__init__(32, f'Invalid argument count for {opcode}: expected {expected} but got {actual}')

class InputFileError(Error):
    def __init__(self, message: str) -> None:
        super().__init__(11, f'Input file error: {message}')