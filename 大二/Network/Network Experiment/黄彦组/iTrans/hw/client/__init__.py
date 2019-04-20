# -*- coding: utf-8 -*-
import socket
import time
import threading
import cPickle as pickle
from base import sockreq
import signal

# import base

class client:
    def __init__(self, host):
        self.buff = None
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect(host)
        signal.signal(signal.SIGINT, self.logout)
    
    def __del__(self):
        self.logout()
        
    def recv(self):
        self.buff = pickle.loads(self.sock.recv(1024))
        return self.buff

    def sendcommand(self, command):
        cin = command.split(':')
        if len(cin) > 1:
            type, mssg = cin[0], cin[1].replace(' ', '')
            if type == 'push':
                filename = 'local/' + mssg
                content = open(filename, 'r').read()
                mssg = [filename, content]
            req = sockreq(type, mssg)
            self.sock.send(pickle.dumps(req))
        else:
            mssg = cin[0].replace(' ', '')
            req = sockreq('', mssg)
            self.sock.send(pickle.dumps(req))
    
    def savelocal(self, filename, content):
        savefile = open('local/' + filename, 'w+')
        for line in content:
            savefile.write(line.encode('utf-8'))
        savefile.close()
        print 'pull file successfully!'


    def handle(self):
        text = ''
        # startl = '--------*--------req--------*--------\n'
        buff = self.recv()
        if isinstance(buff, str):
            text = buff
        elif isinstance(buff, list):
            # self.savelocal(buff[0], buff[1])
            # text = 'zhidao:' + buff + ':\n'
            text += buff[0] + '\n'
            for each in buff[1:]: text += each
        elif isinstance(buff, tuple):
            text = 'list files:\n'
            for each in buff:
                text += each + '\n'
        # endl = '--------*--------end--------*--------\n'
        # text = startl + text + endl
        return text


    def logout(self):
        self.send('logout')
        self.sock.close()

