from __future__ import print_function
import os,socket,sys
from math import floor, sqrt
try:
    import queue
except ImportError:
    import Queue as queue
import Pyro4.core
from workitem import Workitem
from threading import Thread
import time

Pyro4.config.SERIALIZER = 'pickle'
Pyro4.config.SERIALIZERS_ACCEPTED.add('pickle')

WORKERNAME = "Worker_%d@%s" % (os.getpid(), socket.gethostname())
id = os.getpid()

def factorize(n):
    step = lambda x: 1 + (x<<2) - ((x>>1)<<1)
    maxq = long(floor(sqrt(n)))
    d = 1
    q = n % 2 == 0 and 2 or 3 
    while q <= maxq and n % q != 0:
        q = step(d)
        d += 1
    return q <= maxq and [q] + factorize(n//q) or [n]

def process(item):
    print("factorizing %s -->" % item.data)
    sys.stdout.flush()
    item.result = factorize(int(item.data))
    print(item.result)
    item.processedBy = WORKERNAME

def _process(item, master):
    thread = Thread(target=process, args=(item,))
    while thread.is_alive():
        try:
            master.alive(id)
        except Pyro4.errors.CommunicationError:
            pass
        time.sleep(1)
    thread.join()


def main():
    Pyro4.config.MAX_RETRIES = 0
    Pyro4.config.COMMTIMEOUT = 2
    disp_address = str(sys.argv[1])
    master = Pyro4.core.Proxy("PYRO:master@" + disp_address)
    print("This is worker %s" % WORKERNAME)
    while True:
        try:
            item = master.getWork(id)
        except queue.Empty:
            print("no work available yet")
            time.sleep(1)
        except Pyro4.errors.CommunicationError:
            pass
        else:
            process(item)
            ok = False
            while not ok:
                try:
                    master.putResult(id, item)
                    ok = True
                except Pyro4.errors.CommunicationError:
                    pass


if __name__=="__main__":
    main()
