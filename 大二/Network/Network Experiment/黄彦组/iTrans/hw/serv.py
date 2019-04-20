import server
# import threading

host = ('localhost',8003)

if __name__ == '__main__':
    s = server.server(host)
    s.start()