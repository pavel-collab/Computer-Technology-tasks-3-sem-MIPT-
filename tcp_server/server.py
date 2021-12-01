# протокол udp -- каждая посылка это 1 пакет
import socket
from datetime import datetime

PORT = 7555

sct = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
# фиксируем локальный конец
sct.bind(('', PORT))
# создаем очередь соединений
sct.listen(10)

# создаем словарь
clients = {}

try:
    print('listening on port: %d' %PORT)
    while True:
        client_sock, client_addr = sct.accept()
        clients[client_addr] = client_sock

        request = b''
        while True:
            buf = client_sock.recv(4096)
            if (len(buf) == 0):
                break
            request += buf

        reply = 'message is recieved from %s | time = %s\r\n' %(client_addr, datetime.now())
        client_sock.sendall(bytes(reply.encode('utf-8')))
        print('answer is sent')

        client_sock.shutdown(socket.SHUT_RDWR)
        client_sock.close()
except KeyboardInterrupt:
    print('\nend of listening')
    sct.close()

# in cobsol: whois <ip-adr>
# in cobsol: ip a
# nc -u <ip-adr> <port>