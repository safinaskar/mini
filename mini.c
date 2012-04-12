// Первостепенные дела, если я буду публиковать:
	// TO.DO: просто отказаться в пользу c-repl? Дополнить c-repl?
	// TO.DO: проверить на всех нужных платформах наличие (в том числе freestanding) bool. Если есть, то заюзать
	// TO.DO: добавить функцию "без readline"

	// TO.DO: Docs: ОБЯЗАТЕЛЬНО: я хотел добавить простой встроенный интерпретатор, мега-портируемый, в том числе на голое железо, умеющий лишь вызывать функции из hard-coded списка функций, но передумал, так как gdb круче

	// TO.DO: Docs: ОБЯЗАТЕЛЬНО: я хотел добавить glibc://dlfcn/eval.c-подобный, gdb-подобный интерфейс, встроенный простой интерпретатор, умеющий лишь вызывать функции, но произвольные, но передумал, так как дублируется gdb

// TO.DO внедрить все фичи c-repl (docs: спасибо за идею, ссылка; и вообще перечислить в доках альтернативы: gdb, perl и т д)
// TO.DO C-REPL UNDOCUMENTED: давать функциям имена, чтобы из следующих функций их можно было вызывать
// TO.DO: Docs: протестировано на GNU/Linux'е и FreeBSD (TO.DO: надо бы ещё раз протестить, когда прога будет написана)
	// TO.DO: Docs: если solaris /usr/include/dlfcn.h:66:13: error: expected specifier-qualifier-list before `mapobj_result_t', то:
	// typedef struct {
	// 66 -       mmapobj_result_t *dlm_maps;
	// +       struct mmapobj_result_t *dlm_maps;
	//         uint_t            dlm_acnt;
	//         uint_t            dlm_rcnt;
	// } Dl_mapinfo_t;
// TO.DO Docs: W: нужно, чтоб cl был в environment
// TO.DO Docs: W: всё создаётся в текущей папке, она должна быть доступна, также надо убирать/чистить за mini
// TO.DO: Docs: рассказать, рассказать, что эти функции предназначены только для отладки и потому они печатают сообщения об ошибках
// TO.DO Docs: пригодится разработчикам ядер
// TO.DO Docs: всё запускается в рамках одного процесса, то есть getpid выдаёт одно и то же число. Это главное отличие от простого While-Read-Do-Compile-Run
// TO.DO Docs: пример простейшего нетката на mini_connect и т д и mini_cat; код можно выдрать и юзать отдельно
// TO.DO: вкомпилить readline и dl внутрь mini, если это возможно! docs: рассказать, как правильно собирать с либой mini (-lreadline и т д -rdynamic)
// TO.DO: чистить временные файлы при получении сигнала? Или не надо, т. к. это уменьшит свободу выполняемого кода? Если не чистим, то написать об этом в доках
// TO.DO Docs: there is glibc://dlfcn/eval.c
// TO.DO Docs: можно слинковать с мини, вызвать из проги мини, из мини обратно прогу и т д. можно вызвать mini из gdb и наоборот. можно законнектиться к процессу при помощи gdb и сделать mini
// TO.DO Docs: нет, переменные не сохраняются (если это так)
// TO.DO Docs: ОБЯЗАТЕЛЬНО: этот вопрос был задан дважды! Mini не сохраняет прогу, этого можно добиться с помощью tee. Рассказать, как продолжить с заданного места

#ifdef __unix__
# define _POSIX_C_SOURCE 200809L
#elif defined(_WIN32)
# define _UNICODE
# define UNICODE
#else
/* O_o */
# error "This is not UNIX-like OS, nor Windows"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#ifdef __unix__
# include <unistd.h>
# include <sys/wait.h> /* waitpid */
# include <dlfcn.h>
#else
# include <windows.h>
# include <io.h> /* isatty */
# include <tchar.h>
#endif

#ifdef __unix__
# include <readline/readline.h>
# include <readline/history.h>
#endif

#include "getdelim.h"
#include "strtok_r.h"

#include "mini.h"

#ifdef __CYGWIN__ /* Cygwin's standard C library (Newlib) doesn't support new standards yet (as 2012), so we need this hack */
FILE *fdopen(int fd, const char *mode);
int mkstemp(char *template);
int fileno(FILE *stream);
#endif

