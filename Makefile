CC = gcc
CFLAGS = -Wall -Wextra -O3
LDFLAGS = -lpthread -lsqlite3

SRC_DIR = ./src
OBJ_DIR = ./bin
OBJECTS = $(OBJ_DIR)/thanos_common.o \
$(OBJ_DIR)/thanos_io.o \
$(OBJ_DIR)/thanos_mem.o \
$(OBJ_DIR)/thanos_str.o \
$(OBJ_DIR)/thanos_db.o \
$(OBJ_DIR)/thanos_scan.o \
$(OBJ_DIR)/thanos.o


thanos: $(OBJECTS)
	$(CC) -o thanos $(OBJECTS) $(CFLAGS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) thanos
