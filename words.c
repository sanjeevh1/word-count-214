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

void get_word(int fd, void (*use_word)(array_t *arg, char *word), array_t *arg)
{
    int bytes, wordstart, pos;
    int bufsize = BUFSIZE;
    char prev_char, curr_char, next_char;
    char *curr_word;
    char *buf = malloc(BUFSIZE);

    pos = 0;
    wordstart = 0;
	int hyphen_flag = 0;
    while ((bytes = read(fd, buf + pos, bufsize - pos)) > 0) {
        int bufend = pos + bytes;
		if(hyphen_flag) {
			pos--;
			hyphen_flag = 0;
		}
        for ( ; pos < bufend; pos++) {
            curr_char = buf[pos];
            // Hyphen case
            if (curr_char == '-') {
                // Ensure next_char is within buffer limits
				if (pos == 0) {
					wordstart = pos + 1;
                } else if(pos + 1 < bufend) { 
					prev_char = buf[pos-1];
					next_char = buf[pos+1];
                    if (isalpha(next_char) && isalpha(prev_char)) {
                        continue;
                    } else {
                        buf[pos] = '\0';
                        curr_word = buf + wordstart;
                        if (isalpha(*curr_word) || *curr_word == '\'') {
                            use_word(arg, curr_word);
                        }
                        wordstart = pos + 1;
                    }
                } else {
					prev_char = buf[pos - 1];
					if (isalpha(prev_char)) {
						hyphen_flag = 1;
					} else {
						buf[pos] = '\0';
						curr_word = buf + wordstart;
						if (isalpha(*curr_word) || *curr_word == '\'') {
							use_word(arg, curr_word);
						}
					}
            	}
			// Check for punctuation, space, and numbers
            } else if (isspace(curr_char) || 
                isdigit(curr_char) || 
                (ispunct(curr_char) && curr_char != '\'')) {
                buf[pos] = '\0';
                curr_word = buf + wordstart;
                if (isalpha(*curr_word) || *curr_word == '\'') {
                    use_word(arg, curr_word);
                }
                wordstart = pos + 1;
            }
        }
        if (wordstart == pos) {
            // read a word
            pos = 0;
        } else if (wordstart > 0) {
            // move partial word at end of buffer to start of buffer
            int seglength = pos - wordstart;
            memmove(buf, buf + wordstart, seglength);
            pos = seglength;
        } else if (pos == bufsize) {
            // word is larger than buffer
            bufsize *= 2;
            buf = realloc(buf, bufsize);
        }
        wordstart = 0;
    }
    if (wordstart < pos) {
        // incomplete word at end of file/before error
        if (pos == bufsize) {
        buf = realloc(buf, bufsize + 1);
        }
        buf[pos] = '\0';
        use_word(arg, buf + wordstart);
    }
    free(buf);
}

char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *copy = malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}

void count_word(array_t *st, char *word) {

    for (int i = 0; i < st->length; i++) {
        if (strcmp(st->data[i].word, word) == 0) {
            st->data[i].count++;
            return;
        }
    }
    
    word_t new_word;
    new_word.word = strdup(word);
    new_word.count = 1;
    al_append(st, new_word);
}

int valid(char *pathname, char *filename) {
    struct stat stat_data;
    int r = stat(pathname, &stat_data);
    if (r != 0) {
        perror(pathname);
        return 0;
    } else if (filename[0] == '.') {
        return 0;
    } else if (S_ISREG(stat_data.st_mode)) {
        return strcmp(".txt", pathname + strlen(pathname) - strlen(".txt")) == 0;
    } else if (S_ISDIR(stat_data.st_mode)) {
        return 1;
    }
    return 0;
}
             

void process_file(char *pathname, void (*use_word)(array_t *arg, char *line), array_t *arg) {
    struct stat stat_data;
    int r = stat(pathname, &stat_data);
    if (r != 0) {
        perror(pathname);
    } else { 
        if (S_ISREG(stat_data.st_mode)) {
            int fd = open(pathname, O_RDONLY);
            if (fd == -1) {
                perror(pathname);
            } else {
                get_word(fd, use_word, arg);
                r = close(fd);
                if(r != 0) {
                    perror(pathname);
                }
            }
        } else if (S_ISDIR(stat_data.st_mode)) {
            DIR *dirp = opendir(pathname);
            if (dirp == NULL) {
                perror(pathname);
            } else {
                struct dirent *direntp;
                int pathlen = strlen(pathname);
                while ((direntp = readdir(dirp)) != NULL) {
                    int namelen = strlen(direntp->d_name);
                    char *new_path = malloc(pathlen + 1 + namelen + 1);
                    memcpy(new_path, pathname, pathlen);
                    new_path[pathlen] = '/';
                    memcpy(new_path + pathlen + 1, direntp->d_name, namelen + 1);
                    if(valid(new_path, direntp->d_name)) {
                        process_file(new_path, use_word, arg);
                    }
                    free(new_path);
                }
                r = closedir(dirp);
                if (r != 0) {
                    perror(pathname);
                }
            }
        }
    }     
}

int main(int argc, char **argv)
{
    if (argc >= 2) {
        array_t words;
        al_init(&words, 8);
        for (int i = 1; i < argc; i++) {
            process_file(argv[i], count_word, &words);
        }
        for (int i = 0; i < words.length; i++) {
            printf("%s %d\n", words.data[i].word, words.data[i].count);
        }
        al_destroy(&words);
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "./words <FILE_PATH>\n");
        return EXIT_FAILURE;
    }
}
