#ifndef VALIDATIONS_H
#define VALIDATIONS_H
#include <tools.h>

class validations : public tools
{
public:
    validations();
    bool is_less_than_disk(int new_size, Mbr mbr, int current_partition_index);
    bool validate_number_of_chars(char * st,int max_size, string msg);
    bool validate_fit(char fit[2]);
    bool validate_unit(char unit, int *size, int choice);
    bool validate_path(char path[500], bool validate_disk);
    bool validate_partition_type(char *type,Mbr mbr);
    bool validate_mount(char name[50]);
    bool validate_format_type(char* type);
    bool validate_fs(char* fs);
    bool validate_ugo(int ugo);
    int validate_partition_name(char name[50], Mbr mbr);
    //metodo especial
    QString generate_ID(Metadata data);
};

#endif // VALIDATIONS_H
