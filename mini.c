// Первостепенные дела, если я буду публиковать:
  // TO.DO: просто отказаться в пользу c-repl? Дополнить c-repl?
  // TO.DO: как на UNIX, так и на Windows возможно как скомпилировать с readline, так и без него? Также вкл/откл при запуске? Не нашёл readline при запуске -> работает без него?
    // TO.DO: возможно, readline всё-таки нужен, т. к. rlwrap имеет недостатки (у него куча опций для работы с ситуацией, когда прожка spawns pagers)

  // TO.DO: Docs: ОБЯЗАТЕЛЬНО: есть 4 способа сделать что-то вроде интерпретатора Си (способа не четыре, нужно почётче отклассифицировать способы) (представить proof of concept всех 4-х способов??) (показать существующие примеры всех способов) (сводная таблица способов) (ultimate c interpreter manual, с кучей примеров):
    // полноценный интерпретор Си, сложный в написании, тяжёлый в размерах, имеет все фичи (не зависит от gcc и gdb, понимает полноценный Си, не юзает временные файлы, а значит, работает в самых неприхотливых окружениях, типа r/o fs и /sbin/init, портируем), все остальные способы имеют меньше этих фич
      // 1.1. резрешать функции с помощью hardcoded списка функций
      // 1.2. с помощью dlopen
    // mini
      // eval.c в этой папке идеально подходит для учебных целей. И он уже полностью готов, его не надо "ещё раз" смотреть перед публикацией
    // asi (см. ниже)
    // dlfcn/eval.c в glibc (там коммент что-то вроде "you don't need ever to know what is this", добавить этот коммент в доки)
      // otcc компилит си в память и резолвит символы с помощью dlsym (obfusc. tiny cc by f. bellard)
        // otcc скорее ближе к gcc-подходу, чем к подходу с истинной интерпретацией
      // а что если dlfcn/eval.c форт есть? или лисп и т д?
        // и вообще написать в доках, что к теме очень близко стоит форт/лисп
    // использовать gdb (как прогу, как либу, написать аналог gdb)
    // mini понимает не только C expressions, но и C operators. но он неотделим от отлажимаемого процесса. gdb отделим от отлаживаемого процесса (иногда это полезно) и обладает другими фичами. хорошо бы объедить эти плюсы, т. е. добавить поддержку C operators в gdb или хотя бы сказать о такой идее в доках
  // TO.DO: Иметь mini, не запускающий процессов всё-таки здорово (т. е. не запускающий gcc), так как его можно будет запустить в завершившемся PID namespace (где уже fork не работает, правда я не уверен что сработает). и вообще презентовать мою прожку разрабам ядра (linux и др ядра, в том числе создающиеся)
  // TO.DO: Возможно, объединить все способы mini в одну полезную утилиту с кучей применений (возможно, с облегчёнными вариантами для железа, например, хотя, возможно, для железа и модельные подойдут). А отдельные способы - для примеров. Единая полезная утилита может failback-иться между способами, переключаться как-нибудь

  // TO.DO: Docs: ОБЯЗАТЕЛЬНО: я хотел добавить простой встроенный интерпретатор, мега-портируемый, в том числе на голое железо, умеющий лишь вызывать функции из hard-coded списка функций, но передумал, так как gdb круче
    // TO.DO: Кстати, портануть mini на голое железо. Это будет отличным делом для изучающих x86 (и любые другие архитектуры). Не нужно ребутиться, чтоб посмотреть результат, можно прогать прямо на самом железе

  // TO.DO: Docs: ОБЯЗАТЕЛЬНО: я хотел добавить glibc:///dlfcn/eval.c-подобный, gdb-подобный интерфейс, встроенный простой интерпретатор, умеющий лишь вызывать функции, но произвольные, но передумал, так как дублируется gdb

// TO.DO внедрить все фичи c-repl (docs: спасибо за идею, ссылка; и вообще перечислить в доках альтернативы: gdb, perl и т д)
  // TO.DO: написать автору c-repl?
  // Подключение либы по имени, а не по пути (напр., -lm и т д)
// TO.DO C-REPL UNDOCUMENTED: давать функциям (т. е. командам на языке си, набираемым в mini) имена, чтобы из следующих функций их можно было вызывать
// TO.DO: Docs: протестировано на GNU/Linux'е и FreeBSD (TO.DO: надо бы ещё раз протестить, когда прога будет написана)
  // TO.DO: Docs: если solaris /usr/include/dlfcn.h:66:13: error: expected specifier-qualifier-list before 'mapobj_result_t', то:
  // typedef struct {
  // 66 -       mmapobj_result_t *dlm_maps;
  // +       struct mmapobj_result_t *dlm_maps;
  //         uint_t            dlm_acnt;
  //         uint_t            dlm_rcnt;
  // } Dl_mapinfo_t;
  // TO.DO: зарепортить как баг в солярисе. или сделать у меня некое подобие fix-includes из gcc
