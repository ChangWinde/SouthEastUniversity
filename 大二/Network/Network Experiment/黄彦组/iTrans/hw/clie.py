from client import client
import signal
from base import *
from face import iTransFace
from PyQt4 import QtGui

# conn = True

# def close():
    # conn = False

host = ('localhost',8003)
# signal.signal(signal.SIGINT, close)


if __name__ == '__main__':
    app = QtGui.QApplication([])
    face = iTransFace()
    face.show()
    app.exec_()
