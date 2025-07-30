# 编译器配置
CXX := g++
CXXFLAGS := -Wall -Wextra -g -std=c++11
LDFLAGS := 

# 源文件配置（排除 Client.cpp）
SRCS := $(filter-out Client.cpp, $(wildcard *.cpp))
OBJS := $(SRCS:.cpp=.o)
TARGET := Server  # 最终生成的可执行文件名

# 默认目标：构建可执行文件
all: $(TARGET)

# 链接所有对象文件生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# 编译每个 .cpp 文件为 .o 文件
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理生成的文件
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# 重新构建
.PHONY: rebuild
rebuild: clean all