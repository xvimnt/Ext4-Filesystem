#include "file_system.h"

int  file_system::structures_number(int Tparticion)
{
    int tamsuperbloque=sizeof(SuperBloque);
    int tamAVD=sizeof(AVD);
    int tamDetalleDirectorio=sizeof(DD);
    int tamInodo=sizeof(iNodo);

    return (Tparticion-(2*tamsuperbloque))/(27+tamAVD+tamDetalleDirectorio+(5*tamInodo)+(20*sizeof(Bloque))+sizeof(Bitacora));
}

file_system::file_system()
{

}
