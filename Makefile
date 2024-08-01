CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
LDFLAGS = -lzstd -pthread

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TARGET = out.out

POS_CFLAGS = -Wall -Wextra -O2 -I./poc/c/includes
POC_SRC = $(wildcard poc/c/src/*.c)
POC_OBJ = $(POC_SRC:.c=.o)
POC_TARGET = poc.out

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

$(POC_TARGET): $(POC_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) -c -g -o $@ $< $(POS_CFLAGS)

clean:
	rm -f $(OBJ) $(TARGET) $(POC_OBJ) $(POC_TARGET) *.zstd

poc: $(POC_TARGET)
