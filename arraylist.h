typedef struct {
    char* word;
    int count;
} word_t;

typedef struct {
    unsigned length;
    unsigned capacity;
    word_t *data;
} array_t;

void al_init(array_t *, unsigned);
void al_destroy(array_t *);

void al_append(array_t *, word_t);
