/*
 * Предназначен для двух платформ:
 * * UNIX-подобные ОС с компилятором cc и функцией dlopen
 * * Windows с MVC++
 * Протестирован на:
 * * GNU/Linux, gcc 4.7.2
 * * На Windows пока не протестирован :( Но работать должен
 * При запуске на Windows cl.exe должен быть в %path%
 * Создаёт файлы library.c и library.so (library.dll) в текущем каталоге
 * C89
 * Компилировать на GNU/Linux: cc eval.c -ldl
 * Компилировать на Windows: cl eval.c
 */

#ifdef __unix__
#elif defined(_WIN32)
# define _UNICODE
# define UNICODE
#else
# error "This is not UNIX-like OS, nor Windows"
#endif

#include <stdio.h>
#include <stdlib.h>

#ifdef __unix__
# include <dlfcn.h>
#else
# include <windows.h>
# include <tchar.h>
#endif

#ifdef __unix__
# define DECLSPEC /* Nothing */

void *library;

void my_dlerror(const char *message){
	char *error = (char *)dlerror(); /* OpenBSD's dlerror returns `const char *', so we need to convert */

	if(error != 0){
		fprintf(stderr, "%s: %s\n", message, error);
		exit(EXIT_FAILURE);
	}
}

void *compile(const char *symbol){
	void *result;

	if(system("cc -shared -O3 -fPIC -o library.so library.c") != 0){
		fprintf(stderr, "system\n");
		exit(EXIT_FAILURE);
	}

	/* RTLD_NOW, так как я не хочу, чтобы программа падала при undefined symbols */
	library = dlopen("./library.so", RTLD_NOW | RTLD_LOCAL);

	my_dlerror("dlopen");

	result = dlsym(library, symbol);

	my_dlerror("dlsym");

	return result;
}

void clean_up(void){
	dlclose(library);

	my_dlerror("dlclose");
}
#else
# define DECLSPEC "__declspec(dllexport) "

HMODULE library;

void *compile(const char *symbol){
	void *result;

	if(system("cl /LD /nologo /Felibrary library.c") != 0){
		fprintf(stderr, "system\n");
		exit(EXIT_FAILURE);
	}

	/* LoadLibrary(LPCTSTR); */
	library = LoadLibrary(TEXT(".\\library.dll"));

	if(library == 0){
		fprintf(stderr, "LoadLibrary\n");
		exit(EXIT_FAILURE);
	}

	result = GetProcAddress(library, symbol);

	if(result == 0){
		fprintf(stderr, "GetProcAddress\n");
		exit(EXIT_FAILURE);
	}
}

void clean_up(void){
	if(!FreeLibrary(library)){
		fprintf(stderr, "FreeLibrary\n");
		exit(EXIT_FAILURE);
	}
}
#endif

void eval(const char *code){
	void (*evaluating)(void);

	FILE *fout = fopen("library.c", "w");

	if(fout == 0){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fprintf(fout,
		"#include <stdio.h>\n"
		DECLSPEC "void evaluating(void){\n"
		"%s\n"
		";\n"
		"}\n",
		code);

	fclose(fout);

	*(void **)&evaluating = compile("evaluating");

	(*evaluating)();

	clean_up();
}

int main(void){
	eval("printf(\"Hello, world!\\n\")");
}
