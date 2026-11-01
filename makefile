
CC = gcc

CFLAGS = -std=c17 -pedantic -Wall -Wextra -Wformat-security -Wduplicated-cond \
         -Wfloat-equal -Wshadow -Wconversion -Wjump-misses-init \
         -Wlogical-not-parentheses -Wnull-dereference -Wvla -Werror \
         -fstack-protector-strong -fsanitize=undefined -fno-sanitize-recover \
         -g -fno-omit-frame-pointer -O1

TARGET = ocen
OBJS = ocen.o zbior_ary.o

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

ocen.o: makefile ocen.c zbior_ary.h
	$(CC) $(CFLAGS) -c ocen.c -o ocen.o

zbior_ary.o: makefile zbior_ary.c zbior_ary.h
	$(CC) $(CFLAGS) -c zbior_ary.c -o zbior_ary.o

clean:
	rm -f $(OBJS) $(TARGET)
