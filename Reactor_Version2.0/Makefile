# 编译器选项
CXX := g++
COM_OP := -Wall -g
TARGET := Server  # 自定义目标名称

# 自动获取源文件和依赖
SRCS := $(filter-out Client.cpp ,$(wildcard *.cpp))
OBJS := $(SRCS:.cpp=.o)
DEPENDS := $(SRCS:.cpp=.d)

.PHONY: all clean rebuild

# 默认构建单个执行文件
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(COM_OP)

# 生成依赖关系
%.d: %.cpp
	$(CXX) -MM $< -o $@

# 包含依赖文件
-include $(DEPENDS)

clean:
	$(RM) $(TARGET) $(OBJS) $(DEPENDS)

rebuild: clean all