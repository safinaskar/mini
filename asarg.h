/*  asarg.h - main header of the Asarg Library

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

#ifndef _ASARG_H
#define _ASARG_H

#ifdef __cplusplus
extern "C" {
#endif

enum asarg_type{
	asarg_none = 0,

	asarg_char,

	asarg_long,
	asarg_ulong,
	asarg_llong,
	asarg_ullong,

	asarg_float,
	asarg_double,
	asarg_ldouble,

	asarg_str,

	asarg_file_in,
	asarg_file_out
};

struct asarg_opt{
	const char        *short_names;
	const char        *long_names;
	const char        *doc;
	void              *ptr;
	int                val;
	void             (*func)(int val); /* TODO: функция не вызывается для параметров */
	enum asarg_type    param;
	const char        *param_doc;
	int /* bool */     optional;
};

void asarg(char ***argvp, const struct asarg_opt *opts, const char *opers_doc);
void asarg_oper(char ***argvp, void *ptr, enum asarg_type type, int /* bool */ optional);
void asarg_end(char **const *argvp);

void asarg_help(int unused);

#ifdef __cplusplus
}
#endif

#endif /* ! _ASARG_H */
