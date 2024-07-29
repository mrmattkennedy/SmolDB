CC = gcc
CFLAGS = -I./zstd/lib -I./zstd/lib/common -I./zstd/lib/compress -I./zstd/lib/decompress -I./zstd/lib/dictBuilder
LDFLAGS = -L./zstd/lib -lzstd

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = out.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET)
