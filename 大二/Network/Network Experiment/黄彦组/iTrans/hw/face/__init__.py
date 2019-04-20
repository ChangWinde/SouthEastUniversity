# -*- coding: utf-8 -*-

from PyQt4 import QtGui
from PyQt4 import QtCore

from client import client
# import signal
from base import *
import os

host = ('localhost',8003)
# signal.signal(signal.SIGINT, close)

class iTransFace(QtGui.QWidget):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.initUI()
        self.clie = client(host)

    def send(self):
        command = str(self.cmd.text())
        self.clie.sendcommand(command)
        text = self.clie.handle()
        self.recvmsg.setText(text)


    def initUI(self):
        self.resize(500, 350)
        path = os.path.join(os.path.dirname(__file__),'style.qss').replace('\\','/')
        # print path
        css = QtCore.QFile(path)
        css.open(QtCore.QIODevice.ReadOnly)
        if css.isOpen():
            self.setStyleSheet(QtCore.QVariant(css.readAll()).toString())
        css.close()
        self.setWindowTitle('iTrans')
        
        self.sendbtn = QtGui.QPushButton('send', self)
        self.recvmsg = QtGui.QTextEdit(self)
        self.cmd = QtGui.QLineEdit(self)
        self.sendlb = QtGui.QLabel('Command:', self)
        self.recvlb = QtGui.QLabel('SystemRes:', self)

        self.recvlb.setGeometry(40, 50, 100, 30)
        self.sendlb.setGeometry(40, 230, 100, 30)
        self.recvmsg.setGeometry(140, 50, 320, 160)
        self.cmd.setGeometry(140, 230, 320, 30)
        self.sendbtn.setGeometry(230, 280, 130, 30)

        self.connect(self.sendbtn, QtCore.SIGNAL('clicked()'), self.send)





    

