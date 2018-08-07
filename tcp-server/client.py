import socket
import time


TCP_IP = '107.23.249.250'
TCP_PORT = 5005
BUFFER_SIZE = 1024
MESSAGE = "13"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((TCP_IP, TCP_PORT))
s.send(MESSAGE)

time.sleep(1)
MESSAGE = "14"
s.send(MESSAGE)

time.sleep(1)
MESSAGE = "15"
s.send(MESSAGE)

time.sleep(1)
MESSAGE = "16"
s.send(MESSAGE)

s.close()
