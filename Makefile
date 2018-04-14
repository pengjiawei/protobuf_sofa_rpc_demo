CC=g++
INCLUDE_DIR=-Iinclude/ -I/exapp/pengjiawei/sofa-pbrpc/output/include
LIB_DIR=-Llib -L/exapp/pengjiawei/sofa-pbrpc/output/lib -L/usr/local/lib
LIBS=-lsofa-pbrpc	\
-lprotobuf	\
-lsnappy	\
-lpthread	\
-lz	\

OBJS=server_demo.o service.pb.o

all: $(OBJS) server_demo

server_demo: src/server_demo.cpp
	$(CC) $(INCLUDE_DIR) -std=c++11 -o server_demo $(OBJS) $(LIB_DIR) $(LIBS)

server_demo.o: src/server_demo.cpp
	$(CC) $(INCLUDE_DIR) -std=c++11 -c src/server_demo.cpp $(LIB_DIR) $(LIBS)

service.pb.o: include/service.pb.cc
	$(CC) $(INCLUDE_DIR) -std=c++11 -c include/service.pb.cc $(LIB_DIR) $(LIBS)
clean:
	rm -rf server_demo $(OBJS)