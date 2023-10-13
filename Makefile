TARGET := loxic.out

CC := gcc
CFLAGS := -Wall -pedantic -Wextra -Wno-unused-parameter
SRC_DIR := ./src

ifeq ($(MAKECMDGOALS),debug)
	BUILD_DIR := build/debug
	CFLAGS += -g -O0 -DDEBUG=1
else
	BUILD_DIR := build/release
	CFLAGS += -Wno-unused-parameter -O3 -DRELEASE=1
endif

HEADERS := $(wildcard $(SRC_DIR)/*.h)
SOURCES := $(wildcard $(SRC_DIR)/*.c)
OBJECTS := $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

debug: $(TARGET)
release: $(TARGET)
all: default

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	$(RM) $(TARGET) build/debug/*.o build/release/*.o
