#!/usr/bin/python3

from matplotlib import pyplot as plt
import sys

plt.title('sort 100k numbers')
plt.xlabel('threads')
plt.ylabel('time, ms')

for line in sys.stdin:
    tmp = eval(line[:-1])
    algo = tmp[0]
    init = tmp[1]
    res = tmp[2]
    threads = [x for x in res]
    time = [res[x] for x in res]
    plt.plot(threads, time, label=algo + ', ' + init)

plt.legend()
plt.savefig(sys.argv[1])
#plt.show()

