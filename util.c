#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>

#define BUF_SIZE (32 * 1024) // Buffer size for file copying. 32 Kib because Gnulib module `copy-file' uses this size

/* ^_^ */

// TO.DO: Не работает с сокетами/небл. сокетами и т д
int mini_cat(int src, int dest){
	for(;;){
		char buf[BUF_SIZE];

		ssize_t size = read(src, buf, BUF_SIZE);

		if(size == 0){
			break;
		}

		if(size == -1){
			fprintf(stderr, "mini: mini_cat: read(...) failed\n");
			return -1;
		}

		ssize_t write_result = write(dest, buf, size);

		if(write_result == -1){
			fprintf(stderr, "mini: mini_cat: write(...) failed\n");
			return -1;
		}

		if(write_result != size){
			fprintf(stderr, "mini: mini_cat: write(...) didn't write all\n");
			return -1;
		}
	}

	return 0;
}
