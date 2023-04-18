# 变量定义
############################################
progname = iot_temp_humi# 程序名
CC = gcc# 编译器
progpath = $(shell pwd)/bin/# 程序路径
object = $(shell pwd)/obj/# 编译后的.o文件路径
source = $(shell pwd)/src/# 源文件路径
CFLAGS += -I $(shell pwd)/inc/# 头文件路径
libpath = $(shell pwd)/lib/# 库文件路径
libname += iot_temp_humi# 动态库名
LDFLAGS += -L $(libpath) -ldl -lpthread -l$(libname)# 库文件路径
logpath = $(shell pwd)/logger/# 日志文件路径
logname = running.log# 日志文件名
############################################

# 生成目标文件和动态库
############################################
all:  create_dir $(libpath)lib$(libname).so $(progname)

create_dir:
	$(shell if [ ! -d $(object) ]; then mkdir $(object); fi)
	$(shell if [ ! -d $(progpath) ]; then mkdir $(progpath); fi)
	$(shell if [ ! -d $(libpath) ]; then mkdir $(libpath); fi)
	$(shell if [ ! -d $(logpath) ]; then mkdir $(logpath); fi)
	$(shell if [ ! -f $(logpath)$(logname) ]; then touch $(logpath)$(logname); fi)

# 生成.o文件
$(object)%.o: $(source)%.c
	$(CC) -c $< -o $@ -fPIC $(CFLAGS)

# 生成动态库
$(libpath)lib$(libname).so:  $(patsubst $(source)%.c,$(object)%.o,$(wildcard $(source)*.c))
	$(CC) -shared -o $@ $^

# 生成可执行文件
$(progname): $(patsubst $(source)%.c,$(object)%.o,$(wildcard $(source)*.c))
	$(CC) -o $(progpath)$@ $^ $(LDFLAGS)
############################################

# 清理
############################################
.PHONY: clean
clean:
	rm -rf $(object) $(progpath) $(libpath) $(logpath)
############################################
