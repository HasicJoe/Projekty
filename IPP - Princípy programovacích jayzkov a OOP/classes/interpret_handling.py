"""
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
Class stores code(instructions) and labels
from XML representation
"""
from classes.label_handling import Label

class Code_to_interpret:

    def __init__(self):
        self.list = []
        self.labels = []

    def add_instruction(self,instruction):
        """Method adds instruction to list of code"""
        self.list.append(instruction)

    def add_labels(self):
        """Method adds label to list of labels"""
        for inst in self.list:
            if inst.opcode == "LABEL":
                if len(inst.arguments) == 1:
                    this_label = Label(inst.order,inst.arguments[0].name_or_value)
                    self.labels.append(this_label)
        
    def find_label(self,name):
        """Method finds label by name"""
        for label in self.labels:
            if label.label_name == name:
                return label
        return None
                
    def get_next_instruction(self,prev_order):
        """Method generates next instruction via inst. order"""
        for instruction in self.list:
            if instruction.order > prev_order:
                return instruction
        return None
