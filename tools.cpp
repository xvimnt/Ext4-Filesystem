#include "tools.h"

tools::tools()
{

}


void tools::change_permissions_recursively(iNodo inode, int ugo,FILE* file, int inode_start)
{
    //modificar los permisos
    inode.i_perm = ugo;
    //ultima fecha de modificacion
    inode.i_mtime = time(nullptr);
    //guardar cambios
    fseek(file,inode_start,SEEK_SET);
    fwrite(&inode, sizeof(iNodo), 1, file);
    //para guardar el bloque
    Bloque_Carpeta folder_content;
    //para guardar el inodo del bloque
    iNodo sec_inode;
    //recorrido de los bloques directos
    for (int i = 0;i < 12;i++)
    {
        if(inode.i_block[i] != 0)
        {
            //obtener el bloque del objeto
            fseek(file,inode.i_block[i],SEEK_SET);
            fread(&folder_content, sizeof(Bloque_Archivo), 1, file);
            //para sus 4 bloques llamar el mismo metodo
            for (auto cont : folder_content.b_content)
            {
                //leyendo el inodo del bloque si no esta vacio
                if(cont->b_inodo != 0)
                {
                    //se obtiene su inodo
                    fseek(file,cont->b_inodo,SEEK_SET);
                    fread(&sec_inode, sizeof(iNodo), 1, file);
                    //llamar de nuevo el metodo
                    change_permissions_recursively(sec_inode,ugo,file,cont->b_inodo);
                }
            }
        }
    }
}

char * tools::convertNumberIntoArray(unsigned int number) {
    int length = (int)floor(log10((float)number)) + 1;
    char * arr = new char[length];
    int i = 0;
    do {
        arr[i] = number % 10;
        number /= 10;
        i++;
    } while (number != 0);
    return arr;
}

int tools::get_group_id_by_name(QStringList list, QString name)
{
    foreach(auto line, list)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si es un usuario
            if(var[1].simplified() == "G")
            {
                //si su usuario y pwd es correcta
                if(var[2].simplified() == name)
                {
                    return var[0].toInt();
                }
            }
        }
    }
    return -1;
}

void tools::clean(char aux[], int n)
{
    int i;
    for (i = 0; i < n; i++) {
        aux[i] = '\0';
    }
}

QString tools::get_full_file(iNodo inode,FILE *file)
{
    string response;
    Bloque_Archivo content_block;
    for(int i = 0; i < 12; i++)
    {
        if(inode.i_block[i] != 0)
        {
            fseek(file,inode.i_block[i],SEEK_SET);
            fread(&content_block,sizeof (Bloque_Archivo),1,file);
            response += content_block.b_content;
        }
    }
    return QString().fromStdString(response);
}

int tools::get_free_block_index_by_bitmap(SuperBloque sb, FILE *file, bool overwrite)
{
    //obtengo el primer bit
    char bit;
    fseek(file,sb.s_bm_block_start,SEEK_SET);
    fread(&bit, sizeof(char), 1, file);
    //contador para ver en que posicion del bitmap esta el 0
    int count = 0;
    //itero todos hasta encontrar el primer vacio
    while(bit != '\0')
    {
        fread(&bit, sizeof(char), 1, file);
        count++;
    }
    //si se quiere sobrescribir entonces escribir un 1 en el bitmap
    if(overwrite)
    {
        char unity = '1';
        fseek(file,sb.s_bm_block_start + count,SEEK_SET);
        fwrite(&unity, sizeof(char), 1, file);
    }
    //regresar la posicion del bloque vacio
    return sb.s_block_start + count * static_cast<int>(sizeof (Bloque_Archivo));
}

int tools::block_empty_space(char b_content[64])
{
    int count = 0;
    for (int i = 0; i < 64; i++) {
        if(b_content[i] == '\0') count++;
    }
    return count;
}

bool has_free_blocks(int dir,FILE *file)
{
    //tomara como libre el apuntador si encuentra uno vacio
    Bloque_apuntador temp;
    //leer el apuntador
    fseek(file,dir,SEEK_SET);
    fread(&temp,sizeof (Bloque_apuntador),1,file);
    //recorrer sus inodos
    for (int j = 0;j < 16; j++)
    {
        if(temp.b_pointers[j] == 0)
            return true;
    }
    return false;
}

int tools::get_free_pointer(int dir,FILE *file)
{
    Bloque_apuntador temp;
    //leer el apuntador
    fseek(file,dir,SEEK_SET);
    fread(&temp,sizeof (Bloque_apuntador),1,file);
    //recorrer sus apuntadores
    Bloque_apuntador temp_2;
    //para sus punteros
    for(auto dir : temp.b_pointers)
    {
        //si alguno de sus punteros esta vacio entonces devolverlo
        if(dir == 0) return dir;
        //sino buscar si el puntero tiene sus espacios ocupados
        else if(has_free_blocks(dir,file)) return dir;
    }
    return -1;
}

