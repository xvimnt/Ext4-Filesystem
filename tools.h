#ifndef TOOLS_H
#define TOOLS_H
//el fs
#include <file_system.h>
#include <node.h>
//librerias de c
#include <iostream>
using namespace std;

class tools
{
public:
    tools();
    int get_free_pointer(int dir,FILE *file);
    void change_permissions_recursively(iNodo inode, int ugo,FILE* file, int inode_start);
    char * convertNumberIntoArray(unsigned int number);
    int get_group_id_by_name(QStringList list, QString name);
    void clean(char aux[], int n);
    QString get_full_file(iNodo inode,FILE *file);
    int get_free_block_index_by_bitmap(SuperBloque sb, FILE *file, bool overwrite);
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
    string get_report_format(string path);
    int get_primary_index(char fit[2], Mbr mbr, int new_size);
    void fix_first_slash(char st[500]);
    void fix_path(char * source, int size);
    Partition get_mounted_partition(char id[4]);
    int get_extended_partition_index(Mbr mbr);
    SuperBloque get_part_sb(FILE* file, int pt_start);
    FILE *get_file_by_id(char *id);
};

#endif // TOOLS_H