#if defined(__MINGW32__) && !defined(fileno) /* MinGW has the same problem */
int fileno(FILE *stream);
#endif

#define MAXARGS 31 /* Arguments count for .execvp */

#define DECLARATIONS "int mini_listen(const char *port);\n" \
	"int mini_accept(const char *port);\n" \
	"int mini_connect(const char *host, const char *port);\n" \
	"int mini_cat(int src, int dest);\n"

#define MAX_HEADERS 65536

struct state{
	FILE *in;
	FILE *out;
	FILE *err;
	int /* bool */ done;
	int /* bool */ check_errno;
	char headers[MAX_HEADERS];
	int /* bool */ werror;
};

/*
Usage:

START_EXCEPTIONS
	TRY
		int fd = open("foo", O_RDONLY);
		if(fd == -1)THROW;
	IN
		if(write(fd, "bar", 3) == -1)THROW;
	FINALLY
		if(close(fd) == -1)THROW;
	// CATCH is not required
	END
STOP_EXCEPTIONS

IN and FINALLY sections can view TRY objects
*/

/* I use this variable name to avoid name conflicts. Of course, namespace would be better, but this code is not part of any package. It is just my (Askar Safin's) very own reusable code */

#define START_EXCEPTIONS { int _DtQjw7AVrEPWbB2WOTFHBmIUAa6vx = 0; do{
#define STOP_EXCEPTIONS  }while(0); }

#define TRY              {
#undef  IN /* Some environment has `IN' macro */
#define IN               do{
#define FINALLY          }while(0); {
#define CATCH            } if(_DtQjw7AVrEPWbB2WOTFHBmIUAa6vx){ _DtQjw7AVrEPWbB2WOTFHBmIUAa6vx = 0;
#define END              } if(_DtQjw7AVrEPWbB2WOTFHBmIUAa6vx)break; }

#define THROW            { _DtQjw7AVrEPWbB2WOTFHBmIUAa6vx = 1; break; }

static void my_warn(struct state *st, const char *format, ...){
	int saved_errno = errno;

	fflush(st->out);
	fputs("mini: ", st->err);

	va_list ap;
	va_start(ap, format);
	vfprintf(st->err, format, ap);
	va_end(ap);

	fprintf(st->err, ": %s\n", strerror(saved_errno));
	fflush(st->err);
}

static void my_warnx(struct state *st, const char *format, ...){
	fflush(st->out);
	fputs("mini: ", st->err);

	va_list ap;
	va_start(ap, format);
	vfprintf(st->err, format, ap);
	va_end(ap);

	putc('\n', st->err);
	fflush(st->err);
}

#ifdef __unix__
static int my_dlerror(struct state *st, const char *message){
	char *error = (char *)dlerror(); // OpenBSD's dlerror returns `const char *', so we need to convert

	if(error == 0){
		return 0;
	}else{
		my_warnx(st, "%s: %s", message, error);
		return -1;
	}
}
#endif

int check_errno_func(struct state *st){
	if(st->check_errno && errno != 0){
		my_warn(st, "errno");
		return -1;
	}
	return 0;
}


#ifdef __unix__
# define HANDLE void *
#else
# define HANDLE HMODULE
#endif

static HANDLE load_library(struct state *st, const char *library, int /* bool */ global){
#ifdef __unix__
	// RTLD_NOW, так как я не хочу, чтобы программа падала при undefined symbols
	void *result = dlopen(library, RTLD_NOW | (global ? RTLD_GLOBAL : RTLD_LOCAL));

	if(my_dlerror(st, "dlopen") == -1){
		return 0;
	}

	return result;
#else
	/* LoadLibrary(LPCTSTR); */
	HMODULE result = LoadLibrary(TEXT(".\\mini-shared.dll")); /* TO.DO W: в винде не работает .l. исправить */

	if(result == 0){
		my_warnx(st, "cannot load library"); /* TO.DO W: more detail */
		return 0;
	}

	return result;
#endif
}

