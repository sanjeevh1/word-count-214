#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>


/* TODO add argument for arraylist/BST */
void process_file(char *pathname) {
	struct stat *stat_data;
	int r = stat(pathname, stat_data);
	if (r != 0) {
		perror(pathname);
	} else if (pathname[0] != '.') {
		if (S_ISREG(stat_data->st_mode) && (strcmp(".txt", pathname + strlen(pathname) - strlen(".txt")) == 0)) {
			count_words(pathname);
	    } else if (S_ISDIR(stat_data->st_mode)) {
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
			    }
				r = closedir(dirp);
				if (r != 0) {
					perror(pathname);
				}
		    }
		}
	}     
}