// TO.DO Docs: W: нужно, чтоб cl был в environment
// TO.DO Docs: W: всё создаётся в текущей папке, она должна быть доступна, также надо убирать/чистить за mini
// TO.DO: Docs: рассказать, рассказать, что эти функции предназначены только для отладки и потому они печатают сообщения об ошибках
// TO.DO Docs: всё запускается в рамках одного процесса, то есть getpid выдаёт одно и то же число. Это главное отличие от простого While-Read-Do-Compile-Run
// TO.DO: вкомпилить readline и dl внутрь mini, если это возможно? docs: рассказать, как правильно собирать с либой mini (-lreadline и т д -rdynamic)
// TO.DO: чистить временные файлы при получении сигнала? Или не надо, т. к. это уменьшит свободу выполняемого кода? Если не чистим, то написать об этом в доках
// TO.DO Docs: можно слинковать с мини, вызвать из проги мини, из мини обратно прогу и т д. можно вызвать mini из gdb и наоборот. можно законнектиться к процессу при помощи gdb и сделать mini
// TO.DO Docs: нет, переменные не сохраняются (если это так)
// TO.DO Docs: ОБЯЗАТЕЛЬНО: этот вопрос был задан дважды! Mini не сохраняет прогу, этого можно добиться с помощью tee. Рассказать, как продолжить с заданного места
  // TO.DO Docs: но ещё более важный вопрос, который всех интересует: почему переменные не сохраняются между командами?
// TO.DO: ошибки .i не репортятся и необратимы
// TO.DO: есть sysdeps/unix/sysv/linux/syscalls.list в сорцах libc
// TO.DO Docs: пример с fork/exec и с fork в терминале

// TO.DO: "mini> .p mini_connect("irc.tambov.ru", "7770")
// $1 = 3
// mini: errno: Network is unreachable" - mini_connect должен чистить за собой errno (или какое-то другое решение)

// TO.DO: сделать полноценное прогание? Чтоб можно было на ходу создавать функции?
// TO.DO LATER: переписать основную функцию без #if'ов? например, разбить на две функции? (потому что в the art of unix programming #if-ный стиль раскритикован как ужасный)
// TO.DO Docs: большой список вещей, которые можно сделать с мини (например, узнать, может ли один зомби быть сыном другого)
// TO.DO LATER: не распространять mini под GPL, а то она не сможет линковаться со всем подряд? http://en.wikipedia.org/wiki/GPL_linking_exception ?
// TO.DO Docs: я добавил readline только чтобы можно было запустить mini как /sbin/init. Иначе я б не добавил, потому что программа должна делать одну вещь и делать её хорошо, см. также Program design in the UNIX environment, Rob Poke, Brian W. Kernighan, http://harmful.cat-v.org/cat-v
// TO.DO LATER: на коробочной убунте precise команда ".p" из под юзера выдаёт: "Cannot attach to process..." (тут большой текст о том, что not permitted)
// TO.DO LATER: mini должен игнорить ctrl-c и вообще вести себя во всём как шел? написать либу для написания шелов, то есть командных программ?
  // TO.DO: при получении ctrl-c mini должен завершать текущую команду?
// TO.DO LATER: а что если я хочу заюзать mini, чтоб проверить, что чувствует процесс, запущенный из shebang'а? и поэтому mini должен игнорить argv? И поэтому он должен получать инфу о fd, используемом как stdin из других источников?
// TO.DO LATER: document, что команда ";" проверяет, что всё (в том числе все файлы, инклуденные с помощью .i) в порядке? или придумать что-то лучше?

// TO.DO LATER Docs: да хоть юзайте как калькулятор (особенно если не знаете python и т. д., а wolfram - г$но проприетарное): (o_O, в mini работает автодополнение?) (o_O, .i <...> автодополнение работает?!)
/*
Welcome to Mini - C read-eval loop. Type ".h" for help. Type ".q" to quit
mini> .p sqrt(3)
No symbol "sqrt" in current context.
mini> .l /lib/libm
libm-2.11.3.so  libm.so.6       libmemusage.so
mini> .l /lib/libm.so.6
mini> .p sqrt(3)
$1 = 0 // Только не работает ни х%ра
mini> .q
*/
// TO.DO LATER Docs: до хоть сделайте rdinit mini и вызывайте mount(MOVE)
  // TO.DO LATER Docs: тут хорошо бы, чтобы от gcc не зависел, возможно, надо нормальный интерпретатор сделать
