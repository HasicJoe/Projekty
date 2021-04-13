""" 
IPP - Project 2. - Interpret of IPPCODE21
Author - Samuel Valaštín <xvalas10@stud.fit.vutbr.cz> 
"""

import os
import sys
import argparse
import xml.etree.ElementTree as ElementTree
import re
from classes.arguments_handling import Argument
from classes.memory_model_handling import Memory_Model
from classes.variable_handling import Variable
from classes.interpret_handling import Code_to_interpret
from classes.label_handling import Label
from classes.exit_codes_handling import Exit_Codes
from classes.instruction_handling import Instruction
from classes.symb_on_stack_handling import Symb_on_DataStack

def defvar(model, instruction):
    """Function executes a DEFVAR instruction"""     
    variable = instruction.arguments[0]
        
    if variable is None:
        sys.exit(Exit_Codes.SEMANTIC_ERR)
    
    split_var = re.split("@",variable.name_or_value,maxsplit=1)
    var = Variable(split_var[1],None,None)
    if split_var[0] == "GF":
        model.append_var_to_GF(var)
    elif split_var[0] == "TF":
        model.append_var_to_TF(var)
    elif split_var[0] == "LF":
        model.append_var_to_LF(var)
    else:
        sys.exit(Exit_Codes.SEMANTIC_ERR)
 

def move(model, instruction):
    """Function executes a MOVE instruction"""  
    destiny_var = check_find_var(instruction.arguments[0], model)
    source_var = check_find_var(instruction.arguments[1], model)
    
    if destiny_var is None : # have to be var 
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.SEMANTIC_ERR)
        
    if source_var is None: # symb <string,int,bool,nil>
        if instruction.arguments[1].is_string():
            destiny_var.update_value_type(instruction.arguments[1].name_or_value, instruction.arguments[1].type)         
        elif instruction.arguments[1].is_int():
            destiny_var.update_value_type(instruction.arguments[1].name_or_value, instruction.arguments[1].type)  
        elif instruction.arguments[1].is_bool():
            destiny_var.update_value_type(instruction.arguments[1].name_or_value, instruction.arguments[1].type)    
        elif instruction.arguments[1].is_nil():
            destiny_var.update_value_type(instruction.arguments[1].name_or_value, instruction.arguments[1].type)      
    else: 
        destiny_var.update_value_type(source_var.value, source_var.type)
       

def arith(model, instruction, op):
    """Function prepares and controls operands for arithmetical operations(without stack)"""  
    res_var = check_find_var(instruction.arguments[0], model)
    operand_1 = check_find_var(instruction.arguments[1], model)
    operand_2 = check_find_var(instruction.arguments[2], model)

    if res_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    if operand_1 is not None and operand_2 is not None: #var var
        if operand_1.is_int() and operand_2.is_int():
            consider_calculate(res_var, operand_1.value, operand_2.value, op)
        else:
            if operand_1.empty_value() or operand_2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR) 
    elif operand_1 is not None and operand_2 is None:
        instruction.arguments[2].consider_possible_var()
        if operand_1.is_int() and instruction.arguments[2].is_int():
            consider_calculate(res_var, operand_1.value, instruction.arguments[2].name_or_value, op)        
        else:
            if operand_1.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif operand_1 is None and operand_2 is not None:
        instruction.arguments[1].consider_possible_var()
        if instruction.arguments[1].is_int() and operand_2.is_int():   
            consider_calculate(res_var, instruction.arguments[1].name_or_value, operand_2.value, op)      
        else:
            if operand_2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif operand_1 is None and operand_2 is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()
        if instruction.arguments[1].is_int() and instruction.arguments[2].is_int():
            consider_calculate(res_var, instruction.arguments[1].name_or_value, instruction.arguments[2].name_or_value, op)      
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)


def consider_calculate(result, op1_val, op2_val, op):
    """Function executes arithmetical operations(without stack)"""  
    if op == "+":
        try:
            result_value = int(op1_val) + int(op2_val)
        except:
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR) # invalid int value from XML source
    elif op == "-":
        try:
            result_value = int(op1_val) - int(op2_val)
        except:
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR) # invalid int value from XML source
    elif op == "*":
        try:
            result_value = int(op1_val) * int(op2_val)
        except:
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR) # invalid int value from XML source
    elif op == "/":
        if int(op2_val) == 0:
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)        
        try:
            result_value = int(op1_val) // int(op2_val)
        except:
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR) # invalid int value from XML source
    result.update_value_type(result_value, "int")


def not_ins(model, instruction):
    """Function executes NOT instruction""" 
    if instruction.arguments[0].is_var() and instruction.arguments[1].is_var(): # var var
        destiny = check_find_var(instruction.arguments[0], model)
        operand = check_find_var(instruction.arguments[1], model)
        symb = False
    elif instruction.arguments[0].is_var() and not instruction.arguments[1].is_var(): #var symb
        destiny = check_find_var(instruction.arguments[0], model)
        operand = instruction.arguments[1]
        symb = True
    
    if operand is None:
        sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        
    if operand.empty_value():
        sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)    
        
    if not operand.is_bool():
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    if destiny is None:
        sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
    
    if symb:
        if operand.get_value() == "true":
            destiny.update_value_type("false", "bool")
        else:
            destiny.update_value_type("true", "bool")        
    else:
        if operand.get_value() == "true":
            destiny.update_value_type("false", "bool")
        elif operand.get_value() == "false":
            destiny.update_value_type("true", "bool")
        else:
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)


