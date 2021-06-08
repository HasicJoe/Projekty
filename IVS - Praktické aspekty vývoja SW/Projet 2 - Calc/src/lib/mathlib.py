"""@brief Math library and token parsing 
   
   @author IVS-DREAM-TEAM
   
   @file mathlib.py
"""

import random 

class SymbolType():
    NUMBER =        0x0B
    COMMA =         0x11
    
    PLUS =          0x00
    MINUS =         0x01
    MULTIPLY =      0x02
    DIVIDE =        0x03
    MODULO =        0x04
    EXP =           0x05
    
    LEFT_BRACKET =  0x09
    RIGHT_BRACKET = 0x0A

    RANDOM =        0x08
    FACTORIAL =     0x07
    ROOT =          0x06    

# list of entered symbols
SymbolList = []
class Symbol():
    def __init__(self, type, value, display):
        self.type = type
        self.value = value
        self.display = display
        SymbolList.append(self)

# types of tokens to provide execution
class TokenType():
    NUMBER =                0x0B
    # operations
    OPERATION_ADD =         0x00
    OPERATION_SUBSTRACT =   0x01
    OPERATION_MULTIPLY =    0x02
    OPERATION_DIVIDE =      0x03
    OPERATION_MODULO =      0x04
    OPERATION_EXP =         0x05
    # brackets
    LEFT_BRACKET =          0x09
    RIGHT_BRACKET =         0x0A
    # functions
    FUNCTION_RAND =         0x08
    FUNCTION_FACTORIAL =    0x07
    FUNCTION_ROOT =         0x06

    # for syntax analysis
    DOLLAR =                0x0C
    NON_TERMINAL =          0x51
    SHIFT =                 0x52

# list of tokens to be computed
class Token():
    #constructor
    def __init__(self, type, value):
        self.type = type
        self.value = value

# translate symbols into tokens
# handle unary minus in token conversion to prove pista, that it wont actually break anything
def PopulateTokens(eventhandler):
    indexlist = []
    result = []
    i = 0
    numBuff = ''
    while i < len(SymbolList):
        # switching to number token
        if SymbolList[i].type in [SymbolType.NUMBER, SymbolType.COMMA]:
            # there is minus before the number
            if i > 0 and SymbolList[i-1].type == SymbolType.MINUS:
                # add minus as unary minus operation (include in token)
                if i == 1 or (i > 1 and SymbolList[i-2].type in [
                    SymbolType.LEFT_BRACKET,
                    SymbolType.MINUS, 
                    SymbolType.PLUS, 
                    SymbolType.MULTIPLY, 
                    SymbolType.DIVIDE,
                    SymbolType.MODULO]):
                    numBuff = '-'
                    result.pop()
            while i < len(SymbolList) and SymbolList[i].type in [SymbolType.NUMBER, SymbolType.COMMA]:
                numBuff += str(SymbolList[i].value)
                i+=1
            try:
                result.append(Token(TokenType.NUMBER, float(numBuff)))
            except ValueError:
                eventhandler.window.label.setText('SYNTAX ERROR')
                return
            i-=1
            numBuff = ''
        else:
            result.append(Token(SymbolList[i].type, SymbolList[i].value))
        i += 1
    return result

def Compute(eventhandler):
    if SymbolList:
        PSA(PopulateTokens(eventhandler), eventhandler)

