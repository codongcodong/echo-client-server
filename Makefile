all: echo-client echo-server

echo-client:
	g++ -Wall -o echo-client client.cpp -pthread 

echo-server:
	g++ -Wall -o echo-server server.cpp -pthread

clean:
	rm echo-*