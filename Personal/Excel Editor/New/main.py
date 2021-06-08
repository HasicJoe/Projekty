import tkinter as tk
from tkinter import ttk
from tkinter.ttk import Progressbar,Style
from tkinter import filedialog as fd
from modules.parser import Parser


class Window():
    def __init__(self,root, bar, label):
        self.root = root
        self.bar = bar
        self.label = label
        
    def update_label(self,new_label):
        self.label = new_label
        

def init_bar(root):
    
    """ Initialisation of progress bar """
    
    bar = Progressbar(root, length=200, style='black.Horizontal.TProgressbar')
    bar.grid(column=1, row=2)
    bar['value'] = 0
    return bar


def init_label(root,ex):
    
    """ Label setup with error handling display """
    
    if ex == 0:
        label = ttk.Label(root, text = '© Valaštín 2021', background='#303030', foreground='#f1f1f1')
    elif ex == 1:
        label = ttk.Label(root, text = 'Invalid source data', background='#303030', foreground='#f1f1f1')
    else:
        ttk.Label(root, text = 'Internal error', background='#303030', foreground='#f1f1f1')
        
    label.grid(column = 1, row = 3, pady = 20)
    return label
    
    
def open_excel_file(window):
    
    """ Button handling """
    
    filetypes = (('Excel súbory', '*.xlsx'),('Všetky súbory', '*.*'))
    f = fd.askopenfile(filetypes=filetypes)
    parser = Parser(f, window)
    if parser.rv != 0:
        window.update_label(init_label(window.root, parser.rv))

def main():
    
    """ Main window setup """
    root = tk.Tk()
    root.configure(background='#303030')
    root.title('Nahrať štatistiky')
    root.geometry('720x500')
    
    root.update()
    style = Style()
    style.configure('W.TButton', font = ('calibri', 11,), foreground = '#303030')

    window = Window(root, init_bar(root), init_label(root,0))
    open_button = ttk.Button(root, text='Vybrať súbor', style = 'W.TButton', command = lambda: open_excel_file(window))
    open_button.grid(column=1, row=1, sticky='', padx=300, pady=100)

    root.mainloop()
    
    
if __name__ == '__main__':
    main() 