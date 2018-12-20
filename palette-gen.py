#!/usr/bin/python3

import math

def clamp(x):
    return math.sqrt(1-math.pow((x-512.0)/512.0, 2))

for i in range(0, 1024):
    r = int(clamp(i)*(255.0*math.exp(-math.pow((i-128.0)/192.0, 2))))
    g = int(clamp(i)*(255.0*math.exp(-math.pow((i-512.0)/192.0, 2))))
    b = int(clamp(i)*(255.0*math.exp(-math.pow((i-896.0)/192.0, 2))))
    print(str(r) + ', ' + str(g) + ', ' + str(b))
