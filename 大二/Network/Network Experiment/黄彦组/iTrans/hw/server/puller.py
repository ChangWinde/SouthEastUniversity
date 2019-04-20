# -*- coding: utf-8 -*-
import os

def load(filename):
    loadfile = open(filename, 'r')
    content = loadfile.read()
    loadfile.close()
    return [filename, content]

def listall():
    return tuple(os.listdir('pull'))

class pullreq:
    def __init__(self, filename, content=None):
        self.filename = filename
        if content != None:
            self.content = content

    def download(self):
        try:
            filename = 'pull/' + self.filename
            if os.path.exists(filename):
                return load(filename)
            print 'error raised while finding the file'
        except Exception, e:
            print 'err: download file', e
        return 'no such file'

    def savelocal(self):
        try:
            filename = self.filename.split('/')
            if len(filename) > 1:
                filename = 'pull/' + filename[1]
            else:
                filename = 'pull/' + filename[0]
            if filename == '' or filename == None:
                return 'save failed!'
            savefile = open(filename, 'w+')        
            content = self.content
            savefile.write(content)
            savefile.close()
        except Exception, e:
            print 'err: save local file', e
            return 'save failed'
        return 'save successfully!'
