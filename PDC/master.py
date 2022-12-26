from __future__ import print_function
import sys
try:
    import queue
except ImportError:
    import Queue as queue
import Pyro4.core
from threading import Lock
import time
import operator

Pyro4.config.SERIALIZER = 'pickle'
Pyro4.config.SERIALIZERS_ACCEPTED.add('pickle')

class Master(object):
    def __init__(self):
        self.workqueue = queue.Queue()
        self.resultqueue = {}
        self.w_lock = Lock()
        self.c_lock = Lock()
        self.inProgress = {}
        self.workerLastSeen = {}
        self.readyItems = {}



    @Pyro4.expose
    def putWork(self, item, id):
        client = Pyro4.current_context.client.sock.getpeername()
        item.client = client[0] + ':' + str(id)
        self.c_lock.acquire()
        if item.client not in self.resultqueue:
            self.resultqueue[item.client] = queue.Queue()
        self.workqueue.put(item)
        self.c_lock.release()

    @Pyro4.expose
    def getWork(self, id, timeout=3):
        worker = Pyro4.current_context.client.sock.getpeername()
        worker = worker[0] + ':' + str(id)
        self.w_lock.acquire()
        self.workerLastSeen[worker] = time.time()
        badWorker, lastSeen = min(self.workerLastSeen.iteritems(), key=operator.itemgetter(1))
        if 8 < time.time() - lastSeen:
            del self.workerLastSeen[badWorker]
            if badWorker in self.inProgress:
                print(time.time(), " no response from " + badWorker + " for " + str(time.time() - lastSeen))
                item = self.inProgress[badWorker]
                self.inProgress[worker] = item
                self.w_lock.release()
                return item
        try:
            item = self.workqueue.get(False)
        except queue.Empty as e:
            self.w_lock.release()
            raise e
        self.inProgress[worker] = item
        self.w_lock.release()
        return item

    @Pyro4.expose
    def alive(self, id):
        worker = Pyro4.current_context.client.sock.getpeername()
        worker = worker[0] + ':' + str(id)
        self.w_lock.acquire()
        self.workerLastSeen[worker] = time.time()
        self.w_lock.release()

    @Pyro4.expose
    def putResult(self, id, item):
        print(time.time(), " put ", item.data)
        worker = Pyro4.current_context.client.sock.getpeername()
        worker = worker[0] + ':' + str(id)
        self.w_lock.acquire()
        self.workerLastSeen[worker] = time.time()
        if worker in self.inProgress:
            del self.inProgress[worker]
        self.resultqueue[item.client].put(item)
        self.w_lock.release()

    @Pyro4.expose
    def getResult(self, id, timeout=3):
        client = Pyro4.current_context.client.sock.getpeername()
        client = client[0] + ':' + str(id)
        print(time.time(), " ", client, " get")
        self.c_lock.acquire()
        print("locked c")
        #print("rq ", self.resultqueue)
        #print(time.time(), " re ", self.readyItems)
        if client in self.readyItems:
            print(time.time(), " ", client, " get retry")
            item = self.readyItems[client]
            self.c_lock.release()
            print("unlocked c")
            return item
        try:
            item = self.resultqueue[client].get(False)
        except queue.Empty as e:
            print(time.time(), " ", client, " empty q")
            self.c_lock.release()
            print("unlocked c")
            raise e
        except Exception as e:
            pass
            print(time.time(), " caught", e, type(e))
        print(time.time(), " ", client, " get ", item.data)
        self.readyItems[client] = item
        self.c_lock.release()
        print("unlocked c")
        return item

    @Pyro4.expose
    def acceptResult(self, id):
        client = Pyro4.current_context.client.sock.getpeername()
        client = client[0] + ':' + str(id)
        print(time.time(), " ", client, " acceptResult")
        self.c_lock.acquire()
        print("locked c")
        print(time.time(), " ", client, " accept")
        try:
            del self.readyItems[client]
        except KeyError:
            pass
        self.c_lock.release()
        print("unlocked c")

    @Pyro4.expose
    def workQueueSize(self):
        return self.workqueue.qsize()

def main():
    # HOST:PORT
    address = str(sys.argv[1]).split(':')
    host = address[0]
    port = int(address[1])

    daemon = Pyro4.core.Daemon(host, port)
    master = Master()
    uri = daemon.register(master, "master")

    print("Master is running: " + str(uri))
    daemon.requestLoop()

if __name__=="__main__":
    main()
