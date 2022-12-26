import matplotlib.pyplot as plt
from math import log10

T = {}
for line in open('T.log', 'r') :
    t,cutsize = line.split(':')
    t = int(log10(float(t)))
    cutsize = cutsize.split()
    T[t] = [int(x) for x in cutsize]

for i in range(-4, 5):
    plt.plot(range(0, 5001), T[i], label='T = 10^' + str(i) + '    ')

plt.xlabel('Iterations')
plt.ylabel('Sum of maxcut sizes')
plt.legend(bbox_to_anchor=(1.025, 1), loc=2, borderaxespad=0.)
plt.show()




