CC = gcc
CFLAGS = -I./zstd/lib -I./zstd/lib/common -I./zstd/lib/compress -I./zstd/lib/decompress -I./zstd/lib/dictBuilder
LDFLAGS = -L./zstd/lib -lzstd

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = out.out

POC_SRC = $(wildcard poc/*.c)
POC_OBJ = $(POC_SRC:.c=.o)
POC_TARGET = poc.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(POC_TARGET): $(POC_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -g -o $@ $< $(CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) $(POC_OBJ) $(POC_TARGET) *.zstd

poc: $(POC_TARGET)