static int do_eval(struct state *st, const char *code){
	int result = -1;

	if(*(st->headers) == 0){
		strcpy(st->headers,
			"#include <stdio.h>\n"
		);
	}

	START_EXCEPTIONS
		TRY /* Source file */
			FILE *source_fp;

#ifdef __unix__
			char source[] = "/tmp/mini-source-XXXXXX";

			{
				int source_fd = mkstemp(source);

				if(source_fd == -1){
					my_warn(st, "%s: cannot create temporary source file; please try to remount /tmp filesystem read-write or mount tmpfs, for example \".system mount -o remount,rw /\"", source); /* TO.DO: совет portable? */
					THROW;
				}

				source_fp = fdopen(source_fd, "w");

				if(source_fp == 0){
					my_warn(st, "%s: cannot fdopen temporary source file", source);
					close(source_fd);
					remove(source);
					THROW;
				}
			}
#else
			char source[] = "mini-source.c"; // TO.DO W: make better
			source_fp = fopen(source, "w");

			if(source_fp == 0){
				my_warn(st, "%s: cannot create temporary source file", source);
				THROW;
			}
#endif
		IN /* Source file */
			TRY /* Variable `source_fp' */
			IN /* Variable `source_fp' */
				if(fprintf(source_fp,
					"%s"
					DECLARATIONS
#ifndef __unix__
					"__declspec(dllexport) "
#endif
					"void mini_command(void){\n"
					"%s;\n" /* Я сделал это отдельной строчкой, потому что "code" может начинаться на "//" */
					"}\n", st->headers, code) < 0){

					my_warn(st, "%s: cannot write to temporary source file", source);
					THROW;
				}
			FINALLY /* Variable `source_fp' */
				if(fclose(source_fp) == EOF){
					my_warn(st, "%s: cannot close temporary source file", source);
					THROW;
				}
			END /* Variable `source_fp' */

			TRY /* Shared object file */
#ifdef __CYGWIN__
				char shared[] = "/tmp/mini-shared.dll"; /* TO.DO W: make better */
#elif defined(__unix__)
				char shared[] = "/tmp/mini-shared-XXXXXX";
#else
				char shared[] = ".\\mini-shared.dll"; /* TO.DO W: make better */
#endif

#if defined(__CYGWIN__) || !defined(__unix__)
				{
					FILE *shared_fp = fopen(shared, "w");

					if(shared_fp == 0){
						my_warn(st, "%s: cannot create temporary shared object file (DLL, dynamic-link library)", shared);
						THROW;
					}

					if(fclose(shared_fp) == EOF){
						my_warn(st, "%s: cannot close temporary shared object file (DLL, dynamic-link library)", shared);
						THROW;
					}
				}
#else
				{
					int shared_fd = mkstemp(shared);

					if(shared_fd == -1){
						my_warn(st, "%s: cannot create temporary shared object file", shared);
						THROW;
					}

					if(close(shared_fd) == -1){
						my_warn(st, "%s: cannot close temporary shared object file", shared);
						THROW;
					}
				}
#endif
			IN /* Shared object file */
				TRY /* Compiler process */
#ifdef __unix__
// TO.DO: fflush всё перед форком и т. д., вывод gcc куда надо
					pid_t cc = fork();

					/* We cannot use `switch' here because we want THROW to work */
					if(cc == -1){
						my_warn(st, "cannot fork");
						THROW;
					}else if(cc == 0){
						/* Child */
						execlp("gcc", "gcc", "-shared", "-std=gnu99",

						       /* Warnings: */ /* TO.DO Docs: тщательно подобраны варнинги и -Werror. Например, printf(5) теперь не вызывает segfault. Остальные варнинги не включены, так как это позволяет чувствовать себя свободно при написании кода */
							st->werror ? "-Werror" : "-shared",

							"-Wformat",
							"-Wno-format-zero-length",
							"-Wnonnull",
							"-Wimplicit-function-declaration",
							"-Wdiv-by-zero",
#ifndef __CYGWIN__
						       "-fPIC", /* TO.DO: who uses and who don't uses -fPIC? Should I use cc or gcc? Same for other options */
#endif
							"-o", shared, "-x", "c", source, (char *)0);

						my_warn(st, "cc; please make sure you have \"cc\" in your PATH, for example: \".setenv PATH /usr/sbin:/usr/bin:/sbin:/bin\"");
						exit(EXIT_FAILURE);
					}
#else
					STARTUPINFO si;
					PROCESS_INFORMATION pi;

					memset(&si, 0, sizeof(si));
					si.cb = sizeof(si);
					memset(&pi, 0, sizeof(pi));

					/* TO.DO W: Чистить за компилятором лишние файлы */
					/* /LD - create DLL */
					/* /nologo - diable copyright info */
					/* CreateProcess(LPCTSTR, LPTSTR, ...); */
					TCHAR command[] = TEXT("\"cl\" /LD /nologo /Femini-shared mini-source.c");
					if(!CreateProcess(0, command, 0, 0, FALSE, 0, 0, 0, &si, &pi)){
						my_warnx(st, "cannot create process; please, make sure you have \"cl\" in your %%path%%"); /* TO.DO W: more detail */
						THROW;
					}
#endif
				IN /* Compiler process */
					/* Parent */
#ifdef __unix__
					int status;

					if(waitpid(cc, &status, 0) == -1){
						my_warn(st, "cc: cannot waitpid");
						THROW;
					}

					if(! WIFEXITED(status) || WEXITSTATUS(status) != 0){
						my_warnx(st, "cannot compile");
						THROW;
					}
#else
					if(WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0){
						my_warnx(st, "cl: cannot WaitForSingleObject"); /* TO.DO W: more detail */
						THROW;
					}

					DWORD status;

					if(!GetExitCodeProcess(pi.hProcess, &status)){
						my_warn(st, "cl: cannot get exit code"); /* TO.DO W: more detail */
						THROW;
					}

					if(status != 0){
						my_warnx(st, "cannot compile");
						THROW;
					}
#endif
				FINALLY /* Compiler process */
#ifndef __unix__
					CloseHandle(pi.hProcess); /* TO.DO W: check errors */
					CloseHandle(pi.hThread);
#endif
				END /* Compiler process */

				/* We can delete source file now, but my exception system doesn't allow this */

				TRY /* Shared object handle */
					HANDLE handle = load_library(st, shared, 0 /* local */);

					if(handle == 0){
						THROW;
					}
				IN /* Shared object handle */
					void (*mini_command)(void);

#ifdef __unix__
					*(void **)&mini_command = dlsym(handle, "mini_command");

					if(my_dlerror(st, "dlsym") == -1){
						THROW;
					}
#else
					/* GetProcAddress(HMODULE, LPCSTR); */
					*(void **)&mini_command = GetProcAddress(handle, "mini_command");

					if(mini_command == 0){
						my_warnx(st, "cannot get function pointer"); /* TO.DO W: more detail */
						THROW;
					}
#endif

					if(st->check_errno){
						errno = 0;
					}

					(*mini_command)();

					if(check_errno_func(st) == -1){
						THROW;
					}

					result = 0;
				FINALLY /* Shared object handle */
#ifdef __unix__
					dlclose(handle);

					if(my_dlerror(st, "dlclose") == -1){
						THROW;
					}
#else
					if(!FreeLibrary(handle)){
						my_warnx(st, "cannot close handle"); /* TO.DO W: more detail */
						THROW;
					}
#endif
				END /* Shared object handle */

			FINALLY /* Shared object file */
				if(remove(shared) == -1){ /* TO.DO: рушится при setuid(1) */
					my_warn(st, "%s: cannot remove temporary shared object file or DLL (dynamic-link library)", shared);
					THROW;
				}
			END /* Shared object file */
		FINALLY /* Source file */
			if(remove(source) == -1){
				my_warn(st, "%s: cannot remove temporary source file", source);
				THROW;
			}
		END /* Source file */
	STOP_EXCEPTIONS

	return result;
}

