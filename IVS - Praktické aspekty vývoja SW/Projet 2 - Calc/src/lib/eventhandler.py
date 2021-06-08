from PyQt5 import QtWidgets, uic
from PyQt5.QtCore import Qt
import PyQt5.QtGui as gui
import lib.mathlib as math
import webbrowser

class EventHandler():
    # bind mouse and keyboard events to the main window 
    def __init__(self, window):
        self.window = window
        self.enteredList = []
        # keyboard input handler
        self.window.keyPressEvent = self.keyboardEventHandler

        # mouse input handler (left click)
        self.window.key_0.clicked.connect(self.addNumberZero)
        self.window.key_1.clicked.connect(self.addNumberOne)
        self.window.key_2.clicked.connect(self.addNumberTwo)
        self.window.key_3.clicked.connect(self.addNumberThree)
        self.window.key_4.clicked.connect(self.addNumberFour)
        self.window.key_5.clicked.connect(self.addNumberFive)
        self.window.key_6.clicked.connect(self.addNumberSix)
        self.window.key_7.clicked.connect(self.addNumberSeven)
        self.window.key_8.clicked.connect(self.addNumberEight)
        self.window.key_9.clicked.connect(self.addNumberNine)
        self.window.key_point.clicked.connect(self.addDecimalPoint)
        
        # operators
        self.window.key_add.clicked.connect(self.addPlus)
        self.window.key_sub.clicked.connect(self.addMinus)
        self.window.key_mul.clicked.connect(self.addMultiply)
        self.window.key_div.clicked.connect(self.addDivide)
        self.window.key_mod.clicked.connect(self.addModulo)
        
        # math functions
        self.window.key_exp.clicked.connect(self.addExp)
        self.window.key_root.clicked.connect(self.addNthRoot)
        self.window.key_fact.clicked.connect(self.addFactorial)
        self.window.key_rand.clicked.connect(self.addRandom)
        
        # brackets
        self.window.key_lb.clicked.connect(self.addLeftBracket)
        self.window.key_rb.clicked.connect(self.addRightBracket)
        
        # command buttons
        self.window.key_eq.clicked.connect(self.Execute)
        self.window.key_c.clicked.connect(self.Clear)
        self.window.key_help.clicked.connect(self.Help)
        

    # add Number symbols to stack
    def addNumberZero(self):
        math.Symbol(math.SymbolType.NUMBER, 0, '0')
        self.UpdateDisplay()

    def addNumberOne(self):
        math.Symbol(math.SymbolType.NUMBER, 1, '1')
        self.UpdateDisplay()

    def addNumberTwo(self):
        math.Symbol(math.SymbolType.NUMBER, 2, '2')
        self.UpdateDisplay()

    def addNumberThree(self):
        math.Symbol(math.SymbolType.NUMBER, 3, '3')
        self.UpdateDisplay()

    def addNumberFour(self):
        math.Symbol(math.SymbolType.NUMBER, 4, '4')
        self.UpdateDisplay()

    def addNumberFive(self):
        math.Symbol(math.SymbolType.NUMBER, 5, '5')
        self.UpdateDisplay()

    def addNumberSix(self):
        math.Symbol(math.SymbolType.NUMBER, 6, '6')
        self.UpdateDisplay()

    def addNumberSeven(self):
        math.Symbol(math.SymbolType.NUMBER, 7, '7')
        self.UpdateDisplay()

    def addNumberEight(self):
        math.Symbol(math.SymbolType.NUMBER, 8, '8')
        self.UpdateDisplay()

    def addNumberNine(self):
        math.Symbol(math.SymbolType.NUMBER, 9, '9')
        self.UpdateDisplay()

    def addDecimalPoint(self):
        math.Symbol(math.SymbolType.COMMA, '.', ',')
        self.UpdateDisplay()

    # add math operators to the stack
    def addPlus(self):
        math.Symbol(math.SymbolType.PLUS,'+', '+')
        self.UpdateDisplay()

    def addMinus(self):
        math.Symbol(math.SymbolType.MINUS,'-', '-')
        self.UpdateDisplay()

    def addMultiply(self):
        math.Symbol(math.SymbolType.MULTIPLY,'*', '*')
        self.UpdateDisplay()

    def addDivide(self):
        math.Symbol(math.SymbolType.DIVIDE,'/', '/')
        self.UpdateDisplay()

    def addModulo(self):
        math.Symbol(math.SymbolType.MODULO,'%', 'mod')
        self.UpdateDisplay()

    # add math Functions to the stack
    def addExp(self):
        math.Symbol(math.SymbolType.EXP,0, '^')
        math.Symbol(math.SymbolType.LEFT_BRACKET, '(', '(')
        self.UpdateDisplay()

    def addNthRoot(self):
        math.Symbol(math.SymbolType.ROOT,0, 'ˣ√')
        math.Symbol(math.SymbolType.LEFT_BRACKET, '(', '(')
        self.UpdateDisplay()

    def addFactorial(self):
        math.Symbol(math.SymbolType.FACTORIAL,0, '!')
        self.UpdateDisplay()

    def addRandom(self):
        math.Symbol(math.SymbolType.RANDOM,0, 'rand')
        math.Symbol(math.SymbolType.LEFT_BRACKET, '(', '(')
        self.UpdateDisplay()

    # add math brackets to the stack
    def addLeftBracket(self):
        math.Symbol(math.SymbolType.LEFT_BRACKET, '(', '(')
        self.UpdateDisplay()

    def addRightBracket(self):
        math.Symbol(math.SymbolType.RIGHT_BRACKET, ')', ')')
        self.UpdateDisplay()

    def Help(self):
        webbrowser.open('http://www.stud.fit.vutbr.cz/~xvalas10/User_manual.pdf')            
        
    def Clear(self):
        math.SymbolList.clear()
        self.window.label.setText('0,000')

    def Execute(self):
        math.Compute(self)
        #self.UpdateDisplay()

    def delChar(self):
        if math.SymbolList:
            math.SymbolList.pop()
        if len(math.SymbolList) > 1:
            if math.SymbolList[-1].type in [math.SymbolType.RANDOM, 
                                            math.SymbolType.ROOT, 
                                            math.SymbolType.EXP]:
                math.SymbolList.pop()
        self.UpdateDisplay()

    # set value from Symbol list to the calc display
    def UpdateDisplay(self):
        self.window.label.setText(''.join([sym.display for sym in math.SymbolList]))

    # key to action binding
    ActionDict = {
        # numbers
        Qt.Key_0 : addNumberZero,
        Qt.Key_1 : addNumberOne,
        Qt.Key_2 : addNumberTwo,
        Qt.Key_3 : addNumberThree,
        Qt.Key_4 : addNumberFour,
        Qt.Key_5 : addNumberFive,
        Qt.Key_6 : addNumberSix,
        Qt.Key_7 : addNumberSeven,
        Qt.Key_8 : addNumberEight,
        Qt.Key_9 : addNumberNine,
        Qt.Key_Period : addDecimalPoint,
        Qt.Key_Comma : addDecimalPoint,

        # operators
        Qt.Key_Plus : addPlus,
        Qt.Key_Minus : addMinus,
        Qt.Key_Asterisk : addMultiply,
        Qt.Key_Slash : addDivide,
        # key_'%' == 37
        37 : addModulo,
        # key '^' == 94
        94 : addExp,

        #  math operations (done with multiCharacterEventHandler)

        # brackets
        # key_'(' == 40
        40 : addLeftBracket,
        # key_')' == 41 
        41 : addRightBracket,

        # commands
        Qt.Key_Escape : Clear,
        Qt.Key_Enter : Execute,
        Qt.Key_Return : Execute,

        # DEL -> backspace
        Qt.Key_Backspace : delChar
    }

    # key handler which perform actions upon entered multiple keys
    MultiCharActionDict = {
        'FACT' : addFactorial,
        'RAND' : addRandom, 
        'MOD'  : addModulo,
        'EXP'  : addExp,
        'ROOT' : addNthRoot,
        'CLEAR': Clear
    }
    def multiCharacterEventHandler(self, event):
        try:
            self.enteredList.append(chr(event.key()))
        except ValueError:
            # key is not from alphabet range
            pass
        else:
            sequence = ''.join(self.enteredList)
            for key, action in EventHandler.MultiCharActionDict.items():
                if key in sequence:
                    action(self)
                    self.enteredList.clear()

    # general keyboard handler
    def keyboardEventHandler(self, event):
        # get action from the action dictionary
        try:
            function = EventHandler.ActionDict[event.key()]
        # handle multi character binds (may be changed)
        except KeyError:      
            self.multiCharacterEventHandler(event)
        # perform the action
        else:
            function(self)

