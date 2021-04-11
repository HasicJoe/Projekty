class Variable:

    def __init__(self,name,var_type,value):
        self.name = name
        self.type = var_type
        self.value = value

        
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

    
    def update_value(self,value):
        self.value = value
    

    def update_type(self,new_type):
        self.type = new_type

    
    def update_value_type(self,new_value,new_type):
        self.type = new_type
        self.value = new_value


    def empty_value(self):
        if self.value is None:
            return True
        return False

        
    def get_value(self):
        return self.value
    

    def get_type(self):
        return self.type

    
    def get_variable(self):
        return self

    
    def get_variable_type_value(self):
        return [self.type, self.value]


    def __str__(self):
        return "Name: %s, Type: %s, Value: %s" % (self.name, self.type,self.value)

