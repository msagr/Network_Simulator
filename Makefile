CC=gcc
CFLAGS=-g
TARGET:test.exe CommandParser/libcli.a pkt_gen.exe
LIBS=-lpthread -L ./CommandParser -lcli
OBJS=gluethread/glthread.o \
		  graph.o 		   \
		  topologies.o	   \

test.exe:testapp.o ${OBJS} CommandParser/libcli.a
	${CC} ${CFLAGS} testapp.o ${OBJS} -o test.exe ${LIBS}

testapp.o:testapp.c
	${CC} ${CFLAGS} -c testapp.c -o testapp.o

glthread/glthread.o:gluethread/glthread.c
	${CC} ${CFLAGS} -c -I . graph.c -o graph.o 

topologies.o:topologies.c 
	${CC} ${CFLAGS} -c -I . topologies.c -o topologies.o 

clean:
	rm *.o 
	rm gluethread/glthread.o 
	rm *exe