/* TO.DO: more detail */
static void help(struct state *st){
	fprintf(st->out,
		".q, q, quit, exit Quit\n"
		".h, h, help This help\n"
		".i <HEADER>|\"HEADER\"\n"
		".l LIBRARY\n"
		".p EXPRESSION Print\n"
		"\n"
		"Small metacommands:\n" /* TO.DO Docs: ну то есть редкие */
		".errno+\n"
		".errno-\n"
		".signal+ Notice about catched signals\n" /* TO.DO: сделать по-нормальному? signal-? Notice/Ignore_all/Reset_defaults? */
		".werror+\n"
		".werror-\n"
		"\n"
		"C functions:\n"
		DECLARATIONS
		"\n"
		"Metacommands for /sbin/init (and for other strange environments):\n" /* TO.DO Docs: функций избыточно много. Просто на всякий случай. Что-то может понадобиться */
		".execvp FILE [ARGV]... (maximal arguments count including argv[0]: %d)\n" /* TO.DO W: как это работает на винде? */
		".setenv NAME VALUE\n" /* TO.DO W: как это работает на винде? */
		".system COMMAND\n"
		"\n"
		"/sbin/init example:\n"
		".setenv PATH /usr/sbin:/usr/bin:/sbin:/bin\n" /* Standard Debian PATH */
		".system mount -o remount,rw /\n",
	MAXARGS);
}