def check_find_var(operand, model):
    """
    Function checks and finds vars in memory frames.
    In case of symb <int,bool,string,label....> returns None
    """
    if operand.name_or_value is not None:
        if re.match("(^[T|L|G]{1}[F]{1}[@]{1}[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$)", operand.name_or_value):
            split_operand = re.split("@", operand.name_or_value, maxsplit = 1)
            if split_operand[0] == "GF":
                found_operand = model.get_GF_variable(split_operand[1])
            elif split_operand[0] == "TF":
                model.is_TF_defined()
                found_operand = model.get_TF_variable(split_operand[1])
                if found_operand is None:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
            elif split_operand[0] == "LF":
                model.is_LF_defined()
                found_operand = model.get_LF_variable(split_operand[1])
                if found_operand is None:
                    sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
            else:
                sys.exit(Exit_Codes.SEMANTIC_ERR)
            if found_operand is not None:
                return found_operand
    return None
            

def consider_typecast_st_int(model, destiny_var,string_val, len_val):
    """Function executes typecast from string to integer"""
    if int(len_val) <= len(string_val)-1:
        new_value = ord(string_val[int(len_val)])
        destiny_var.update_value_type(new_value, "int")
    else:
        sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
        
     
def stri2int(model, instruction):
    """Function checks and prepares operands to typecast from string to integer"""    
    destiny_var = check_find_var(instruction.arguments[0], model)
    source_string = check_find_var(instruction.arguments[1], model)
    source_len = check_find_var(instruction.arguments[2], model)
    
    if destiny_var is None: 
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.SEMANTIC_ERR)
    
    if source_string is None and source_len is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()
        source_string_type, source_string_value = instruction.arguments[1].get_inst_data_without_position()
        len_type, len_value = instruction.arguments[2].get_inst_data_without_position()
        if instruction.arguments[2].is_int() and instruction.arguments[1].is_string():
            consider_typecast_st_int(model, destiny_var, source_string_value, len_value)      
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif source_string is None and source_len is not None:
        instruction.arguments[1].consider_possible_var()
        source_string_type, source_string_value = instruction.arguments[1].get_inst_data_without_position()
        if source_len.is_int() and instruction.arguments[1].is_string():
            consider_typecast_st_int(model, destiny_var, source_string_value, source_len.value)      
        else:
            if source_len.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    elif source_string is not None and source_len is None:
        instruction.arguments[2].consider_possible_var()
        len_type, len_value = instruction.arguments[2].get_inst_data_without_position()
        if instruction.arguments[2].is_int() and source_string.is_string(): # len -> symb , string -> var -> var,symb
            consider_typecast_st_int(model, destiny_var, source_string.value, len_value)          
        else:
            if source_string.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    elif source_string is not None and source_len is not None:
        if source_string.is_string() and source_len.is_int():
            consider_typecast_st_int(model, destiny_var, source_string.value, source_len.value)  
        else:
            if source_string.empty_value() or source_len.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)      


def int2char(model, instruction):
    """Function checks and prepares operands to typecast from integer to char(string in our case)"""
    destiny_var = check_find_var(instruction.arguments[0], model)
    source = check_find_var(instruction.arguments[1], model)
    
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    if source is None:
        if not instruction.arguments[1].is_int():
            instruction.arguments[1].consider_possible_var()
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        try:
            new_value = chr(int(instruction.arguments[1].name_or_value))
        except:
            sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
        destiny_var.update_value_type(new_value, "string")
        
    else:
        if not source.is_int():
            if source.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        try:
            new_value = chr(int(source.value))
        except:
            sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
        destiny_var.update_value_type(new_value, "string")

            
def logic(model, instruction, op):
    """
    Function checks and prepares operands to executing logical
    instructions <AND,OR> 
    Parameter op determine which of these operation will be performed
    """
    result = check_find_var(instruction.arguments[0], model)
    if result is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.SEMANTIC_ERR)

    operand_1 = check_find_var(instruction.arguments[1], model)
    operand_2 = check_find_var(instruction.arguments[2], model)

    if operand_1 is None and operand_2 is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()
        if instruction.arguments[1].is_bool() and instruction.arguments[2].is_bool():
            consider_logic(result, instruction.arguments[1].name_or_value, instruction.arguments[2].name_or_value, op)      
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif operand_1 is None and operand_2 is not None:
        instruction.arguments[1].consider_possible_var()
        if instruction.arguments[1].is_bool() and operand_2.is_bool():
            consider_logic(result, instruction.arguments[1].name_or_value, operand_2.value, op) 
        else:
            if operand_2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif operand_1 is not None and operand_2 is None:
        instruction.arguments[2].consider_possible_var()
        if operand_1.is_bool() and instruction.arguments[2].is_bool():
            consider_logic(result, operand_1.value, instruction.arguments[2].name_or_value, op) 
        else:
            if operand_1.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif operand_1 is not None and operand_2 is not None:
        if operand_1.is_bool() and operand_2.is_bool():
            consider_logic(result, operand_1.value, operand_2.value, op)
        else:
            if operand_1.empty_value() or operand_2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    else:
        sys.exit(Exit_Codes.SEMANTIC_ERR)


