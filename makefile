# Compiler
CXX = g++

# Executable name
CXXFLAGS = -Wall -Wextra -g -std=c++11 -pthread

# Target executables
TARGETS = tcpServer tcpClient

.PHONY: clean

all: tcpServer tcpClient

tcpClient:
	$(CXX) $(CXXFLAGS) tcpClient.cpp -o tcpClient

tcpServer:
	$(CXX) $(CXXFLAGS) tcpServer.cpp -o tcpServer

tcpClient.o: tcpServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o tcpServer tcpClient
