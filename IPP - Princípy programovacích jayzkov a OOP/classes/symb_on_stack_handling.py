
"""
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
"""

import sys
from classes.exit_codes_handling import Exit_Codes

class Symb_on_DataStack:
    
    def __init__(self,symb_type,symb_value):
        self.type = symb_type
        self.value = symb_value
    
    def is_int(self):
        """Method verifies validity of datatype"""
        if self.type == "int":
            return True
        return False
    
    def is_bool(self):
        """Method verifies validity of datatype"""
        if self.type == "bool":
            return True
        return False

    def is_string(self):
        """Method verifies validity of datatype"""
        if self.type == "string":
            return True
        return False

    def is_nil(self):
        """Method verifies validity of datatype"""
        if self.type == "nil":
            return True
        return False
    
    