def consider_logic(result, value1, value2, op):
    """Function executes logical instructions <AND,OR>
    parameters value1 and value2 are boolean operands"""
    if op == "OR":
        if value1 == "true" or value2 == "true":
            result.update_value_type("true", "bool")
        elif value1 == "false" and value2 == "false":
            result.update_value_type("false", "bool") 
        else:
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
    elif op == "AND":
        if value1 == "true" and value2 == "true":
            result.update_value_type("true", "bool")
        elif value1 == "false" or value2 == "false":
            result.update_value_type("false", "bool")
        else:
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
    else:
        sys.exit(Exit_Codes.SEMANTIC_ERR)


def consider_concat(model, destiny_var, str1, str2):
    """Function executes concatenation of two strings (CONCAT instruction)"""
    if str1 is None and str2 is None:
        destiny_var.update_value_type("", "string")
    elif str1 is None:
        destiny_var.update_value_type(str2, "string")
    elif str2 is None:
        destiny_var.update_value_type(str1, "string")
    else:
        concat_string = str1 + str2
        destiny_var.update_value_type(concat_string, "string")


def concat(model, instruction):
    """ Function checks and prepares operands to executing CONCAT instruction"""
    destiny_var = check_find_var(instruction.arguments[0], model)
    source_str1 = check_find_var(instruction.arguments[1], model)
    source_str2 = check_find_var(instruction.arguments[2], model)
    
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    if source_str1 is None and source_str2 is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()
        if instruction.arguments[1].is_string() and instruction.arguments[2].is_string():
            consider_concat(model, destiny_var, instruction.arguments[1].name_or_value, instruction.arguments[2].name_or_value)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif source_str1 is None and source_str2 is not None:
        instruction.arguments[1].consider_possible_var()
        if instruction.arguments[1].is_string() and source_str2.is_string():
            consider_concat(model, destiny_var, instruction.arguments[1].name_or_value, source_str2.value)      
        else:
            if source_str2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif source_str1 is not None and source_str2 is None:
        instruction.arguments[2].consider_possible_var()
        if source_str1.is_string() and instruction.arguments[2].is_string():
            consider_concat(model, destiny_var, source_str1.value, instruction.arguments[2].name_or_value)    
        else:
            if source_str1.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    elif source_str1 is not None and source_str2 is not None:
        if source_str1.is_string() and source_str2.is_string():
            consider_concat(model, destiny_var, source_str1.value, source_str2.value)    
        else:
            if source_str1.empty_value() or source_str2.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    else:
        sys.exit(Exit_Codes.SEMANTIC_ERR)


def strlen(model, instruction):
    """Function executes STRLEN instruction"""
    destiny_var = check_find_var(instruction.arguments[0], model)
    string = check_find_var(instruction.arguments[1], model)
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    if string is None:
        if instruction.arguments[1].is_string():
            if instruction.arguments[1].name_or_value is not None:
                str_len = len(instruction.arguments[1].name_or_value)
            else:
                str_len = 0
            destiny_var.update_value_type(str_len, "int")
        elif instruction.arguments[1].is_var():
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    else:
        if string.empty_value():
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        if string.is_string():
            if string.value is not None:
                str_len = len(string.value)
            else:
                str_len = 0
            destiny_var.update_value_type(str_len, "int")
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)


def consider_getchar(model, destiny_var, string, position):
    """Function executes GETCHAR instruction"""
    if int(position) <= len(string) - 1:
        destiny_var.update_value_type(string[int(position)], "string")
    else:
        sys.exit(Exit_Codes.INVALID_STR_OP_ERR)
    
    
def getchar(model, instruction):
    """Function checks and prepares operands to GETCHAR instruction"""
    destiny_var = check_find_var(instruction.arguments[0], model)
    string = check_find_var(instruction.arguments[1], model)
    position = check_find_var(instruction.arguments[2], model)
    
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()    
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    if string is None and position is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()  
        if instruction.arguments[1].is_string() and instruction.arguments[2].is_int():         
            consider_getchar(model, destiny_var, instruction.arguments[1].name_or_value, instruction.arguments[2].name_or_value)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif string is None and position is not None:
        instruction.arguments[1].consider_possible_var()    
        if instruction.arguments[1].is_string() and position.is_int():
            consider_getchar(model, destiny_var, instruction.arguments[1].name_or_value, position.value)
        else:
            if position.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    
    elif string is not None and position is None:
        instruction.arguments[2].consider_possible_var()
        if string.is_string() and posiinstruction.arguments[2].is_int():
            consider_getchar(model, destiny_var, string.value, instruction.arguments[1].name_or_value)
        else:
            if string.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
    elif string is not None and position is not None:
        if string.is_string() and position.is_int():
            consider_getchar(model, destiny_var, string.value, position.value)
        else:
            if string.empty_value() or position.empty_value():
                sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    else:
        sys.exit(Exit_Codes.SEMANTIC_ERR)
 
      
def consider_setchar(model, destiny_var, position, string):
    """Function executes SETCHAR instruction"""
    if string is not None and len(string) > 0 and int(position) <= len(destiny_var.get_value()) - 1:
        value = destiny_var.get_value()
        value = list(value)
        value[int(position)] = string[0]
        new_value = "".join(value)
        destiny_var.update_value_type(new_value, "string")
    else:
        sys.exit(Exit_Codes.INVALID_STR_OP_ERR)

               