PrecedenceSyntaxTable = {         #   +    -    *     /   %    ^    √    !    R    (    )    i    $
    TokenType.OPERATION_ADD :       [">", ">", "<", "<", "<", "<", "<", "<", "<", "<", ">", "<", ">"],
    TokenType.OPERATION_SUBSTRACT : [">", ">", "<", "<", "<", "<", "<", "<", "<", "<", ">", "<", ">"],
    TokenType.OPERATION_MULTIPLY :  [">", ">", ">", ">", ">", "<", "<", "<", "<", "<", ">", "<", ">"],
    TokenType.OPERATION_DIVIDE :    [">", ">", ">", ">", ">", "<", "<", "<", "<", "<", ">", "<", ">"],
    TokenType.OPERATION_MODULO :    [">", ">", ">", ">", ">", "<", "<", "<", "<", "<", ">", "<", ">"],
    TokenType.OPERATION_EXP :       [">", ">", ">", ">", ">", "<", "<", "<", "<", "<", ">", "<", ">"],

    TokenType.FUNCTION_ROOT :       [">", ">", ">", ">", ">", ">", ">", "<", "<", "<", ">", "<", ">"],
    TokenType.FUNCTION_FACTORIAL :  [">", ">", ">", ">", ">", ">", ">", ">", ">", "<", ">", "<", ">"],
    TokenType.FUNCTION_RAND :       [">", ">", ">", ">", ">", ">", ">", ">", ">", "<", ">", "<", ">"],

    TokenType.LEFT_BRACKET :        ["<", "<", "<", "<", "<", "<", "<", "<", "<", "<", "=", "<", "S"],
    TokenType.RIGHT_BRACKET :       [">", ">", ">", ">", ">", ">", ">", ">", ">", "S", ">", ">", ">"],
    
    TokenType.NUMBER :              [">", ">", ">", ">", ">", ">", ">", ">", ">", "S", ">", "S", ">"],
    
    TokenType.DOLLAR :              ["<", "<", "<", "<", "<", "<", "<", "<", "<", "<", "S", "<", "S"]
}

def toNonTerminal(tokenlist):
    return Token(TokenType.NON_TERMINAL, tokenlist[0].value)

def removeBrackets(tokenlist):
    return Token(TokenType.NON_TERMINAL, tokenlist[1].value)

def addTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, add(tokenlist[0].value, tokenlist[2].value))

def substractTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, sub(tokenlist[0].value, tokenlist[2].value))

def multiplyTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, mul(tokenlist[0].value, tokenlist[2].value))

def divideTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, div(tokenlist[0].value, tokenlist[2].value))

def moduloTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, mod(tokenlist[0].value, tokenlist[2].value))

def expTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, exp(tokenlist[0].value, tokenlist[2].value))

def factToken(tokenlist):
    return Token(TokenType.NON_TERMINAL, fact(tokenlist[0].value))

def randToken(tokenlist):
    return Token(TokenType.NON_TERMINAL, rand(tokenlist[1].value))

def rootTokens(tokenlist):
    return Token(TokenType.NON_TERMINAL, root(tokenlist[0].value, tokenlist[2].value))

Grammar = {
    (TokenType.NUMBER,) :                                                               toNonTerminal,
    (TokenType.LEFT_BRACKET, TokenType.NON_TERMINAL, TokenType.RIGHT_BRACKET,) :        removeBrackets,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_ADD, TokenType.NON_TERMINAL,) :        addTokens,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_SUBSTRACT, TokenType.NON_TERMINAL,) :  substractTokens,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_MULTIPLY, TokenType.NON_TERMINAL,) :   multiplyTokens,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_DIVIDE, TokenType.NON_TERMINAL,) :     divideTokens,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_MODULO, TokenType.NON_TERMINAL,) :     moduloTokens,
    (TokenType.NON_TERMINAL, TokenType.OPERATION_EXP, TokenType.NON_TERMINAL,) :        expTokens,
    (TokenType.NON_TERMINAL, TokenType.FUNCTION_FACTORIAL,) :                           factToken,
    (TokenType.FUNCTION_RAND, TokenType.NON_TERMINAL,) :                                randToken,
    (TokenType.NON_TERMINAL, TokenType.FUNCTION_ROOT, TokenType.NON_TERMINAL,) :        rootTokens,
}

