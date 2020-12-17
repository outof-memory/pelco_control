## pelco-d format
| Byte 1    | Byte 2  | Byte 3    | Byte 4    | Byte 5 | Byte 6 |  Byte 7  |
| --------- | ------- | --------- | --------- | ------ | ------ | :------: |
| Sync Byte | Address | Command 1 | Command 2 | Data 1 | Data 2 | Checksum |


## suppport commands in repos
### "UP": [0x00, 0x08],
### "DOWN": [0x00, 0x10],
### "LEFT": [0x00, 0x04],
### "RIGHT": [0x00, 0x02],
### "CLEAN": [0x00, 0x00],
### "SETPRE": [0x00, 0x03],
### "SETP": [0x00, 0x4b],
### "SETT": [0x00, 0x4d],
### 'GOTO': [0x00, 0x07],
### "QUERYP": [0x00, 0x51],
### "QUERYT": [0x00, 0x53]}

## usage
data0 = data // 256, data1 = data % 256
python3 pelco_d_utils.py --cmd command --data 0
example: python3 pelco_d_utils.py --cmd left --data 10
command is from "support commands", can be lower; data represents data0 and data1
sett: valid range degree -15 ~ 90, corresponding data is 0 ~ 9000, 34500 ~ 36000
setp: valid range degree 0 ~ 180, corresponding data is 0 ~ 18000
