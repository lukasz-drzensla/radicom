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

#main RLIB_OBJS
CEXAMPLE_OBJ = $(BUILD_DIR)/main.o
RLIB_OBJS = $(BUILD_DIR)/radicom.o
JNI_LIB_OBJS = $(LIB_DIR)/libCRadicom.so

APP_OPT = -DDBG

INCLUDE = -I$(SRC_DIR) -I$(SRC_DIR)/jradicom

default: $(BUILD_DIR) $(RLIB) $(RLIB_OBJS) $(LIB_DIR) $(JNI_LIB_OBJS) $(CEXAMPLE_OBJ) $(APP)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(LIB_DIR):
	@mkdir $(LIB_DIR)

$(RLIB_OBJS):$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDE) $(APP_OPT) -c $< -o $@

$(CEXAMPLE_OBJ):$(BUILD_DIR)/%.o: $(SRC_DIR)/example/%.c
	$(CC) $(CFLAGS) $(INCLUDE) $(APP_OPT) -c $< -o $@

$(RLIB): $(RLIB_OBJS)
	$(CC) $(CFLAGS) $(INCLUDE) $(APP_OPT) $(RLIB_OBJS) -o $@

$(JNI_LIB_OBJS):$(LIB_DIR)/lib%.so: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(JNI_FLAGS) $(INCLUDE) $< -o $@ $(RLIB_OBJS)

$(APP):$(CEXAMPLE_OBJ)
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@ $(RLIB_OBJS)

clean:
	rm -rf $(BUILD_DIR)/*.o
	rm -rf $(LIB_DIR)/*.*o
	rm $(BUILD_DIR)/example

