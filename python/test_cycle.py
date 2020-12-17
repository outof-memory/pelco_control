'''
time of cycle
0.01: 262s
0.02: 131s
0.03: 87s
0.05: 52s
0.1: 5s

'''
from pelco_d_utils import rotateCycle
import time

for i in range(10):
    t0 = time.time()
    rotateCycle(step=0.03)
    t1 = time.time()
    print(f"time is {t1-t0}")
