SRCS:=$(wildcard *.cpp)
OBJS:=$(patsubst %.cpp, %, $(SRCS))
CXX:=g++
FLAGS:=-g

ALL:$(OBJS)

%:%.cpp
	$(CXX)  $^ -o $@ $(FLAGS)

clean:
	rm -rf $(OBJS)