// TO.DO: а как собрать mini статически? как он тогда будет общаться со своим dynamic линкером?
// TO.DO: добавить поддержку c++? хотя б сделать так, чтоб из c++ можно было вызывать mini? и кто тогда должен вызываться? gcc или g++?
// TO.DO Docs: даёт полный контроль над процессом насколько это возможно. "exec bash" не даёт такой контроль, while read do eval не даёт, while (read){ system } не даёт
// TO.DO: Аргументы к проге никогда не указываются как fd, вместо этого юзаются файлы vs а что, если на target os нет способа типа /proc/self/fd преобразовать fd в файл?
// TO.DO: именно mini (причём без gcc) отличается от gdb тем, что можно запустить его в init или rdinit и быть уверенным, что набранные в нём сисвызовы в точности первые сисвызовы в user space, что набранные в нём fork'и первые форки. И что не надо запускать rlwrap перед mini (+ rlwrap'а может не быть, его сложно запустить из cmdline ядра, его нужно искать в PATH и т д)

// TO.DO: этот файл ужасен из-за ifdef'ов. Наверное, нужно сделать так: The best method of managing system-specific
// variants is to follow those same basic principles:
// define a portable interface to suitably-chosen primi-
// tives, and then implement different variants of the
// primitives for different systems. The well-defined
// interface is the important part: the bulk of the
// software, including most of the complexity, can be
// written as a single version using that interface, and
// can be read and understood in portable terms.
// -ifdef considered harmful, cat-v.org

// TO.DO: возможность отключать варниги gcc? и вообще воз-ть указывать свою командную строку gcc? в том числе воз-ть отключать надоедливый варнинг о том, что не подключен хедер? отключать его по дефолту? и вообще, может внести в мейнстрим некоторые изменения из моего личного патча либо контролировать их аргументами командной строки или метакомандами?

// TO.DO: что делать, когда юзер запускает exit/fork/exec и т д? как удалять временные файлы и т д?

// TO.DO: сделать так, чтоб mini полностью подходил для init'а. Например, повесить обработчик SIGCHLD (можно verbose'ный, можно не только для init'а)
// TO.DO: добавить метакоманды для смены st.in, out и err? и вообще всё переписать, чтобы не забыть добавить метакоманды на все случаи жизни?
  // TO.DO: сделать все фичи доступными в виде метакомманд, опций командной строки и API. хотя, может, метакоманды не надо и вместо этого можно просто из mini вызывать API типа такого:
  // # mini
  // mini> mini_eval("printf(\"Hi\")")
  // mini> mini_help()
  // поэтому добавить возможность вызывать mini API отовсюду, в том числе и из интерпретатора с жёстким набором функций и голого железа?
  // можно написать метакоманды для смены lazy/now mode
  // может быть, добавить mini -c '...'
// TO.DO изучить c interpreter из последнего Research UNIX. и из plan 9, если он там есть
// TO.DO Docs: Б%$, разве вам никогда не хотелось из одной функции одной прожки вызвать функцию другой? Почему раньше никто не додумался сделать _командный интерфейс к dynamic линкеру_? Вот, например, мне в д/з по численном методе понадобилось вызвать функцию из matrix-framework. Берём, компилим прожки как C (C++ почему-то не вышло) с -shared -fPIC -rdynamic (не уверен, что всё надо), и вызываем всё из mini (или наоброт, из одной прожки mini, а из mini - другую). Б%^, да mini как клей, с помощью которого можно склеить всё чё угодно со всем, что угодно
  // TO.DO: как из gdb сделать dlopen? и как это сделать, если прожка собрана без -ldl? Может, интерфейс к своему динамик линкеру всё равно есть?
    // p __libc_dlopen_mode("/usr/lib/x86_64-linux-gnu/libm.so", 0x102), 0x102 - это RTLD_NOW | RTLD_GLOBAL
      // После этого совсем не обязательно получать указатели с помощью dlsym, у меня не удавалось это сделать. Можно просто вызывать функции из gdb
// TO.DO Docs: а ещё с помощью mini можно потрогать какую-нибудь либу
// TO.DO Конкурент :( "gdb $SHELL $$". Только, наверное, так можно с сегфолтом упасть
// TO.DO gcc-вариант mini - это, оказывается, jit. более того, мой mini - это лучший jit, т. к. он мультиплатформенный
  // TO.DO может, сделать настоящий jit?
  // TO.DO можно сослаться на мои комменты здесь: http://habrahabr.ru/post/182356
// TO.DO: хороший тест: mini_eval(mini_eval(printf(Hello, world)))
// TO.DO: можно написать полноценный компилятороинтерпретатор си, со всеми плавными степенями между компилятором и интерпретатором. Чтоб поддерживал процессинг полных файлов на си с инклудами, но, скажем, с доступ к рантайму из них, т. е. к интерпретатору
  // TO.DO: хороший тест: запустить интерпретатор в ин-ре и т. д.
  // TO.DO: это имеет некое отношение к mini поверх голого железа и к C OS (т. е. к OS, в которой поддерживается разработка на std C и никаких других фич). Поэтому, список способов написать interpreter/compiler C увеличивается с около 4-х до около 10-и
    // TO.DO: Можно реально добавить в mini поддержку llvm. C OS на основе llvm
