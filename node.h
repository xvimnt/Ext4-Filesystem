#ifndef NODE_H
#define NODE_H
#include "QString"
#include "qlist.h"
#include <QString>
typedef struct
{
  //mandatory values
  int size;
  char path[500];
  char group[10];
  char name[50];
  char user[10];
  //optional values
  char unit;
  //for partitions
  char type;
  char format_type[5];
  char fit[2];
  //for the fs
  char fs[3];
  char id[4];
  char Delete[4];
  int add;
  //for unmount
  bool recursively;
  QString content;

}Metadata;

class node
{
public:
    node();
    node(int l, int c, QString t, QString v);
    void add(node *n);
    QString tipo;
    QString valor;
    int linea;
    int columna;
    int tipo_; // Este nos servirá para la ejecución, ya que aquí no hay swicht(QString); vamos a hacer un Swicht(int);
    QList<node*> hijos;
    int getTipo();
    //atributos del analizador semantico
    Metadata metadata;
};

#endif // NODE_H
