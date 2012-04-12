#define _POSIX_C_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "asarg.h"

#include "mini.h"

static FILE *use(int fd, FILE *standard_stream, const char *mode){
	if(fd != -1 && fd == fileno(standard_stream)){
		return standard_stream;
	}else{
		FILE *result = fdopen(fd, mode);
		if(result == 0){
			fprintf(stderr, "mini: %d: cannot use file descriptor: %s\n", fd, strerror(errno));
			exit(EXIT_FAILURE);
		}
		return result;
	}
}

int main(int argc, char *argv[]){
	long in_fd = fileno(stdin); /* TO.DO: Разобраться с int vs long */
	long out_fd = fileno(stdout);
	long err_fd = fileno(stderr);

	asarg(&argv, 0, "IN-FD OUT-FD ERR-FD");
	asarg_oper(&argv, &in_fd,  asarg_long, 1);
	asarg_oper(&argv, &out_fd, asarg_long, 1);
	asarg_oper(&argv, &err_fd, asarg_long, 1);
	asarg_end(&argv);

	mini(use((int)in_fd, stdin, "r"), use((int)out_fd, stdout, "w"), use((int)err_fd, stderr, "w"));

	return 0;
}
