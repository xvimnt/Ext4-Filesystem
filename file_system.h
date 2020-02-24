#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

typedef struct
{
    char sb_nombre_hd[25];
    int  sb_arbol_virtual_count;
    int  sb_detalle_directorio_count;
    int  sb_inodos_count;
    int  sb_bloques_count;
    int  sb_arbol_virtual_free;
    int  sb_detalle_directorio_free;
    int  sb_inodos_free;
    int  sb_bloques_free;
    char sb_date_creacion[20];
    char sb_date_ultimo_montaje[20];
    int  sb_montajes_count;
    int  sb_ap_bitmap_arbol_directorio;
    int  sb_ap_arbol_directorio;
    int  sb_ap_bitmap_detalle_directorio;
    int  sb_ap_detalle_directorio;
    int  sb_ap_bitmap_tabla_inodo;
    int  sb_ap_tabla_inodo;
    int  sb_ap_bitmap_bloques;
    int  sb_ap_bloques;
    int  sb_ap_log;
    int  sb_size_struct_arbol_directorio;
    int  sb_size_struct_detalle_directorio;
    int  sb_size_struct_inodo;
    int  sb_size_struct_bloque;
    int  sb_first_free_bit_arbol_directorio;
    int  sb_first_free_bit_detalle_directorio;
    int  sb_first_free_bit_tabla_inodo;
    int  sb_first_free_bit_bloques;
    int  sb_magic_num;
}SuperBloque;

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
    char avd_fecha_creacion[20];
    char avd_nombre_directorio[25];
    int  avd_ap_array_subdirectorios[6];
    int  avd_ap_detalle_directorio;
    int  avd_ap_arbol_virtual_directorio;
    char avd_proper[25];
}AVD;

typedef struct
{
    char dd_file_nombre[25];
    int  dd_file_ap_inodo;
    char dd_file_date_creacion[20];
    char dd_file_date_modificacion[20];
}DD_File;

typedef struct
{
    DD_File dd_array_files[5];
    int     dd_ap_detalle_directorio;
}DD;

typedef struct
{
    int  i_count_inodo;
    int  i_size_archivo;
    int  i_count_bloques_asignados;
    int  i_array_bloques[4];
    int  i_ap_indirecto;
    char i_id_proper[25];
}iNodo;

typedef struct
{
    char db_data[25];
}Bloque;

typedef struct
{
    char log_tipo_operacion;
    char log_tipo;
    char log_nombre[100];
    char log_contenido;
    char log_fecha[20];
}Bitacora;

typedef struct{
    char id[4];
    char name[10];
    char password[10];
    char group_name[10];
}user;

typedef struct{
    char id[4];
    char name[10];
}group;

class file_system
{
public:
    file_system();
    static int  structures_number(int Tparticion);
};

#endif // FILE_SYSTEM_H
