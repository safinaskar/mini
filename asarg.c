/*  asarg.c - main file of the Asarg Library

    Copyright (C) 2012  Askar Safin <safinaskar@mail.ru>

    This file is part of the Asarg Library
    This is snapshot of the library. See lastest version of the library on https://github.com/safinaskar/asarg

    The Asarg Library is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    The Asarg Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

/* Features (markdown) (устарело). TODO: дописать и вставить в доки!
                                                                        asarg getopt argp
**** Возможности, связанные с удобством пользоавния библиотекой
** В качестве действия, выполняющегося при задании опции, можно задать
Возврат определённого значения из библиотеки


Параметры                                                               No    Yes
Необязательные параметры                                                -     Yes
Устаревший стиль параметров (например, `patch -p1`)                     -     ?
Аргумент `--`                                                           Yes   Yes

Длинные опции                                                           Yes   Yes
Параметры у длинных опций вида `--prefix=/usr`                          No    Yes
Параметры у длинных опций вида `--prefix /usr`                          No    Yes

a = b (ptr, val)                                                        Yes   Yes

--help (yes, no, opt)                                                   Opt   No
--usage (yes, no, opt)                                                  Opt   No
--version (yes, no, opt)                                                No    No

help2man работает                                                       ?     -      ?
Несколько форм запуска                                                  No    -      Yes

Парсинг операндов                                                       Yes   No
Самостоятельный парсинг операндов                                       Yes   -

Целые и вещественные типы                                               Yes   No
Тип char                                                                Yes   No

Алиасы                                                                  Yes   -
Сообщения об ошибках                                                    Yes   Yes
Отключение сообщений об ошибках                                         No    Yes

Любой порядок (не буду реализовывать)                                   No    Yes
Поток-save                                                              Yes   No
Legacy-style                                                            No    Yes    Yes
Языки                                                                   No    Yes
*/

