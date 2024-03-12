# Copyright (C) 2024  Lukasz Drzensla

PLATFORM ?= linux

CC =
APP = 

CFLAGS = -g -O0 -Wall -std=c99 -pedantic -Wunused -Wmissing-declarations

ifeq ($(PLATFORM), linux)
	CC = gcc
	APP = $(BUILD_DIR)/example
endif

SRC_DIR = src
BUILD_DIR = build
LIB_DIR = lib

#main library objects, main.o to be moved
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/radicom.o

APP_OPT = -DDBG

INCLUDE = $(SRC_DIR)

default: $(BUILD_DIR) $(APP)
library: $(BUILD_DIR) $(LIB)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(OBJECTS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) -c $< -o $@

$(LIB_OBJS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) -c $< -o $@

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) $(OBJECTS) -o $@

$(LIB): $(LIB_OBJS)
	$(CC) $(CCOPTS) -I$(INCLUDE) $(LIB_OPT) $(LIB_OBJS) -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o
	rm $(BUILD_DIR)/example

