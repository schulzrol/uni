from typing import Any, Dict, Optional, Sequence, Tuple
import ipppy.Errors as ippE

class FrameManager(object):
    def __init__(self, named_frames: Sequence[Tuple[str, bool]] = None) -> None:
        self.frames = {name: [dict()] if do_init else list() for name, do_init in named_frames or []}

    def __contains__(self, frame_name):
        return frame_name in self.frames.keys()
        
    def get(self, frame_name: str, key: str) -> Optional[Dict[str, Any]]:
        if not isinstance(key, str):
            raise ippE.InvalidInstructionArgumentTypesError("doesn't matter", 'str')

        if len(key.split('@')) > 1:
            key = key.split('@')[1]

        frames = self.frames.get(frame_name)
        return frames[-1].get(key) if frames else None
    
    def set(self, frame_name: str, key: str, val: Optional[Dict[str, Any]]) -> None:
        if not isinstance(key, str):
            raise ippE.InvalidInstructionArgumentTypesError("doesn't matter", 'str')

        frames = self.frames.get(frame_name)

        if len(key.split('@')) > 1:
            key = key.split('@')[1]

        if not isinstance(val, dict):
            val = {'value': val}

        if frames:
            if not key in frames[-1]:
                raise ippE.UndefinedVariableError(key)
            frames[-1][key] = val
        else:
            if frames is None:
                raise ippE.UndefinedFrameError(frame_name)
            raise ippE.UndefinedVariableError(key)

    def init_frame(self, frame_name: str) -> None:
        self.frames[frame_name] = [dict()]

    def pop(self, frame_name: str) -> Optional[Dict[str, Any]]:
        frames = self.frames.get(frame_name)
        if frames:
            metadata = frames.pop()
        else:
            metadata = None
        return metadata 
    
    def append(self, frame_name: str, new_frame: Dict[str, Any] = None):
        self.frames[frame_name].append(new_frame or dict())
        
    def is_defined(self, frame_name: str) -> bool:
        if frame_name not in self:
            return False

        return len(self.frames.get(frame_name)) > 0

    def move_to(self, frame_from: str, frame_to: str) -> None:
        self.append(frame_to, self.pop(frame_from))

    def frame_name_from_var(self, possible_frame_name: str) -> str:
        if possible_frame_name in self:
            return possible_frame_name
        
        return possible_frame_name.split('@')[0]

    def define_var(self, varname: str, value: Any = None) -> None:
        if not isinstance(varname, str):
            raise ippE.InvalidInstructionArgumentTypesError("doesn't matter", 'str')

        key = varname
        if len(varname.split('@')) > 1:
            key = varname.split('@')[1]

        if self.is_var_defined(varname):
            raise ippE.VariableAlreadyDefinedError(varname)

        frame_name = self.frame_name_from_var(varname)
        if not self.is_defined(frame_name):
            raise ippE.UndefinedFrameError(frame_name)

        self.frames[frame_name][-1][key] = {'value': value} if not isinstance(value, dict) else value

    def is_var_defined(self, varname: str) -> bool:
        frame_name = self.frame_name_from_var(varname)
        return self.get(frame_name, varname) is not None
    
    def set_var_value(self, varname: str, value: Any) -> None:
        frame_name = self.frame_name_from_var(varname)
        self.set(frame_name, varname, {'value': value})
    
    def get_var_metadata(self, varname: str) -> Optional[Dict[str, Any]]:
        frame_name = self.frame_name_from_var(varname)
        return self.get(frame_name, varname)
    
    def get_var_value_or_default(self, varname: str, default_value: Any) -> Any:
        #default_value = default_value if default_value is not None else varname

        frame_name = self.frame_name_from_var(varname)
        var = self.get(frame_name, varname)
        return var['value'] if var else default_value