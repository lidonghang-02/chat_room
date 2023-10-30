CC := g++  # 编译器
CFLAGS := -std=c++11 -Wall -Wextra  # 编译选项
LDFLAGS := -lsqlite3  # 链接选项

# 目标文件
SERVER_OBJ := server.o sqlite.o
CLIENT_OBJ := client.o sqlite.o

# 默认目标
all: server client

# 服务器端目标
server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) $(SERVER_OBJ) -o server $(LDFLAGS)

# 客户端目标
client: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) $(CLIENT_OBJ) -o client $(LDFLAGS)

# 依赖关系
server.o: server.cpp chat_room.h
	$(CC) $(CFLAGS) -c server.cpp

client.o: client.cpp chat_room.h
	$(CC) $(CFLAGS) -c client.cpp

sqlite.o: sqlite.cpp chat_room.h
	$(CC) $(CFLAGS) -c sqlite.cpp

# 清理目标文件和可执行文件
clean:
	rm -f server client $(SERVER_OBJ) $(CLIENT_OBJ) chat_room.db