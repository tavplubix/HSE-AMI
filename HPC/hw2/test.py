#!/usr/bin/python3

import subprocess as sp
import os
import sys

sp.check_call(['g++-8', '-O2', '-fopenmp', 'main.cpp', '-o', 'hw2'])

os.environ['OMP_NESTED'] = 'TRUE'

num_runs = int(sys.argv[2])
N = int(sys.argv[1])

for algorithm in ['quick', 'merge', 'selection', 'insertion']:
    for init in ['random', 'reverse']:
        avg_res = {}
        for threads in [1, 2, 4, 8, 16, 32]:
            print('Run ', algorithm, ' on ', init, ' with ', threads, ' threads:', file=sys.stderr)
            sys.stderr.flush()
            sum_time = 0
            i = 0
            for i in range(num_runs):
                time = int(sp.check_output(['./hw2', str(N), algorithm, init, str(threads)]))
                print('\titeration ', i, time, file=sys.stderr)
                sys.stderr.flush()
                sum_time += time
            avg_res[threads] = sum_time / num_runs
        print([algorithm, init, avg_res])
        sys.stdout.flush()

