cc=g++
.PHONY: all
all:  udpServer
udpServer: udpServer.cc
	$(cc) -o $@  $^ -std=c++11

.PHONY: clean
clean:
	rm -f  udpServer