// TO.DO: функция - полный абзац; я даже не думал об инклудах и портируемости
static void sighandler(int signum){
	fprintf(stdout, "mini: got signal %d\n", signum);
}

static int eval(struct state *st, const char *command){
	int result = -1;

	char *tokens;

	{
		size_t size = strlen(command);
		tokens = (char *)malloc(size + 1);
		memcpy(tokens, command, size + 1);
	}

	char *save_ptr;
	const char *token = _mini_strtok_r(tokens, " ", &save_ptr);

	if(token != 0){
		if(token[0] == '.'){
			if(strcmp(token, ".q") == 0){
				st->done = 1;
				result = 0;
			}else if(strcmp(token, ".h") == 0){
				help(st);
				result = 0; /* TO.DO Docs: если сделал chroot и т д, то ты сам дурак. Вообще тут полно способов отстрелить себе ногу. Когда делаете exec(...) или .execvp и т д из функции, никто чистить не будет. С форками нужно быть крайне аккуратным */
			}else if(strcmp(token, ".i") == 0){ /* TODO Docs: вы сами должны чесаться по поводу того, где ищет gcc хидеры (следить за pwd и т д) */
				const char *header = _mini_strtok_r(0, " ", &save_ptr);
				int len;
				if(header == 0 || (len = strlen(header)) < 3 || !((header[0] == '<' && header[len - 1] == '>') || (header[0] == '\"' && header[len - 1] == '\"'))){
					my_warnx(st, "usage: .i <HEADER>|\"HEADER\"");
				}else{
					if(strlen(st->headers) + strlen("#include ") + strlen(header) + 1 /* '\n' */ + 1 /* '\0' */ > MAX_HEADERS){
						my_warnx(st, ".i: too many headers or too long names of headers");
					}else{
						strcat(st->headers, "#include ");
						strcat(st->headers, header);
						strcat(st->headers, "\n");
						result = 0;
					}
				}
			}else if(strcmp(token, ".l") == 0){
				const char *library = _mini_strtok_r(0, " ", &save_ptr);
				if(library == 0){
					my_warnx(st, "usage: .l LIBRARY");
				}else{
					/* We don't need to check return value, because load_library prints error message automatically */
					if(load_library(st, library, 1 /* global */) != 0){
						result = 0;
					}
				}
			}else if(strcmp(token, ".p") == 0){
				/* WARN! We assume behavior of this version of strtok_r (taken from Gnulib) */
				const char *expression = save_ptr + strspn(save_ptr, " ");
				if(*expression == 0){
					my_warnx(st, "usage: .p EXPRESSION");
				}else{
					/* TO.DO: errors */
					/* TO.DO: temporary file */
					/* TO.DO: перенаправить ввод, вывод и т д */
					FILE *commands = fopen("/tmp/mini-commands.gdb", "w");
					fprintf(commands, "attach %d\nprint %s\n", getpid(), expression);
					fclose(commands);
					if(st->check_errno){
						errno = 0;
					}
					system("gdb -batch -x /tmp/mini-commands.gdb 2>&1 | grep -v '^[0-9/]'");
					check_errno_func(st); /* TO.DO: this is hack! */
					result = 0;
				}
			}else if(strcmp(token, ".errno+") == 0){
				st->check_errno = 1; /* TO.DO Docs: успешные функции могут менять errno! */
				result = 0;
			}else if(strcmp(token, ".errno-") == 0){
				st->check_errno = 0;
				result = 0;
			}else if(strcmp(token, ".signal+") == 0){
				/* TO.DO: фрагмент - полный абзац; я даже не думал об инклудах и портируемости */
				/* TO.DO Docs: сигналы нужно перевыставлять после их прихода */
				for(int i = 1; i <= 32; ++i){
					if(signal(i, &sighandler) == SIG_ERR){ // bad function
						fprintf(st->err, "mini: cannot set signal handler for signal %d: %s\n", i, strerror(errno));
					}
				}
				result = 0;
			}else if(strcmp(token, ".werror+") == 0){
				st->werror = 1;
				result = 0;
			}else if(strcmp(token, ".werror-") == 0){
				st->werror = 0;
				result = 0;
			}else if(strcmp(token, ".execvp") == 0){
				const char *file = _mini_strtok_r(0, " ", &save_ptr);
				if(file == 0){
					my_warnx(st, "usage: .execvp FILE [ARGV]...");
				}else{
					char *args[MAXARGS + 1];
					int i = 0;
					for(;;){
						if(i == MAXARGS + 1){
							my_warnx(st, ".execvp: too many arguments");
							break;
						}
						args[i] = _mini_strtok_r(0, " ", &save_ptr);
						if(args[i] == 0){
							execvp(file, args);
							my_warn(st, ".execvp %s ...", file);
							break;
						}
						++i;
					}
					/* If we are here, something is wrong */
				}
			}else if(strcmp(token, ".setenv") == 0){
				const char *name  = _mini_strtok_r(0, " ", &save_ptr);
				const char *value = _mini_strtok_r(0, " ", &save_ptr);
				if(name == 0 || value == 0){
					my_warnx(st, "usage: .setenv NAME VALUE");
				}else{
					if(setenv(name, value, 1 /* Overwrite */) == -1){
						my_warn(st, ".setenv %s %s", name, value);
					}else{
						result = 0;
					}
				}
			}else if(strcmp(token, ".system") == 0){
				/* WARN! We assume behavior of this version of strtok_r (taken from Gnulib) */
				const char *command = save_ptr + strspn(save_ptr, " ");
				if(*command == 0){
					my_warnx(st, "usage: .system COMMAND");
				}else{
					int status = system(command);
					if(status == -1){
						my_warn(st, ".system %s", command);
					}else{
						if(WIFEXITED(status) && WEXITSTATUS(status) == 127){
							my_warnx(st, ".system %s: system(...) returned 127, shell is probably unavailable", command);
						}else{
							result = 0;
						}
					}
				}
			}else{
				my_warnx(st, "%s: unknown metacommand", token);
			}
		}else{
			if(_mini_strtok_r(0, " ", &save_ptr) == 0){
				if(strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0){ /* TO.DO: docs: не то же самое, что и exit(0) */
					st->done = 1;
					result = 0;
				}else if(strcmp(token, "help") == 0 || strcmp(token, "h") == 0){
					help(st);
					result = 0;
				}else{
					result = do_eval(st, command);
				}
			}else{
				result = do_eval(st, command);
			}
		}
	}

	free(tokens);
	return result;
}

