# -*- coding: utf-8 -*-
import urllib
from bs4 import BeautifulSoup
import os
from puller import load

class baiduzd:
    def __init__(self):
        pass

    def search(self, search_word):
        self.search_word = search_word
        # if self.query_exsist():
        #     path = 'zhidao/' + self.search_word + '.html'
        #     self.ans = load(path)
        #     return self.ans
        self.query = \
          'http://zhidao.baidu.com/search?&lm=0&rn=10&pn=0&fr=search&ie=utf8&word='\
          + urllib.quote(self.search_word)
        self.res = urllib.urlopen(self.query)
        content = self.res.read()
        self.soup = BeautifulSoup(content)
        unicode(self.soup)
        atuple = self.soup.find_all("a", attrs={"target":"_blank", "class":"ti"})
        self.ans = [a.get_text()[1:] for a in atuple]
                
    def info(self):
        return self.ans

    def query_exsist(self):
        path = 'zhidao/' + self.search_word + '.html'
        if os.path.exists(path):
            return load(path)


    def save(self):
        if not os.path.exists('zhidao'):
            os.mkdir('zhidao')
        path = 'zhidao/' + self.search_word + '.html'
        with open(path,'w+') as html_file:
            for line in self.ans:
                html_file.write(line.encode('utf-8'))
        print 'saving file for next search...100%'