int tools::get_last_block_index(int i_block[15],FILE *file)
{
    int block_index = 0;
    for (int i = 0;i < 15;i++)
    {
        if(i < 12)
        {
            if(i_block[i] == 0)
            {
                block_index = i - 1;
                break;
            }
        }
        else
        {
            switch(i)
            {
                case 12:
                if(i_block[12] == 0)
                {
                    block_index = i - 1;
                    break;
                }
                else
                {
                    //si encuentra uno vacio entonces devolver el index pasado sino continuar
                    if(has_free_blocks(i_block[12],file)) return i - 1;
                }
                break;
            case 13:
                //si paso a este caso quiere decir que el 12 esta lleno
                //si tiene bloques libres devolver el pasado sino continuar
                if(get_free_pointer(i_block[13],file) != -1) return i - 1;
                break;
            case 14:
                Bloque_apuntador temp;
                //leer el apuntador
                fseek(file,i_block[14],SEEK_SET);
                fread(&temp,sizeof (Bloque_apuntador),1,file);
                for(auto dir : temp.b_pointers)
                {
                    //si alguna de sus casillas esta vacia devolcer el anterior
                    if(dir == 0) return i - 1;
                    //si alguno de sus hijos tiene punteros libres devolver el anterior
                    else if(get_free_pointer(dir,file) != -1) return i -1;
                }
                break;
            }
        }
    }
    return block_index;
}

void tools::read_partitions(char path[500])
{
    //if the disk exists then return a new ID to mount it
    FILE* file= fopen(path, "rb+");
    if (file == nullptr)
    {
        printf("\n********Error al abrir: \"%s\" (No existe la ruta)***********\n",path);
    }
    else
    {
        Mbr mbr; //creamos estructura mbr
        fseek(file,0,SEEK_SET);//puntero al inicio del files
        fread(&mbr, sizeof(Mbr), 1, file);//realiza la lectura de la estructura mbr escrita al crear el disco

        //------------------------Recorrido De partitions----------------------
        int z=0;
        for(z=0;z<4;z++){ //recorre el arreglo de partitions primarias
            printf("\n*******************************************************\n");
            printf("Nombre: %s \n",mbr.partitions[z].name);
            printf("Bit Inicial: %i \n",mbr.partitions[z].start);
            printf("Estado: %i \n",mbr.partitions[z].status);
            printf("Tipo: %c \n",mbr.partitions[z].type);
            printf("Tipo De Ajuste: %s \n",mbr.partitions[z].fit);
            printf("Tamaño: %i \n", mbr.partitions[z].size);
            printf("\n*******************************************************\n");
        }//fin de Recorrido de partitions
    }
    fclose(file);
}

int tools::get_block_count(int size)
{
    int temp = size;
    int result = 0;

    while(temp > 64)
    {
        temp -= 64;
        result++;
    }
    result++;
    return result;
}

string tools::get_current_time()
{
    time_t tiempo = time(nullptr);
    struct tm *tlocal = localtime(&tiempo);
    //para guardar la fecha de creacion
    char output[20];
    clean(output,20);
    strftime(output,20,"%d/%m/%y %H:%M:%S",tlocal);
    return output;
}

void tools::make_directories(char path[500])
{
    int pointer=0;
    char folder[500];
    clean(folder,500);
    char terminal[500];
    clean(terminal,500);
    int folder_pointer = 0;
    while(path[pointer]!='.')
    {
        if(path[pointer]!='/')
        {
            folder[folder_pointer] = path[pointer];
        }
        else
        {
            strcat(terminal,"mkdir ");
            strcat(terminal,"\"");
            strcat(terminal,folder);
            strcat(terminal,"\"");
            strcat(terminal,"\n");
            strcat(terminal,"cd ");
            strcat(terminal,"\"");
            strcat(terminal,folder);
            strcat(terminal,"\"");
            strcat(terminal,"\n");
            strcat(folder,"");
            clean(folder,500);
            folder_pointer = -1;
        }
        pointer++;
        folder_pointer++;
    }
    system(terminal);
}

Mbr tools::get_mbr(FILE* file)
{
    //Mbr donde se guarda el resultado
    Mbr mbr;
    //poner un valor vacio por si da error
    mbr.size = -1;
    //si hubo error al leer el disco devuelve un size -1
    if (file == nullptr) return mbr;
    else
    {
        fseek(file,0,SEEK_SET);//puntero al inicio del files
        fread(&mbr, sizeof(Mbr), 1, file);//realiza la lectura de la estructura mbr escrita al crear el disco
    }
    return mbr;
}

