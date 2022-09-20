build_flags := 

SRC_DIR=./src
OBJ_DIR=./obj
INC_DIR=./include
TEST_DIR=./test
LIB_DIR=./lib

LIB_NAME=rle

debug: build_flags := -g -Wall

test: build_flags := -g -Wall

$(OBJ_DIR)/lib$(LIB_NAME).o: $(INC_DIR)/lib$(LIB_NAME).h $(SRC_DIR)/lib$(LIB_NAME).c
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_DIR)
	mkdir -p $(LIB_DIR)
	gcc $(build_flags) -c -o $(OBJ_DIR)/lib$(LIB_NAME).o $(SRC_DIR)/lib$(LIB_NAME).c -I$(INC_DIR)

lib: $(OBJ_DIR)/lib$(LIB_NAME).o
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/lib$(LIB_NAME).o

debug: $(OBJ_DIR)/lib$(LIB_NAME).o
	ar rcs $(LIB_DIR)/lib$(LIB_NAME)d.a $(OBJ_DIR)/lib$(LIB_NAME).o

test: debug $(SRC_DIR)/test.c
	gcc $(build_flags) -o $(TEST_DIR)/testlib$(LIB_NAME) $(SRC_DIR)/test.c -I$(INC_DIR) -L$(LIB_DIR) -l$(LIB_NAME)d

clean:
	rm -f $(LIB_DIR)/*
	rm -f $(OBJ_DIR)/*
	rm -f $(TEST_DIR)/*