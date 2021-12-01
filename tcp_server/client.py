import select
import socket
import sys

PORT = 7555
MY_IP = '192.168.2.115'

sct = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
sct.connect((MY_IP, PORT))

try:

    sct.sendall(b'GET / HTTP/1.1\r\nHost: mipt.ru\r\nConnection: close\r\n\r\n')
    sct.shutdown(socket.SHUT_WR)
    reply = b''
    while True:
        buf = sct.recv(4096)
        if (len(buf) == 0):
            break
        reply += buf

    print(reply.decode('utf-8'))
except KeyboardInterrupt:
    sct.close()