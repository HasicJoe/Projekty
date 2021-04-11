
import sys
from classes.exit_codes_handling import Exit_Codes

class Symb_on_DataStack:
    
    
    def __init__(self,symb_type,symb_value):
        self.type = symb_type
        self.value = symb_value
    
    
    def is_int(self):
        if self.type == "int":
            return True
        return False
    
    
    def is_bool(self):
        if self.type == "bool":
            return True
        return False


    def is_string(self):
        if self.type == "string":
            return True
        return False


    def is_nil(self):
        if self.type == "nil":
            return True
        return False

    
    def __str__(self):
        return "Type: %s, Value: %s" % (self.type,  self.value)
    
