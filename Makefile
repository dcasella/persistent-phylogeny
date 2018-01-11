# G++

CC     = g++ -std=c++14
CFLAGS = -O3 -Wall
CEXTRA =

# Boost C++11 ABI changes compatibility
CXX11_ABI = -D_GLIBCXX_USE_CXX11_ABI=1

# Boost linked libraries
BOOST_LIB_PO = boost_program_options
BOOST_LIB_PY = boost_python
BOOST_LIBS   = -l$(BOOST_LIB_PO) -l$(BOOST_LIB_PY)

# Python linked library and directory
PYTHON_LIB  = python2.7
PYTHON_LIBS = -l$(PYTHON_LIB)
PYTHON_DIR  = /usr/include/$(PYTHON_LIB)

CC_FULL = $(CC) $(CFLAGS) $(CEXTRA) $(CXX11_ABI) -I$(SRC_DIR) -I$(PYTHON_DIR)

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


# Targets

all: $(TARGET) python

test: CEXTRA+=-DTEST
test: all

# C++ Main

$(TARGET): $(OBJECTS) $(OBJ_DIR)/main.o
	$(CC) -o $@ $^ $(BOOST_LIBS) $(PYTHON_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CC_FULL) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(BIN_DIR)/*.pyc

# C++ Tests

$(TEST_DIR): $(TEST_TARGETS)

$(TEST_TARGETS): $(TEST_DIR)/%: $(OBJ_DIR)/%.o $(OBJECTS)
	$(CC) -o $@ $^

$(TEST_OBJECTS): $(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CC_FULL) -c -o $@ $<

$(TEST_DIR)/clean:
	rm -f $(TEST_OBJECTS) $(TEST_TARGETS)

# Python

python:
	python -m compileall -q $(BIN_DIR)

# Settings

.PHONY: clean $(TEST_DIR)/clean

.SILENT: python
