#ifndef PLOTTER_H
#define PLOTTER_H
#include <fstream>
#include <iostream>
using namespace std;
#include "node.h"
#include <file_system.h>
class graficador
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
    static void generate_bitmap(SuperBloque sb,QString choice,int partition_size,string output_path,FILE *file);
    static int get_structures_number(int SizeParticion);
    static void generate_tree_dd(Mbr mbr2,QString pathT,QString pathSalida,QString extension);
};

#endif // PLOTTER_H
