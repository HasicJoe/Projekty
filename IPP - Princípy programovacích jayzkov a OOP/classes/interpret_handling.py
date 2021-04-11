from classes.label_handling import Label

class Code_to_interpret:

    def __init__(self):
        self.list = []
        self.labels = []


    def add_instruction(self,instruction):
        self.list.append(instruction)


    def add_labels(self):
        for inst in self.list:
            if inst.opcode == "LABEL":
                if len(inst.arguments) == 1:
                    this_label = Label(inst.order,inst.arguments[0].name_or_value)
                    self.labels.append(this_label)
    
    def get_labels(self):
        return self.labels
    
    def find_label(self,name):
        for label in self.labels:
            if label.label_name == name:
                return label
        return None
            
    
    def get_next_instruction(self,prev_order):
        for instruction in self.list:
            if instruction.order > prev_order:
                return instruction
        return None


    def __str__(self):
        return "List is %s label is %s" % (self.list,self.labels)
