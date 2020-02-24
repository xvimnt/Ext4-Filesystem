#ifndef IO_USER_H
#define IO_USER_H
#include <iostream>
#include <fstream> //para leer desde un archivo
#include <parser.h>   // Nuestro parser
#include <scanner.h>  // Nuestro scanner
#include <semantic.h> // Nuestro Analizador Semantico
#include <plotter.h> // nuestro graficador

//funciones de Qt
#include <QString>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>

//declaraciones
extern node * root;

class io_user
{
public:
    io_user();
    static void analize(std::string input);
    static void read_from_file(Metadata data);
    static void pause_system();
};

#endif // IO_USER_H
