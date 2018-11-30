TARGET = wsclock-test
CC = gcc
OBJ = memory.o process.o wsclock-test.o

.PHONY: clean

all: clean $(TARGET)

%.o: %.c
	$(CC) $< -c -o $@

$(TARGET): $(OBJ)
	$(CC) $^ -o $@
	
clean:
	rm -rf $(OBJ) $(TARGET)
