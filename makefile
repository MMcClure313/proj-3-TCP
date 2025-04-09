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
	$(CXX) $(CXXFLAGS) tcpServer.cpp UserHandler.cpp User.cpp -o tcpServer

testMain:
	$(CXX) $(CXXFLAGS) testMain.cpp UserHandler.cpp User.cpp -o testMain

clean:
	rm -f *.o tcpServer tcpClient testMain

