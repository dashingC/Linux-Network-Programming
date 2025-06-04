cc=g++
.PHONY: all
all:  tcpserver  tcpclient
tcpserver: tcpServer.cc
	$(cc) -o $@  $^ -std=c++11

tcpclient: tcpClient.cc
	$(cc) -o $@  $^ -std=c++11

.PHONY: clean
clean:
	rm -f  tcpServer tcpClient