def setchar(model,instruction):
    """Function checks and prepares operands to SETCHAR instruction"""
    destiny_var = check_find_var(instruction.arguments[0], model)

    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
    if destiny_var.empty_value():
        sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        
    if not destiny_var.is_string():
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
    if instruction.arguments[1].is_var() and instruction.arguments[2].is_var():
        position = check_find_var(instruction.arguments[1], model)
        char_from_str = check_find_var(instruction.arguments[2], model)
        if position or char_from_str is None:
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        else:
            if position.is_int() and char_from_str.is_string():
                consider_setchar(model, destiny_var, position.value, char_from_str.value)
            else:
                if position.is_none() or char_from_str.is_none():
                    sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
                else:
                    sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif instruction.arguments[1].is_var() and not instruction.arguments[2].is_var():
        position = check_find_var(instruction.arguments[1],model)
        if position is None:
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        else:
            if position.is_int() and instruction.arguments[2].is_string():
                consider_setchar(model, destiny_var, position.value, instruction.arguments[2].name_or_value)
            else:
                if position.is_none():
                    sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
                else:
                    sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif not instruction.arguments[1].is_var() and not instruction.arguments[2].is_var():
        
        if instruction.arguments[1].is_int() and instruction.arguments[2].is_string():
            consider_setchar(model, destiny_var, instruction.arguments[1].name_or_value, instruction.arguments[2].name_or_value)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif not instruction.arguments[1].is_var() and instruction.arguments[2].is_var():
        char_from_str = check_find_var(instruction.arguments[2], model)
        if char_from_str is None:
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        else:
            if instruction.arguments[1].is_int() and char_from_str.is_string():
                consider_setchar(model, destiny_var, instruction.arguments[1].name_or_value, char_from_str.value)
            else:
                if char_from_str.is_none():
                    sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
                else:
                    sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
 
 
def consider_var_type(model, destiny_var, symb_type):
    """ Function updates value and type of variable (TYPE instruction)"""
    destiny_var.update_value_type(symb_type, "string")


def get_var_type(model, instruction):
    """Function checks and prepare operands for TYPE instruction"""
    destiny_var = check_find_var(instruction.arguments[0], model)
    symb = check_find_var(instruction.arguments[1], model)
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR) 
    if symb is None:
        consider_var_type(model, destiny_var, instruction.arguments[1].type)
    else:
        if symb.type is None:
            consider_var_type(model, destiny_var, "")
        else:
            consider_var_type(model, destiny_var, symb.type)


def break_debug(model, instruction):
    """ Function executes BREAK instruction"""
    print("Actual position:", model.position,"\t Actual instruction name:", model.instruction_name, file=sys.stderr)


def push_symb(model, instruction):
    """ Function executes PUSHS instruction """
    defined_var = check_find_var(instruction.arguments[0], model)
    if defined_var is None:
        instruction.arguments[0].consider_possible_var()
        symb_type, symb_value = instruction.arguments[0].get_inst_data_without_position()
    else:
        symb_type, symb_value = defined_var.get_variable_type_value() 
    symb = Symb_on_DataStack(symb_type,symb_value)
    model.pushs_to_dataframe(symb)


def pop_var(model, instruction):
    """ Function pops value and type from dataframe (POPS instruction)"""
    var = check_find_var(instruction.arguments[0], model)
    if var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
    var_from_stack = model.pops_from_dataframe()
    var.update_value_type(var_from_stack.value, var_from_stack.type) 
    
    
def exit_it(model, instruction):
    """Function executes EXIT instruction which terminate interpretation of code"""
    var = check_find_var(instruction.arguments[0], model)
    if var is None:
        instruction.arguments[0].consider_possible_var()
        if instruction.arguments[0].is_int():
            if int(instruction.arguments[0].name_or_value) >= 0 and int(instruction.arguments[0].name_or_value) < 50:
                sys.exit(int(instruction.arguments[0].name_or_value)) 
            else:
                sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    else:
        if var.empty_value():
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        
        if var.is_int():
            if int(var.value) >= 0 and int(var.value) < 50:
                sys.exit(int(var.value))
            else:
                sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        

def compare(model, instruction, op):
    """
    Function checks operands and prepare them to comparation
    parameter op determine which type of comparsion have to be done <LT,GT,EQ>
    """
    destiny_var = check_find_var(instruction.arguments[0], model)
    op1 = check_find_var(instruction.arguments[1], model)
    op2 = check_find_var(instruction.arguments[2], model)
    
    if destiny_var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.SEMANTIC_ERR)    
    if op1 is None and op2 is None:
        instruction.arguments[1].consider_possible_var()
        instruction.arguments[2].consider_possible_var()    
        op1_type, op1_value = instruction.arguments[1].get_inst_data_without_position()
        op2_type, op2_value = instruction.arguments[2].get_inst_data_without_position()
    elif op1 is None and op2 is not None:
        instruction.arguments[1].consider_possible_var()
        op1_type,op1_value = instruction.arguments[1].get_inst_data_without_position()
        op2_type,op2_value = op2.get_variable_type_value()
        if op2.empty_value() and not op2.is_string():
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
    elif op1 is not None and op2 is None:
        instruction.arguments[2].consider_possible_var() 
        op1_type, op1_value = op1.get_variable_type_value()
        op2_type, op2_value = instruction.arguments[2].get_inst_data_without_position()
        if op1.empty_value() and not op1.is_string():
            
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
    elif op1 is not None and op2 is not None:
        op1_type, op1_value = op1.get_variable_type_value()
        op2_type, op2_value = op2.get_variable_type_value()
        if op1.empty_value() and not op1.is_string() or op2.empty_value() and not op2.is_string():
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
    consider_compare([op1_type,op1_value], [op2_type,op2_value], destiny_var, op, True)
    

