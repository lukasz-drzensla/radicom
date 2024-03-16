# Copyright (C) 2024  Lukasz Drzensla

PLATFORM ?= linux

CC =
APP = 

CFLAGS = -g -O0 -Wall -std=c99 -pedantic -Wunused -Wmissing-declarations
JNI_FLAGS = -shared -I/usr/lib/jvm/java-18-openjdk-amd64/include -I/usr/lib/jvm/java-18-openjdk-amd64/include/linux -fPIC

ifeq ($(PLATFORM), linux)
	CC = gcc
	APP = $(BUILD_DIR)/example
endif

SRC_DIR = src
BUILD_DIR = out
LIB_DIR = lib

#main objects, main.o to be moved
OBJECTS = $(BUILD_DIR)/main.o $(BUILD_DIR)/radicom.o
LIB_OBJS = $(LIB_DIR)/libCRadicom.so

APP_OPT = -DDBG

INCLUDE = $(SRC_DIR)

default: $(BUILD_DIR) $(APP) $(LIB_DIR) $(LIB_OBJS)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(OBJECTS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) -c $< -o $@

$(APP): $(OBJECTS)
	$(CC) $(CFLAGS) -I$(INCLUDE) $(APP_OPT) $(OBJECTS) -o $@

$(LIB_OBJS):$(LIB_DIR)/lib%.so: $(SRC_DIR)/%.c
	$(CC) $(JNI_FLAGS) $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o
	rm -rf $(LIB_DIR)/*.*o
	rm $(BUILD_DIR)/example

