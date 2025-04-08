# Compiler
CXX = g++

# Executable name
CXXFLAGS = -Wall -Wextra -g -std=c++11 -pthread

# Target executables
TARGETS = httpServer httpClient

.PHONY: clean

all: httpClient httpServer

httpClient:
	$(CXX) $(CXXFLAGS) httpClient.cpp -o httpClient

httpServer:
	$(CXX) $(CXXFLAGS) httpServer.cpp -o httpServer

httpServer.o: tcpServer.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o httpServer httpClient
