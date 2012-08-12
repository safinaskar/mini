#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#define BUF_SIZE (32 * 1024) // Buffer size for file copying. 32 KiB because Gnulib module `copy-file' uses this size

/* ^_^ */

// TO.DO: Не работает с сокетами/небл. сокетами и т д
/* Написано на C89, чтобы можно было легко копировать куда-нибудь */
int mini_cat(int src, int dest){
	for(;;){
		char buf[BUF_SIZE];
		ssize_t size;
		ssize_t write_result;

		size = read(src, buf, BUF_SIZE);

		if(size == 0){
			break;
		}

		if(size == -1){
			perror("mini: mini_cat: read(...) failed");
			return -1;
		}

		write_result = write(dest, buf, size);

		if(write_result == -1){
			perror("mini: mini_cat: write(...) failed");
			return -1;
		}

		if(write_result != size){
			fprintf(stderr, "mini: mini_cat: write(...): data is written partially\n");
			return -1;
		}
	}

	return 0;
}
