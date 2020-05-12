all: tcpserver.o runtcpserver.o
	g++ tcpserver.o runtcpserver.o -o tcpserver -lpthread -std=c++11 -I./ -I /Users/mac/Documents/websocketpp-master/ -I/usr/local/Cellar/boost/1.72.0_2/include/ -L/usr/local/Cellar/boost/1.72.0_2/lib/

tcpserver.o: tcpserver.cpp
	g++ tcpserver.cpp -c -lpthread -std=c++11

runtcpserver.o: runtcpserver.cpp
	g++ runtcpserver.cpp -c -lpthread -std=c++11 -I./ -I /Users/mac/Documents/websocketpp-master/ -I/usr/local/Cellar/boost/1.72.0_2/include/ -L/usr/local/Cellar/boost/1.72.0_2/lib/

clean:
	rm -rf *.o
