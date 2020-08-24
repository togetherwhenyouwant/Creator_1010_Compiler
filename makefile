# 定义编译命令变量
CC  := g++
rm  := rm -f
# @$(CC) -g -O3 -o main *.cpp *.hpp
all :
#	@$(CC) -g -o compiler *.cpp *.hpp
	@$(CC) -g compiler.cpp -o main
	@./main
#clean规则 
.PHONY: clean
clean:
#清除编译生成的所有文件
#$(RM) $(executable) $(objects) $(deps)
#清除编译生成的所有文件,不包括可执行文件
#	@$(rm) main
