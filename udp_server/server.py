# протокол udp -- каждая посылка это 1 пакет
import socket
from datetime import datetime

PORT = 7555

sct = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
# фиксируем локальный конец
sct.bind(('', PORT))

# создаем множество клиентов
clients = set()

try:
    print('listening on port: %d' %PORT)
    while True:
        data, adr_info = sct.recvfrom(4096)
        message = 'adres: %s data = %r' %(adr_info, data.decode('utf-8'))
        print(message)

        # если появился новый клиент, добавляем его в список
        if adr_info not in clients:
            print('new client: %s:%d' %(adr_info))
            clients.add(adr_info)
            print('All clients: ', sorted(clients))

        for client_addr in clients:
            reply = 'message is recieved from %s | time = %s\n' %(adr_info, datetime.now())
            sct.sendto(reply.encode('utf-8'), client_addr)

except KeyboardInterrupt:
    print('\nend of listening')
    sct.close()

# in cobsol: whois <ip-adr>
# in cobsol: ip a
# nc -u <ip-adr> <port>