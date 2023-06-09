/* Copyright (C) 1991 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with the GNU C Library; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* ANSI and traditional C compatibility macros

   ANSI C is assumed if __STDC__ is #defined.

   Macro       ANSI C definition  Traditional C definition
   ---------   ---- - ----------  ----------- - ----------
   PTR          `void *'           `char *'
   LONG_DOUBLE  `long double'      `double'
   CONST        `const'            `'
   VOLATILE     `volatile'         `'
   SIGNED       `signed'           `'
   PTRCONST     `void *const'      `char *'

   DEFUN(name, arglist, args)
 	'name'       defines the function.
	'arglist'    lists the arguments, separated by commas and enclosed in
	             parentheses.  'arglist' becomes the argument list in
                traditional C.
	'args'       list of arguments with their types.  It becomes a prototype in
	             ANSI C, and the type declarations in traditional C.  Arguments
                should be separated with `AND'.  For functions with a variable
                number of arguments, the last thing listed should be `DOTS'.

   DEFUN_VOID(name)
	'name'       defines a function which takes no arguments.

   EXFUN(name, prototype)
   'name'       declares an external function.
   'prototype'  in ANSI C it is `NAMEPROTOTYPE' (so 'prototype' should be
                enclosed in parentheses).  In traditional C it is `NAME()'.
	             For a function that takes no arguments, 'prototype' should
                be `(NOARGS)'.

   Examples:

	extern int EXFUN(printf, (CONST char *format DOTS));

	int
   DEFUN(fprintf, (stream, format),
		  FILE *stream AND CONST char *format DOTS)
   {
       ...
   }

	void
   DEFUN_VOID(abort)
   {
       ...
   }
*/

#ifndef	_ANSIDECL_H

#define	_ANSIDECL_H	1


/* Every source file includes this file,
   so they will all get the switch for lint.  */
/* LINTLIBRARY */


#ifdef	__STDC__

#define	PTR            void *
#define	PTRCONST       void *CONST
#define	LONG_DOUBLE    long double

#define	AND            ,
#define	NOARGS         void
#define	CONST          const
#define	VOLATILE       volatile
#define	SIGNED         signed
#define	DOTS           , ...

#define	EXFUN(name, proto)            name proto
#define	DEFUN(name, arglist, args)    name(args)
#define	DEFUN_VOID(name)              name(NOARGS)

#else	/* Not ANSI C.  */

#define	PTR            char *
#define	PTRCONST	PTR
#define	LONG_DOUBLE    double

#define	AND            ;
#define	NOARGS
#define	CONST
#define	VOLATILE
#define	SIGNED
#define	DOTS

#define	EXFUN(name, proto)            name()
#define	DEFUN(name, arglist, args)    name arglist args;
#define	DEFUN_VOID(name)              name()

#endif	/* ANSI C.  */


#endif	/* ansidecl.h	*/
