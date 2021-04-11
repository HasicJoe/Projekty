import re
import sys
from classes.exit_codes_handling import Exit_Codes
class Argument:


    def __init__(self,arg_type,position,value):
        self.type = arg_type
        self.position = position
        self.name_or_value = value

    
    def validate_string_value(self):
        if self.name_or_value is None:
            self.name_or_value = ""
            return
        if not re.match("^([^\#|^\s|^\\\]|([\\\][0-1][0-9][0-9]))*$",self.name_or_value):
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)

    
    def validate_bool_value(self):
        if not re.match("^(true|false)$",self.name_or_value):
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)

            
    def validate_nil_value(self):
        if not re.match("^nil$",self.name_or_value):
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)


    def is_nil(self):
        if self.type == "nil":
            return True
        else:
            return False

    
    def is_int(self):
        if self.type == "int":
            return True
        else:
            return False

            
    def is_string(self):
        if self.type == "string":
            return True
        else:
            return False

    
    def is_bool(self):
        if self.type == "bool":   
            return True
        else:
            return False     

    
    def is_none(self):
        if self.type is None:
            return True
        else:
            return False

    
    def is_var(self):
        if self.type == "var":
            #print(self.name_or_value)
            return True
        else:
            return False
    def get_value(self):
        return self.name_or_value
    
    
    def empty_value(self):
        if self.name_or_value is None:
            return True
        return False
    
    
    def consider_possible_var(self):
        if self.type == "var":
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
    
        
    def get_inst_data(self):
        return [self.type, self.position, self.name_or_value]


    def get_inst_data_without_position(self):
        return [self.type, self.name_or_value]

    
    def __str__(self):
        return "Type: %s, Position: %s, Name or value: %s" % (self.type, self.position, self.name_or_value)