string tools::get_report_format(string path)
{
    string flag = path.substr(path.find('.'), path.length());
    QString comp = QString(flag.c_str());
    comp = comp.toUpper();
    if(comp.toUpper() == ".PDF"){
        return "PDF";
    }
    else if(comp.toUpper() == ".PNG"){
        return "PNG";
    }
    else if(comp.toUpper() == ".JPG"){
        return "JPG";
    }else {
        return  "0";
    }
}

bool low_to_high(const QPair<int,int>& e1, const QPair<int,int>& e2) { // sorting by QHash Value
    if (e1.second < e2.second) return true;
    return false;
}

bool high_to_low(const QPair<int,int>& e1, const QPair<int,int>& e2) { // sorting by QHash Value
    if (e1.second > e2.second) return true;
    return false;
}

QList<QPair<int,int>> get_list_of_spaces(Mbr mbr, int new_size)
{
    //este metodo me devuelve una lista con los slots vacios con la diferencia de la nueva particion
    //guarda los indices de las particiones y su inicio
    QList<QPair<int,int>> stack;
    int size, diference;
    for (int i = 0; i < 3; i++) {
        //si tenemos un espacio vacio
        if(mbr.partitions[i].size == 0)
        {
            //obtener el size correcto
            size = mbr.partitions[i + 1].start - mbr.partitions[i].start;
            //Si cabe en el espacio
            if(size > new_size)
            {
                //para ver la diferencia de size
                diference = size - new_size;

                stack.append(qMakePair(i,diference));
            }
        }
    }
    //si tenemos un espacio vacio
    if(mbr.partitions[3].size == 0)
    {
        //obtener el size correcto
        size = mbr.size - mbr.partitions[3].start;
        //Si cabe en el espacio
        if(size > new_size)
        {
            //para ver la diferencia de size
            diference = size - new_size;
            stack.append(qMakePair(3,diference));
        }
    }
    return  stack;
}

int tools::get_primary_index(char fit[2], Mbr mbr, int new_size)
{
    //Quitarle el -1 a los 4 espacios
    for(int i=0;i<4;i++)
    {
        if(mbr.partitions[i].start==-1)
        {//si esta particion esta llena
            //el nuevo espacio donde se guardara
            return i;
        }
    }

    //Ya no tienen -1 entonces hacer el ajuste
    auto stack = get_list_of_spaces(mbr,new_size);

    if(strcmp(fit,"BF"))
    {
        qSort(stack.begin(),stack.end(),high_to_low);
        return stack.first().first;
    }
    else if (strcmp(fit,"WF"))
    {
        qSort(stack.begin(),stack.end(),low_to_high);
        return stack.first().first;
    }
    else
    {
        //First Fit
        return stack.first().first;
    }
}

void tools::fix_first_slash(char st[500])
{
    if(st[0] == '"')
    {
        if(st[1] == '/')
        {
            st[0] = ' ';
            st[1] = '"';
            memmove(st,st+1,500);
        }
    }
    else
    {
        if(st[0] == '/')
        {
            memmove(st,st+1,500);
        }
    }
}

void tools::fix_path(char * source, int size)
{
    //quita comillas del path
    char temp_path[500];
    strcpy(temp_path, source);
    if(temp_path[0]=='\"')
    {
        clean(source,size);
        int q=1;
        while(temp_path[q]!='\"')
        {
            char c2[1];
            c2[0]=temp_path[q];
            strncat(source,c2,1);
            q++;
        }
    }
}

int tools::get_extended_partition_index(Mbr mbr)
{
    //obtener una particion extendida, si no hay una marcar error
    int extended_partition_index = -1;
    for (int i = 0; i < 4;i++)
    {
        if(mbr.partitions[i].type=='E') extended_partition_index = i;
    }
    return extended_partition_index;
}

SuperBloque tools::get_part_sb(FILE* file, int pt_start)
{
    SuperBloque blck_rtrn;
    //escribiendo el superbloque de la particion
    fseek(file,pt_start,SEEK_SET);
    fread(&blck_rtrn, sizeof(SuperBloque), 1, file);
    return blck_rtrn;
}

FILE *tools::get_file_by_id(char *id)
{
    //encontrar el path del disco de la particion montada
    std::string disk_path = path_by_id(QChar(id[2]).toUpper().toLatin1());
    //abrir el disco para escribir en el
    return fopen(disk_path.c_str(), "rb+");
}
