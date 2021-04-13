"""
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
Class to store variables
"""

class Variable:

    def __init__(self,name,var_type,value):
        self.name = name
        self.type = var_type
        self.value = value
        
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
    
    def update_value_type(self,new_value,new_type):
        """Method updates value and type of variable"""
        self.type = new_type
        self.value = new_value

    def empty_value(self):
        """Method verifies validity of value"""
        if self.value is None:
            return True
        return False

    def get_value(self):
        """Method returns value of variable"""
        return self.value
    
    def get_variable_type_value(self):
        """Method returns variable type and value"""
        return [self.type, self.value]

