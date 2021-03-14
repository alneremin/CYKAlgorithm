TARGET = cyk-1
JS_TARGET = docs/$(TARGET).js
TEST_TARGET = $(TARGET)-test

EMCC = emcc


CC = gcc
RM = rm -f
CFLAGS = -Wall -std=c99 -DDEBUG=1

TEST_LIBS = -lcheck -lrt -lm -lpthread -lsubunit
BUILD_DIR = ./build
JS_DIR = ./emscripten/
BUILD_JS_DIR = $(JS_DIR)$(BUILD_DIR)
SRC_DIR = ./
TEST_DIR = ./tests/
TEST_BUILD_DIR = ./tests/build/
MAIN_OBJECT_FILE = main.c.o
# $(shell ls $(SRC_DIR) | grep .c$)
# export LC_ALL=ru_RU

SRCS = $(shell find $(SRC_DIR) -maxdepth 1 -type f -name "*.c" -printf "%f\n" | sort)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
JS_OBJS = $(SRCS:%=$(BUILD_JS_DIR)/%.o)

TEST_SRCS = $(shell find $(TEST_DIR) -type f  -and -name "*.c" -printf "%f\n" | sort)
TEST_OBJS = $(TEST_SRCS:%=$(TEST_BUILD_DIR)/%.o)

#INC_DIRS := $(shell find $(SRC_DIR) -type h)
#INC_FLAGS := $(addprefix -I,$(INC_DIRS))

build: $(OBJS)
	$(CC) $(OBJS) -o $(SRC_DIR)$(BUILD_DIR)/$(TARGET)
	@echo "exe-file:" $(SRC_DIR)$(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

build_js: $(JS_OBJS)
	$(EMCC) $(JS_OBJS) -o $(JS_TARGET) \
	-s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
	-s EXPORTED_FUNCTIONS=['_run'] #--pre-js docs/pre-js.js
	@echo "js-file:" $(JS_TARGET)

$(BUILD_JS_DIR)/%.c.o: %.c
	$(MKDIR_P) $(BUILD_JS_DIR)
	$(EMCC) -c $< -o $@


# ТЕСТЫ

test : $(TEST_TARGET)
	$(TEST_DIR)$(BUILD_DIR)/$<

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) $(shell find $(OBJS) -not -name $(MAIN_OBJECT_FILE)) -o $(TEST_DIR)$(BUILD_DIR)/$@ $(TEST_LIBS)

$(TEST_BUILD_DIR)%.c.o: $(TEST_DIR)%.c
	$(MKDIR_P) $(TEST_BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) $(SRC_DIR)$(BUILD_DIR)/$(TARGET)
	$(RM) -r $(TEST_BUILD_DIR) $(TEST_DIR)$(BUILD_DIR)/$(TEST_TARGET)
	$(RM) -r $(JS_DIR)
MKDIR_P ?= mkdir -p