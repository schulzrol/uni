# Compiler and flags
CC = g++
CFLAGS = -Wall -std=c++11 -I./src -ggdb

# Directories
SRC_DIR = src
OBJ_DIR = ./build
BIN_DIR = .

# Source files
CLIENT_SRCS = $(wildcard $(SRC_DIR)/client/*.cpp)
SERVER_SRCS = $(wildcard $(SRC_DIR)/server/*.cpp)
SHARED_SRCS = $(wildcard $(SRC_DIR)/shared/*.cpp)

# Object files
CLIENT_OBJS = $(patsubst $(SRC_DIR)/client/%.cpp,$(OBJ_DIR)/client/%.o,$(CLIENT_SRCS))
SERVER_OBJS = $(patsubst $(SRC_DIR)/server/%.cpp,$(OBJ_DIR)/server/%.o,$(SERVER_SRCS))
SHARED_OBJS = $(patsubst $(SRC_DIR)/shared/%.cpp,$(OBJ_DIR)/shared/%.o,$(SHARED_SRCS))

# Executables
CLIENT_EXE = $(BIN_DIR)/tftp-client
SERVER_EXE = $(BIN_DIR)/tftp-server

.PHONY: all clean

all: $(CLIENT_EXE) $(SERVER_EXE)

$(CLIENT_EXE): $(CLIENT_OBJS) $(SHARED_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(SERVER_EXE): $(SERVER_OBJS) $(SHARED_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/client/%.o: $(SRC_DIR)/client/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/server/%.o: $(SRC_DIR)/server/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/shared/%.o: $(SRC_DIR)/shared/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(CLIENT_OBJS) $(SERVER_OBJS) $(SHARED_OBJS) $(CLIENT_EXE) $(SERVER_EXE)

# Ensure that the output directories exist
$(shell mkdir -p $(OBJ_DIR)/client)
$(shell mkdir -p $(OBJ_DIR)/server)
$(shell mkdir -p $(OBJ_DIR)/shared)
$(shell mkdir -p $(BIN_DIR))
