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

SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
HEADERS = $(wildcard $(SRC_DIR)/*.hpp)
OBJECTS = $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
TARGET  = $(BIN_DIR)/ppp

# Tests

TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)
TEST_TARGETS = $(TEST_SOURCES:.cpp=)


# Main

$(TARGET): $(SRC_DIR)/main.cpp $(HEADERS)
	$(CC_FULL) -o $@ $< $(SRC_DIR)/functions.cpp

clean:
	# rm -f $(OBJ_DIR)/*.o
	rm -f $(TARGET)

# Tests

$(TEST_DIR): $(TEST_TARGETS)

$(TEST_DIR)/%: $(TEST_DIR)/%.cpp $(HEADERS)
	$(CC_FULL) -o $@ $< $(SRC_DIR)/functions.cpp

tests_clean:
	# rm -f $(TEST_DIR)/*.o
	rm -f $(TEST_TARGETS)