def update_compare(res_var, res_update, value, res_type):
    """
    Function decides via parameter res_update
    about updating result var or only returning value
    """ 
    if res_update:
        res_var.update_value_type(value, res_type)
    else:
        if value == "true":
            return True
        elif value == "false":
            return False
        
    
def consider_compare(op1,op2,res,op,res_update):
    """
    Function executes comparsion between operands
    parameters op1, op2 are lists - 0th element is type
    and 1th element is value of operand
    parameter res_update decides about updating value of variable
    """
    if op1[0] == "bool" and op2[0] == "bool":
        if op == "GT":
            if op1[1] == "true" and op2[1] == "false":
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
        elif op == "LT":
            if op1[1] == "false" and op2[1] == "true":
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
        elif op == "EQ":
            if op1[1] == "true" and op2[1] == "true" or op1[1] == "false" and op2[1] == "false":
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
    elif op1[0] == "string" and op2[0] == "string":
        if op1[1] is None or op2[1] is None:
            if op1[1] is None and op2[1] is None:
                if op == "EQ":
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
            elif op1[1] is None:
                if op == "LT":
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
            elif op2[1] is None:
                if op == "GT":
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
        elif op1[1] is not None and op2[1] is not None:
            if op == "GT":
                if op1[1] > op2[1]:
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
            elif op == "LT":
                if op1[1] < op2[1]:
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
            elif op == "EQ":
                if op1[1] == op2[1]:
                    ret_val = update_compare(res, res_update, value="true", res_type="bool")
                    return ret_val
                else:
                    ret_val = update_compare(res, res_update, value="false", res_type="bool")
                    return ret_val
    elif op1[0] == "int" and op2[0] == "int":
        if op == "GT":
            if int(op1[1]) > int(op2[1]):
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
        elif op == "LT":
            if int(op1[1]) < int(op2[1]):
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
        elif op == "EQ":
            if int(op1[1]) == int(op2[1]):
                ret_val = update_compare(res, res_update, value="true", res_type="bool")
                return ret_val
            else:
                ret_val = update_compare(res, res_update, value="false", res_type="bool")
                return ret_val
    elif op1[0] == "nil" and op2[0] == "nil":
        if op1[1] == "nil" and op2[1] == "nil":
           ret_val = update_compare(res, res_update, value="true", res_type="bool")
           return ret_val
        else:
            sys.exit(Exit_Codes.INVALID_OP_VAL_ERR)
    elif op1[0] == "nil" or op2[0] == "nil":
        ret_val = update_compare(res, res_update, value="false", res_type="bool")
        return ret_val
    else:
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)


def call(model, instruction, code):
    """Function executes CALL instruction"""
    label = code.find_label(instruction.arguments[0].name_or_value)
    if label is None:
        sys.exit(Exit_Codes.SEMANTIC_ERR) # undefined label
    model.add_return_position(model.position)
    model.next_position_to_call(label.order)   


def return_handling(model, instruction, code):
    """Function executes RETURN instruction"""
    return_position = model.get_and_rem_return_position()
    model.next_position_to_call(return_position)


def jump(model, instruction, code):
    """Function executes JUMP instruction"""
    label = code.find_label(instruction.arguments[0].name_or_value)
    if label is None:
        sys.exit(Exit_Codes.SEMANTIC_ERR)    # TODO LOOK IF JUMP TO UNDEFINED LABEL
    model.next_position_to_call(label.order)

   
def stack_jump(model, instruction, op, code):
    """
    Function searchs for valid label to JUMPS instruction
    and after that gives control to memory model
    """ 
    label = code.find_label(instruction.arguments[0].name_or_value)
    if label is None:
        sys.exit(Exit_Codes.SEMANTIC_ERR)
    model.cond_jump(op,label)

    
def conditional_jump(model, instruction, code, op):
    """ 
    Function checks and prepares types and values of operands
    to conditional jump instructions <JUMPIFEQ,JUMPIFNEQ>
    parameter op selects instruction
    """
    label = code.find_label(instruction.arguments[0].name_or_value)
    if label is None:
        sys.exit(Exit_Codes.SEMANTIC_ERR)
    op1 = check_find_var(instruction.arguments[1], model)
    op2 = check_find_var(instruction.arguments[2], model)
    if op1 is None and op2 is None:
        op1_type, op1_value = instruction.arguments[1].get_inst_data_without_position()
        op2_type, op2_value = instruction.arguments[2].get_inst_data_without_position()
        if op1_type == "var" or op2_type == "var":
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
    elif op1 is None and op2 is not None:
        op1_type, op1_value = instruction.arguments[1].get_inst_data_without_position()
        if op1_type == "var":
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
        op2_type, op2_value = op2.get_variable_type_value()
        if op2_value is None and not op2_type == "string":
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        if op2_type is None:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    elif op1 is not None and op2 is None:
        op1_type, op1_value = op1.get_variable_type_value()
        if op1_value is None and not op1_type == "string":
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        if op1_type is None:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        op2_type, op2_value = instruction.arguments[2].get_inst_data_without_position()
        if op2_type == "var":
            sys.exit(Exit_Codes.INVALID_VAR_ACCESS_ERR)
    elif op1 is not None and op2 is not None:
        op1_type, op1_value = op1.get_variable_type_value()
        op2_type, op2_value = op2.get_variable_type_value()
        
        if op2_value is None or op1_value is None and not op2_type == "string" and not op1_type == "string":
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        if op2_type is None or op1_type is None:
            sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
        
    if op == "EQ":
        jump = consider_compare([op1_type,op1_value], [op2_type,op2_value], "", "EQ", False)
        if jump:
            model.next_position_to_call(label.order)
    elif op == "NEQ":
        jump = consider_compare([op1_type,op1_value], [op2_type,op2_value], "", "EQ", False)
        if not jump:
            model.next_position_to_call(label.order)


