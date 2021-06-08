
"""@brief Main for start of aplication
   
   @author IVS-DREAM-TEAM
   
   @file main.py
"""

import sys
import lib.eventhandler as eh
from PyQt5 import QtWidgets, QtCore, uic
from PyQt5.QtCore import Qt
from gui.gui import Ui_MainWindow


class MainWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self, *args, obj=None, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.setupUi(self)

def main():   
    """
    Aplication run
    """    
    app = QtWidgets.QApplication(sys.argv)
    window = MainWindow()
    window.show()
    # bind event handler to the main window
    eh.EventHandler(window)
    app.exec()

if __name__ == "__main__": 
    main()
    sys.exit(0)