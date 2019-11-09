SRC_DIR := ./src
OBJ_DIR := ./obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))
CFLAGS := -std=c++17 -g -Werror -O3

MAIN = POHeapTester

all: clean $(MAIN)

.PHONY: clean
clean:
	$(RM) $(OBJ_DIR)/*.o *~ $(MAIN)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	g++ $(CFLAGS) -c -o $@ $<


$(MAIN): $(OBJ_FILES) 
	g++ $(CFLAGS) -o $@ $^
