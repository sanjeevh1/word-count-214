#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include "arraylist.h"

#ifndef BUFSIZE
#define BUFSIZE 8
#endif

#ifndef DEBUG
#define DEBUG 0
#endif

// void process_file(char *pathname) {
//     struct stat stat_data;
//     int r = stat(pathname, &stat_data);
//     if (r != 0) {
//         perror(pathname);
//     } else {
//         int i = strlen(pathname) - 1;
//         while (i >= 0 && pathname[i] != '/') {
//             i--;
//         }
//         if (pathname[i + 1] != '.') { 
//             if (S_ISREG(stat_data.st_mode) && (strcmp(".txt", pathname + strlen(pathname) - strlen(".txt")) == 0)) {
//                 int fd = open(pathname, O_RDONLY);
//                 if (fd == -1) {
//                     perror(pathname);
//                 } else {
//                     count_words(fd);
//                     r = close(fd);
//                     if(r != 0) {
//                         perror(pathname);
//                     }
//                 }
//             } else if (S_ISDIR(stat_data.st_mode)) {
//                 DIR *dirp = opendir(pathname);
//                 if (dirp == NULL) {
//                     perror(pathname);
//                 } else {
//                     struct dirent *direntp;
//                     while ((direntp = readdir(dirp)) != NULL) {
//                         char *new_path = malloc(strlen(pathname) + strlen(direntp->d_name) + 2);
//                         strcpy(new_path, pathname);
//                         strcat(new_path, "/");
//                         strcat(new_path, direntp->d_name);
//                         process_file(new_path);
//                         free(new_path);
//                     }
//                     r = closedir(dirp);
//                     if (r != 0) {
//                         perror(pathname);
//                     }
//                 }
//             }
//         }
//     }     
// }

void get_words(int fd, void (*use_line)(array_t *arg, char *line), array_t *arg)
{
    int bytes, wordstart, pos;
    int bufsize = BUFSIZE;
    char *buf = malloc(BUFSIZE);

    pos = 0;
    wordstart = 0;
    while ((bytes = read(fd, buf + pos, bufsize - pos)) > 0) {
        if (DEBUG) printf("read %d bytes\n", bytes);
        int bufend = pos + bytes;
        for ( ; pos < bufend; pos++) {
            if (DEBUG > 1) printf("%d/%d/%d: '%c'\n", wordstart, pos, bufend, buf[pos]);
            // if (isspace(buf[pos])) {
            if (isspace(buf[pos])) {
                if (DEBUG) printf("line in bytes %d to %d\n", wordstart, pos);
                buf[pos] = '\0';
                use_line(arg, buf + wordstart);
                wordstart = pos + 1;
            }
        }
        if (wordstart == pos) {
            // read a complete line
            pos = 0;
        } else if (wordstart > 0) {
            // move partial line at end of buffer to start of buffer
            int seglength = pos - wordstart;
            memmove(buf, buf + wordstart, seglength);
            pos = seglength;
            if (DEBUG) printf("Moved %d bytes\n", seglength);
        } else if (pos == bufsize) {
            // line is larger than buffer
            bufsize *= 2;
            buf = realloc(buf, bufsize);
            if (DEBUG) printf("Expanded buffer to %d\n", bufsize);
        }
        wordstart = 0;
    }
    if (DEBUG) printf("read returned %d, linestart %d pos %d\n", bytes, wordstart, pos);
    if (wordstart < pos) {
        // incomplete line at end of file/before error
        if (pos == bufsize) {
        buf = realloc(buf, bufsize + 1);
        }
        buf[pos] = '\0';
        use_line(arg, buf + wordstart);
    }
    free(buf);
}

char *my_strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

void number_words(array_t *st, char *word) {

    for (int i = 0; i < st->length; i++) {
        if (strcmp(st->data[i].word, word) == 0) {
            st->data[i].count++;
            return;
        }
    }
    
    word_t new_word;
    new_word.word = my_strdup(word);
    new_word.count = 1;
    al_append(st, new_word);
}

int main(int argc, char **argv)
{
    int fd;
    if (argc < 2) {
        fd = STDIN_FILENO;
    } else {
        fd = open(argv[1], O_RDONLY);
        if (fd < 0) {
            perror(argv[1]);
            return EXIT_FAILURE;
        }
    }
    array_t words;
    al_init(&words, 8);
    get_words(fd, number_words, &words);
    // print out words
    for (int i = 0; i < words.length; i++) {
        printf("%s %d\n", words.data[i].word, words.data[i].count);
    }
    al_destroy(&words);
    return EXIT_SUCCESS;
}