def read(model, instruction):
    """
    Function executes READ instruction which includes
    loading input line from memory model
    """
    var = check_find_var(instruction.arguments[0], model)
    
    if var is None:
        instruction.arguments[0].consider_possible_var()
        sys.exit(Exit_Codes.SEMANTIC_ERR)
        
    if instruction.arguments[1].name_or_value == "bool":
        value = model.get_bool_input_line()
        if value is None:
            var.update_value_type("nil", "nil")
        else:
            var.update_value_type(value, "bool")
    elif instruction.arguments[1].name_or_value == "int":
        value = model.get_int_input_line()
        if value is None:
            var.update_value_type("nil", "nil")
        else:
            var.update_value_type(value, "int")
    elif instruction.arguments[1].name_or_value == "string":
        value = model.get_string_input_line()
        if value is None:
            var.update_value_type("nil", "nil")
        else:
            var.update_value_type(value, "string")
    else:
        sys.exit(Exit_Codes.INVALID_OP_TYPES_ERR)
    

def write(model, instruction):
    """Function executes WRITE instruction"""
    write_var = check_find_var(instruction.arguments[0], model)
    if write_var is None:
        instruction.arguments[0].consider_possible_var()

        if instruction.arguments[0].is_nil():
            print("", end="")
        else:
            print(instruction.arguments[0].name_or_value, end="")
    else:
        
        if write_var.empty_value() and not write_var.is_string():
            sys.exit(Exit_Codes.INVALID_VAR_VALUE_ERR)
        if write_var.is_nil():
            print("", end="")
        else:
            if not write_var.empty_value():
                print(write_var.value, end="")


def dprint(model, instruction):
    """Function executes DPRINT instruction"""
    var = check_find_var(instruction.arguments[0], model)
    if var is None:
        instruction.arguments[0].consider_possible_var()
        if instruction.arguments[0].empty_value:
            print("", file=sys.stderr)
        else:
            print(instruction.arguments[0].name_or_value, file=sys.stderr)  
    else:
        if var.empty_value():
            print("", file=sys.stderr)
        else:
            print(var.value, file=sys.stderr)  
 
            
def start_interpretation(code, args):
    """
    Main function which interprets code 
    parameter code represents parsed code from XML structure
    parameter args represents command line arguments and via
    method add_input loads input for READ instruction
    model handles program flow control, frames control and also stack instructions
    """
    model = Memory_Model()
    model.add_input(args["input"])
    while True:
        instruction = code.get_next_instruction(model.position)
        if instruction is not None:
            model.current_instruction(instruction)
            if model.instruction_name == "MOVE": 
                move(model, instruction)  
            elif model.instruction_name == "CREATEFRAME": 
                model.create_TF_frame()
            elif model.instruction_name == "PUSHFRAME": 
                model.push_TF_to_LF_frame()
            elif model.instruction_name == "POPFRAME":  
                model.pop_LF_to_TF_frame()
            elif model.instruction_name == "DEFVAR": 
                defvar(model, instruction)
            elif model.instruction_name == "CALL": 
                call(model, instruction, code)
            elif model.instruction_name == "RETURN":    
                return_handling(model, instruction, code)
            elif model.instruction_name == "PUSHS": 
                push_symb(model, instruction)
            elif model.instruction_name == "POPS":  
                pop_var(model, instruction)
            elif model.instruction_name == "ADD": 
                arith(model, instruction, "+")
            elif model.instruction_name == "SUB" : 
                arith(model, instruction, "-")
            elif model.instruction_name == "MUL": 
                arith(model, instruction, "*")
            elif model.instruction_name == "IDIV":
                arith(model, instruction, "/")
            elif model.instruction_name == "LT":    
                compare(model, instruction, "LT")
            elif model.instruction_name == "GT":    
                compare(model, instruction, "GT")
            elif model.instruction_name == "EQ":    
                compare(model, instruction, "EQ")
            elif model.instruction_name == "AND": 
                logic(model, instruction, "AND")
            elif model.instruction_name == "OR": 
                logic(model, instruction, "OR")
            elif model.instruction_name == "NOT":
                not_ins(model, instruction)
            elif model.instruction_name == "INT2CHAR":
                int2char(model, instruction)
            elif model.instruction_name == "STRI2INT": 
                stri2int(model, instruction)
            elif model.instruction_name == "READ":
                read(model, instruction)
            elif model.instruction_name == "WRITE":
                write(model, instruction)
            elif model.instruction_name == "CONCAT": 
                concat(model, instruction)
            elif model.instruction_name == "STRLEN": 
                strlen(model, instruction)
            elif model.instruction_name == "GETCHAR": 
                getchar(model, instruction)
            elif model.instruction_name == "SETCHAR": 
                setchar(model, instruction)
            elif model.instruction_name == "TYPE":
                get_var_type(model, instruction)   
            elif model.instruction_name == "LABEL": 
                continue
            elif model.instruction_name == "JUMP": 
                jump(model, instruction, code)
            elif model.instruction_name == "JUMPIFEQ": 
                conditional_jump(model, instruction, code, "EQ")
            elif model.instruction_name == "JUMPIFNEQ": 
                conditional_jump(model, instruction, code, "NEQ")
            elif model.instruction_name == "EXIT": 
                exit_it(model, instruction)
            elif model.instruction_name == "BREAK": 
                break_debug(model, instruction)
            elif model.instruction_name == "DPRINT": 
                dprint(model, instruction)  
            elif model.instruction_name == "CLEARS": 
                model.clears()
            elif model.instruction_name == "ADDS":
                model.stack_arith("+")
            elif model.instruction_name == "SUBS": 
                model.stack_arith("-")
            elif model.instruction_name == "MULS": 
                model.stack_arith("*")
            elif model.instruction_name == "IDIVS": 
                model.stack_arith("/")
            elif model.instruction_name == "LTS": 
                model.stack_comp("<")
            elif model.instruction_name == "GTS": 
                model.stack_comp(">")
            elif model.instruction_name == "EQS": 
                model.stack_comp("=")
            elif model.instruction_name == "ANDS": 
                model.stack_logic("A")
            elif model.instruction_name == "ORS": 
                model.stack_logic("O")
            elif model.instruction_name == "NOTS": 
                model.neg()
            elif model.instruction_name == "INT2CHARS": 
                model.i2ch()
            elif model.instruction_name == "STRI2INTS":
                model.s2i()
            elif model.instruction_name == "JUMPIFEQS":
                stack_jump(model, instruction, "EQ", code)
            elif model.instruction_name == "JUMPIFNEQS":
                stack_jump(model, instruction, "NEQ", code)
            else:
                sys.exit(Exit_Codes.SEMANTIC_ERR)
        else:
            sys.exit(Exit_Codes.OK)


