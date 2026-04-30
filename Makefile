CC = gcc

CFLAGS = -O3 -march=native -mfma -mavx2 -std=c11 -Iinclude
LDFLAGS = -lm

SRC = src/main.c \
      src/common.c \
      src/bench.c \
      src/accuracy_logger.c \
      src/v0.c \
      src/v1.c \
      src/v2.c \
      src/v3.c \
      src/v4.c \
      src/v5.c \
      src/v6.c \
      src/v7_avx2.c

lab: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o lab $(LDFLAGS)

clean:
	rm -f lab