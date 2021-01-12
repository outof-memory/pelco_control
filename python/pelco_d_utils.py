import argparse
import time
import numpy as np
from serial import Serial
from functools import reduce

cmdDict = {"UP": [0x00, 0x08],
           "DOWN": [0x00, 0x10],
           "LEFT": [0x00, 0x04],
           "RIGHT": [0x00, 0x02],
           "DOWNLEFT": [0x00, 0x14],
           "UPLEFT": [0x00, 0x0c],
           "CLEAN": [0x00, 0x00],
           "SETPRE": [0x00, 0x03],
           "SETP": [0x00, 0x4b],
           "SETT": [0x00, 0x4d],
           'GOTO': [0x00, 0x07],
           "QUERYP": [0x00, 0x51],
           "QUERYT": [0x00, 0x53]}


SYNC = 0xff

def command(cmdName, data1, data2):
    av = [1]
    cmdName = cmdName.upper()
    if cmdName in cmdDict.keys():
        cmd = cmdDict[cmdName]
        av.extend(cmd)
        if cmdName == "CLEAN" or cmdName[:5] == "QUERY":
            data1 = data2 = 0
        av.append(data1)
        av.append(data2)
    else:
        print("cmd name not suppported")
    checksum = sum(av) & 255
    return bytearray((SYNC,) + tuple(av) + (checksum,))

def recvData(ser):
    buffer = []
    while True:
        recv = ser.read()
        if len(recv) > 0:
            buffer.append(recv)
        if len(buffer) == 7:
            break
        print(recv)
    return reduce(lambda x, y: x+y, buffer)


def test():
    cmd1 = command("queryp", 0, 0)
    cmd2 = command("left", 1, 1)
    cmd3 = command("clean", 1, 1)
    port = "/dev/ttyTHS0"
    ser = Serial(port, 9600, timeout=1)

    cmd = cmd1
    print("send ", np.ndarray(buffer=cmd, shape=(7,), dtype=np.uint8))
    ser.write(bytes(cmd))
    response = recvData(ser)
    print("recv ", np.ndarray(buffer=response, shape=(7,), dtype=np.uint8))

    cmd = cmd2
    print("send ", np.ndarray(buffer=cmd, shape=(7,), dtype=np.uint8))
    ser.write(bytes(cmd))
    response = ser.read()
    print(response)
    time.sleep(3)

    cmd = cmd3
    print("send ", np.ndarray(buffer=cmd, shape=(7,), dtype=np.uint8))
    ser.write(bytes(cmd))
    response = ser.read()
    print(int.from_bytes(response, 'big'))

    cmd = cmd1
    print("send ", np.ndarray(buffer=cmd, shape=(7,), dtype=np.uint8))
    ser.write(bytes(cmd))
    response = recvData(ser)
    print("recv ", np.ndarray(buffer=response, shape=(7,), dtype=np.uint8))

def rotateCycle(step=0.1):
    port = "/dev/ttyTHS0"
    with Serial(port, 9600, timeout=1) as ser:
        step = step * 100 # 100 smallest step = 1 degree
        angles = np.arange(0, 36000+0.001, int(step), dtype=np.int)
        for angle in angles[:]:
            data0 = angle // 256
            data1 = angle % 256
            cmd = command("setp", data0, data1)
            ser.write(bytes(cmd))
            # response = recvData(ser)
            # if angle % 20 == 0:
            #     print("recv ", np.ndarray(buffer=response, shape=(7,), dtype=np.uint8))
    print(f"complete a cycle with {step}")

if __name__ == "__main__":
    # test()
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('--cmd', required=True,
                        help='command')
    parser.add_argument('--data', default=0, type=int,
                        help='data')
    args = parser.parse_args()
    data0 = args.data // 256
    data1 = args.data % 256
    cmd = command(args.cmd, data0, data1)
    port = "/dev/ttyTHS0"
    ser = Serial(port, 9600, timeout=1)
    print("send:", cmd)
    ser.write(bytes(cmd))
    if args.cmd[:5] == "query":
        response = recvData(ser)
        print("recv ", np.ndarray(buffer=response, shape=(7,), dtype=np.uint8))
        #response = ser.read()
        #print(response)
    else:
        response = ser.read()
        if response.decode() == '\x01':
            print("command %s response" %args.cmd)
        elif response.decode() == "":
            print("command %s not response" %args.cmd)
        else:
            print(response)
    ser.close()

