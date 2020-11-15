all: echo-client echo-server

echo-client: client.cpp
	g++ -Wall -o echo-client client.cpp -pthread 

echo-server: server.cpp
	g++ -Wall -o echo-server server.cpp -pthread

clean:
	rm echo-*