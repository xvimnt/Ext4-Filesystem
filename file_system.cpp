#include "file_system.h"

int  file_system::structures_number(int fs, int partition_size)
{
    double sb = sizeof (struct::SuperBloque);
    double inode = sizeof(iNodo);
    double block =  sizeof (Bloque_Archivo);
    double journal = sizeof (Bitacora);

    double structures_size = 4 + 3 * block + inode;
    if(fs == 3) structures_size += journal;

    double free_space = partition_size - sb;

    return static_cast<int>(floor(free_space / structures_size));
}

file_system::file_system()
{

}
