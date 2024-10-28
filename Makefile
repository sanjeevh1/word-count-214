CC = gcc
CFLAGS = -std=c99 -Wall -Wvla -fsanitize=address,undefined

words: words.c
	$(CC) $(CFLAGS) words.c -o words
