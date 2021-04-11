import sys
from classes.exit_codes_handling import Exit_Codes
import re
from classes.symb_on_stack_handling import Symb_on_DataStack


class Memory_Model:


    def __init__(self):
        self.GF = []
        self.LF = None
        self.TF = None
        self.LF_counter = 0
        self.dataframe = []
        self.act_inst = None
        self.position = 0
        self.instruction_name = None
        self.instruction_arguments = None
        self.return_position = []
        self.input_data = []
        self.input_line = 0
    
    
    def clears(self):
        self.dataframe = []
    
    
    def get_operands(self):
        if len(self.dataframe) >= 2:
            return [self.dataframe[0],self.dataframe[1]]
        else:
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
    
    
    def get_single_operand(self):
        if len(self.dataframe) >= 1:
            return self.dataframe[0]
        else:
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
    
    
    def pop_operands_from_datastack(self):
        self.dataframe.pop(0)
        self.dataframe.pop(0)
    
        
    def pop_single_operand_from_datastack(self):
        self.dataframe.pop(0)
    
    
    def s2i(self):
        str_len, string = self.get_operands()
        if str_len.is_int() and string.is_string():
            if int(str_len.value) <= len(string.value) - 1:
                try:
                    value = ord(string.value[int(str_len.value)])
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
                result = Symb_on_DataStack("int",value)
                self.pop_operands_from_datastack()
                self.pushs_to_dataframe(result)
            else:
                sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
                
    def i2ch(self):
        op = self.get_single_operand()
        if op.is_int():
            try:
                value = chr(int(op.value))
            except:
                sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
            self.pop_single_operand_from_datastack()
            result = Symb_on_DataStack("string",value)
            self.pushs_to_dataframe(result)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
            
            
    def neg(self):
        op = self.get_single_operand()
        if op.is_bool():
            if op.value == "true":
                op.value = "false"
            elif op.value == "false":
                op.value = "true"
            else:
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
        
    def stack_logic(self,logic_op):
        op1,op2 = self.get_operands()
        if op1.is_bool() and op2.is_bool():
            if logic_op == "A": #and
                if op1.value == "true" and op2.value == "true":
                    result = Symb_on_DataStack("bool","true")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
                else:
                    result = Symb_on_DataStack("bool","false")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
            elif logic_op == "O": # or
                if op1.value == "true" or op2.value == "true":
                    result = Symb_on_DataStack("bool","true")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
                else:
                    result = Symb_on_DataStack("bool","false")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    
    def cond_jump(self,op,label):
        if op == "EQ":
            self.stack_comp("=")
            jump = self.get_single_operand()
            if jump.value == "true":
                self.next_position_to_call(label.order)
                
        elif op == "NEQ":
            self.stack_comp("=")
            jump = self.get_single_operand()
            if jump.value == "false":
                self.next_position_to_call(label.order)
    
    
    def stack_comp(self,op):
        op1,op2 = self.get_operands()
        if op1.is_bool() and op2.is_bool():
            if op == "<":
                if op1.value == "true" and op2.value == "false":
                    result = Symb_on_DataStack("bool","true")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
                else:
                    result = Symb_on_DataStack("bool","false")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
            
            elif op == ">":
                if op1.value == "false" and op2.value == "true":
                    result = Symb_on_DataStack("bool","true")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
                else:
                    result = Symb_on_DataStack("bool","false")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
            elif op == "=":
                if op1.value == op2.value:
                    result = Symb_on_DataStack("bool","true")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
                else:
                    result = Symb_on_DataStack("bool","false")
                    self.pop_operands_from_datastack()
                    self.pushs_to_dataframe(result)
        elif op1.is_int() and op2.is_int():
            if op == "<":
                try:
                    if int(op1.value) > int(op2.value):
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
                        
            elif op == ">":
                try:
                    if int(op1.value) < int(op2.value):
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
            elif op == "=":
                try:
                    if int(op1.value) == int(op2.value):
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
                         
        elif op1.is_string() and op2.is_string():
            if op == "<":
                try:
                    if op1.value > op2.value:
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
                        
            elif op == ">":
                try:
                    if op1.value < op2.value:
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
            
            elif op == "=":
                try:
                    if op1.value == op2.value:
                        result = Symb_on_DataStack("bool","true")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                    else:
                        result = Symb_on_DataStack("bool","false")
                        self.pop_operands_from_datastack()
                        self.pushs_to_dataframe(result)
                except:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)  
        elif op1.is_nil() and op2.is_nil():
            if op == "=":
                result = Symb_on_DataStack("bool","true")
                self.pop_operands_from_datastack()
                self.pushs_to_dataframe(result)
            else:
                result = Symb_on_DataStack("bool","false")
                self.pop_operands_from_datastack()
                self.pushs_to_dataframe(result)
        elif op1.is_nil() or op2.is_nil():
            if op == "=":
                result = Symb_on_DataStack("bool","false")
                self.pop_operands_from_datastack()
                self.pushs_to_dataframe(result)
            else:
                sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    
    def stack_arith(self,op):
        op1, op2 = self.get_operands()
        if op1.is_int() and op2.is_int():
            try:
                if op == "+":
                    res = int(op2.value) + int(op1.value)
                elif op == "-":
                    res = int(op2.value) - int(op1.value)
                elif op == "*":
                    res = int(op2.value) * int(op1.value)
                elif op == "/":
                    res = int(op2.value) // int(op1.value)
                    #print(op2.value,"/",op1.value,"=",res)
            except:
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            result = Symb_on_DataStack("int",res)
            self.pop_operands_from_datastack()
            self.pushs_to_dataframe(result)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
    
    def is_LF_defined(self):
        if self.LF_counter == 0:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)
    
    
    def is_TF_defined(self):
        if self.TF is None:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)
    
    
    def get_int_input_line(self):
        if len(self.input_data) > self.input_line:
            try:
                value = int(self.input_data[self.input_line])
                self.input_line = self.input_line + 1
            except:
                self.input_line  = self.input_line + 1
                return None
            return value
        else:
            return None
    
    
    def get_bool_input_line(self):
        if len(self.input_data) > self.input_line:
            try:
                value = str(self.input_data[self.input_line])
                self.input_line = self.input_line + 1
            except:
                self.input_line  = self.input_line + 1
                return None
            value = value.lower()
            if re.match("^(true|false)$",value):
                return value
            else:
                return "false"
        else:
            return None
    
    
    def get_string_input_line(self):
        if len(self.input_data) > self.input_line:
            try:
                value = str(self.input_data[self.input_line])
                self.input_line = self.input_line + 1
            except:
                self.input_line  = self.input_line + 1
                return None
            return value
        else:
            return None
    
        
    def add_input(self,args_input):
        self.input_data = args_input
    
    
    def add_return_position(self,position):
        self.return_position.insert(0,position)
    
    
    def get_and_rem_return_position(self):
        if len(self.return_position) == 0:
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        return_pos = self.return_position[0]
        self.return_position.pop(0)
        return return_pos
    
    
    def next_position_to_call(self,position):
        self.position = position


    def pushs_to_dataframe(self,symb):
        self.dataframe.insert(0,symb)

    
    def pops_from_dataframe(self):
        if len(self.dataframe) == 0:
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        var = self.dataframe[0]
        self.dataframe.pop(0)
        return var

        
    def push_TF_to_LF_frame(self):
        if self.TF is None:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)
      
        if self.LF is None:
            self.LF = []
        self.LF.insert(0,self.TF)
        self.LF_counter = self.LF_counter + 1
        self.TF = None
     
            
    def pop_LF_to_TF_frame(self):
        if self.LF_counter == 0:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)
        self.TF = self.LF[0]
        self.LF.pop(0)
        self.LF_counter = self.LF_counter - 1
        
        
    def create_TF_frame(self):
        if self.TF is None:
            self.TF = []
        else:
            self.TF.clear()
        
    
    def is_defined_in_GF(self,var):
        for variable in self.GF:
            if var.name == variable.name:
                return 1
        return 0


    def append_var_to_GF(self,var):
        if self.GF is not None:
            duplicity = self.is_defined_in_GF(var)
            if duplicity:
                sys.exit(Exit_Codes.SEMANTIC_ERR)
            else:
                self.GF.append(var)
        else:
            sys.exit(Exit_Codes.SEMANTIC_ERR)


    def get_GF_variable(self,variable_name):
        for variable in self.GF:
            if variable.name == variable_name:
                return variable
        return None


    def is_defined_in_LF(self,var):
        for variable in self.LF[0]: # frame on top of stack
            if var.name == variable.name:
                return 1
        return 0


    def append_var_to_LF(self,var):
        if self.LF_counter != 0:
            duplicity = self.is_defined_in_LF(var)
            if duplicity:
                sys.exit(Exit_Codes.SEMANTIC_ERR)
            else:
                self.LF[0].append(var)
        else:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)
            

    def get_LF_variable(self,variable_name):
        for variable in self.LF[0]:
            if variable.name == variable_name:
                return variable
        return None


    def is_defined_in_TF(self,var):
        for variable in self.TF:
            if var.name == variable.name:
                return 1
        return 0


    def append_var_to_TF(self,var):
        if self.TF is not None:
            duplicity = self.is_defined_in_TF(var)
            if duplicity:
                sys.exit(Exit_Codes.SEMANTIC_ERR)
            else:
                self.TF.append(var)
        else:
            sys.exit(Exit_Codes.INVALID_FRAME_ERR)


    def get_TF_variable(self,variable_name):
        for variable in self.TF:
            if variable.name == variable_name:
                return variable
        return None


    def current_instruction(self,instruction):
        self.position = instruction.order
        self.act_inst = instruction
        self.instruction_name = instruction.opcode
        self.instruction_arguments = instruction.arguments
