#ifndef TOOLS_H
#define TOOLS_H
//el fs
#include <file_system.h>
#include <node.h>
//librerias de c
#include <iostream>
using namespace std;
static int report_count = 0;
static bool is_deleted = false;

class tools
{
public:
    tools();
    QString get_pointer_blocks_report(int dir, FILE *file,int type, int recursevely);
    static void set_count(int cont);
    static bool get_deleted();
    int make_new_pointer_block(FILE* file, SuperBloque sb);
    int make_new_folder_block(FILE *file, SuperBloque sb);
    int get_last_folder_block_with_free_space(int inode_dir, FILE *file, SuperBloque sb);
    int make_folder_in_inode(int inode_dir, FILE *file, QString name, int size, SuperBloque sb);
    int search_path_in_inode(iNodo inode, FILE *file, QString name);
    int make_folder_in_block(FILE *file,int dir,iNodo parent, int size, SuperBloque sb,const char * name);
    int make_file_in_block(FILE *file,int dir,iNodo parent, int size, SuperBloque sb,const char * name,QString content);
    bool append_text_in_inode(int inode_dir, QString new_text, FILE *file, SuperBloque sb);
    int get_free_block(int dir,FILE *file);
    int get_free_pointer(int dir,FILE *file);
    void change_permissions_recursively(iNodo inode, int ugo,FILE* file, int inode_start);
    char * convertNumberIntoArray(unsigned int number);
    int get_group_id_by_name(QStringList list, QString name);
    void clean(char aux[], int n);
    QString get_full_file(iNodo inode,FILE *file);
    int get_free_block_index_by_bitmap(SuperBloque sb, FILE *file, bool overwrite);
    int get_free_inode_index_by_bitmap(SuperBloque sb, FILE *file, bool overwrite);
    char return_free_char(char pointer);
    string path_by_id(char unit);
    int block_empty_space(char b_content[64]);
    int get_last_block_index(int i_block[15],FILE *file);
    void read_mounted_partitions();
    void read_partitions(char path[500]);
    int get_block_count(int size);
    string get_current_time();
    void make_directories(char path[500]);
    Mbr get_mbr(FILE* file);
    string get_report_format(QString path);
    int get_primary_index(char fit[2], Mbr mbr, int new_size);
    void fix_first_slash(char st[500]);
    void fix_path(char * source, int size);
    Partition get_mounted_partition(char id[4]);
    int get_extended_partition_index(Mbr mbr);
    SuperBloque get_part_sb(FILE* file, int pt_start);
    FILE *get_file_by_id(char *id);
    //Para graficar
    QString get_tree_report(iNodo inode,FILE* file);
    void switch_deleted(bool d_var);
    QString get_full_inode_report(SuperBloque sb, FILE *file);
    void print_ls(FILE* file, iNodo inode, QStringList users);
    int find_inode(FILE* file, SuperBloque sb, QString path);
    void add_new_log(Bitacora* log,FILE *file,int log_start);
    QString get_log_report(FILE *file,int log_start);
    QString get_full_blocks_report(SuperBloque sb, FILE *file);
private:
    QString get_pointer_string_report(Bloque_apuntador pointer);
    QString get_file_block_report(Bloque_Archivo file_block);
    QString get_folder_block_report(Bloque_Carpeta folder_block);
    QString switch_block_report(int type,FILE *file, int dir);
    QString get_blocks_report(iNodo inode, FILE * file);
    QString get_pointer_ls(Bloque_apuntador pointer_block,FILE *file, QStringList users, int recursevly);
    QString get_pointer_report(int dir, FILE *file,int type, int recursevely);
    QString switch_blocks(int type,FILE *file, int dir);
    QString get_inode_report(iNodo inode);
    int get_block_with_free_content(int dir, FILE *file, SuperBloque sb, int recursevely);
    int search_path_in_indirect_blocks(iNodo inode, FILE* file, QString name);
    int search_path_in_2d(int pointer_dir,FILE *file,QString name);
    int search_path_in_3d(int pointer_dir,FILE *file,QString name);
    int search_path_in_pointer(int pointer_dir,FILE *file,QString name);
    int get_folder_direction(FILE *file,int dir, QString folder_name);
    int append_text_in_a_new_2d(iNodo *inode, QString new_text, FILE *file, SuperBloque sb);
    int append_text_in_2d(QString txt, FILE *file, int dir);
    int append_text_in_a_new_pointer(iNodo *inode, QString new_text, FILE *file, SuperBloque sb);
    int append_text_in_pointer(QString txt, FILE *file, int dir);
    int append_text_in_block(QString txt, FILE *file, int dir);
    int append_text_to_last_block(iNodo *inode, QString new_text, FILE *file, SuperBloque sb);
    int make_new_direct_block(QString txt, iNodo *inode, SuperBloque sb, FILE * file);
    QString get_inode_info(iNodo inode, QStringList users);
    QString get_time(time_t rawtime);
    QString get_inode_type(int type);
    QString get_ls(Bloque_Carpeta folder_block,FILE *file, QStringList users);
};

#endif // TOOLS_H
