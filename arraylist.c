#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "arraylist.h"

void al_init(array_t *L, unsigned cap)
{
    L->length = 0;
    L->capacity = cap;
    L->data = malloc(sizeof(word_t) * cap);
}

void al_destroy(array_t *L)
{
    for (unsigned i = 0; i < L->length; i++) {
        free(L->data[i].word);
    }
    free(L->data);
}

void al_append(array_t *L, word_t item)
{
    if (L->length == L->capacity) {
        L->capacity *= 2;
        L->data = realloc(L->data, L->capacity * sizeof(word_t));
    }

    L->data[L->length] = item;
    L->length++;
}

int wordcmp(const void *a, const void *b)
{
	word_t word_1 = *(word_t *) a;
	word_t word_2 = *(word_t *) b;
	if (word_1.count < word_2.count) {
		return 1;
	}
	if (word_1.count > word_2.count) {
		return -1;
	}
	return strcmp(word_1.word, word_2.word);
}
