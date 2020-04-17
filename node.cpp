#include "node.h"
#include <qdebug.h> // Para imprimir en consola.

node::node()
{
    tipo ="";
    valor = "";
    linea = 0;
    columna = 0;
    hijos =  QList<node*>();
}

node::node(int l, int c, QString t, QString v)
{
    linea = l;
    columna = c;
    tipo = t;
    valor = v;
    tipo_ = getTipo();
    hijos = QList<node*>();
}

void node::add(node *n)
{
    this->hijos.append(n);
}

int node::getTipo()
{
    if(this->tipo=="size"){return 1;}
    if(this->tipo=="fit"){return 2;}
    if(this->tipo=="unit"){return 3;}
    if(this->tipo=="path"){return 4;}
    if(this->tipo=="type"){return 5;}
    if(this->tipo=="delete"){return 6;}
    if(this->tipo=="name"){return 7;}
    if(this->tipo=="add"){return 8;}
    if(this->tipo=="id"){return 9;}
    if(this->tipo=="number"){return 11;}
    if(this->tipo=="string"){return 12;}
    if(this->tipo=="attrib"){return 13;}
    if(this->tipo=="attrib_list"){return 14;}
    if(this->tipo=="mkdisk"){return 15;}
    if(this->tipo=="rmdisk"){return 16;}
    if(this->tipo=="fdisk"){return 17;}
    if(this->tipo=="mount"){return 18;}
    if(this->tipo=="unmount"){return 19;}
    if(this->tipo=="command"){return 20;}
    if(this->tipo=="idn"){return 21;}
    if(this->tipo=="exec"){return 22;}
    if(this->tipo=="pause"){return 23;}
    //aqui empieza los tipos del fs
    if(this->tipo=="makegroup"){return 24;}
    if(this->tipo=="makefs"){return 25;}
    if(this->tipo=="rep"){return 26;}
    if(this->tipo=="recovery"){return 27;}
    if(this->tipo=="loss"){return 28;}
    if(this->tipo=="usr"){return 29;}
    if(this->tipo=="pwd"){return 30;}
    if(this->tipo=="login"){return 31;}
    if(this->tipo=="logout"){return 32;}
    if(this->tipo=="ruta"){return 33;}
    if(this->tipo=="mkdir"){return 34;}
    if(this->tipo=="tp"){return 35;}
    if(this->tipo=="mkfile"){return 36;}
    if(this->tipo=="cont"){return 37;}
    if(this->tipo=="fs"){return 38;}
    if(this->tipo=="rmgroup"){return 39;}
    if(this->tipo=="grp"){return 40;}
    if(this->tipo=="mkusr"){return 41;}
    if(this->tipo=="rmusr"){return 42;}
    if(this->tipo=="chmod"){return 43;}
    if(this->tipo=="ugo"){return 44;}
    if(this->tipo=="tr"){return 45;}
    if(this->tipo=="cat"){return 46;}
    if(this->tipo=="file"){return 47;}
    if(this->tipo=="ren"){return 48;}
    if(this->tipo=="cp"){return 49;}
    if(this->tipo=="dest"){return 50;}
    return 0;
}
