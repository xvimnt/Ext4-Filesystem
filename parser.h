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
    tusr = 281,
    tpwd = 282,
    tlogout = 283,
    tmkgrp = 284,
    trmgrp = 285,
    tmkusr = 286,
    tgrp = 287,
    trmusr = 288,
    tchmod = 289,
    tugo = 290,
    tplusr = 291,
    tmkfile = 292,
    tpcreation = 293,
    tcont = 294,
    tcat = 295,
    tren = 296,
    tmkdir = 297,
    tcp = 298,
    tdestiny = 299,
    tiddestiny = 300,
    tmv = 301,
    tfind = 302,
    tchown = 303,
    trep = 304,
    tchgrp = 305,
    texec = 306,
    tpause = 307,
    trecovery = 308,
    tloss = 309,
    truta = 310,
    tidn = 311
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 25 "sintactico.y"

  char TEXT [256];
  class node *Node;

#line 119 "parser.h"

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
