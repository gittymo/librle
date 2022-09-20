build_flags := 

SRC_DIR=src
OBJ_DIR=obj
INC_DIR=include
TEST_DIR=test
LIB_DIR=lib
DIST_DIR=dist

LIB_NAME=rle

debug: build_flags := -g -Wall

test: build_flags := -g -Wall

outdirs:
	mkdir -p $(OBJ_DIR)
	mkdir -p $(TEST_DIR)
	mkdir -p $(LIB_DIR)
	mkdir -p $(DIST_DIR)

$(OBJ_DIR)/lib$(LIB_NAME).o: outdirs $(INC_DIR)/lib$(LIB_NAME).h $(SRC_DIR)/lib$(LIB_NAME).c
	
	gcc $(build_flags) -c -o $(OBJ_DIR)/lib$(LIB_NAME).o $(SRC_DIR)/lib$(LIB_NAME).c -I$(INC_DIR)

lib: $(OBJ_DIR)/lib$(LIB_NAME).o
	ar rcs $(LIB_DIR)/lib$(LIB_NAME).a $(OBJ_DIR)/lib$(LIB_NAME).o

debug: $(OBJ_DIR)/lib$(LIB_NAME).o
	ar rcs $(LIB_DIR)/lib$(LIB_NAME)d.a $(OBJ_DIR)/lib$(LIB_NAME).o

test: debug $(SRC_DIR)/test.c
	gcc $(build_flags) -o $(TEST_DIR)/testlib$(LIB_NAME) $(SRC_DIR)/test.c -I$(INC_DIR) -L$(LIB_DIR) -l$(LIB_NAME)d

all: lib test

dist: all
	tar -czvf $(DIST_DIR)/lib$(LIB_NAME).tar.gz $(INC_DIR) $(LIB_DIR) $(TEST_DIR)/testlib$(LIB_NAME)

clean: outdirs
	rm -f $(LIB_DIR)/*
	rm -f $(OBJ_DIR)/*
	rm -f $(TEST_DIR)/*
	rm -f $(DIST_DIR)/*