#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H
#include <ctime>
#include <cmath>
#include <string>
using namespace std;

typedef struct
{
    int status;
    char type;
    char fit[2];
    int size;
    int start;
    char name[50];
}Partition;

typedef struct
{
    int size;
    char date[25];
    int signature;
    char fit[2];
    Partition partitions[4];
}Mbr;

typedef struct
{
    char status;
    char fit[2];
    int start;
    int size;
    int next;
    char name[16];
}Ebr;

typedef struct{
    char id[4];
    char name[10];
    char password[10];
    int group;
    int tag;
}user;

typedef struct{
    char id[4];
    char name[10];
}group;


class file_system
{
public:
    file_system();
    static int  structures_number(int fs, int partition_size);
};

typedef struct iNodo
{
    int i_uid;
    int i_gid;
    int i_size;
    time_t i_atime;
    time_t i_ctime;
    time_t i_mtime;
    int i_block[15];
    char i_type;
    int i_perm;

    iNodo()
    {
         i_uid=  0;
         i_gid = 0;
         i_size = 0;
         i_atime = 0;
         i_ctime = 0;
         i_mtime = 0;
         i_type = 0;
         i_perm = 000;
         for (int i = 0; i < 12;i++) i_block[i] = 0;
    }
    iNodo(int i_uid, int i_gid, int i_size, int i_type, int i_perm)
    {
         this->i_uid=  i_uid;
         this->i_gid = i_gid;
         this->i_size = i_size;
         i_atime = time(nullptr);
         i_ctime = time(nullptr);
         i_mtime = time(nullptr);
         this->i_type = i_type;
         this->i_perm = i_perm;
         for (int i = 0; i < 12;i++) i_block[i] = 0;
    }
}iNodo;

typedef struct content
{
    char b_name[12];
    int b_inodo;
    content()
    {
        for (int i =0;i <12;i++) b_name[i] = '\0';
        b_inodo = 0;
    }
    content(string name, int inode)
    {
        for (unsigned int i =0;i <name.size();i++) b_name[i] = name[i];
        b_inodo = inode;
    }
}content;

typedef struct Bloque_Carpeta
{
    content* b_content[4];
    Bloque_Carpeta()
    {
        for(auto cont : b_content) cont = new content();
    }
}Bloque_Carpeta;

typedef  struct Bloque_Archivo
{
    char b_content[64];
    Bloque_Archivo()
    {
        for (int i = 0; i < 64;i++) b_content[i] = '\0';
    }
    //ese constructor escribe una cadena en el bloque
    Bloque_Archivo(string st)
    {
        for (int i = 0; i < 64;i++) b_content[i] = '\0';
        for (unsigned long i = 0; i < st.size() ;i++) b_content[i] = st[i];
    }
}Bloque_Archivo;

typedef struct Bloque_apuntador
{
    int b_pointers[16];
    Bloque_apuntador()
    {
        for (int i = 0;i<16;i++) b_pointers[i] = 0;
    }
} Bloque_apuntador;

typedef struct Bitacora
{
    char log_tipo_operacion;
    char log_tipo;
    char log_nombre[100];
    char log_contenido;
    char log_fecha[20];
    Bitacora()
    {
        log_tipo_operacion = 'N';
        log_tipo = 'N';
        log_contenido = 'N';
    }
}Bitacora;

typedef struct SuperBloque
{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_size;
    int s_block_size;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;

    SuperBloque()
    {
        s_filesystem_type = 0;
        s_inodes_count = 0;
        s_blocks_count = 0;
        s_free_blocks_count = 0;
        s_free_inodes_count = 0;
        s_mtime = 0;
        s_umtime = 0;
        s_mnt_count = 0;
        s_magic = 0;
        s_inode_size = 0;
        s_block_size = 0;
        s_firts_ino = 0;
        s_first_blo = 0;
        s_bm_inode_start = 0;
        s_bm_block_start = 0;
        s_inode_start = 0;
        s_block_start = 0;
    }

    SuperBloque(Partition current_p, int fs)
    {
        //calculando numero de estructuras para size de la particion
        int st_number = file_system::structures_number(fs,current_p.size);
        // inicio de direcciones de estructuras
        s_bm_inode_start = current_p.start + static_cast<int>(sizeof (SuperBloque));
        if(fs == 3) s_bm_inode_start += static_cast<int>(sizeof (Bitacora));
        s_bm_block_start = s_bm_inode_start + st_number;
        s_inode_start = s_bm_block_start + ( st_number * 3 );
        s_block_start = s_inode_start + st_number * static_cast<int>(sizeof (iNodo));
        //atributos del superbloque
        s_filesystem_type = fs;
        s_inodes_count = 2;
        s_blocks_count = 2;
        s_free_blocks_count = (3 * st_number) - 2;
        s_free_inodes_count = st_number - 2;
        s_mtime = 0;
        s_umtime = 0;
        s_mnt_count = 0;
        s_magic = 201700831;
        s_inode_size = sizeof (iNodo);
        s_block_size = sizeof (Bloque_Archivo);
        s_firts_ino = 2;
        s_first_blo = 2;
    }
}SuperBloque;


#endif // FILE_SYSTEM_H