/* Alternatives: TODO: дописать чуть-чуть и вставить в доки! написать, что моя либа круче их всех. написать, что она полнофункциональна в большинстве случаев и почти не уступает argp из libc
Lang    Name                   Debian developer package      URL
sh      getopt                 util-linux                    ?
sh      getopts                (any shell)                   -
C       getopt                 libc-dev                      http://www.gnu.org/software/libc/
C       argp                   libc-dev                      http://www.gnu.org/software/libc/
C       popt                   libpopt-dev                   http://directory.fsf.org/project/popt
C++     tclap                  libtclap-dev                  http://tclap.sourceforge.net/
C       argtable               libargtable2-dev              http://argtable.sourceforge.net/
C       libcfg+                                              http://platon.sk/projects/main_page.php?project_id=3
C++     getpot                                               http://getpot.sourceforge.net/
C++     options                                              http://www.cmcrossroads.com/bradapp/ftp/src/libs/C++/Options.html
C++/sh  cmdline                                              http://www.cmcrossroads.com/bradapp/ftp/src/libs/C++/CmdLine.html
C++     boost program options  libboost-program-options-dev  http://www.boost.org/
C       optlist                                              http://michael.dipperstein.com/optlist/
C       gopt                                                 http://www.purposeful.co.uk/software/gopt/
C++     getoptpp                                             http://code.google.com/p/getoptpp/
C++     clp-parser                                           http://sourceforge.net/projects/clp-parser/
C++     qgetopts                                             http://code.google.com/p/qgetopts/
gen C   gengetopt              gengetopt                     http://www.gnu.org/software/gengetopt/gengetopt.html (в принципе, можно (точно?))
C++     libgetopt++            libgetopt++-dev               ?
C       libslack                                             http://libslack.org/
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "asarg.h"

static const char *argv_0;

static void my_err(const char *format, ...){
	int saved_errno = errno;

	fflush(stdout);
	fprintf(stderr, "%s: ", argv_0);

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fprintf(stderr, ": %s\n", strerror(saved_errno));
	exit(EXIT_FAILURE);
}

static void my_errx(const char *format, ...){
	fflush(stdout);
	fprintf(stderr, "%s: ", argv_0);

	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

void asarg_help(int unused){
}

// Print help message. opts and opers_doc are from `asarg' function arguments
static void help(const struct asarg_opt *opts, const char *opers_doc){
	printf("Usage: %s [OPTION]... %s\n\n", argv_0, opers_doc);

	for(const struct asarg_opt *opt = opts; opt->short_names != 0 || opt->long_names != 0; ++opt){
		int short_len = opt->short_names ? strlen(opt->short_names) : 0;
		int long_len  = opt->long_names  ? strlen(opt->long_names ) : 0;
		int param_len = opt->param_doc   ? strlen(opt->param_doc  ) : 0;
		char *buf = (char *)malloc(4 + short_len * 4 + (long_len + 1) / 2 * 5 + 1 + param_len + 1);
		char *buf_end = buf;

		if(buf == 0){
			my_errx("no memory");
		}

		if(opt->short_names == 0 || opt->short_names[0] == 0){
			*buf_end = ' ';
			++buf_end;
			*buf_end = ' ';
			++buf_end;
			*buf_end = ' ';
			++buf_end;
			*buf_end = ' ';
			++buf_end;
		}else{
			const char *c = opt->short_names;
			do{
				*buf_end = '-';
				++buf_end;
				*buf_end = *c;
				++buf_end;
				*buf_end = ',';
				++buf_end;
				*buf_end = ' ';
				++buf_end;
				++c;
			}while(*c != 0);
		}

		if(opt->long_names != 0 && opt->long_names[0] != 0){
			const char *s = opt->long_names;
			for(;;){
				*buf_end = '-';
				++buf_end;
				*buf_end = '-';
				++buf_end;

				while(*s != ' ' && *s != 0){
					*buf_end = *s;
					++buf_end;
					++s;
				}

				*buf_end = ',';
				++buf_end;
				*buf_end = ' ';
				++buf_end;

				if(*s == 0){
					break;
				}
				++s;
			}
		}

		--buf_end; // We don't need last comma
		--buf_end;

		if(opt->param_doc != 0 && opt->param_doc[0] != 0){
			if(opt->long_names != 0 && opt->long_names[0] != 0){
				*buf_end = '=';
			}else{
				*buf_end = ' ';
			}
			++buf_end;

			memcpy(buf_end, opt->param_doc, strlen(opt->param_doc));
			buf_end += strlen(opt->param_doc);
		}

		*buf_end = 0;

		const char *doc;

		if(opt->doc == 0){
			if(opt->func == &asarg_help){
				doc = "display this help and exit";
			}else{
				doc = "";
			}
		}else{
			doc = opt->doc;
		}

		printf("  %-25s  %s\n", buf, doc);
		free(buf);
	}
}

static void str2any(void *ptr, const char *str, enum asarg_type type){
	if(ptr == 0){
		return;
	}

	switch(type){
		case asarg_none:
			break;
		case asarg_char:
			if(str[0] == 0 || str[1] != 0){
				my_errx("%s: not a charaster", str);
			}
			*(char *)ptr = str[0];
			break;
		case asarg_long:
		case asarg_ulong:
		case asarg_llong:
		case asarg_ullong:
		case asarg_float:
		case asarg_double:
		case asarg_ldouble:
			{
				errno = 0;
				char *endptr;
				switch(type){
					case asarg_long:
						*(long               *)ptr = strtol  (str, &endptr, 0);
						break;
					case asarg_ulong:
						*(unsigned long      *)ptr = strtoul (str, &endptr, 0);
						break;
					case asarg_llong:
						*(long long          *)ptr = strtoll (str, &endptr, 0);
						break;
					case asarg_ullong:
						*(unsigned long long *)ptr = strtoull(str, &endptr, 0);
						break;

					case asarg_float:
						*(float              *)ptr = strtof  (str, &endptr);
						break;
					case asarg_double:
						*(double             *)ptr = strtod  (str, &endptr);
						break;
					case asarg_ldouble:
						*(long double        *)ptr = strtold (str, &endptr);
						break;
					default: // To avoid compiler warnings
						;
				}
				if(errno != 0){ // Out of range
					my_err("%s", str);
				}
				// If *endptr != 0, there are extra charasters after number
				// If str[0] == 0, operand is empty
				if(*endptr != 0 || str[0] == 0){
					my_errx("%s: invalid number", str);
				}
			}
			break;
		case asarg_str:
			*(const char **)ptr = str;
			break;
		case asarg_file_in:
			if(strcmp(str, "-") == 0){
				*(FILE **)ptr = stdin;
			}else{
				*(FILE **)ptr = fopen(str, "r");
				if(*(FILE **)ptr == 0){
					my_err("%s", str);
				}
			}
			break;
		case asarg_file_out:
			if(strcmp(str, "-") == 0){
				*(FILE **)ptr = stdout;
			}else{
				*(FILE **)ptr = fopen(str, "w");
				if(*(FILE **)ptr == 0){
					my_err("%s", str);
				}
			}
			break;
	}
}

// Do option action. opts and opers_doc are from `asarg' function arguments
static void process_opt(const struct asarg_opt *opt, const struct asarg_opt *opts, const char *opers_doc){
	if(opt->func == 0){
	}else if(opt->func == &asarg_help){
		help(opts, opers_doc);
		exit(EXIT_SUCCESS);
	}else{
		(*(opt->func))(opt->val);
	}
	if(opt->ptr != 0 && opt->param == asarg_none){
		*(int *)(opt->ptr) = opt->val;
	}
}

// TODO Doc: I don't modify **argvp and ***argvp
void asarg(char ***argvp, const struct asarg_opt *opts, const char *opers_doc){
	const struct asarg_opt default_opts[] = {
		{0, "help", 0, 0, 0, &asarg_help},
		{0}
	};

	argv_0 = (*argvp)[0];

	if(argv_0 == 0){
		argv_0 = "(asarg)";
		fflush(stdout);
		fprintf(stderr, "%s: warning: a NULL argv[0] was passed through an exec system call\n", argv_0);
		fflush(stderr);
		return;
	}

	++*argvp;

	if(opts == 0)opts = default_opts;
	if(opers_doc == 0)opers_doc = "";

	for(;; ++*argvp){
		if(**argvp == 0){ // Empty argument
			break;
		}
		if((**argvp)[0] != '-'){ // Operand
			break;
		}
		if((**argvp)[1] == '-'){ // Long option
			if((**argvp)[2] == 0){ // This is `--'
				++*argvp;
				break;
			}else{
				for(const struct asarg_opt *opt = opts; opt->short_names != 0 || opt->long_names != 0; ++opt){
					if(opt->long_names != 0 && opt->long_names[0] != 0){
						const char *s = opt->long_names;
						for(;;){
							const char *token = s;
							while(*s != ' ' && *s != 0){
								++s;
							}

							int len = s - token;
							if(strncmp(**argvp + 2, token, len) == 0 && ((**argvp)[2 + len] == 0 || (**argvp)[2 + len] == '=')){
								if(opt->param == asarg_none){
									if((**argvp)[2 + len] == '='){
										my_errx("%s: option doesn't require parameter", **argvp);
									}
								}else{
									if((**argvp)[2 + len] == 0){
										if(!opt->optional){
											my_errx("option `%s' requires parameter", **argvp);
										}
									}else{
										str2any(opt->ptr, **argvp + 2 + len + 1, opt->param);
									}
								}
								process_opt(opt, opts, opers_doc);
								goto long_found;
							}

							if(*s == 0){
								break;
							}
							++s;
						}
					}
				}

				my_errx("invalid option `%s'", **argvp);
				long_found:;
			}
		}else{ // Short option or `-'
			const char *c = **argvp + 1; // Current charaster

			if(*c == 0){
				break; // This is `-'
			}

			do{
				for(const struct asarg_opt *opt = opts; opt->short_names != 0 || opt->long_names != 0; ++opt){
					if(opt->short_names != 0 && strchr(opt->short_names, *c) != 0){
						if(opt->param != asarg_none){
							if(*(c + 1) != 0 || *(*argvp + 1) == 0){
								my_errx("missing parameter of option `%c'", *c);
							}
							str2any(opt->ptr, *(*argvp + 1), opt->param);
							++*argvp;
						}
						process_opt(opt, opts, opers_doc);
						goto short_found;
					}
				}
				my_errx("invalid option `%c'", *c);
				short_found:;
				++c;
			}while(*c != 0);
		}
	}
}

// TODO Doc: I don't modify **argvp and ***argvp
void asarg_oper(char ***argvp, void *ptr, enum asarg_type type, int /* bool */ optional){
	if(**argvp == 0){
		if(!optional){
			my_errx("missing operand");
		}
	}else{
		str2any(ptr, **argvp, type);
		++*argvp;
	}
}

// TODO Doc: I don't modify anything
void asarg_end(char **const *argvp){
	if(**argvp != 0){
		my_errx("extra operand: %s", **argvp);
	}
}
