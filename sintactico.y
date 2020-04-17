%{
#include "scanner.h"//se importa el header del analisis sintactico
#include "node.h"
#include "qdebug.h"
#include <iostream>
#include <map>
extern int yylineno; //linea actual donde se encuentra el parser (analisis lexico) lo maneja BISON
extern int columna; //columna actual donde se encuentra el parser (analisis lexico) lo maneja BISON
extern char *yytext; //lexema actual donde esta el parser (analisis lexico) lo maneja BISON
extern node *root; // Raiz del arbol

int yyerror(const char* msj)
{
    std::cout << msj << " " << yytext << std::endl;
    return 0;
}

%}
%defines "parser.h"
%output "parser.cpp"
%define parse.error verbose
%locations

%union
{
  char TEXT [256];
  class node *Node;
}

%start START;

%token <TEXT>tsize
%token <TEXT>tfit
%token <TEXT>tunit
%token <TEXT>tpath
%token <TEXT>ttype
%token <TEXT>tdelete
%token <TEXT>tname
%token <TEXT>tadd
%token <TEXT>tid
%token <TEXT>tmkdisk
%token <TEXT>trmdisk
%token <TEXT>tfdisk
%token <TEXT>tmount
%token <TEXT>tunmount
%token <TEXT>tallocation
%token <TEXT>equal
%token <TEXT>minus
%token <TEXT>tiden
%token <TEXT>integer
%token <TEXT>String
%token <TEXT>linux_path
//aqui inician los tokens del fs
%token <TEXT>tlogin
%token <TEXT>tmkfs
%token <TEXT>tfs
%token <TEXT>tusr
%token <TEXT>tpwd
%token <TEXT>tlogout
%token <TEXT>tmkgrp
%token <TEXT>trmgrp
%token <TEXT>tmkusr
%token <TEXT>tgrp
%token <TEXT>trmusr
%token <TEXT>tchmod
%token <TEXT>tugo
%token <TEXT>tplusr
%token <TEXT>tmkfile
%token <TEXT>tpcreation
%token <TEXT>tcont
%token <TEXT>tcat
%token <TEXT>tren
%token <TEXT>tmkdir
%token <TEXT>tcp
%token <TEXT>tdestiny
%token <TEXT>tiddestiny
%token <TEXT>tmv
%token <TEXT>tfind
%token <TEXT>tchown
%token <TEXT>trep
%token <TEXT>tchgrp
%token <TEXT>texec
%token <TEXT>tpause
%token <TEXT>trecovery
%token <TEXT>tloss
%token <TEXT>truta
%token <TEXT>tidn
%token <TEXT>tfile
%token <TEXT>t2fs
%token <TEXT>t3fs

%type <Node> START
%type <Node> COMMAND
%type <Node> COMMAND_TYPE
%type <Node> COMMAND_ATTRIB_LIST
%type <Node> ATTRIB
%type <Node> ATTRIB_NAME
%type <Node> VALUE

%%
START: COMMAND { root = $1; }
| tpause {root = new node(yylineno, columna,"pause",yytext); }
| tlogout {root = new node(yylineno, columna,"logout",yytext); }
;

COMMAND: COMMAND_TYPE COMMAND_ATTRIB_LIST {
    $$ = new node(yylineno, columna,"command",yytext);
    $$->add($1);
    $$->add($2);
  }
;

COMMAND_TYPE: tmkdisk { $$ = new node(yylineno, columna,"mkdisk",yytext);}
    | trmdisk { $$ = new node(yylineno, columna,"rmdisk",yytext);}
    | tfdisk { $$ = new node(yylineno, columna,"fdisk",yytext);}
    | tmount { $$ = new node(yylineno, columna,"mount",yytext);}
    | tunmount { $$ = new node(yylineno, columna,"unmount",yytext);}
    | texec { $$ = new node(yylineno, columna,"exec",yytext);}
    | tmkgrp { $$ = new node(yylineno, columna,"makegroup",yytext);}
    | trmgrp { $$ = new node(yylineno, columna,"rmgroup",yytext);}
    | tmkfs { $$ = new node(yylineno, columna,"makefs",yytext);}
    | trep { $$ = new node(yylineno, columna,"rep",yytext);}
    | trecovery { $$ = new node(yylineno, columna,"recovery",yytext);}
    | tloss { $$ = new node(yylineno, columna,"loss",yytext);}
    | tlogin { $$ = new node(yylineno, columna,"login",yytext);}
    | tmkdir { $$ = new node(yylineno, columna,"mkdir",yytext);}
    | tmkfile { $$ = new node(yylineno, columna,"mkfile",yytext);}
    | tmkusr { $$ = new node(yylineno, columna,"mkusr",yytext);}
    | trmusr { $$ = new node(yylineno, columna,"rmusr",yytext);}
    | tchmod { $$ = new node(yylineno, columna,"chmod",yytext);}
    | tcat { $$ = new node(yylineno, columna,"cat",yytext);}
    | tren { $$ = new node(yylineno, columna,"ren",yytext);}
    | tcp { $$ = new node(yylineno, columna,"cp",yytext);}
;


COMMAND_ATTRIB_LIST: COMMAND_ATTRIB_LIST ATTRIB{
                                                    $$ = $1;
                                                    $$->add($2);
                                               }
                   | ATTRIB {
                                $$ = new node(yylineno, columna,"attrib_list",yytext);
                                $$->add($1);
                            }
;

ATTRIB:  ATTRIB_NAME equal VALUE {
                                        $$ = new node(yylineno, columna,"attrib",yytext);
                                        $$->add($1);
                                        $$->add($3);
                                      }
        | tpcreation { $$ = new node(yylineno, columna,"tp",yytext);}
        | tplusr { $$ = new node(yylineno, columna,"tr",yytext);}
;

VALUE: tiden { $$ = new node(yylineno, columna,"iden",yytext);}
    | integer { $$ = new node(yylineno, columna,"number",yytext);}
    | String { $$ = new node(yylineno, columna,"string",yytext);}
    | linux_path { $$ = new node(yylineno, columna,"l_path",yytext);}
    | t2fs { $$ = new node(yylineno, columna,"2fs",yytext);}
    | t3fs { $$ = new node(yylineno, columna,"3fs",yytext);}
;

ATTRIB_NAME: tsize { $$ = new node(yylineno, columna,"size",yytext);}
        |tfit { $$ = new node(yylineno, columna,"fit",yytext);}
        |tunit { $$ = new node(yylineno, columna,"unit",yytext);}
        |tpath { $$ = new node(yylineno, columna,"path",yytext);}
        |ttype { $$ = new node(yylineno, columna,"type",yytext);}
        |tdelete { $$ = new node(yylineno, columna,"delete",yytext);}
        |tname { $$ = new node(yylineno, columna,"name",yytext);}
        |tadd { $$ = new node(yylineno, columna,"add",yytext);}
        |tfs { $$ = new node(yylineno, columna,"fs",yytext);}
        |tid { $$ = new node(yylineno, columna,"id",yytext);}
        | tgrp  { $$ = new node(yylineno, columna,"grp",yytext);}
        | tusr  { $$ = new node(yylineno, columna,"usr",yytext);}
        | tpwd { $$ = new node(yylineno, columna,"pwd",yytext);}
        | truta { $$ = new node(yylineno, columna,"ruta",yytext);}
        | tcont { $$ = new node(yylineno, columna,"cont",yytext);}
        | tugo { $$ = new node(yylineno, columna,"ugo",yytext);}
        | tfile { $$ = new node(yylineno, columna,"file",yytext);}
        | tdestiny { $$ = new node(yylineno, columna,"dest",yytext);}
;
%%
