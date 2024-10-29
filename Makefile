CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined

words: words.c arraylist.c
	$(CC) $(CFLAGS) words.c arraylist.c -o words