def check_xml_args(arguments, inst_type):
    """Function controls validity of instruction arguments len"""
    if inst_type == 0:
        if len(arguments) == 0:
            return
    elif inst_type == 1:
        if len(arguments) == 1:
            return
    elif inst_type == 2:
        if len(arguments) == 2:
            return
    elif inst_type == 3:
        if len(arguments) == 3:
            return
    elif inst_type == 4:
        if len(arguments) == 3:
            return
    elif inst_type == 5:
        if len(arguments) == 1:
            return
    elif inst_type == 6:
        if len(arguments) == 1:
            return
    sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
        

def check_code(code, args):
    """ Function checks if the data from XML file are valid(order,opcode...)"""
    # 0 - empty, 1 - label, 2 - var,symb, 3 - label,symb,symb, 4 - var,symb,symb, 5 - symb , 6 - var type
    inst_list = [["CREATEFRAME", "PUSHFRAME","POPFRAME","RETURN","BREAK","CLEARS",
    "ADDS","SUBS","MULS","IDIVS","LTS","GTS","EQS","ANDS","ORS","NOTS",
    "INT2CHARS","STRI2INTS"],["CALL","JUMP","LABEL","JUMPIFEQS","JUMPIFNEQS"],
    ["MOVE","INT2CHAR","STRLEN","TYPE","NOT","READ"], ["JUMPIFEQ","JUMPIFNEQ"],
    ["ADD","SUB","MUL","IDIV","LT","GT","EQ","AND","OR","STRI2INT","CONCAT","GETCHAR","SETCHAR"],
    ["PUSHS","WRITE","EXIT","DPRINT"],["DEFVAR","POPS"]]
    order_check = 0
    for instruct in code.list:
        founded = False
        if instruct.order > order_check:
            order_check = instruct.order
            for inst_type in range(len(inst_list)):
                if instruct.opcode in inst_list[inst_type]:
                    valid_args = check_xml_args(instruct.arguments, inst_type)
                    instruct.add_type(inst_type)
                    founded = True
            if not founded:
                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
        else:
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
    start_interpretation(code, args)      


def init():
    """Function parsing command line arguments"""
    
    if(len(sys.argv) < 2):
        sys.exit(Exit_Codes.INVALID_COMMAND_LINE_ERR)

    argument_parser = argparse.ArgumentParser(add_help=False)
    argument_parser.add_argument("--help", action='store_true')
    argument_parser.add_argument("--source", type=str, nargs='?')
    argument_parser.add_argument("--input", type=str, nargs='?')
    arguments, unknown_args = argument_parser.parse_known_args()
    if unknown_args:
        sys.exit(Exit_Codes.INVALID_COMMAND_LINE_ERR)
    checked_args = check_arguments(arguments)
    init_sources(checked_args)


def print_help():
    """Function prints help message"""
    print("IPP 2020/2021 - interpret.py")
    print("Author: Samuel Valaštín")
    print("Usage: python3.8 interpret.py [--help] [--source=FILE] [--input=FILE]")
    sys.exit(Exit_Codes.OK)


