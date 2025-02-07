server:
	gcc zmq_server.c -o server.out $(pkg-config --cflags --libs libzmq)

run: server
	./server.out