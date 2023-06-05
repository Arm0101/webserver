all: server.out
	clear && ./server.out 1234 /home/arma
server.out: main.o server.o response.o tools.o
	gcc -o server.out main.o server.o response.o tools.o

main.o: main.c
	gcc -c main.c

server.o: server.c
	gcc -c server.c

response.o: response.c
	gcc -c response.c

tools.o: tools.c
	gcc -c tools.c
clean:
	rm -f shell *.o *.out && clear