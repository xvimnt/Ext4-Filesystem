%option case-insensitive
%option yylineno
%{
    #include <cstdlib>
    #include "parser.h"
    #include <QString>
    #include <stdio.h>
    #include <iostream>
    #include <node.h>
    #include "qlist.h"
    #include "error.h"

    int yylex();
        void yyerror(const char *s);
        int linea=0;
        int columna = 0;
        int anterior = 0;
        node *root;
%}

%option noyywrap
%option nounput
%option header-file= "scanner.h"
%option outfile="scanner.cpp"

digit [0-9]
integer "-"?{digit}+
number "-"?{digit}+("."{digit}+)
letra [a-zñA-ZÑ]+
decimal [0-9]+"."[0-9]+
iden {letra}({letra}|{digit}|"_")*
String "\""[^"\""]*"\""
path  ("/"|{letra})({letra}|{digit}|"_"|"/"|".")*

%%
INITIAL
{

    "-p"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tpcreation;}
    "-cont"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return truta;}
    "-size"  	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tsize;}
    "-fit"   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tfit;}
    "-unit" 	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tunit;}
    "-path" 	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tpath;}
    "-type"   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return ttype;}
    "-delete" { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tdelete;}
    "-name"	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tname;}
    "-add"	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tadd;}
    "-id"	   { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tid;}
    "mkdisk"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkdisk;}
    "rmdisk"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return trmdisk;}
    "fdisk"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tfdisk;}
    "mkfs"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkfs;}
    "mount"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmount;}
    "unmount"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tunmount;}
    "=" 	{ columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return equal;}
    "login"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tlogin;}
    "-usr"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tusr;}
    "2fs"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return t2fs;}
    "3fs"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return t3fs;}
    "-fs"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tfs;}
    "-pwd"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tpwd;}
    "logout"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tlogout;}
    "mkgrp"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkgrp;}
    "rmgrp"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return trmgrp;}
    "mkusr"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkusr;}
    "-grp"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tgrp;}
    "rmusr"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return trmusr;}
    "chmod"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tchmod;}
    "-ugo"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tugo;}
    "-r"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tplusr;}
    "-ruta"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return truta;}
    "mkfile"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkfile;}
    "cat"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tcat;}
    "rm"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkgrp;}
    "edit"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkgrp;}
    "ren"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tren;}
    "rep"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return trep;}
    "mkdir"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmkdir;}
    "cp"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tcp;}
    "-destiny"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tdestiny;}
    "-iddestiny"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tiddestiny;}
    "mv"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tmv;}
    "find"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tfind;}
    "-file"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tfile;}
    "chown"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tchown;}
    "chgrp"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tchgrp;}
    "exec"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return texec;}
    "pause"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tpause;}
    "recovery"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return trecovery;}
    "loss"  { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return tloss;}
    {iden}     { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return(tiden);}
    {integer}     { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return(integer);}
    {String}     { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return(String);}
    {path}     { columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);return(linux_path);}

    "\t" 	 {strcpy(yylval.TEXT , yytext); columna = 26;}
    [[:blank:]]	 {columna++;}
    "\n" 	 	 {linea++; columna = 0; anterior = 0;}
    .   {
            columna = columna+ anterior;  anterior = strlen(yytext);strcpy(yylval.TEXT, yytext);
            std::cout <<yytext <<" Error Lexico en la linea "<< linea << " Columna:"<< columna<<  std::endl;
        }
}
%%
