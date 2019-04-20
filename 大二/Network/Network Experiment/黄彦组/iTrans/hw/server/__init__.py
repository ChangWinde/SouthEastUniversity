# -*- coding: utf-8 -*-
import socket
import threading
import crawler
import puller
from base import sockreq
import cPickle as pickle
import traceback

class server:
    servers = {}
    def __init__(self, host):
        self.host = host
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.bind(host)
        self.sock.listen(5)

    def start(self):
        while True:
            print 'accept waiting...'
            conn, address = self.sock.accept()
            print 'accepted!'
            conn.settimeout(None)
            self.connect_client(conn, address)

    def connect_client(self, conn, address):
        client_thread = single_server(conn)
        server.servers[conn] = address
        client_thread.start()


class single_server(threading.Thread):
    lock = threading.Lock()
    def __init__(self, conn):
        threading.Thread.__init__(self)
        self.conn = conn

    def run(self):
        try:
            while True:
                self.handle()
        except Exception, e:
            traceback.print_exc()

    def send(self, msg):
        self.conn.send(pickle.dumps(msg))

    def recv(self):
        return pickle.loads(self.conn.recv(1024))

    def handle(self):
        recvmsg = self.recv()
        if(isinstance(recvmsg, sockreq)):
            print recvmsg.content
            if recvmsg.type == 'zhidao':
                print 'search in baidu zhidao'
                spider = crawler.baiduzd()
                spider.search(recvmsg.content)
                self.send(spider.info())
                spider.save()
            elif recvmsg.type == 'pull':
                print 'pull file'
                pull = puller.pullreq(recvmsg.content)
                self.send(pull.download())
            elif recvmsg.type == 'ls':
                print 'list files'
                lists = puller.listall()
                self.send(lists)
            elif recvmsg.type == 'push':
                print 'push file'
                pull = puller.pullreq(recvmsg.content[0], recvmsg.content[1])
                res = pull.savelocal()
                self.send(res)
            elif  recvmsg.type == '':
                print 'undefined message type'
                msg  = "hello...O_o...\n"
                msg += "you should type command I could recogonize!\n"
                msg += "like this format: ('#' is comment)\n"
                msg += "ls: # for list all files in download directory.\n"
                msg += "zhidao: *** # search sth in baidu zhidao.\n"
                msg += "pull: *** # download file from server.\n"
                self.send(msg)
            else:
                print 'invalid message'
                msg = "I don't know, heiheihei..."
                self.send(msg)
        else:
            msg = "----------------------------------------------\n"+\
            "Analyse failed!\nSend request structure please\n"+\
            "----------------------------------------------\n"
            self.send(msg)

