CC = gcc
CFLAGS = -Wall -Wextra -O2 -Iinclude
TARGET = bzip2_impl

SRC = src/main.c src/config.c src/block.c src/rle.c src/bwt.c src/archive.c
OBJ = $(SRC:.c=.o)

WINCC = x86_64-w64-mingw32-gcc
WINTARGET = bzip2_impl.exe

all: run-test

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET) $(WINTARGET)

windows:
	$(WINCC) $(CFLAGS) -o $(WINTARGET) $(SRC)

run-test: $(TARGET)
	./$(TARGET) encode input.txt encoded.bin
	./$(TARGET) decode encoded.bin decoded.txt
	cmp input.txt decoded.txt && echo "Round-trip OK"

.PHONY: all clean windows run-test