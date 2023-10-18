CC = g++
CFLAGS = -Wall -ggdb -pedantic -std=c++11
INCLUDES = -I./$(SOURCES_DIR)
SOURCES_DIR=./src
SERVER_SOURCES_DIR=$(SOURCES_DIR)/server
CLIENT_SOURCES_DIR=$(SOURCES_DIR)/client

CLIENT_SOURCES=$(CLIENT_SOURCES_DIR)/*.cpp
SERVER_SOURCES=$(SERVER_SOURCES_DIR)/*.cpp

BUILD_DIR=.

CLIENT_TARGET=$(BUILD_DIR)/tftp-client
SERVER_TARGET=$(BUILD_DIR)/tftp-server

.PHONY: all clean
# ---

all: $(CLIENT_TARGET) $(SERVER_TARGET)

$(CLIENT_TARGET): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(CLIENT_TARGET) $(CLIENT_SOURCES)

$(SERVER_TARGET): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(SERVER_TARGET) $(SERVER_SOURCES)

# ---
RM = rm -f
clean:
	$(RM) $(BUILD_DIR)/*.o 
	$(RM) $(CLIENT_TARGET)
	$(RM) $(SERVER_TARGET)

# Makefile debugging
#------------------------------------------------------------------------------
# prints all variables in makefile for makefile debugging
debug :
	$(foreach v, $(.VARIABLES), $(info $(v) = $($(v))))