def check_arguments(arguments):
    """
    Function checks and loads command line arguments
    returns valid arguments dictionary
    """
    if arguments.help is True:
        if arguments.source is None and arguments.input is None:
            print_help()
        else:
            sys.exit(Exit_Codes.INVALID_COMMAND_LINE_ERR)

    arguments_dict = dict()
    if arguments.input is not None and arguments.source is not None:
        if os.path.isfile(arguments.input) and os.path.isfile(arguments.source):
            arguments_dict["source"] = arguments.source
            try:
                file_lines = []
                file = open(arguments.input)
                for line in file:
                    file_lines.append(line.rstrip("\n"))
            except:
                sys.exit(Exit_Codes.INVALID_INPUT_FILE_ERR)    
            arguments_dict["input"] = file_lines
            
        else:
            sys.exit(Exit_Codes.INVALID_COMMAND_LINE_ERR)
    elif arguments.input is None and arguments.source is None:
        sys.exit(Exit_Codes.INVALID_COMMAND_LINE_ERR)
    elif arguments.input is None:
        if os.path.isfile(arguments.source):
            arguments_dict["source"] = arguments.source    
        try:
            file_lines = []
            if not sys.stdin.isatty():
                for line in sys.stdin:
                    file_lines.append(line.rstrip("\n"))
        except:
            sys.exit(Exit_Codes.INVALID_INPUT_FILE_ERR)
        arguments_dict["input"] = file_lines
        
    elif arguments.source is None:
        if os.path.isfile(arguments.input):
            arguments_dict["input"] = arguments.input
            try:
                file = open(arguments.input,"r")
                file_lines = []
                for line in file:
                    file_lines.append(line.rstrip("\n"))
            except:
                sys.exit(Exit_Codes.INVALID_INPUT_FILE_ERR)
        arguments_dict["input"] = file_lines
        arguments_dict["source"] = sys.stdin
    return arguments_dict


def check_root(root):
    """Function checks if the XML root is valid"""
    if root is not None:
        if root.tag == "program":
            if root.attrib["language"] is not None:
                if root.attrib["language"] == "IPPcode21":
                    return
    sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)


def init_sources(args):
    """
    Function converts XML to code class and also perform lexical and syntax
    analysis of XML representation
    """
    try:
        root = ElementTree.parse(args["source"]).getroot()
    except:
        sys.exit(Exit_Codes.INVALID_XML_FORMAT_ERR)
    check_root(root)
    known_types = ["int", "string", "bool", "nil", "label", "type", "var"]
    store_code = Code_to_interpret()
    try:
        root[:] = sorted(root, key=lambda root:int(root.attrib["order"]))
    except:
        sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
    for line in root:
        if len(line.attrib) == 2 and "order" in line.attrib and "opcode" in line.attrib and "instruction" in line.tag:
            try:
                order = int(line.attrib["order"])
                instruction = line.attrib["opcode"].upper()
            except Exception:
                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
            store_inst = Instruction(order, instruction)
            try:
                line[:] = sorted(line, key = lambda argument:argument.tag) # sorting arguments by tag
            except:
                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
        else: 
            sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
        counter = 1    
        for argument in line:
            if re.match("^arg[1|2|3]$", argument.tag):
                pos = re.split("arg", argument.tag)[1]  
                if int(pos) == counter:
                    counter = counter + 1
                else:
                    sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)   
                if "type" in argument.attrib:
                    #known_types = ["int","string","bool","nil","label","symb","type","var"]
                    if argument.attrib["type"] in known_types:
                       
                        if argument.attrib["type"] == "string":
                            if argument.text is not None:
                                if re.match("^([^\#|^\s|^\\\\]|([\\\\][0-1][0-9][0-9]))*$", argument.text):
                                    if re.search("([\\\\][0-1][0-9][0-9])", argument.text):
                                        # find all escape sequencies by using set -> ignore duplicates
                                        escape_sequencies = set(re.findall("([\\\\][0-1][0-9][0-9])", argument.text)) 
                                        for seq in escape_sequencies:
                                            number_seq = re.sub("[\\\\]", "", seq)
                                            replacement = chr(int(number_seq))
                                            argument.text = argument.text.replace(seq, replacement)
                                    store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                    store_inst.add_argument(store_arg)
                                else:
                                    sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                            else:
                                store_arg = Argument(argument.attrib["type"], int(pos), None)
                                store_inst.add_argument(store_arg)
                        elif argument.attrib["type"] == "bool":
                            if re.match("^(true|false)$", argument.text):
                                store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                store_inst.add_argument(store_arg)
                            else:
                                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                        elif argument.attrib["type"] == "nil":
                            if re.match("^(nil)$", argument.text):
                                store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                store_inst.add_argument(store_arg)
                            else:
                                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                        elif argument.attrib["type"] == "label":
                            if re.match("^[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$", argument.text):
                                store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                store_inst.add_argument(store_arg)
                            else:
                                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                        elif argument.attrib["type"] == "type":
                            if re.match("^(int|bool|string)$", argument.text):
                                store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                store_inst.add_argument(store_arg)
                            else:
                                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                                
                        elif argument.attrib["type"] == "var":
                            if re.match("(^[T|L|G]{1}[F]{1}[@]{1}[a-zA-Z_\-$&%*!?][a-zA-Z0-9_\-$&%*!?]{0,}$)", argument.text):
                                store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                                store_inst.add_argument(store_arg)
                            else:
                                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                        else:
                            store_arg = Argument(argument.attrib["type"], int(pos), argument.text)
                            store_inst.add_argument(store_arg)
                    else:
                        sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
                else:
                    sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
            else:
                sys.exit(Exit_Codes.INVALID_XML_STRUCT_ERR)
    
        store_code.add_instruction(store_inst)
    store_code.add_labels()
    check_code(store_code,args)


    
init()
