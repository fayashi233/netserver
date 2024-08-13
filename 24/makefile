all:client echoserver 

client:client.cpp
	g++ -g -o client client.cpp
echoserver:echoserver.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp
	g++ -g -o echoserver echoserver.cpp InetAddress.cpp Socket.cpp Epoll.cpp Channel.cpp EventLoop.cpp TcpServer.cpp Acceptor.cpp Connection.cpp Buffer.cpp EchoServer.cpp

clean:
	rm -f client echoserver
