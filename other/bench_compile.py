#!/usr/bin/env python

# usage
# ./bench_compile.py src_dir build_dir

import sys
import os
import time
from math import sqrt
from subprocess import call

def average(nums):
    return sum(nums)/len(nums)

def stdevp(nums, mean):
    return sqrt(sum([(x-mean)**2 for x in nums])/len(nums))

src_dir = sys.argv[1]
bld_dir = sys.argv[2]

results = {}
for root, dirs, files in os.walk(src_dir):
    for file in files:
        if file.endswith('.h') or file.endswith('.cpp'):
            fullpath = os.path.join(root, file)
            call(['touch', fullpath])
            start = time.time()
            call(['ninja', '-C', bld_dir])
            end = time.time()
            results[fullpath] = (end-start)

vals = results.values();
mean = average(vals)
print 'mean', mean, 'std', stdevp(vals, mean), 'max', max(vals)

