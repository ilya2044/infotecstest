.PHONY: all lib app clean run test

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -fPIC
LDFLAGS := -L./bin -Wl,-rpath,./bin
INCLUDES := -I./include

SRC_DIR := src
INCLUDE_DIR := include
BIN_DIR := bin
OBJ_DIR := obj

LOGGER_SRC := $(SRC_DIR)/logger.cpp
LOGGER_OBJ := $(OBJ_DIR)/logger.o
LOGGER_LIB := $(BIN_DIR)/liblogger.so

MAIN_SRC := $(SRC_DIR)/main.cpp
MAIN_OBJ := $(OBJ_DIR)/main.o
MAIN_BIN := $(BIN_DIR)/logger_app

TEST_SRC := test/test_logger.cpp
TEST_OBJ := $(OBJ_DIR)/test_logger.o
TEST_BIN := $(BIN_DIR)/test_logger

all: lib app

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LOGGER_OBJ): $(LOGGER_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(LOGGER_SRC) -o $(LOGGER_OBJ)

$(LOGGER_LIB): $(LOGGER_OBJ) | $(BIN_DIR)
	$(CXX) -shared -o $(LOGGER_LIB) $(LOGGER_OBJ)

lib: $(LOGGER_LIB)
	@echo "Библиотека собрана: $(LOGGER_LIB)"

$(MAIN_OBJ): $(MAIN_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(MAIN_SRC) -o $(MAIN_OBJ)

$(MAIN_BIN): $(MAIN_OBJ) $(LOGGER_LIB)
	$(CXX) $(MAIN_OBJ) -o $(MAIN_BIN) $(LDFLAGS) -llogger

app: $(MAIN_BIN)
	@echo "Приложение собрано: $(MAIN_BIN)"

clean:
	@rm -rf $(OBJ_DIR)
	@rm -rf $(BIN_DIR)

LOGFILE ?= default.txt
LOGLEVEL ?= MEDIUM

run: app
	$(MAIN_BIN) $(LOGFILE) $(LOGLEVEL)

$(TEST_OBJ): $(TEST_SRC) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $(TEST_SRC) -o $(TEST_OBJ)

$(TEST_BIN): $(TEST_OBJ) $(LOGGER_LIB)
	$(CXX) $(TEST_OBJ) -o $(TEST_BIN) $(LDFLAGS) -llogger

test: $(TEST_BIN)
	@$(TEST_BIN)
