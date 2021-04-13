"""
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
Class to store each instruction and their 
arguments(arguments come classes/from arguments_handling.py) 
"""
class Instruction:

    def __init__(self,order,opcode):
        self.order = order 
        self.opcode = opcode
        self.arguments = []
        self.type = None

    def add_type(self,inst_type):
        """Method adds type to instruction"""
        self.type = inst_type

    def add_argument(self,argument):
        """Method adds arguments to instruction arguments"""
        self.arguments.append(argument)

