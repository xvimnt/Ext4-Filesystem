/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_PARSER_H_INCLUDED
# define YY_YY_PARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    tsize = 258,
    tfit = 259,
    tunit = 260,
    tpath = 261,
    ttype = 262,
    tdelete = 263,
    tname = 264,
    tadd = 265,
    tid = 266,
    tmkdisk = 267,
    trmdisk = 268,
    tfdisk = 269,
    tmount = 270,
    tunmount = 271,
    tallocation = 272,
    equal = 273,
    minus = 274,
    tiden = 275,
    integer = 276,
    String = 277,
    linux_path = 278,
    tlogin = 279,
    tmkfs = 280,
    tfs = 281,
    tusr = 282,
    tpwd = 283,
    tlogout = 284,
    tmkgrp = 285,
    trmgrp = 286,
    tmkusr = 287,
    tgrp = 288,
    trmusr = 289,
    tchmod = 290,
    tugo = 291,
    tplusr = 292,
    tmkfile = 293,
    tpcreation = 294,
    tcont = 295,
    tcat = 296,
    tren = 297,
    tmkdir = 298,
    tcp = 299,
    tdestiny = 300,
    tiddestiny = 301,
    tmv = 302,
    tfind = 303,
    tchown = 304,
    trep = 305,
    tchgrp = 306,
    texec = 307,
    tpause = 308,
    trecovery = 309,
    tloss = 310,
    truta = 311,
    tidn = 312,
    tfile = 313,
    t2fs = 314,
    t3fs = 315
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 25 "sintactico.y"

  char TEXT [256];
  class node *Node;

#line 123 "parser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_PARSER_H_INCLUDED  */
