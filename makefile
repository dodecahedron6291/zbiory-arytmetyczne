
CC = gcc

CFLAGS = -std=c17 -pedantic -Wall -Wextra -Wformat-security -Wduplicated-cond \
         -Wfloat-equal -Wshadow -Wconversion -Wjump-misses-init \
         -Wlogical-not-parentheses -Wnull-dereference -Wvla -Werror \
         -fstack-protector-strong -fsanitize=undefined -fno-sanitize-recover \
         -g -fno-omit-frame-pointer -O1

TARGET = ocen ocen2 ocen3
OBJS = ocen.o zbior_ary.o

.PHONY: all clean

all: $(TARGET)

ocen: ocen.o zbior_ary.o
	$(CC) $(CFLAGS) -o $@ $(OBJS)

ocen.o: makefile ocen.c zbior_ary.h
	$(CC) $(CFLAGS) -c ocen.c -o ocen.o

ocen2: ocen2.o zbior_ary.o
	$(CC) $(CFLAGS) -o $@ ocen2.o zbior_ary.o

ocen2.o: makefile ocen2.c zbior_ary.h
	$(CC) $(CFLAGS) -c ocen2.c -o ocen2.o

ocen3: ocen3.o zbior_ary.o
	$(CC) $(CFLAGS) -o $@ ocen3.o zbior_ary.o

ocen3.o: makefile ocen3.c zbior_ary.h
	$(CC) $(CFLAGS) -c ocen3.c -o ocen3.o

zbior_ary.o: makefile zbior_ary.c zbior_ary.h
	$(CC) $(CFLAGS) -c zbior_ary.c -o zbior_ary.o

clean:
	rm -f *.o $(TARGET)
