#ifndef PLOTTER_H
#define PLOTTER_H
#include <fstream>
#include <iostream>
using namespace std;
#include "node.h"
#include "tools.h"
#include <file_system.h>

class graficador : public tools
{
public:
    graficador();
    graficador(node *raiz);
    node *raiz;
    int contador;
    QString grafo;
    QString graficar();
    void recorrerAST(QString padre, node *hijo);
    QString escapar(QString cadena);
    void generarImagen();
    static void generate_SB_table_content(string output_path, SuperBloque sb);
    static void generate_graphviz_table(string extension,QString dot,string path,QString disco,string tipo);
    static void generate_mbr_table_content(string output_path,Mbr mbr, string choice);
    static void generate_ebr_table_content(string output_path,Ebr ebr, string choice);
    static void generate_dsk_table_content(string output_path,Mbr mbr,FILE *file);
    static void generate_bitmap(SuperBloque sb,QString choice,int partition_size,string output_path,FILE *file,int fs);
    static void generate_file_report(string full_file,string output_path);
    static void generate_tables(string output_path,QString extension,FILE *file2, int choice,SuperBloque sb);
};

#endif // PLOTTER_H
