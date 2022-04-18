from typing import Any, Dict, Sequence
from .InstructionManager import Instruction, LABELInstruction

class LabelManager():
    def __init__(self, labels_init: Dict[str, Any] = None) -> None:
        self.labels = labels_init or dict()
    
    def get(self, label):
        return self.labels.get(label)
    
    def set(self, label, metadata: Dict[str, Any]):
        self.labels[label] = metadata

    def is_defined(self, label):
        return label in self.labels.keys()
    
    def get_label_address(self, label):
        return self.labels.get(label)['address']
    
    def define_label(self, label, metadata: Dict[str, Any]):
        self.set(label, metadata)

    @classmethod
    def from_instructions(cls, instructions: Sequence[Instruction]):
        label_instructions = {labins.args[0].value: labins.order - 1 for labins in instructions if isinstance(labins, LABELInstruction)}
        return cls(label_instructions)