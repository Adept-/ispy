#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include <stdio.h>
#include <unistd.h>
#include "ispy.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_LEN (10 * sizeof(struct inotify_event) + NAME_MAX + 1)

int inotify_fd, eventmask; /* Need access in dir() and main() */


/* This function is called by ftw() for each file it finds in a given dir */
int dir(const char *fpath, const struct stat *sb, int typeflag)
{
	int wd = 0;
	struct pathkey *pk;
	if(typeflag == FTW_D){

		if((wd = inotify_add_watch(inotify_fd, fpath, eventmask)) == -1){
			printf("Error adding %s to watch list, abort\n", fpath);
			return -1;
		}

		if((pk = add_path_key(fpath, wd)) == NULL){
			printf("Error adding pathkey\n");
			return -1;
		}

		printf("Adding %s\n", fpath);
		return 0;
	}
	return 0;
}

int main(int argc, char *argv[])
{

	char buf[BUF_LEN]; /* Buffer for use in read() */
	char *p; /* Temporary pointer */ 
	struct inotify_event *event; /* Inotify events */
	int numread, wd; /* watch discriptor and integer to hold size read */
        struct pathkey *pk; /* Path Keys contain a watch discriptor as a key and path has its value */
	char *newpath = malloc(NAME_MAX + 1); /* Build new paths to add to watch list */

	/* setup the bitmask to indicate which events to watch for */
	eventmask = IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO;

	/* Make sure the user entered a directory to monitor */
	if(argc < 2){
		printf("USAGE: %s directory\n", argv[0]);
		return -1;
	}

	/* Initialize the queue or quit on error */
	if((inotify_fd = inotify_init()) == -1){
		
		return -1;
	}

	/* Crawl the given directory adding all directories and sub directories
	 * to the watch list */
	if(ftw(argv[1], dir, 10) == -1){
		printf("Error crawling directory aborting\n");
		return -1; 
	}

	/* Start infinite loop to read from the inotify que. When there is
	 * an event, print the event. If a directory was added or deleted
	 * add or remove it from the inotify que */
		for(;;){

		/* Read from the inotify FD, if -1 or 0 something is wrong abort */
		if((numread=read(inotify_fd, buf, BUF_LEN)) == -1 || numread == 0){
			printf("Error reading from stream aborting\n");
			return -1;
		}

		/* increment through all the events in the buffer and log all events. 
		 * If directories are added or deleted add or delete them from the watch list*/
		for(p = buf; p < buf + numread; ){
			event = (struct inotify_event *) p;
			if(event->mask & IN_CREATE){
				/*Log the event to console*/
				printf("Event: IN_CREATE\n");
				printf("Name: %s\n", event->name);
				printf("\n");

				/*If the new file being created is a dir, add it to the watch list
				 * and add a new path key*/
				if(event->mask & IN_ISDIR){

					/* Find the path key associated with event. Use the path from the
					 * key and the name in the event to build a new path to monitor */
					if((pk = search_path_key(event->wd)) == NULL){
						printf("Pathkey not found aborting\n");
						return -1;
					}

					/* construct new path to monitor */
					newpath = memset(newpath, 0x00, (NAME_MAX + 1));
					strcat(newpath, pk->path);
					strcat(newpath, "/");
					strcat(newpath, event->name);

					/* add the new path to the watch queue and add a new pathkey */
					if((wd = inotify_add_watch(inotify_fd,newpath,eventmask)) == -1 ){
							printf("Error adding dir to que aborting\n");
							return -1;
					}

					if((pk = add_path_key(newpath, wd)) == NULL){
						printf("Error adding new pathkey aborting\n");
						return -1;
					}
				}	


			}
		
			
			if(event->mask & IN_DELETE){
				printf("Event: IN_DELETE\n");
				printf("Name: %s\n", event->name);
				printf("\n");

				/*If the file being deleted is a dir remove its path key */
				if(event->mask & IN_ISDIR){

					struct pathkey *pk;
					if((pk = search_path_key(event->wd)) == NULL){
						printf("Error getting path key aboting\n");
						return -1;
					}

					/* Build the path to the directory we want to remove */
					newpath = memset(newpath, 0x00, (NAME_MAX + 1));
					strcat(newpath, pk->path);
					strcat(newpath, "/");
					strcat(newpath, event->name);
					
					struct pathkey *subdir;
					if((subdir = search_path(newpath)) == NULL){
						printf("Can't find path key aborting\n");
						return -1;
					}
					
					if(rm_path_key(subdir->wd) == -1){
						printf("Error removing path key for deleted directory\n");
						return -1;
					}
			        }
			}

			if(event->mask & IN_MOVED_FROM){
				printf("Event: IN_MOVED_FROM\n");
				printf("Name: %s\n", event->name);
				printf("\n");
			}
			if(event->mask & IN_MOVED_TO){
				printf("Event: IN_MOVED_T\n");
				printf("Name: %s\n", event->name);
				printf("\n");
			}
			p += sizeof(struct inotify_event) + event->len;
		}
	}
		free(newpath);
		return 0;
}
