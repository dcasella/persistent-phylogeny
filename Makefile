# G++

CC          = g++
CFLAGS      = -O3 -Wall
CEXTRA      =
BOOST_LIB_G = /usr/include/boost/libs/graph/build
BOOST_LIB_R = /usr/include/boost/libs/regex/build
BOOST_LIBS  = -L$(BOOST_LIB_G) -L$(BOOST_LIB_R)
CC_FULL     = $(CC) $(CFLAGS) $(CEXTRA) -I$(SRC_DIR)

# Folders

SRC_DIR  = src
OBJ_DIR  = obj
BIN_DIR  = bin
TEST_DIR = tests

# Main

SOURCES = $(filter-out $(SRC_DIR)/main.cpp, $(wildcard $(SRC_DIR)/*.cpp))
HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TARGET  = $(BIN_DIR)/ppp

# Tests

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)
TEST_TARGETS = $(TEST_SOURCES:.cpp=)


# Main

$(TARGET): $(OBJECTS) $(OBJ_DIR)/main.o
	mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	mkdir -p $(OBJ_DIR)
	$(CC_FULL) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Tests

$(TEST_DIR): $(TEST_TARGETS)

$(TEST_TARGETS): $(TEST_DIR)/%: $(OBJ_DIR)/%.o $(OBJECTS)
	$(CC) -o $@ $^

$(TEST_OBJECTS): $(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS)
	mkdir -p $(OBJ_DIR)
	$(CC_FULL) -c -o $@ $<

tests_clean:
	rm -rf $(OBJ_DIR) $(TEST_TARGETS)
