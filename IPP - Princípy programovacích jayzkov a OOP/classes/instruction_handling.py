class Instruction:

    def __init__(self,order,opcode):
        self.order = order 
        self.opcode = opcode
        self.arguments = []
        self.type = None


    def add_type(self,inst_type):
        self.type = inst_type


    def add_argument(self,argument):
        self.arguments.append(argument)


    def __str__(self):
        return "Order: %s, OPCODE: %s" % (self.order, self.opcode)
