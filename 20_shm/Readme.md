## О программе

Проект представляет собой две программы: клиент и сервер, которые работают параллельно в отдельных потоках, обмениваются сообщениями через разделяемую память и синхронизуются с помощью именованого семафора.

Сервер создает область разделяемой памяти и начинает писать туда текущее время (UTC), клиент доступается до области разделяемой помяти и начинает читать оттуда значение времени с заданным интервалом. Обе программы завершаются по сигналу.

## Сборка и запуск

Для сборки воспользуйтесь makefile.
```
make
```
В начале запускается сервер:
```
./server
```
После этого в отдельном потоке (например в отдельной консоли) запускается клиент
```
./client
```
Корректно завершить обе программы можно с помощью сигналов SIGINT или SIGTERM. Для того, чтобы отчистить рабочую дерикторию от исполняемый файлов используйте
```
make clean
```