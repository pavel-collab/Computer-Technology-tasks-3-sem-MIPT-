import select
import socket
import sys

PORT = 7555
MY_IP = '192.168.2.115'

sct = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sct.connect((MY_IP, PORT))

# используем обертку системного вызова poll:
sources = select.poll()
# закидываем stdin и socket и ждем событие POLLIN
sources.register(sys.stdin, select.POLLIN)
sources.register(sct, select.POLLIN)

print('ready for sending messages')
print('to get current time send message \'get datetime\'\n')
try:
    while True:
        for fd, event in sources.poll():
            # если прилетает из stdin, то формируем и отправляем посылку
            if fd == sys.stdin.fileno():
                message = sys.stdin.readline().strip() # обрезаем символы \n
                sct.sendall(message.encode('utf-8'))
            # если прилетает из сокета, то печатаем то, что прилетело
            else:
                reply = sct.recv(4096).decode('utf-8').strip()
                print(reply)
except KeyboardInterrupt:
    print('\nend of sending messages')
    sct.close()