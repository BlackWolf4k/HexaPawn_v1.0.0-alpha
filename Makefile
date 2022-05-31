CC = g++
CFLAGS = -Wall -g

C_SOURCES_SERVER = $(wildcard server.cpp hptp.cpp hptp_tf.cpp)
C_SOURCES_CLIENT = $(wildcard client.cpp hptp.cpp hptp_tf.cpp)
C_HEADERS = $(wildcard *.h)

OBJ = ${C_SOURCES:.c=.o}

server:
	${CC} ${CFLGAS} ${C_SOURCES_SERVER} -o server

client:
	${CC} ${CFLGAS} ${C_SOURCES_CLIENT} -o client

#%.o: %.c ${C_HEADERS}
#	${CC} ${CFLAGS} $< -o $@

run: server client
	./server
	make clean

debug: server client
	gdb server
	make clean

clean:
	$(RM) server
	$(RM) client