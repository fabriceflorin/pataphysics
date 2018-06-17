#!/usr/bin/python

from Tkinter import *

class Pane:
    def __init__(self, master):
        self.master = master
        master.title("A simple GUI")

        w = Canvas(master, width=800, height=480,
            background='blue')
        w.pack(padx=10,pady=10)

        button = Button(master, text="Greet", command=self.greet)
        button.pack()

        hbar = Scrollbar(master,orient=HORIZONTAL)
        hbar.pack(side=BOTTOM,fill=X)

        hbar.config(command=w.xview)
        w.config(xscrollcommand=hbar.set)

    def greet(self):
        print("Greetings!")

root = Tk()
my_gui = Pane(root)
root.mainloop()
