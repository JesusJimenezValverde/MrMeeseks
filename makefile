# Makefile from https://spin.atomicobject.com/2016/08/26/makefile-c-projects/

TARGET_EXEC ?= main

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

#SRCS := $(shell find $(SRC_DIRS) -type f -name *.c ! \( -name main.c \)) src/main.c
#SRCS := src/lex.yy.c src/fileHandler.c src/help.c src/preprocessor.c  src/functions.c src/main.c
#SRCS := src/readFile.c src/scanner.c src/parser.c src/help.c src/main.c
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p