// TO.DO: в случае сегфолта делать setjmp обратно в mini. и обработка прочих сигналов, в т. ч. ctrl-c
  // TO.DO: для обработки сегфолта иногда нужен sigaltstack
  // TO.DO: если я буду использовать mini вместо gdb/lldb (если, например, приду к выводу, что gdb/lldb невозможен), то особенно нужно реализовать эту фичу. Если я очень долго ждал воспроизведения какого-нибудь бага (например, в торговом боте), наконец подключился к процессу с помощью mini, будет очень глупо упасть с segfault'ом
    // TO.DO: mini во многом лучше gdb/lldb. Довести mini до полноценного дебаггера?
// TO.DO: http://root.cern.ch/drupal/content/cling
// TO.DO: в mini есть до фига способов выстрелить себе в ногу, напр. signal(SIGCHLD, SIG_IGN). Document this. нужно быть устойчивым к ним или нет? нужно из-за них держать настоящий встроенный интерпретер?
// TO.DO: совершенно точно добавить режим mini -c или что-то такое. Т. к. бывает нужно, например, запустить mini -c 'close(0); close(1); close(2); execlp(...)'
// TO.DO: можно сделать так: при варнингах спрашивать подтверждение на запуск
// TO.DO: Перейти на libsh. Если mini перейдёт на libsh, то ловить в mini исключения, брошенные юзерским кодом
// TO.DO: если присвоить строку глобальное переменной, и в следующей строчке прочитать, будет плохо
// TO.DO: настоящий интерпретатор всё-таки по-настоящему нужен. Чтобы, например, сделать с его помощью какую-нибудь хитрую комбинацию chroot и chdir и посмотреть, куда мы выйдем
  // TO.DO: но и mini нужен, чтобы протестить фичи компилятора
    // TO.DO: mini должен уметь понимать любую конкретную версию компилятора, c, c++, любой набор флагов и т д, чтобы учить c++ regexes, например
// TO.DO: "This patch series is half of a project to let gdb reuse gcc" - gcc-patches@gcc.gnu.org ("Compiling and injecting code in gdb" в доках gdb)

// TO.DO W: не компилить mini как c++, а то, например, не получится сделать её сишной либой?
// TO.DO W: есть папка sync:///github/mini-tools/mini-windows, там лежит старая версия mini, которая, скорее всего, работает на винде
// TO.DO W: возможно, в винде нельзя трогать загруженный .dll

#ifdef __unix__
# define _POSIX_C_SOURCE 200809L /* Нужно обязательно установить как минимум в 200112L, так как юзаем mkstemp */
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
#include <limits.h>
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

#ifdef __CYGWIN__ /* Cygwin's standard C library (Newlib) doesn't support new standards yet (as on 2012), so we need this hack */
FILE *fdopen(int fd, const char *mode);
int mkstemp(char *template);
int fileno(FILE *stream);
#endif

#if defined(__MINGW32__) && !defined(fileno) /* MinGW has the same problem */
int fileno(FILE *stream);
#endif

#define DECLARATIONS \
  "void mini_poll (struct pollfd fds[], nfds_t nfds, int timeout);\n"

#define MAX_ARGS 4096 /* Max count of arguments for .execlp */

#define MAX_HEADERS 65536