# Precedence Syntax Analysis
def PSA(tokens, eventhandler): 
    # error occured while translating symbols
    if not tokens:
        return
    # temporary list for reduce operation
    templist = []    
    # add a '$' to the end of input
    tokens.append(Token(TokenType.DOLLAR, '$'))
    # stack holding the expression evaluation, add '$' at the bottom of the stack
    Stack = [Token(TokenType.DOLLAR, '$')]
    while(True):
        # ending condition (successfull evaluation)
        if len(tokens) == 1 and tokens[0].type == TokenType.DOLLAR and len(Stack) == 2 and Stack[1].type == TokenType.NON_TERMINAL:
            SymbolList.clear()
            # round the result for 8 decimal places
            result = round(Stack.pop().value, 8)
            for sym in list(str(result)):
                if sym == '.' or sym == ',':
                    Symbol(SymbolType.COMMA, '.',',')
                else:
                    Symbol(SymbolType.NUMBER, sym, sym)
            eventhandler.UpdateDisplay()
            return
        try:
            for StackTok in Stack[::-1]:
                if StackTok.type != TokenType.NON_TERMINAL and StackTok.type != TokenType.SHIFT:
                    break
            operation = PrecedenceSyntaxTable[StackTok.type][tokens[0].type]
        except KeyError:
            eventhandler.window.label.setText('SYNTAX ERROR')
            return
        # operation shift
        if operation == "<":
            if Stack[-1].type == TokenType.NON_TERMINAL:
                temp = Stack.pop()
                Stack.append(Token(TokenType.SHIFT, '<'))
                Stack.append(temp)
            else:
                Stack.append(Token(TokenType.SHIFT, '<'))
            Stack.append(tokens.pop(0))
        # operation reduce
        elif operation == ">" or operation == "=":
            if operation == "=":
                Stack.append(tokens.pop(0))
            templist.clear()
            while Stack[-1].type != TokenType.SHIFT:
                templist.append(Stack.pop())
            try:
                templist = templist[::-1]
                action = Grammar[tuple([tok.type for tok in templist])]
            except KeyError:
                eventhandler.window.label.setText('SYNTAX ERROR')
                return
            else:          
                if Stack[-1].type == TokenType.SHIFT:
                    Stack.pop()
                try:
                    Stack.append(action(templist))
                except:
                    eventhandler.window.label.setText('MATH ERROR')
                    return
        # syntax err
        else:
            eventhandler.window.label.setText('SYNTAX ERROR')
            return


def add(a, b):
    """Function for + operation

    Args:
        a (float): first argument
        b (float): second argument

    Returns:
        (float): result of a + b
    """    
    return a + b

def sub(a, b):
    """Function for - operation

    Args:
        a (float): first argument
        b (float): second argument

    Returns:
        (float): result of a - b
    """    
    return a - b

def mul(a, b):
    """Function for * operation

    Args:
        a (float): first argument
        b (float): second argument

    Returns:
        (float): result of a * b
    """    
    return a * b

def div(a, b):
    """Function for / operation

    Args:
        a (float): first argument
        b (float): second argument

    Returns:
        (float): result of a / b
    """    
    return a / b

def exp(a, b):
    """Function for ^ operation

    Args:
        a (float): first argument
        b (float): second argument

    Returns:
        (float): result of a ^ b
    """    
    return a ** b

def root(a, b):
    """Function for calculating n-th root

    Args:
        a (float): number of power of root
        b (float): number from which the root is calculated

    Raises:
        ValueError: if root sould be complex number
        ValueError: if power of root == 0

    Returns:
        (float): result of a-th root of b
    """    
    if a % 2 == 0 and b < 0:
        raise ValueError
    if a == 0:
       raise ValueError
    if a % 2 == 1 and b < 0:
        return -(abs(b) ** (1/float(a)))
    return b ** (1/float(a))

def rand(n):
    """function for returning random number

    Args:
        n (float): range of random number (0 - n)

    Returns:
        (float): random number
    """    
    return random.random()*n

def arith_average(sum,list):
    """function for calculating of arithmetical average from list

    Args:
        sum (float): sum of all elements of list
        list (list): list of elements

    Returns:
        (float): arithmetical average of elements of list
    """    
    return sum / len(list)

def list_len(list):
    """function for calculating length of list

    Args:
        list (list): list which length is returned

    Returns:
        (int): number of elements of list
    """    
    return len(list)

def mod(a,b):
    """function for calculating modulo

    Args:
        a (int): divided number
        b (int): number to divide argumet a b

    Raises:
        TypeError: checks if argument is type int

    Returns:
        (int): result of operation a % b
    """    
    if int(a) != a or int(b) != b:
        raise TypeError
    return a % b

def fact(n):
    """function for calculating factorial of given number

    Args:
        n (int): number of which factorial is calculated

    Raises:
        TypeError: checks if parameter N is of type int
        ValueError: checks if parameter N is > than 0

    Returns:
        (int): n!
    """    
    if n != int(n):
        raise TypeError
    n = int(n)
    if n < 0:
        raise ValueError
    result = 1
    for i in range(n):
        result *= (i+1)
    return result