/* TODO Docs: состояние не запоминается (т. к. не хочу переусложнять) */
int mini_eval(FILE *out, FILE *err, const char *command){
	struct state st = {0, out, err};

	return eval(&st, command);
}

// TO.DO: разобраться с работой с сигналами в readline
// TO.DO Docs: mini(...) errors ignored
void mini(FILE *in, FILE *out, FILE *err){
	struct state st = {in, out, err};

	int /* bool */ interactive = (isatty(fileno(st.in)) && isatty(fileno(st.out)));

	if(interactive){
		fputs("Welcome to Mini - C read-eval-print loop. Type `.h' for help. Type `.q' to quit\n", st.out);
	}

#ifdef __unix__
	if(interactive){
		rl_instream  = st.in;
		rl_outstream = st.out;
		using_history();

		while(!st.done){
			char *buffer = readline("mini> ");

			if(buffer == 0){
				fputs(".q\n", st.out);
				break;
			}

			if(*buffer != 0 && *buffer != ' '){
				add_history(buffer);
			}

			eval(&st, buffer);

			free(buffer);
		}
	}else
#endif
	{
		char *buffer = 0;
		size_t buffer_size;

		while(!st.done){
#ifndef __unix__
			if(interactive){
				fputs("mini> ", st.out);
				fflush(st.out);
			}
#endif
			ptrdiff_t len = _mini_getline(&buffer, &buffer_size, st.in);

			if(len == -1){
#ifndef __unix__
				if(interactive){
					fputs(".q\n", st.out);
				}
#endif
				break;
			}

			if(buffer[len - 1] == '\n'){
				buffer[len - 1] = 0;
			}

			eval(&st, buffer);
		}

		free(buffer);
	}
}
