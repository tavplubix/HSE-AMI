#!/usr/bin/python3

from matplotlib import pyplot as plt
import sys

num_elems = [2000, 10000, 50000]
T = [0.1, 0.01, 0.001]
num_threads = [1, 2, 4, 8, 16, 32, 64, 128]

res = {}

iter = 0
for line in sys.stdin:
    res[iter] = [int(x) for x in line[:-2].split('\t')]
    print(res)
    iter += 1

time_for_elems = {}
for i in range(3):
    tmp = [res[key][i] for key in res]
    time_for_elems[num_elems[i]] = tmp

print(time_for_elems)

for i in range(3):
    n = num_elems[i]
    plt.title('N = ' + str(n) + ", T = " + str(T[i]))
    plt.xlabel('threads')
    plt.ylabel('time, ms')

    plt.plot(num_threads, time_for_elems[n])
    #plt.legend()
    plt.savefig(str(n) + '.png')
    plt.close()
