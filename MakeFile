CC = g++
CFLAGS = -Wall -std=c++11

all: servidor cliente

servidor: server.cpp
	$(CC) $(CFLAGS) -o servidor server.cpp

cliente: client.cpp
	$(CC) $(CFLAGS) -o cliente client.cpp

clean:
	rm -f servidor cliente