server_system:server.o do_client.o
	gcc server.o do_client.o -o server_system -lsqlite3 

server.o:
	gcc -c server.c -o server.o 

do_client.o:
	gcc -c do_client.c -o do_client.o 

clean:
	rm -rf *.o server_system
