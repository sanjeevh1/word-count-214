CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined

words: words.o arraylist.o
	$(CC) $(CFLAGS) words.o arraylist.o -o words

words.o: words.c arraylist.h
	$(CC) $(CFLAGS) -c words.c

arraylist.o: arraylist.c arraylist.h
	$(CC) $(CFLAGS) -c arraylist.c
