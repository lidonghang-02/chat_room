CC = g++
CFLAGS = -std=c++11 -Wall
LDFLAGS = -lsqlite3

# 源文件路径
SRC_ClI_DIR = Client
SRC_SER_DIR = Server
DB_DIR = database
COMMON_DIR = Common
cJSON_DIR = cJSON

# 源文件列表
SRC_FILES = $(wildcard $(SRC_ClI_DIR)/*.cpp)
DB_SRC_FILES =  $(wildcard $(DB_DIR)/sql_func.cpp)

# 目标文件列表
OBJ_FILES = $(patsubst $(SRC_ClI_DIR)/%.cpp, $(SRC_ClI_DIR)/%.o, $(SRC_FILES))
DB_OBJ_FILES = $(patsubst $(DB_DIR)/%.cpp, $(DB_DIR)/%.o, $(DB_SRC_FILES))

# 目标可执行文件
TARGET = ./Client/client

all: $(TARGET)

$(TARGET): $(OBJ_FILES) $(DB_OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(SRC_ClI_DIR)/%.o: $(SRC_ClI_DIR)/%.cpp $(SRC_ClI_DIR)/Login_UI.h
	$(CC) $(CFLAGS) -c $< -o $@

$(DB_DIR)/%.o: $(DB_DIR)/%.cpp $(DB_DIR)/sql_func.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ_FILES) $(DB_OBJ_FILES)
