// IPv4 for listen, IPv4 and IPv6 for connect, TCP, блокирующиеся сокеты, C99/C++
// Если часто коннектиться и отправлять небольшие пакеты данных, то пакеты могут придти в неправильном порядке на локальной машине
// При установке своего обработчика какого-либо сигнала, нужно позаботиться о том, чтобы сигнал не прерывал сисвызов
// Порты могут закончиться

// TO.DO MINI: Добавить метакоманды, соответствующие этим функциям и другим таким же (например, из util.c)?

#define _POSIX_C_SOURCE 200112L // OK

/* Headers for core network code */
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

/* Additional headers for Mini */
#include <stdio.h>
#include <stdarg.h>

#define tcp_warn(format...) _tcp_warn(__func__, format)
#define tcp_warnx(format...) _tcp_warnx(__func__, format)

static void _tcp_warn(const char *func, const char *format, ...){
	int saved_errno = errno;

	fflush(stdout);
	fprintf(stderr, "mini: %s: ", func);

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fprintf(stderr, ": %s\n", strerror(saved_errno));
}

static void _tcp_warnx(const char *func, const char *format, ...){
	fflush(stdout);
	fprintf(stderr, "mini: %s: ", func);

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputc('\n', stderr);
}

/* Написано на C89, чтобы можно было легко копировать куда-нибудь */
int mini_listen(const char *port){
	in_port_t num_port; /* <netinet/in.h> */
	int result;

	{
		char *endptr;

		errno = 0; /* <errno.h> */
		num_port = strtol(port, &endptr, 0); /* <stdlib.h> */

		if(errno != 0){
			tcp_warn("port %s", port);
			return -1;
		}

		if(*endptr != 0 || port[0] == 0){
			tcp_warnx("port %s: invalid number", port);
			return -1;
		}
	}

	result = socket(AF_INET, SOCK_STREAM, 0); /* <sys/socket.h> (в том числе AF_INET и SOCK_STREAM; there is no PF_INET in POSIX.1-2008) */

	if(result == -1){
		tcp_warn("socket(...) failed");
		return -1;
	}

	do{
		{
			const int opt = 1;

			if(setsockopt(result, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1){ /* <sys/socket.h> (в том числе SOL_SOCKET и SO_REUSEADDR) */
				tcp_warn("setsockopt(...) failed");
				break;
			}
		}

		{
			struct sockaddr_in addr; /* <netinet/in.h> */

			memset(&addr, 0, sizeof(struct sockaddr_in)); /* <string.h> */

			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_ANY); /* htonl: <arpa/inet.h>, INADDR_ANY: <netinet/in.h> */
			addr.sin_port = htons(num_port); /* <arpa/inet.h> */

			if(bind(result, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){ /* bind: <sys/socket.h> (в том числе sockaddr) */
				tcp_warn("port %s: bind(...) failed", port);
				break;
			}
		}

		if(listen(result, 5) == -1){ /* <sys/socket.h>, 5 - это backlog. Его нужно правильно выбрать. 5 is common value for backlog */
			tcp_warn("port %s: listen(...) failed", port);
			break;
		}

		return result;
	}while(0);

	if(close(result) == -1){ /* <unistd.h> */
		tcp_warn("port %s: close(...) failed", port);
	}

	return -1;
}

int mini_accept(const char *port){
	int listening_fd = mini_listen(port);

	if(listening_fd == -1){
		return -1;
	}

	int result;

	while((result = accept(listening_fd, 0, 0)) == -1){ // <sys/socket.h>
		tcp_warn("port %s: accept(...) failed, retrying", port);
	}

	if(close(listening_fd) == -1){ // <unistd.h>
		tcp_warn("warning: port %s: close(...) failed, ignoring", port);
	}

	return result;
}

/* Написано на C89, чтобы можно было легко копировать куда-нибудь */
/* TO DO: обязательно заюзать libsh в mini, хотя бы чтобы использовать sh_tcp_connect. После этого слить этот код (находящийся в mini) с таким же кодом из libsh */
int mini_connect(const char *host, const char *port){
	struct addrinfo hints; /* <netdb.h> */
	struct addrinfo *ai;
	int ai_error;
	int result;

	memset(&hints, 0, sizeof(struct addrinfo)); /* <string.h> */

	hints.ai_family = AF_UNSPEC; /* <sys/socket.h> (there is no PF_INET in POSIX.1-2008) */
	hints.ai_socktype = SOCK_STREAM; /* <sys/socket.h> */

	ai_error = getaddrinfo(host, port, &hints, &ai); /* <netdb.h> */

	if(ai_error != 0){
		tcp_warnx("%s:%s: getaddrinfo(...) failed: %s", host, port, gai_strerror(ai_error)); /* <netdb.h> */
		return -1;
	}

	{
		struct addrinfo *curr;

		for(curr = ai; curr != 0; curr = curr->ai_next){
			result = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol); /* <sys/socket.h> */

			if(result == -1){
				continue;
			}

			if(connect(result, curr->ai_addr, curr->ai_addrlen) == 0){ /* <sys/socket.h> */
				freeaddrinfo(ai); /* <netdb.h> */
				return result;
			}
		}
	}

	tcp_warn("%s:%s: all addresses returned by getaddrinfo failed, last socket(...) or connect(...) failed with the following reason", host, port); /* После "the following reason" будет добавлен strerror */

	close(result); /* <unistd.h> */
	freeaddrinfo(ai);

	return -1;
}
