CC := g++
CFLAGS := -Wall -Wextra -std=c++11
# INCLUDE_DIRS := -IcJSON
# LIBRARIES := -LcJSON -lcjson

all: client server

client: client.cpp cJSON/cJSON.h cJSON/cJSON.c
	$(CC) $(CFLAGS)  -o client client.cpp cJSON/cJSON.c 

server: server.cpp cJSON/cJSON.h cJSON/cJSON.c
	$(CC) $(CFLAGS)  -o server server.cpp cJSON/cJSON.c 

clean:
	rm -f client server

.PHONY: all clean
