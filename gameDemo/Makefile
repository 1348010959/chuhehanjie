WORK_DIR=$(shell pwd)
cc=g++
.PHONY:bin
bin:gameServer 
#client
src=gameServer.cpp thread_pool.cpp condition.cpp User.pb.cc
LDFLAGS=-std=c++11 -lprotobuf -lpthread

gameServer:$(src)
		$(cc) -g -o $@ $^ $(LDFLAGS)
#client:client.cpp User.pb.cc
#		$(cc) -g -o $@ $^ $(LDFLAGS)
.PHONY:clean
clean:
	rm -f gameServer

