#ifndef _MINI_H
#define _MINI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

int mini_eval(FILE *out, FILE *err, const char *command);
void mini(FILE *in, FILE *out, FILE *err);

#ifdef __cplusplus
}
#endif

#endif
