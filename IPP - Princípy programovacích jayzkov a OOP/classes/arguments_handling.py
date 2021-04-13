"""
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
Class is used for work with OPCODE arguments 
each argument has its own type,position and value
"""

import re
import sys
from classes.exit_codes_handling import Exit_Codes


class Argument:

    def __init__(self, arg_type, position, value):
        self.type = arg_type
        self.position = position
        self.name_or_value = value
    
    def is_nil(self):
        """Method verifies validity of datatype"""
        if self.type == "nil":
            return True
        else:
            return False
    
    def is_int(self):
        """Method verifies validity of datatype"""
        if self.type == "int":
            return True
        else:
            return False
            
    def is_string(self):
        """Method verifies validity of datatype"""
        if self.type == "string":
            return True
        else:
            return False
    
    def is_bool(self):
        """Method verifies validity of datatype"""
        if self.type == "bool":   
            return True
        else:
            return False     
    
    def is_none(self):
        """Method verifies validity of datatype"""
        if self.type is None:
            return True
        else:
            return False
    
    def is_var(self):
        """Method verifies validity of datatype"""
        if self.type == "var":
            return True
        else:
            return False

    def get_value(self):
        """Method returns value of arguments"""
        return self.name_or_value
        
    def empty_value(self):
        """Method verifies validity of value"""
        if self.name_or_value is None:
            return True
        return False
    
    def consider_possible_var(self):
        """
        Method controls possibility to var datatype
        is cases where this type is forbidden
        """
        if self.type == "var":
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        
    def get_inst_data_without_position(self):
        """Method returns type and value of argument"""
        return [self.type, self.name_or_value]
