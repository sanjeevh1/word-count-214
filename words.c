#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>



void process_file(char *pathname) {
	struct stat stat_data;
	int r = stat(pathname, &stat_data);
	if (r != 0) {
		perror(pathname);
	} else {
		int i = strlen(pathname) - 1;
		while (i >= 0 && pathname[i] != '/') {
			i--;
		}
		if (pathname[i + 1] != '.') { 
			if (S_ISREG(stat_data.st_mode) && (strcmp(".txt", pathname + strlen(pathname) - strlen(".txt")) == 0)) {
				int fd = open(pathname, O_RDONLY);
				if (fd == -1) {
					perror(pathname);
				} else {
					count_words(fd);
				}
				r = close(fd);
				if(r != 0) {
					perror(pathname);
				}
	    	} else if (S_ISDIR(stat_data.st_mode)) {
		    	DIR *dirp = opendir(pathname);
		    	if (dirp == NULL) {
		    		perror(pathname);
		    	} else {
			    	struct dirent *direntp;
			    	while ((direntp = readdir(dirp)) != NULL) {
				    	char *new_path = malloc(strlen(pathname) + strlen(direntp->d_name) + 2);
				    	strcpy(new_path, pathname);
						strcat(new_path, "/");
				    	strcat(new_path, direntp->d_name);
				    	process_file(new_path);
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
}
