#!/usr/bin/python3

import math

def clamp(x):
    if x == 512:
        return 1.0
    else:
        return (1/math.pi)*(math.sin(math.pi*(x-512.0)/512.0)/((x-512.0)/512.0))

for i in range(0, 1024):
    r = int(clamp(i)*(((1024.0-i)/1024.0)*255.0))
    g = int(clamp(i)*(255.0*math.exp(-math.pow((i-512.0)/512.0, 2))))
    b = int(clamp(i)*((i/1024.0)*(-128.0*math.cos((i/128.0)*math.pi)+127.0)))
    print(str(r) + ', ' + str(g) + ', ' + str(b))
