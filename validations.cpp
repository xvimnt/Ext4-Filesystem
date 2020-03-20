#include "validations.h"

validations::validations()
{

}

//****************Validaciones****************//

bool validations::is_less_than_disk(int new_size, Mbr mbr, int current_partition_index)
{
    int remain_space = 0;
    for (int i = (current_partition_index + 1) ; i < 3; i++) {
        int current_size = mbr.partitions[i + 1].start - mbr.partitions[i].start;
        remain_space += current_size;
    }
    remain_space += mbr.size - mbr.partitions[3].start;
    if(new_size > remain_space) return false;
    return  true;
}

bool validations::validate_number_of_chars(char * st,int max_size, string msg)
{
    if(QString(st).size() > max_size)
    {
        cout << "Sistema: " << msg << " no puede ser mayor a " << max_size << " caracteres " << st << endl;
        return false;
    }
    return true;
}

bool validations::validate_fit(char fit[2])
{
    //si el ajuste esta vacio asignar el primer ajuste por defecto
    if(fit[0] == 0)
    {
        fit[0] = 'F';
        fit[1] = 'F';
    }
    else
    {
        if(fit[1] == 'F')
        {
            switch(fit[0])
            {
            case 'B':
                break;
            case 'W':
                break;
            case 'F':
                break;
            default:
                printf("Error: Ajuste especificado no reconocido\n");
                return false;
            }
        }
        else
        {
            printf("Error: Ajuste especificado no reconocido\n");
            return false;
        }
    }
    return true;
}

bool validations::validate_unit(char unit, int *size, int choice)
{
    //para choice = 0 validar mk disk sino para fdisk
    //el size tiene que ser mayor que 0
    if(*size == 0)
    {
        printf("Error: Especificar los bites del disco, tienen que ser distinto a 0\n");
        return false;
    }
    //el unit tiene que ser k o b si esta vacio entonces es m
    switch(unit)
    {
    case 0:
        if(choice == 0)
        {
            unit = 'M';
            *size *= 1024 * 1024;
        }
        else
        {
            unit = 'K';
            *size *= 1024;
        }
        break;
    case 'K':
        *size *= 1024;
        break;
    case 'M':
        *size *= 1024 * 1024;
        break;
    case 'B':
        if(choice == 0 )
        {
            printf("Error: Unit especificado no reconocido\n");
            return false;
        }
        break;
    default:
        printf("Error: Unit especificado no reconocido\n");
        return false;
    }
    return true;
}

bool validations::validate_path(char path[500], bool validate_disk)
{
    //el path tiene que existir y terminar con extension .disk
    if(path[0] == 0)
    {
        printf("Error: La ruta del disco no ha sido especificada\n");
        return false;
    }
    //quita el primer simbolo / y las comillas del path
    fix_path(path,500);
    fix_first_slash(path);
    if(validate_disk)
    {
        if(!QString(path).contains(".disk"))
        {
            return false;
        }
    }
    return true;
}

bool validations::validate_partition_type(char *type,Mbr mbr)
{
    int primary_partitions_count = 0, extended_partitions_count = 0;
    //contar el numero de particiones en el mbr
    for (int i = 0;i < 4;i++) {
        if(mbr.partitions[i].type == 'P') primary_partitions_count++;
        if(mbr.partitions[i].type == 'E') extended_partitions_count++;
    }
    //el tipo tiene que ser p, e o l si esta vacio entonces es p
    switch(*type)
    {
    case 0:
        *type = 'P';
        primary_partitions_count++;
        break;
    case 'P':
        primary_partitions_count++;
        break;
    case 'E':
        extended_partitions_count++;
        break;
    case 'L':
        break;
    default:
        printf("Error: Tipo de particion especificado no reconocido\n");
        return false;
    }
    //verificar si el numero no rebasa el especificado
    if(extended_partitions_count>1 ||
            (extended_partitions_count+primary_partitions_count)>4)
    {
        printf("Error: el numero de particiones es incorrecto\n");
        return  false;
    }
    return true;
}

bool validations::validate_format_type(char* type){
    if(strcmp(type,"FULL") == 0) return true;
    else if(strcmp(type,"FAST") == 0) return true;
    printf("Error: Tipo de formateo no aceptado\n");
    return false;
}

bool validations::validate_fs(char* fs)
{
    if(fs[0] == 0)
    {
        strcpy(fs,"2FS");
        return true;
    }
    if(strcmp(fs,"2FS") == 0) return true;
    if(strcmp(fs,"3FS") == 0) return true;

    return false;
}

//Metodo que retorna -2 si el nombre esta vacio
// -1 si el nombre no lo tiene otra particion
// y el index de 1 a 4 si se encuentra usado
int validations::validate_partition_name(char name[50], Mbr mbr)
{
    //int que devuelve si el nombre de la particion ha sido encontrada en el mbr
    int current_partition_index = -1;
    //con el choice es 0 para crear una y 1 para eliminar
    //el nombre tiene que existir
    if(name[0] == 0)
    {
        printf("Error: el nombre del disco no ha sido especificado\n");
        return -2;
    }
    for (int part_index = 0;part_index < 4;part_index++)
    {
        if(strcmp(mbr.partitions[part_index].name,name) == 0)
        {
            //Se encuentra el index de la particion
            //con el nombre que buscan
            current_partition_index = part_index;
        }
    }
    //retorna -1 si no encuentra nada
    return current_partition_index;
}

bool validations::validate_ugo(int ugo)
{
    char* analize = convertNumberIntoArray(static_cast<int>(ugo));
    if((analize[0] < 0 || analize[0] > 7) || (analize[1] < 0 || analize[1] > 7) || (analize[2] < 0 || analize[2] > 7))
    {
        printf("Error: el comando 'ugo' no es valido: %i\n",ugo);
        return false;
    }
    return true;
}