struct state
{
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
    if (fd == -1)THROW;
  IN
    if (write(fd, "bar", 3) == -1)THROW;
  FINALLY
    if (close(fd) == -1)THROW;
  END
STOP_EXCEPTIONS

All is required

IN and FINALLY sections can view TRY objects
*/

/* I use this variable name to avoid name conflicts. Of course, namespace would be better, but this code is not part of any package. It is just my (Askar Safin's) very own reusable code */

#define START_EXCEPTIONS { int _DtQjw7AVrEPWbB2WOTFHBmIUAa6vx = 0; do{
#define STOP_EXCEPTIONS  }while (0); }

#define TRY              {
#undef  IN /* Some environment has "IN" macro */
#define IN               do{
#define FINALLY          }while (0); {
#define END              } if (_DtQjw7AVrEPWbB2WOTFHBmIUAa6vx)break; }

#define THROW            { _DtQjw7AVrEPWbB2WOTFHBmIUAa6vx = 1; break; }

static void my_warn (struct state *st, const char *format, ...)
{
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

static void my_warnx (struct state *st, const char *format, ...)
{
  fflush(st->out);
  fputs("mini: ", st->err);

  va_list ap;
  va_start(ap, format);
  vfprintf(st->err, format, ap);
  va_end(ap);

  fputc('\n', st->err);
  fflush(st->err);
}

#ifdef __unix__
static int my_dlerror(struct state *st, const char *message)
{
  char *error = (char *)dlerror(); // OpenBSD's dlerror returns "const char *", so we need to convert

  if (error == NULL){
    return 0;
  }else
    {
      my_warnx (st, "%s: %s", message, error);
      return -1;
    }
}
#endif

int check_errno_func(struct state *st)
{
  if (st->check_errno && errno != 0)
    {
      my_warn (st, "errno");
      return -1;
    }
  return 0;
}


#ifdef __unix__
# define HANDLE void *
#else
# define HANDLE HMODULE
#endif

static HANDLE load_library(struct state *st, const char *library, int /* bool */ global)
{
#ifdef __unix__
  // RTLD_NOW, так как я не хочу, чтобы программа падала при undefined symbols (TODO: точно? проверить везде, с какими флагами вызываются dlopen. Может, может понадобиться загрузить либу, в которой не все зависимости разрешены)
  void *result = dlopen(library, RTLD_NOW | (global ? RTLD_GLOBAL : RTLD_LOCAL));

  if (my_dlerror(st, "dlopen") == -1)
    {
      return NULL;
    }

  return result;
#else
  /* LoadLibrary(LPCTSTR); */
  HMODULE result = LoadLibrary(TEXT(".\\mini-shared.dll")); /* TO.DO W: в винде не работает .l. исправить */

  if (result == 0)
    {
      my_warnx (st, "cannot load library"); /* TO.DO W: more detail */
      return 0;
    }

  return result;
#endif
}

static int do_eval(struct state *st, const char *code)
{
  int result = -1;

  START_EXCEPTIONS
    TRY /* Source file */
      FILE *source_fp;

#ifdef __unix__
      char source[] = "/tmp/mini-source-XXXXXX";

      {
        int source_fd = mkstemp(source);

        if (source_fd == -1){
          my_warn (st, "%s: cannot create temporary source file; please try to remount /tmp filesystem read-write or mount tmpfs, for example \".system /bin/mount -o remount,rw /\"; error message", source); /* TO.DO: сделать совет portable? После "error message" my_warn добаляет error message */
          THROW;
        }

        source_fp = fdopen(source_fd, "w");

        if (source_fp == NULL){
          my_warn (st, "%s: cannot fdopen temporary source file", source);
          close(source_fd);
          remove(source);
          THROW;
        }
      }
#else
      char source[] = "mini-source.c"; // TO.DO W: make better
      source_fp = fopen(source, "w");

      if (source_fp == NULL){
        my_warn (st, "%s: cannot create temporary source file", source);
        THROW;
      }
#endif
    IN /* Source file */
      TRY /* Variable "source_fp" */
      IN /* Variable "source_fp" */
        if (fprintf(source_fp,
          "%s"
          DECLARATIONS /* DECLARATIONS after headers, because DECLARATIONS need headers */
#ifndef __unix__
          "__declspec(dllexport) "
#endif
          "void mini_command(void){\n"
          "%s\n" /* Я сделал это отдельной строчкой, потому что "code" может начинаться на "//" */
          ";\n" /* А это я сделал отдельной строчкой, потому что code может заканчиваться на // */
          "}\n", st->headers, code) < 0){

          my_warn (st, "%s: cannot write to temporary source file", source);
          THROW;
        }
      FINALLY /* Variable "source_fp" */
        if (fclose(source_fp) == EOF){
          my_warn (st, "%s: cannot close temporary source file", source);
          THROW;
        }
      END /* Variable "source_fp" */

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

          if (shared_fp == NULL){
            my_warn (st, "%s: cannot create temporary shared object file (DLL, dynamic-link library)", shared);
            THROW;
          }

          if (fclose(shared_fp) == EOF){
            my_warn (st, "%s: cannot close temporary shared object file (DLL, dynamic-link library)", shared);
            THROW;
          }
        }
#else
        {
          int shared_fd = mkstemp(shared);

          if (shared_fd == -1){
            my_warn (st, "%s: cannot create temporary shared object file", shared);
            THROW;
          }

          if (close(shared_fd) == -1){
            my_warn (st, "%s: cannot close temporary shared object file", shared);
            THROW;
          }
        }
#endif
      IN /* Shared object file */
        TRY /* Compiler process */
#ifdef __unix__
// TO.DO: fflush всё перед форком и т. д., вывод gcc куда надо (т. е. в нужный fd)
          pid_t cc = fork();

          /* We cannot use "switch" here because we want THROW to work */
          if (cc == -1){
            my_warn (st, "cannot fork");
            THROW;
          }else if (cc == 0){
            /* Child */
            execlp("gcc", "gcc", "-shared", "-std=gnu99",
              "-O0", /* Для компиляторов, отличных от gcc. Они могут включать оптимизацию по умолчанию, а это замедляет компиляцию */
              "-s", /* Аналогично. Возможно, такая опция требуется даже для gcc, я точно не знаю */

              /* Warnings: */ /* TO.DO Docs: тщательно подобраны варнинги и -Werror. Например, printf(5) (это вызов функции, а не man) теперь не вызывает segfault. Остальные варнинги не включены, так как это позволяет чувствовать себя свободно при написании кода */
              /* TO.DO: включить или нет werror по дефолту? чтоб printf(5) не падало с сегфолтом? */
              /* Wnonnull, напр.: execvp(0, 0) падает с сегфолтом */
              st->werror ? "-Werror" : "-shared",

              "-Wformat",
              "-Wno-format-zero-length",
              "-Wnonnull",
              "-Wimplicit-function-declaration",
              "-Wdiv-by-zero",
#ifndef __CYGWIN__
              "-fPIC", /* TO.DO: who uses and who don't uses -fPIC? Should I use cc or gcc? Same for other options */
#endif
              "-o", shared, "-x", "c", source, (char *)NULL);

            my_warn (st, "cannot run gcc; please make sure you have \"gcc\" in your PATH, for example: \".setenv PATH /usr/sbin:/usr/bin:/sbin:/bin\"; error message"); /* После "error message" my_warn добаляет error message */
            exit (EXIT_FAILURE);
          }
#else
          STARTUPINFO si;
          PROCESS_INFORMATION pi;

          memset (&si, '\0', sizeof(si));
          si.cb = sizeof(si);
          memset (&pi, '\0', sizeof(pi));

          /* TO.DO W: Чистить за компилятором лишние файлы */
          /* /LD - create DLL */
          /* /nologo - diable copyright info */
          /* CreateProcess(LPCTSTR, LPTSTR, ...); */
          TCHAR command[] = TEXT("\"cl\" /LD /nologo /Femini-shared mini-source.c");
          if (!CreateProcess(0, command, 0, 0, FALSE, 0, 0, 0, &si, &pi)){
            my_warnx (st, "cannot create process; please, make sure you have \"cl\" in your %%path%%"); /* TO.DO W: more detail */
            THROW;
          }
#endif
        IN /* Compiler process */
          /* Parent */
#ifdef __unix__
          int status;

          if (waitpid(cc, &status, 0) == -1){
            my_warn (st, "cc: cannot waitpid");
            THROW;
          }

          if (! WIFEXITED(status) || WEXITSTATUS(status) != 0){
            my_warnx (st, "cannot compile"); // TO.DO: добавить пояснение "see наверху вывод компилятора или невозможно запустить компилятор"
            THROW;
          }
#else
          if (WaitForSingleObject(pi.hProcess, INFINITE) != WAIT_OBJECT_0){
            my_warnx (st, "cl: cannot WaitForSingleObject"); /* TO.DO W: more detail */
            THROW;
          }

          DWORD status;

          if (!GetExitCodeProcess(pi.hProcess, &status)){
            my_warn (st, "cl: cannot get exit code"); /* TO.DO W: more detail */
            THROW;
          }

          if (status != 0){
            my_warnx (st, "cannot compile");
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

          if (handle == 0){
            THROW;
          }
        IN /* Shared object handle */
          void (*mini_command)(void);

#ifdef __unix__
          *(void **)&mini_command = dlsym(handle, "mini_command"); /* TO.DO: posix 2008 говорит, что теперь так не надо? */

          if (my_dlerror(st, "dlsym") == -1){
            THROW;
          }
#else
          /* GetProcAddress(HMODULE, LPCSTR); */
          *(void **)&mini_command = GetProcAddress(handle, "mini_command");

          if (mini_command == NULL){
            my_warnx (st, "cannot get function pointer"); /* TO.DO W: more detail */
            THROW;
          }
#endif

          if (st->check_errno){
            errno = 0;
          }

          (*mini_command)();

          if (check_errno_func(st) == -1){
            THROW;
          }

          result = 0;
        FINALLY /* Shared object handle */
#ifdef __unix__
          dlclose(handle);

          if (my_dlerror(st, "dlclose") == -1){
            THROW;
          }
#else
          if (!FreeLibrary(handle)){
            my_warnx (st, "cannot close handle"); /* TO.DO W: more detail */
            THROW;
          }
#endif
        END /* Shared object handle */

      FINALLY /* Shared object file */
        if (remove(shared) == -1){ /* TO.DO: рушится при setuid(1), fork и т д; можно удалять временные файлы по файловому дескриптору, чтобы не было проблем с chroot'ами; и вообще решить проблему с удалением временных файлов, например, удалением этих файлов сразу же после создания или пайпами */
          my_warn (st, "%s: cannot remove temporary shared object file or DLL (dynamic-link library)", shared);
          THROW;
        }
      END /* Shared object file */
    FINALLY /* Source file */
    // TODO: закомментировано для отладки. в будущем, наверное, добавить опцию, мол, сохранять временные файлы (и как же узнавать их имена?)
      /*if (remove(source) == -1){
        my_warn (st, "%s: cannot remove temporary source file", source);
        THROW;
      }*/
    END /* Source file */
  STOP_EXCEPTIONS

  return result;
}

/* TO.DO: more details */
static void help(struct state *st)
{
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
    ".execvp FILE [ARGV]...\n" /* TO.DO W: как это работает на винде? */
    ".setenv NAME VALUE\n" /* TO.DO W: как это работает на винде? */
    ".system COMMAND\n"
    "\n"
    "/sbin/init example:\n"
    ".setenv PATH /usr/sbin:/usr/bin:/sbin:/bin\n" /* Standard Debian PATH */
    ".system mount -o remount,rw /\n"
  );
}

// TO.DO: функция - полный абзац; я даже не думал об инклудах и портируемости
static void sighandler(int signum)
{
  fprintf(stdout, "mini: got signal %d\n", signum);
  signal(signum, &sighandler);
}

static int eval(struct state *st, const char *command)
{
  int result = -1;

  // TO.DO: почему сделано именно *(headers) == '\0', а не headers == NULL?
  if (*(st->headers) == '\0')
    {
      strcpy(st->headers,
        "#define _POSIX_C_SOURCE 200809L\n" /* Because I want to get all poll constants */
        "#include <stdio.h>\n"
        "#include <poll.h>\n" /* poll.h is needed for mini_poll */
      );
    }

  char *tokens;

  {
    size_t size = strlen(command);
    tokens = (char *)malloc(size + 1);
    memcpy(tokens, command, size + 1);
  }

  char *save_ptr;
  const char *token = _mini_strtok_r(tokens, " ", &save_ptr);

  if (token != NULL){
    if (token[0] == '.'){
      if (strcmp(token, ".q") == 0){
        st->done = 1;
        result = 0;
      }else if (strcmp(token, ".h") == 0){
        help(st);
        result = 0; /* TO.DO Docs: если сделал chroot и т д, то ты сам дурак. Вообще тут полно способов отстрелить себе ногу. Когда делаете exec(...) или .execvp и т д из функции, никто чистить не будет. С форками нужно быть крайне аккуратным */
      }else if (strcmp(token, ".i") == 0){ /* TODO Docs: вы сами должны чесаться по поводу того, где ищет gcc хедеры (следить за pwd и т д) */
        const char *header = _mini_strtok_r(NULL, " ", &save_ptr);
        int len;
        if (header == NULL || (len = strlen(header)) < 3 || !((header[0] == '<' && header[len - 1] == '>') || (header[0] == '\"' && header[len - 1] == '\"'))){
          my_warnx (st, "usage: .i <HEADER>|\"HEADER\"");
        }else{
          if (strlen(st->headers) + strlen("#include ") + strlen(header) + 1 /* '\n' */ + 1 /* '\0' */ > MAX_HEADERS){
            my_warnx (st, ".i: too many headers or too long names of headers");
          }else{
            strcat(st->headers, "#include ");
            strcat(st->headers, header);
            strcat(st->headers, "\n");
            result = 0;
          }
        }
      }else if (strcmp(token, ".l") == 0){
        const char *library = _mini_strtok_r(NULL, " ", &save_ptr);
        if (library == NULL){
          my_warnx (st, "usage: .l LIBRARY");
        }else{
          /* We don't need to check return value, because load_library prints error message automatically */
          if (load_library(st, library, 1 /* global */) != 0){
            result = 0;
          }
        }
      }else if (strcmp(token, ".p") == 0){
        /* WARN! We assume behavior of this version of strtok_r (taken from Gnulib) */
        /* TO.DO: не работает в /sbin/init */
        const char *expression = save_ptr + strspn(save_ptr, " ");
        if (*expression == '\0'){
          my_warnx (st, "usage: .p EXPRESSION");
        }else{
          /* TO.DO: errors */
          /* TO.DO: temporary file, удалить его */
          /* TO.DO: перенаправить ввод, вывод и т д */
          FILE *commands = fopen("/tmp/mini-commands.gdb", "w");
          fprintf(commands, "attach %d\nprint %s\n", getpid(), expression);
          fclose(commands);
          if (st->check_errno){
            errno = 0;
          }
          system("gdb -batch -x /tmp/mini-commands.gdb 2>&1 | grep -v '^[0-9/]'");
          check_errno_func(st); /* TO.DO: this is hack! */
          result = 0;
        }
      }else if (strcmp(token, ".errno+") == 0){
        st->check_errno = 1; /* TO.DO Docs: успешные функции могут менять errno! */
        result = 0;
      }else if (strcmp(token, ".errno-") == 0){
        st->check_errno = 0;
        result = 0;
      }else if (strcmp(token, ".signal+") == 0){
        /* TO.DO: фрагмент - полный абзац; я даже не думал об инклудах и портируемости */
        /* TO.DO Docs: сигналы нужно перевыставлять после их прихода */
        for (int i = 1; i <= 32; ++i){
          if (signal(i, &sighandler) == SIG_ERR){ // bad function
            fprintf(st->err, "mini: cannot set signal handler for signal %d: %s\n", i, strerror(errno));
          }
        }
        result = 0;
      }else if (strcmp(token, ".werror+") == 0){
        st->werror = 1;
        result = 0;
      }else if (strcmp(token, ".werror-") == 0){
        st->werror = 0;
        result = 0;
      }else if (strcmp(token, ".execvp") == 0){
        const char *file = _mini_strtok_r(NULL, " ", &save_ptr);
        if (file == NULL){
          my_warnx (st, "usage: .execvp FILE [ARGV]...");
        }else{
          char *args[MAX_ARGS + 1];
          int i = 0;
          for (;;){
            if (i == MAX_ARGS + 1){
              my_warnx (st, ".execvp: too many arguments");
              break;
            }
            args[i] = _mini_strtok_r(NULL, " ", &save_ptr);
            if (args[i] == NULL){
              execvp(file, args);
              my_warn (st, ".execvp %s ...", file);
              break;
            }
            ++i;
          }
          /* If we are here, something is wrong */
        }
      }else if (strcmp(token, ".setenv") == 0){
        const char *name  = _mini_strtok_r(NULL, " ", &save_ptr);
        const char *value = _mini_strtok_r(NULL, " ", &save_ptr);
        if (name == NULL || value == NULL){
          my_warnx (st, "usage: .setenv NAME VALUE");
        }else{
          if (setenv(name, value, 1 /* Overwrite */) == -1){
            my_warn (st, ".setenv %s %s", name, value);
          }else{
            result = 0;
          }
        }
      }else if (strcmp(token, ".system") == 0){
        /* WARN! We assume behavior of this version of strtok_r (taken from Gnulib) */
        const char *command = save_ptr + strspn(save_ptr, " ");
        if (*command == '\0'){
          my_warnx (st, "usage: .system COMMAND");
        }else{
          int status = system(command);
          if (status == -1){
            my_warn (st, ".system %s", command);
          }else{
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127){
              my_warnx (st, ".system %s: system(...) returned 127, shell is probably unavailable", command);
            }else{
              result = 0;
            }
          }
        }
      }else{
        my_warnx (st, "%s: unknown metacommand", token);
      }
    }else{
      if (_mini_strtok_r(NULL, " ", &save_ptr) == NULL){
        if (strcmp(token, "exit") == 0 || strcmp(token, "quit") == 0 || strcmp(token, "q") == 0){ /* TO.DO: docs: не то же самое, что и exit (EXIT_SUCCESS) */
          st->done = 1;
          result = 0;
        }else if (strcmp(token, "help") == 0 || strcmp(token, "h") == 0){
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
int mini_eval(FILE *out, FILE *err, const char *command)
{
  struct state st = {NULL, out, err};

  return eval(&st, command);
}

// TO.DO: разобраться с работой с сигналами в readline
// TO.DO Docs: mini(...) errors ignored
void mini(FILE *in, FILE *out, FILE *err)
{
  struct state st = {in, out, err};

  int /* bool */ interactive = (isatty(fileno(st.in)) && isatty(fileno(st.out)));

  if (interactive)
    {
      fputs("Welcome to Mini - C read-eval loop. Type \".h\" for help. Type \".q\" to quit\n", st.out);
    }

  eval(&st, ";");

/* TO.DO Сделать всё unbuffered, на случай, если mini запущен во всяких сокетах? Например, до запуска MPI_Init в mpich-программе */
/* TO.DO Мож принимать в mini fd, а не FILE? А мож имя файла? */
/* TO.DO Переписать нормально, т. е. без вложенных if/#ifdef? */
#ifdef __unix__
  if (interactive){
    rl_instream  = st.in;
    rl_outstream = st.out;
    using_history();

    while (!st.done){
      char *buffer = readline("mini> ");

      if (buffer == NULL){
        fputs(".q\n", st.out);
        break;
      }

      if (*buffer != '\0' && *buffer != ' '){
        add_history(buffer);
      }

      eval(&st, buffer);

      free(buffer);
    }
  }else
#endif
  {
    char *buffer = NULL;
    size_t buffer_size;

    while (!st.done){
#ifndef __unix__
      if (interactive){
        fputs("mini> ", st.out);
        fflush(st.out);
      }
#endif
      /* Only _mini_getline returns ptrdiff_t, getline returns ssize_t */
      ptrdiff_t len = _mini_getline(&buffer, &buffer_size, st.in);

      if (len == -1){
#ifndef __unix__
        if (interactive){
          fputs(".q\n", st.out);
        }
#endif
        break;
      }

      if (buffer[len - 1] == '\n'){
        buffer[len - 1] = '\0';
      }

      eval(&st, buffer);
    }

    free(buffer);
  }
}
