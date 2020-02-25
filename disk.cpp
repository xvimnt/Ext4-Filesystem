#include "disk.h"
//exec &path->minimos.sh
disk::disk()
{

}

/**************************************************************************
*********************************Herramientas******************************
***************************************************************************/
int getAVDFree(SuperBloque sb,QString pathM){
    int inicio=0;
    char pr=2;
    bool encontrado=true;
    int contador=0;
    FILE*Archivo=fopen(pathM.toStdString().c_str(),"rb+");

    while(encontrado) {
        inicio=sb.sb_ap_bitmap_arbol_directorio+(contador*sizeof (char));
        fseek(Archivo,inicio,SEEK_SET);
        fread(&pr,sizeof (char),1,Archivo);
        if(pr==0){
            pr=1;
            encontrado=false;
        }
        contador++;
    }
    fclose(Archivo);
    return contador;
}

int getDirectorioFree(SuperBloque sb,QString pathM){
    int inicio=0;
    char pr=2;
    bool encontrado=true;
    int contador=0;
    FILE*Archivo=fopen(pathM.toStdString().c_str(),"rb+");

    while(encontrado) {
        inicio=sb.sb_ap_bitmap_arbol_directorio+(contador*sizeof (char));
        fseek(Archivo,inicio,SEEK_SET);
        fread(&pr,sizeof (char),1,Archivo);
        if(pr==0){
            pr=1;
            encontrado=false;
        }
        contador++;
    }
    fclose(Archivo);
    return contador;
}

int getInodoFree(SuperBloque sb,QString pathM){
    int inicio=0;
    char pr=2;
    bool encontrado=true;
    int contador=0;
    FILE*Archivo=fopen(pathM.toStdString().c_str(),"rb+");

    while(encontrado) {
        inicio=sb.sb_ap_bitmap_arbol_directorio+(contador*sizeof (char));
        fseek(Archivo,inicio,SEEK_SET);
        fread(&pr,sizeof (char),1,Archivo);
        if(pr==0){
            pr=1;
            encontrado=false;
        }
        contador++;
    }
    fclose(Archivo);
    return contador;
}

int getBloqueFree(SuperBloque sb,QString pathM){
    int inicio=0;
    char pr=2;
    bool encontrado=true;
    int contador=0;
    FILE*Archivo=fopen(pathM.toStdString().c_str(),"rb+");

    while(encontrado) {
        inicio=sb.sb_ap_bitmap_arbol_directorio+(contador*sizeof (char));
        fseek(Archivo,inicio,SEEK_SET);
        fread(&pr,sizeof (char),1,Archivo);
        if(pr==0){
            pr=1;
            encontrado=false;
        }
        contador++;
    }
    fclose(Archivo);
    return contador;
}

void Abecedario(char *valor,int size)
{
    char abc='a';
    for (int i=0;i<size;i++) {
        valor[i]=abc;
        abc++;
        if(abc=='z'){
            abc='a';
        }
    }
}

int getCantidadInodos(int size){
    int aux=size;
    int contador=0;

    while (aux>4) {
        aux-=4;
        contador++;
    }
    if(aux==0 && contador==0){
        contador=0;
    }
    else if(aux<=4){
        contador+=1;
    }
    return contador;
}

int get_remains_blocks(int size)
{
    int aux = size;
    if(aux < 25)
    {
        aux = 0;
    }
    else {
        aux = aux % 25;
    }
    return  aux;
}

void clean(char aux[], int n) {
    int i;
    for (i = 0; i < n; i++) {
        aux[i] = '\0';
    }
}

void fix_first_slash(char st[500])
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

void fix_path(char source[500] , int size)
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

char return_free_char(char pointer)
{
    bool ocupada = false;
    foreach(char value, mounted_disks)
    {
        if(value == pointer) ocupada = true;
    }
    if(ocupada){
        char new_char = pointer;
        new_char++;
        return return_free_char(new_char);
    }
    else return pointer;
}

bool is_mounted_partition(char id[4])
{
    return mounted_partitions.contains(QString(id).toUpper());
}

std::string path_by_id(char unit)
{
    QString disk_path;
    foreach (char v, mounted_disks)
    {
        if (v == unit)
            disk_path = mounted_disks.key(unit);
    }
    return disk_path.toStdString();
}

bool validate_mount(char name[50]);

//Para generar los ID al montar los discos
//VD % LETRA % NUMERO.
QString generate_ID(Metadata data)
{
    QString result = "VD";
    //if the disk isn't mounted yet, use a letter to mount it
    if(mounted_disks[data.path] == '\0')
    {
        QString new_disk_letter = QString(return_free_char('A'));
        result += new_disk_letter;
        mounted_disks[data.path] = new_disk_letter[0].toLatin1();
    }
    else
    {
        //el disco ya tiene asignada una letra
        result += mounted_disks[data.path];
        if(!validate_mount(data.name)){
            printf("El disco ya tiene montada esta particion");
            return "-1";
        }
    }

    return result;
}

void read_mounted_partitions()
{
    for (Partition partition: mounted_partitions) {
        printf("\n****************Particion montada**********************\n");
        printf("Nombre: %s \n",partition.name);
        printf("Bit Inicial: %i \n",partition.start);
        printf("Estado: %i \n",partition.status);
        printf("Tipo: %c \n",partition.type);
        printf("Tipo De Ajuste: %s \n",partition.fit);
        printf("Tamaño: %i \n", partition.size);
        printf("\n*******************************************************\n");
    }
}

void read_partitions(char path[500])
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

int get_block_count(int size)
{
    int temp = size;
    int result = 0;

    while(temp > 25)
    {
        temp -= 25;
        result++;
    }
    result++;
    return result;
}

std::string get_current_time()
{
    time_t tiempo = time(nullptr);
    struct tm *tlocal = localtime(&tiempo);
    //para guardar la fecha de creacion
    char output[20];
    clean(output,20);
    strftime(output,20,"%d/%m/%y %H:%M:%S",tlocal);
    return output;
}

SuperBloque make_new_superblock(int number_of_structures, int start_partition)
{
    //constructor para un struct de superbloque
    SuperBloque super;
    clean(super.sb_nombre_hd,25);
    super.sb_arbol_virtual_count=1;
    super.sb_detalle_directorio_count=1;
    super.sb_inodos_count=1;
    super.sb_bloques_count=2;
    super.sb_arbol_virtual_free=(number_of_structures)-1;
    super.sb_detalle_directorio_free=(number_of_structures)-1;
    super.sb_inodos_free=(5*number_of_structures)-1;
    super.sb_bloques_free=(4*5*number_of_structures)-2;
    clean(super.sb_date_creacion,25);
    clean(super.sb_date_ultimo_montaje,25);
    //funcion que devuelve el current time
    strcpy(super.sb_date_creacion,get_current_time().c_str());
    strcpy(super.sb_date_ultimo_montaje,get_current_time().c_str());
    super.sb_montajes_count=0;
    super.sb_size_struct_arbol_directorio=sizeof(AVD);
    super.sb_size_struct_detalle_directorio=sizeof(DD);
    super.sb_size_struct_inodo=sizeof(iNodo);
    super.sb_size_struct_bloque=sizeof(Bloque);
    super.sb_first_free_bit_arbol_directorio=1;
    super.sb_first_free_bit_detalle_directorio=1;
    super.sb_first_free_bit_tabla_inodo=1;
    super.sb_first_free_bit_bloques=2;
    super.sb_magic_num=201700831;
    //Apuntadores 9 chars
    super.sb_ap_bitmap_arbol_directorio=start_partition+static_cast<int>(sizeof (SuperBloque))+1;
    super.sb_ap_arbol_directorio=super.sb_ap_bitmap_arbol_directorio+number_of_structures+1;
    super.sb_ap_bitmap_detalle_directorio=super.sb_ap_arbol_directorio+(number_of_structures*static_cast<int>(sizeof (AVD)))+1;
    super.sb_ap_detalle_directorio=super.sb_ap_bitmap_detalle_directorio+number_of_structures+1;
    super.sb_ap_bitmap_tabla_inodo=super.sb_ap_detalle_directorio+(number_of_structures*static_cast<int>(sizeof (DD)))+1;
    super.sb_ap_tabla_inodo=super.sb_ap_bitmap_tabla_inodo+(5*number_of_structures)+1;
    super.sb_ap_bitmap_bloques=super.sb_ap_tabla_inodo+(5*number_of_structures*static_cast<int>(sizeof (iNodo)))+1;
    super.sb_ap_bloques=super.sb_ap_bitmap_bloques+(20*number_of_structures)+1;
    super.sb_ap_log=super.sb_ap_bloques+(20*number_of_structures*static_cast<int>(sizeof (Bloque)))+1;
    return super;
}

SuperBloque superblock_vacio()
{
    //constructor para un struct de superbloque
    SuperBloque super;
    clean(super.sb_nombre_hd,25);
    super.sb_arbol_virtual_count=0;
    super.sb_detalle_directorio_count=0;
    super.sb_inodos_count=0;
    super.sb_bloques_count=0;
    super.sb_arbol_virtual_free=0;
    super.sb_detalle_directorio_free=0;
    super.sb_inodos_free=0;
    super.sb_bloques_free=0;
    clean(super.sb_date_creacion,25);
    clean(super.sb_date_ultimo_montaje,25);
    super.sb_montajes_count=0;
    super.sb_size_struct_arbol_directorio=0;
    super.sb_size_struct_detalle_directorio=0;
    super.sb_size_struct_inodo=0;
    super.sb_size_struct_bloque=0;
    super.sb_first_free_bit_arbol_directorio=0;
    super.sb_first_free_bit_detalle_directorio=0;
    super.sb_first_free_bit_tabla_inodo=0;
    super.sb_first_free_bit_bloques=0;
    super.sb_magic_num=0;
    //Apuntadores 9 chars
    super.sb_ap_bitmap_arbol_directorio=0;
    super.sb_ap_arbol_directorio=0;
    super.sb_ap_bitmap_detalle_directorio=0;
    super.sb_ap_detalle_directorio=0;
    super.sb_ap_bitmap_tabla_inodo=0;
    super.sb_ap_tabla_inodo=0;
    super.sb_ap_bitmap_bloques=0;
    super.sb_ap_bloques=0;
    super.sb_ap_log=0;
    return super;
}

void make_directories(char path[500])
{
    int pointer=0;
    char folder[500];
    clean(folder,100);
    char terminal[500];
    clean(terminal,100);
    while(path[pointer]!='.')
    {
        if(path[pointer]!='/')
        {
            char c1[1];
            c1[0]=path[pointer];
            strncat(folder,c1,1);
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
        }
        pointer++;
    }
    system(terminal);
}

Mbr get_mbr(FILE* file)
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

string get_report_format(string path)
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

void print_list(QList<QPair<int,int>> stack)
{
    for(auto pointer : stack){
        printf("%i",pointer.first);
        printf("%i",pointer.second);
        cout << "index " << pointer.first << " dif " << pointer.second;
    }
}

int get_primary_index(char fit[2], Mbr mbr, int new_size)
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
/**********************************************************
 ********************** Metodos para validar parametros ***********
 ******************************************************************/

bool validate_fit(char fit[2])
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

bool validate_unit(char unit, int *size, int choice)
{
    //para choice = 0 validar mk disk sino para fdisk
    //el size tiene que ser mayor que 0
    if(*size <= 0)
    {
        printf("Error: Especificar los bites del disco, tienen que ser mayor a 0\n");
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

bool validate_path(char path[500])
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
    if(!QString(path).contains(".disk"))
    {
        return false;
    }
    return true;
}

bool validate_partition_type(char *type,Mbr mbr)
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

bool validate_mount(char name[50])
{
    for(Partition part_point : mounted_partitions)
    {
        if(strcmp(part_point.name,name) == 0) return false;
    }
    return true;
}

//Metodo que retorna -2 si el nombre esta vacio
// -1 si el nombre no lo tiene otra particion
// y el index de 1 a 4 si se encuentra usado
int validate_partition_name(char name[50], Mbr mbr)
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

int get_extended_partition_index(Mbr mbr)
{
    //obtener una particion extendida, si no hay una marcar error
    int extended_partition_index = -1;
    for (int i = 0; i < 4;i++)
    {
        if(mbr.partitions[i].type=='E') extended_partition_index = i;
    }
    return extended_partition_index;
}
/*************************************************************************************
*************************** Area de metodos de un disco ******************************
**************************************************************************************/
void disk::makeDisk(Metadata data)
{
    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    if(!validate_fit(data.fit)) return;
    if(!validate_unit(data.unit,&data.size,0)) return;
    if(!validate_path(data.path)) return;


    /*******************************************
     * PARAMETROS CORRECTOS: CREACION DEL DISCO
     *******************************************/
    //******limpiando sesion****//
    clean(cookie.name,10);
    clean(cookie.password,10);
    //creacion de mbr//
    Mbr mbr;

    //fecha de creacion
    clean(mbr.date,25);
    strcpy(mbr.date,get_current_time().c_str());

    //******** agregando los parametros del mbr *********/
    //mbr.signature;
    mbr.signature = std::time(nullptr) + random();
    //mbr ajuste
    strcpy(mbr.fit,data.fit);
    //mbr size
    mbr.size=data.size;

    //------------------------------------------------------------------
    //Para crear folders en los Directorios si estos no an sido Creados
    //------------------------------.------------------------------------
    //crea directorios
    make_directories(data.path);

    //------------------------------------------------------------------
    //-------Aqui  La lectura y Escritura en el file-----------------
    //------------------------------------------------------------------
    //Creacion De file que simula el Disco y llenando con \0
    FILE* file= fopen(data.path, "ab");
    if (file==nullptr)
    {
        printf("\nInterprete (X)#: Error[16]: Al tratar de Acceder al file\n");
    }
    else
    {
        //Escribiendo 0s en el archivo del disco
        int fin=(data.size/1024);
        char buffer[1024];
        int i=0;
        for(i=0;i<1024;i++){
            buffer[i]='\0';
        }
        int j=0;
        while(j!=fin){
            fwrite(&buffer,1024 , 1, file);
            j++;
        }
        fclose(file);
    }
    //Fin Creacion de file


    for(int i=0;i<4;i++)//recorre el arreglo de las 4 partitions
    {//estableciendo datos por defecto
        mbr.partitions[i].start=-1;
        mbr.partitions[i].status=0;
        mbr.partitions[i].size=0;
        mbr.partitions[i].fit[0] = 'F';
        mbr.partitions[i].fit[1] = 'F';
        mbr.partitions[i].type = 0;
    }

    //abriendo el file que simula el disco virtual
    file = fopen(data.path, "rb+");
    fseek(file,0,SEEK_SET);// estableciendo puntero al inicio

    if (file==nullptr)//si el file no contiene nada
    {
        printf("\nInterprete (X)#: Error[17]: Al tratar de Acceder al file file \n\n");
    }
    else
    {
        fwrite(&mbr, sizeof(Mbr), 1, file);//escribiendo la estructura del MBR
        fclose(file);
        printf("Sistema: Disco creado con exito: '%s'\n",data.path);
    }
}

void disk::removeDisk(Metadata data)
{
    validate_path(data.path);
    printf("Eliminar este disco? %s\n",data.path);
    printf("1.Si, Eliminar\n");
    printf("2.No, Conservar\n");

    std::string ins ;
    getline(std::cin,ins);
    if(std::stoi(ins) == 1)
    {

        if( remove( data.path ) != 0 )
            printf("Error: No se pudo eliminar el disco '%s' no existente\n",data.path);
        else
            printf("Sistema: '%s' eliminado correctamente\n",data.path);


    }
    else if(std::stoi(ins) == 2)
    {

    }
    else
    {
        printf("Sistema: Opcion invalida intente de nuevo\n");
        removeDisk(data);
    }
}

void disk::fDisk(Metadata data)
{
    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    //el size tiene que ser mayor que 0
    if(!validate_path(data.path)) return;

    //Abrir el file
    FILE* file = fopen(data.path, "rb+");
    //Para verificar el nombre tenemos que obtener el mbr
    Mbr mbr = get_mbr(file);
    //verificar si el mbr es correcto
    if(mbr.size == -1)
    {
        printf("Error al leer el disco: %s",data.path);
        return;
    }
    //Guardando el index de la particion con el nombre si no existe muestra -1
    int current_partition_index = validate_partition_name(data.name,mbr);

    //Este metodo verificara si se quiere crear, eliminar o expandir una particion
    if(data.Delete[0] != 0)
    {
        //Se eliminara
        //mensaje de eliminacion
        printf("\n********************ELIMINAR PARTICION*****************"
               "\nquieres eliminar: '%s'",data.name);
        printf("del disco: '%s' \n",data.path);
        printf("1.Si\n2.No\n");
        int choice;
        cin >> choice;
        if(choice!=1) {
            printf("Sistema: No se elimino la particion '%s'",data.name);
            return;
        }

        //ver el tipo de borrado
        if(strcmp(QString(data.Delete).toUpper().toStdString().c_str(),"FULL") == 0
                && current_partition_index != -1)
        {
            //puntero al inicio de la particion
            fseek(file,mbr.partitions[current_partition_index].start,SEEK_SET);
            //guardara el numero de KB de la particion
            int fin=(mbr.partitions[current_partition_index].size/1024);
            //Simula 1 KB
            char buffer[1024];
            //Lo llena de 0
            for(int i=0;i<1024;i++){
                buffer[i]='\0';

                int j=0;
                //Escribe todos los KB de la particion con el KB de 0
                while(j!=fin){
                    fwrite(&buffer,1024 , 1, file);
                    j++;
                }
                //Limpia los atributos de la particion
                clean(mbr.partitions[current_partition_index].name,50);
                clean(mbr.partitions[current_partition_index].fit,2);
                mbr.partitions[current_partition_index].size = 0;
                mbr.partitions[current_partition_index].type = 0;
                mbr.partitions[current_partition_index].status = 0;
                //escribe el mbr
                fseek(file,0,SEEK_SET);//puntero al inicio del files
                fwrite(&mbr, sizeof(Mbr), 1, file);//realiza la lectura de la estructura mbr escrita al crear el disco
                fclose(file);
                printf("Sistema: Borrado completo de particion '%s' realizado con exito",data.name);
            }
        }
        else if(strcmp(QString(data.Delete).toUpper().toStdString().c_str(),"FAST") == 0
                && current_partition_index != -1)
        {
            //Solo se limpian los 4 atributos de una particion
            clean(mbr.partitions[current_partition_index].name,50);
            clean(mbr.partitions[current_partition_index].fit,2);
            mbr.partitions[current_partition_index].size = 0;
            mbr.partitions[current_partition_index].type = 0;
            mbr.partitions[current_partition_index].status = 0;
            //escribe el mbr
            fseek(file,0,SEEK_SET);//puntero al inicio del files
            fwrite(&mbr, sizeof(Mbr), 1, file);//realiza la lectura de la estructura mbr escrita al crear el disco
            fclose(file);
            printf("Sistema: Borrado rapido de particion '%s' realizado con exito",data.name);
        }
        else
        {

            //buscar entre las logicas
            int extended_partition_index = get_extended_partition_index(mbr);
            if(extended_partition_index == -1)
            {
                printf("Error: nombre de particion no encontrado'%s'",data.name);
                return;
            }
            Ebr ebr;
            fseek(file,mbr.partitions[extended_partition_index].start,SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
            //buscando el nombre entre las particiones logicas
            while(strcmp(ebr.name,data.name) != 0 && ebr.next != -1)
            {
                fseek(file,ebr.next,SEEK_SET);
                fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
            }
            if(ebr.next == -1)
            {
                printf("Error: nombre de particion no encontrado: '%s'\n",data.name);
                return;
            }
            //para guardar la particion logica y agregarla a ram
            Partition logic_partition;
            //Guardando nuestra particion en ram
            fseek(file,ebr.start,SEEK_SET);
            fread(&logic_partition,sizeof (Partition),1,file);
            //Viendo el tipo de borrado
            if(strcmp(QString(data.Delete).toUpper().toStdString().c_str(),"FULL") == 0)
            {
                //guardara el numero de KB de la particion
                int fin=(logic_partition.size/1024);
                //Simula 1 KB
                char buffer[1024];
                //Lo llena de 0
                for(int i=0;i<1024;i++){
                    buffer[i]='\0';
                }
                int j=0;
                //puntero al inicio de la particion logica
                fseek(file,ebr.start,SEEK_SET);
                //Escribe todos los KB de la particion con el KB de 0
                while(j!=fin){
                    fwrite(&buffer,1024 , 1, file);
                    j++;
                }

                //limpiando atributos de la particion
                clean(logic_partition.name,50);
                logic_partition.size = 0;
                logic_partition.type = 0;
                logic_partition.start = -1;
                logic_partition.status = 0;
                clean(logic_partition.fit,2);

                //Guardando nuestra particion en memoria secundiaria
                fseek(file,ebr.start,SEEK_SET);
                fwrite(&logic_partition,sizeof (Partition),1,file);
            }
            else if(strcmp(QString(data.Delete).toUpper().toStdString().c_str(),"FAST") == 0)
            {
            }
            else
            {
                printf("Error: no se elimino particion '%s' tipo de formateo invalido",data.name);
                return;
            }
            //limpiando atributos del ebr
            clean(ebr.fit,2);
            clean(ebr.name,50);
            //posicion donde empieza el ebr y no la particion ya que se va a borrar el inicio
            int ebr_position = ebr.start - static_cast<int>(sizeof (Ebr));
            ebr.start = -1;
            ebr.status = 0;
            //puntero al inicio del ebr
            fseek(file,ebr_position,SEEK_SET);
            fwrite(&ebr, sizeof(Ebr), 1, file);
            fclose(file);
            //output de usuario
            printf("Sistema: se elimino con exito %s",data.name);
        }
    }
    else if(data.add != 0)
    {
        //modificara la particion
    }
    else if (current_partition_index == -1)
    {
        //creara la particion
        createPartition(data,file,mbr);
    }
}

void disk::createPartition(Metadata data,FILE* file,Mbr mbr)
{

    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    if(!validate_unit(data.unit,&data.size,1)) return;
    if(!validate_fit(data.fit)) return;
    //valida si la letra es indicada o no rebasa el numero especificado
    if(!validate_partition_type(&data.type,mbr)) return;
    //Valida el size de la particion con el del disco
    if(data.size>mbr.size)
    {
        printf("Error: El Tamaño de la Particion es mayor que el disco\n");
        return;
    }


    /******************************
     * CREACION DE PARTICION
     *****************************/
    //esta sera -1 cuando sea logica
    int current_partition_index = -1;
    // Busca un espacio en las particiones primarias
    if(data.type != 'L') current_partition_index = get_primary_index(data.fit,mbr,data.size);
    //si es la posicion 0 no cambia
    int new_partition_start = sizeof (Mbr);
    //si es otra posicion es el start de la anterior  + su size
    if(current_partition_index > 0 )
        new_partition_start = mbr.partitions[current_partition_index - 1].start + mbr.partitions[current_partition_index - 1].size;
    //Seteando el nuevo comienzo o el mismo si ya estaba, si no es logica
    if(current_partition_index != -1) mbr.partitions[current_partition_index].start = new_partition_start;
    //si es extendida o logica necesita de un EBR
    if(data.type == 'E' || data.type == 'L')
    {
        //creando un ebr nuevo
        Ebr ebr;
        strcpy(ebr.fit,data.fit);
        strcpy(ebr.name,data.name);
        ebr.status = 0;
        ebr.size = data.size;
        //se creara una particion logica
        if (data.type=='L')
        {
            //Encontrar la particion extendida donde se guardara la nueva logica
            int extended_partition_index = get_extended_partition_index(mbr);
            if(extended_partition_index == -1)
            {
                printf("Error: No existe una particion extendida en el disco para crear una logica");
                return;
            }

            //crear logica en el arreglo de la extendida
            Partition new_partition;
            strcpy(new_partition.fit , data.fit);
            clean(new_partition.name,50);
            strcpy(new_partition.name,data.name);
            new_partition.size = data.size;
            new_partition.type = data.type;
            new_partition.status = 0;
            //leer el ebr de la extendida
            Ebr ebr_pointer;
            fseek(file,mbr.partitions[extended_partition_index].start,SEEK_SET);
            fread(&ebr_pointer, sizeof(Ebr), 1, file);//lectura del ebr
            //si es el primer ebr
            if(ebr_pointer.next == -1)
            {
                //el ebr start me muestra el inicio de la particion
                ebr.start = ebr_pointer.start;
                //el inicio de la particion con el ebr start
                new_partition.start = ebr.start;
                //crear una nueva particion
                ebr.next = data.size + ebr.start;
                fseek(file,mbr.partitions[extended_partition_index].start,SEEK_SET);
                //escribe ebr de la logica
                fwrite(&ebr, sizeof(Ebr), 1, file);
                //escribe la particion
                fseek(file,ebr.start ,SEEK_SET);
                fwrite(&new_partition,sizeof (Partition),1,file);
                fclose(file);
            }
            else
            {
                //ya existen particiones logicas asi que buscar la ultima
                //cambia el ebr.start
                while(ebr_pointer.next != -1)
                {
                    //Encontrar el siguiente ebr
                    fseek(file,ebr_pointer.next,SEEK_SET);
                    fread(&ebr_pointer, sizeof(Ebr), 1, file);//lectura del ebr
                }
                //setear el nuevo next ya que no existe
                ebr.next = ebr_pointer.start + data.size;
                ebr.start = ebr_pointer.start;
                //inicio de la particion con el ebr start
                new_partition.start = ebr.start;
                //escribir el nuevo ebr
                fseek(file,ebr_pointer.start - static_cast<int>(sizeof (Ebr)),SEEK_SET);
                fwrite(&ebr, sizeof(Ebr), 1, file);//escribe ebr
                //escribe la particion
                fseek(file,ebr.start,SEEK_SET);
                fwrite(&new_partition,sizeof (Partition),1,file);
                fclose(file);
            }
            //escribir un ebr vacio
            file= fopen(data.path, "rb+");
            //crear un ebr vacio
            Ebr new_ebr;
            strcpy(new_ebr.name,"vacio");
            new_ebr.next = -1;
            new_ebr.start = ebr.next + static_cast<int>(sizeof (Ebr));
            //escribe ebr vacio
            fseek(file,ebr.next,SEEK_SET);
            fwrite(&new_ebr, sizeof(Ebr), 1, file);
            fclose(file);
        }
        else
        {
            //Se creara una particion extendida
            //datos del mbr
            mbr.partitions[current_partition_index].size=data.size;
            clean(mbr.partitions[current_partition_index].fit,2);
            strcpy(mbr.partitions[current_partition_index].fit,data.fit);
            clean(mbr.partitions[current_partition_index].name,50);
            strcpy(mbr.partitions[current_partition_index].name,data.name);
            mbr.partitions[current_partition_index].status = 0;
            mbr.partitions[current_partition_index].type=data.type;
            mbr.partitions[current_partition_index].start = new_partition_start;
            //crear una particion extendida
            ebr.next = -1;
            ebr.start = mbr.partitions[current_partition_index].start + static_cast<int>(sizeof (Ebr));
            //escribe ebr raiz vacio
            fseek(file,mbr.partitions[current_partition_index].start,SEEK_SET);
            fwrite(&ebr, sizeof(Ebr), 1, file);//escribe ebr
            //Escribir el MBR
            fseek(file,0,SEEK_SET);
            fwrite(&mbr, sizeof(Mbr), 1, file);//escribe Mbr
            fclose(file);
        }
    }
    else
    {
        //crear una particion primaria
        mbr.partitions[current_partition_index].size=data.size;
        clean(mbr.partitions[current_partition_index].fit,2);
        strcpy(mbr.partitions[current_partition_index].fit,data.fit);
        clean(mbr.partitions[current_partition_index].name,50);
        strcpy(mbr.partitions[current_partition_index].name,data.name);
        mbr.partitions[current_partition_index].status = 0;
        mbr.partitions[current_partition_index].type=data.type;
        //puntero al inicio del file
        fseek(file,0,SEEK_SET);
        fwrite(&mbr, sizeof(Mbr), 1, file);//escribe mbr
        fclose(file);
    }
    //output para el usuario
    printf("Particion creada con exito: '%s'",data.name);
    printf(", en disco %s\n",data.path);
    //read_partitions(data.path);
}

void disk::mountDisk(Metadata data)
{

    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    if(!validate_path(data.path)) return;
    //Abrir el file
    FILE* file = fopen(data.path, "rb+");
    //Para verificar el nombre tenemos que obtener el mbr
    Mbr mbr = get_mbr(file);
    //verificar si el mbr es correcto
    if(mbr.size == -1)
    {
        printf("Error al leer el disco: %s\n",data.path);
        return;
    }
    //validar que el nombre exista
    if(validate_partition_name(data.name,mbr) == -1){
        printf("Error: Nombre de particion inexistente: %s\n",data.name);
    }
    else if(validate_partition_name(data.name,mbr) == -2) return;

    /******************************
     * MONTAR DISCO
     *****************************/
    //Guardando el index de la particion con el nombre si no existe muestra -1
    int current_partition_index = validate_partition_name(data.name,mbr);
    //si no se encuentra entre las primarias puede que sea logica
    if(current_partition_index == -1)
    {
        //para guardar el index de la particion extendida
        int extended_index= get_extended_partition_index(mbr);
        //Si no hay particion extendida
        if(extended_index == -1)
        {
            printf("Error de montaje: nombre de particion no encontrado: '%s'\n",data.name);
            return;
        }
        //buscar la particion logica en los ebr de index
        Ebr ebr;
        fseek(file,mbr.partitions[extended_index].start,SEEK_SET);
        fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
        //buscando el nombre entre las particiones logicas
        while(strcmp(ebr.name,data.name) != 0 && ebr.next != -1)
        {
            fseek(file,ebr.next,SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
        }
        if(ebr.next == -1)
        {
            printf("Error de montaje: nombre de particion no encontrado: '%s'\n",data.name);
            return;
        }
        //para guardar la particion logica y agregarla a ram
        Partition logic_partition;
        //Guardando nuestra particion
        fseek(file,ebr.start,SEEK_SET);
        fread(&logic_partition,sizeof (Partition),1,file);
        //se busca un numero para la particion
        int partition_number = 1;
        QString new_id = generate_ID(data);
        //si ya tenia montada una entonces salir
        if(new_id.compare("-1") == 0) return;
        //iterar el mounted partitions <map> para buscar un numero sin usar
        while(mounted_partitions.contains(QString(new_id + "%1").arg(partition_number)))partition_number++;
        //se agrega la particion a la lista de partitions montadas
        new_id = QString(new_id + "%1").arg(partition_number);
        //subiendo nuestra particion a ram
        mounted_partitions[new_id] = logic_partition;
        //cambia el estado de la logic a "montada"
        ebr.status = 1;
        //puntero al inicio del ebr
        fseek(file,ebr.start - static_cast<int>(sizeof (Ebr)),SEEK_SET);
        fwrite(&ebr, sizeof(Ebr), 1, file);//escribe mbr
        fclose(file);
        //output de usuario
        printf("Sistema: %s Montada con exito, ",data.name);
        printf("codigo: '%s'\n",new_id.toStdString().c_str());

    }
    else
    {
        //se procede a montar la particion
        mbr.partitions[current_partition_index].status = 1;
        //se busca un numero para la particion
        int partition_number = 1;
        QString new_id = generate_ID(data);
        //si ya tenia montada una entonces salir
        if(new_id.compare("-1") == 0) return;
        //iterar el mounted partitions
        while(mounted_partitions.contains(QString(new_id + "%1").arg(partition_number)))partition_number++;
        //se agrega la particion a la lista de partitions montadas
        new_id = QString(new_id + "%1").arg(partition_number);
        mounted_partitions[new_id] = mbr.partitions[current_partition_index];
        //puntero al inicio del file
        fseek(file,0,SEEK_SET);
        fwrite(&mbr, sizeof(Mbr), 1, file);//escribe mbr
        fclose(file);
        //output de usuario
        printf("Sistema: %s Montada con exito, ",data.name);
        printf("codigo: '%s'\n",new_id.toStdString().c_str());
    }
    read_mounted_partitions();
}

void disk::unmountDisk(QString unmount_id_partition)
{
    if(mounted_partitions.contains(QString(unmount_id_partition).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(unmount_id_partition[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        //Abrir el file
        FILE* file = fopen(disk_path.c_str(), "rb+");
        //Para verificar el nombre tenemos que obtener el mbr
        Mbr mbr = get_mbr(file);
        //verificar si el mbr es correcto
        if(mbr.size == -1)
        {
            printf("Error: al leer el disco: %s\n",disk_path.c_str());
            return;
        }
        //guardar el nombre de la particion
        char current_partition_name[50];
        strcpy(current_partition_name,mounted_partitions[QString(unmount_id_partition).toUpper()].name);
        //Buscar el nombre en las particiones primarias
        int current_partition_index = validate_partition_name(current_partition_name,mbr);
        //si la encuentra en las primarias
        if(current_partition_index != -1)
        {
            //se encuentra la particoin entre las primarias
            mbr.partitions[current_partition_index].status = 0;
            //puntero al inicio del file
            fseek(file,0,SEEK_SET);
            fwrite(&mbr, sizeof(Mbr), 1, file);//escribe mbr
            fclose(file);
            //removiendo la particion de la ram
            mounted_partitions.remove(QString(unmount_id_partition).toUpper());
            printf("Sistema: Se desmonto con exito '%s'\n",unmount_id_partition.toStdString().c_str());
        }
        else
        {
            //puede que sea una logica
            //para guardar el index de la particion extendida
            int extended_index= get_extended_partition_index(mbr);
            //Si no hay particion extendida
            if(extended_index == -1)
            {
                printf("Error de desmontaje: id de particion no montada: '%s'\n",unmount_id_partition.toStdString().c_str());
                return;
            }
            //buscar la particion logica en los ebr de index
            Ebr ebr;
            fseek(file,mbr.partitions[extended_index].start,SEEK_SET);
            fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
            //buscando el nombre entre las particiones logicas
            while(strcmp(ebr.name,current_partition_name) != 0 && ebr.next != -1)
            {
                fseek(file,ebr.next,SEEK_SET);
                fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
            }
            if(ebr.next == -1)
            {
                printf("Error de desmontaje: nombre de particion no encontrado: '%s'\n",unmount_id_partition.toStdString().c_str());
                return;
            }
            else
            {
                //se encuentra la particion logica
                ebr.status = 0;
                fseek(file,ebr.start - static_cast<int>(sizeof (Ebr)),SEEK_SET);
                fwrite(&ebr, sizeof(Ebr), 1, file);//escribe ebr
                //para guardar la particion del ebr
                Partition partition_pointer;
                //desmontando la particion
                fseek(file,ebr.start,SEEK_SET);
                fread(&partition_pointer,sizeof (Partition),1,file);
                partition_pointer.status = 0;
                //guardando los cambios
                fseek(file,ebr.start,SEEK_SET);
                fwrite(&partition_pointer,sizeof (Partition),1,file);
                fclose(file);
                //removiendo la particion de la ram
                mounted_partitions.remove(QString(unmount_id_partition).toUpper());
                //output de usuario
                printf("Sistema: '%s' desmontada con exito",unmount_id_partition.toStdString().c_str());
            }
        }

    }
    else
    {
        printf("Error: al desmontar '%s', no existe el ID\n",unmount_id_partition.toStdString().c_str());
    }
    //read_mounted_partitions();
}

void disk::formatDisk(Metadata data)
{
    if(!is_mounted_partition(data.id))
    {//si la particion no esta montada
        printf("\n**********Error al formatear, la particion '%s' no esta montada*************", data.id);
    }
    else
    {//si la particion esta montada
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        { //si existe el file
            //Lee el mbr
            Mbr mbr;
            fseek(file,0,SEEK_SET);
            fread(&mbr, sizeof(Mbr), 1, file);
            //Integers que guardan el size de la particion y su nombre
            int number_of_structures = 0;
            std::string current_partition_name;
            int current_partition_start = -1;
            //Para buscar si la particion es primaria o secundaria
            int primary_partition_index=-1;
            //verificar si la particion es primaria
            for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                /*if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,mbr.partitions[z].name))//si encuentra el nombre
                {
                    primary_partition_index=z;//indica la posicion en las primarias
                    break;
                }*/
            }
            if(primary_partition_index==-1)
            {//La particion es extendida o logica
                //Se procedera a buscar la particion logica
                int extended_partitions_index = -1;
                for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                    if(mbr.partitions[z].type=='E'||mbr.partitions[z].type=='e')//si encuentra Extendida(ya que solo hay una)
                    {
                        extended_partitions_index=z;
                    }
                }
                //se encontro una particion extendida
                if(extended_partitions_index==-1)
                {
                    printf("\n************Error, no se puede encontrar una particion extendida**************\n");
                }
                else
                {//existe la particion Extendida
                    //activador para ver si existe particion logica
                    int logic_partitions_index = -1;

                    //para guatdar el ebr
                    Ebr ebr;
                    int pointer=mbr.partitions[extended_partitions_index].start;
                    //leyendo el ebr
                    file= fopen(disk_path.c_str(), "rb+");
                    fseek(file,pointer,SEEK_SET);
                    fread(&ebr, sizeof(Ebr), 1, file);
                    //puntero en el que se guarda el proximo ebr
                    int next=ebr.next;
                    //para llevar el numero de particiones logicas
                    int logic_partitions_number=0;
                    int espaciolibre=mbr.partitions[extended_partitions_index].size;
                    espaciolibre-=32;
                    do{
                        //si tiene un ebr siguiente
                        if(ebr.next!=-1){
                            //avanza el puntero toda la estructura del ebr
                            pointer+=sizeof(Ebr);
                            pointer+=ebr.size;
                            //lee el proximo ebr
                            fseek(file,pointer,SEEK_SET);
                            fread(&ebr, sizeof(Ebr), 1, file);
                            //se traslada al proximo ebr y se aumenta el logic_partitions_number
                            next=ebr.next;
                            logic_partitions_number++;
                        }
                        //hace esto 500 veces si los ebr no se terminan
                        logic_partitions_number++;
                    }while(next!=-1 && logic_partitions_number <= 500);
                    //un array con el numero de particiones logicas
                    Ebr index[logic_partitions_number+1];
                    logic_partitions_number=0;
                    //guardando el primer ebr
                    pointer=mbr.partitions[extended_partitions_index].start;
                    fseek(file,pointer,SEEK_SET);
                    fread(&index[logic_partitions_number], sizeof(Ebr), 1, file);
                    next = -1;
                    do{
                        //iterando todas las logicas
                        if(index[logic_partitions_number].next!=-1)
                        {
                            pointer+=sizeof(Ebr);
                            pointer+=index[logic_partitions_number].size;
                            fseek(file,pointer,SEEK_SET);
                            fread(&index[logic_partitions_number+1], sizeof(Ebr), 1, file);
                            next=index[logic_partitions_number].next;
                        }
                        logic_partitions_number++;
                    }while(next!=-1);
                    //para el numero de particiones logicas si su start existe
                    for(int i=0;i<logic_partitions_number;i++){
                        if(index[i].start!=-1 ){//&& index[i].status!='0'
                            //ver si el nombre coincide y si esta montada
                            /*if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,index[i].name) && index[i].status!='0')
                            {
                                //Seteando el index en la particion extendida de la particion logica
                                logic_partitions_index=i;
                            }*/
                        }
                        //Seteando el nombre he inicio de la particion logica
                        current_partition_name=index[logic_partitions_index].name;
                        current_partition_start = index[logic_partitions_index].start;
                    }
                    if(logic_partitions_index==-1)
                    {
                        printf("\n************Error, particion no es primaria ni logica**************\n");
                        return;
                    }
                    else
                    {// si existe en las logicas
                        number_of_structures=file_system::structures_number(index[logic_partitions_index].size);
                    }
                }
            }
            else
            {//existe en las primarias
                number_of_structures=file_system::structures_number(mbr.partitions[primary_partition_index].size);
                current_partition_start = mbr.partitions[primary_partition_index].start;
            }
            //Creacion del superbloque
            SuperBloque super = make_new_superblock(number_of_structures,current_partition_start);
            strcpy(super.sb_nombre_hd,current_partition_name.c_str());
            //escribiendo Superbloque
            file= fopen(disk_path.c_str(), "rb+");
            fseek(file,current_partition_start,SEEK_SET);//puntero al inicio de la particion
            fwrite(&super, sizeof(SuperBloque), 1, file);// escribiendo el Superbloque
            fclose(file);
            //escribiendo copia de Superbloque
            file= fopen(disk_path.c_str(), "rb+");
            int iniciocopia=super.sb_ap_log+(number_of_structures*static_cast<int>(sizeof(Bitacora)))+1;
            fseek(file,iniciocopia,SEEK_SET);//puntero al inicio de la particion
            fwrite(&super, sizeof(SuperBloque), 1, file);// escribiendo el Superbloque
            fclose(file);
            //escribiendo log
            Bitacora Log;
            file= fopen(disk_path.c_str(), "rb+");
            Log.log_tipo = 'N';
            Log.log_contenido = 'N';
            Log.log_tipo_operacion = 'N';
            clean(Log.log_nombre,100);
            clean(Log.log_fecha,20);
            //la posicion del primer log
            int log_pointer=super.sb_ap_log;
            for (int c=0;c<number_of_structures;c++)
            {
                file= fopen(disk_path.c_str(), "rb+");
                fseek(file,(log_pointer),SEEK_SET);
                fwrite(&Log,sizeof (Bitacora),1,file);
                fclose(file);
                log_pointer+=sizeof (Bitacora);
            }
            //nos da el codigo del char vacio
            char empty_char = '\0';
            //escribiendo el bitmap del AVD
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerArbol=super.sb_ap_bitmap_arbol_directorio;//coloca el inicio del bitmap de AVD
            for(int i=0; i<(number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointerArbol,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerArbol+=sizeof(char);
            }
            fclose(file);
            //escribiendo el bitmap del DD
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerDetalleDirectorio=super.sb_ap_bitmap_detalle_directorio;//coloca el inicio del bitmap de DD
            for(int i=0; i<(number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointerDetalleDirectorio,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerDetalleDirectorio+=sizeof(char);
            }
            fclose(file);
            //Escribiendo el bitmap de los inodos
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointeri=super.sb_ap_bitmap_tabla_inodo;//coloca el inicio del bitmap de inodos
            for(int i=0; i<(5*number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointeri,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointeri+=sizeof(char);
            }
            fclose(file);
            //Escribiendo el bitmap de bloques
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerBloques=super.sb_ap_bitmap_bloques;//coloca el inicio del bitmap de bloques
            for(int i=0; i<(5*4*number_of_structures); i++){//setea 0 en el arreglo de bitmaps de bloques
                fseek(file,bb_start_pointerBloques,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerBloques+=sizeof(char);
            }
            fclose(file);
            //***********se crean los inodos***************
            int start_of_inode=super.sb_ap_tabla_inodo;
            iNodo inode;
            file= fopen(disk_path.c_str(), "rb+");
            //crea todos los inodos posibles
            for(int i=0; i<(5*number_of_structures); i++){
                //Valores por defecto para cada inodo
                inode.i_ap_indirecto=-1;
                inode.i_array_bloques[0]=-1;
                inode.i_array_bloques[1]=-1;
                inode.i_array_bloques[2]=-1;
                inode.i_array_bloques[3]=-1;
                inode.i_count_bloques_asignados=0;
                inode.i_count_inodo=i;
                inode.i_size_archivo=0;
                clean(inode.i_id_proper,10);
                strcpy(inode.i_id_proper,"root");
                fseek(file,start_of_inode,SEEK_SET);
                fwrite(&inode, sizeof(iNodo), 1, file);
                //Se pasa al inodo siguiente
                start_of_inode+=sizeof(iNodo);
            }
            //Guarda la informacion del login
            QString users="1, G, root \n1, U, root, root, 201700831 \n";
            //Llenar inodo raiz con informacion del login
            inode.i_size_archivo=users.size();
            inode.i_count_bloques_asignados=get_block_count(users.size());
            inode.i_ap_indirecto=-1;
            inode.i_count_inodo = 0;
            //Escribir el primer usuario y grupos
            for (int k=0;k<inode.i_count_bloques_asignados;k++) {
                inode.i_array_bloques[k]=k;
            }
            fseek(file,super.sb_ap_tabla_inodo,SEEK_SET);
            fwrite(&inode,sizeof (iNodo),1,file);
            int bm = 1;
            //Escitura en el bitmap de la tabla de inodos
            fseek(file,super.sb_ap_bitmap_tabla_inodo,SEEK_SET);
            fwrite(&bm,sizeof (char),1,file);
            fclose(file);
            //**********se crean todos los bloques posibles***********
            int start_of_blocks=super.sb_ap_bloques;
            //Objeto que nos sirce como iterador
            Bloque bloque;
            file= fopen(disk_path.c_str(), "rb+");
            for(int i=0; i<(4*5*number_of_structures); i++){
                //limpia los 25 caracteres del bloque
                clean(bloque.db_data,25);
                strcpy(bloque.db_data,"vacio");
                fseek(file,start_of_blocks,SEEK_SET);
                fwrite(&bloque, sizeof(Bloque), 1, file);
                start_of_blocks+=sizeof(Bloque);
            }
            //metiendo el users.txt a bloques
            int bloque_start_pointer=super.sb_ap_bloques;
            int auxC=0;
            int iniciobm=super.sb_ap_bitmap_bloques;
            for (int k=0;k<inode.i_count_bloques_asignados;k++) {
                fseek(file,bloque_start_pointer,SEEK_SET);
                fread(&bloque,sizeof (Bloque),1,file);

                fseek(file,iniciobm,SEEK_SET);
                fwrite(&bm,sizeof (char),1,file);

                iniciobm+=sizeof (char);
                if(k==(inode.i_count_bloques_asignados-1)){
                    for (int j=0;j<get_remains_blocks(users.size());j++) {
                        bloque.db_data[j]=users.toStdString().c_str()[auxC];
                        auxC++;
                    }
                }
                else {
                    for (int j=0;j<25;j++)
                    {
                        bloque.db_data[j]=users.toStdString().c_str()[auxC];
                        auxC++;
                    }
                }
                fseek(file,bloque_start_pointer,SEEK_SET);
                fwrite(&bloque,sizeof (Bloque),1,file);
                bloque_start_pointer+=sizeof (Bloque);
            }
            fclose(file);
            //ver el tipo de formateo
            if(!strcasecmp(data.format_type,"fast"))
            {
                //ya no se crean las estructuras
                printf("\n***********Formateo rapido exitoso en particion ID='%s'************\n",data.id);
            }
            else if (!strcasecmp(data.format_type,"full"))
            {
                //crear estructuras ademas de los bitmaps
                //para guardar la fecha de creacion
                std::string current_time = get_current_time();

                //se crea la raiz del arbol de directorios
                AVD avd_root;
                clean(avd_root.avd_fecha_creacion,25);
                strcpy(avd_root.avd_fecha_creacion,get_current_time().c_str());//set fecha
                clean(avd_root.avd_nombre_directorio,50);
                strcpy(avd_root.avd_nombre_directorio,"avd_root");//set nombre
                for(int i=0; i<6; i++){//setea 0 en el array de subdirectorios
                    avd_root.avd_ap_array_subdirectorios[i]=-1;
                }
                avd_root.avd_ap_detalle_directorio=0;//apunta a su detalle de Diractorio Pos en el bitmap
                avd_root.avd_ap_arbol_virtual_directorio=-1;//apuntador indirecto
                clean(avd_root.avd_proper,10);
                strcpy(avd_root.avd_proper,"root");
                //escribe la raiz
                file= fopen(disk_path.c_str(), "rb+");
                fseek(file,super.sb_ap_arbol_directorio,SEEK_SET);
                fwrite(&avd_root, sizeof(AVD), 1, file);
                fclose(file);
                //se crea la raiz del detalle de directorio
                DD dd_root;
                dd_root.dd_ap_detalle_directorio=-1;//apntador indiracto del detalle directorio
                clean(dd_root.dd_array_files[0].dd_file_nombre,50);
                strcpy(dd_root.dd_array_files[0].dd_file_nombre,"users.txt");//setea el nombre del file
                dd_root.dd_array_files[0].dd_file_ap_inodo=0;//apuntador al inodo posicion del bitmap de inodos
                clean(dd_root.dd_array_files[0].dd_file_date_creacion,25);
                clean(dd_root.dd_array_files[0].dd_file_date_modificacion,25);
                strcpy(dd_root.dd_array_files[0].dd_file_date_creacion,current_time.c_str());//fecha
                strcpy(dd_root.dd_array_files[0].dd_file_date_modificacion,current_time.c_str());//fecha
                //escribe la raiz
                file= fopen(disk_path.c_str(), "rb+");
                fseek(file,super.sb_ap_detalle_directorio,SEEK_SET);
                fwrite(&dd_root, sizeof(DD), 1, file);
                fclose(file);
                printf("\n***********Formateo completo exitoso en particion ID='%s'************\n",data.id);
            }
            else
            {
                printf("\n*******Error en el tipo de formato requerido='%s', use fast o full*******\n",data.format_type);
            }
        }//Verificar si la ruta del disco existe
    }//Verificar si esta montada la particion
}

void disk::makeReport(Metadata data)
{
    //arreglar el path
    validate_path(data.path);
    //crear directorios
    make_directories(data.path);
    //devuelve el formato del reporte
    string format_type(data.path);
    format_type = get_report_format(format_type);
    if(format_type == "0"){
        printf("Error: Formato del reporte erroneo");
        return;
    }
    if(mounted_partitions.contains(QString(data.id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        //comprobar si no hay error al abrir el file
        if(file != nullptr)
        {
            Mbr mbr = get_mbr(file);
            //switching data name (case insensitive)
            QString choice = QString(data.name).toUpper();
            if(choice == "MBR")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_mbr_table_content(data.path,mbr,format_type);
            }
            else if(choice == "FILE")
            {

            }
            else if(choice == "DISK")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_dsk_table_content(data.path,mbr,file);
            }
            else
            {
                printf("Error, nombre del reporte no aceptado '%s'\n",data.name);
            }
        }
        else
        {
            printf("Error al abrir el disco '%s'\n",disk_path.c_str());
        }
    }
    else
    {
        printf("Error: particion no montada '%s'\n",data.id);
    }
}

//*****************AQUI TERMINA LA PRIMERA FASE ***************

void disk::loss(Metadata data)
{
    if(!is_mounted_partition(data.id))
    {//si la particion no esta montada
        printf("\n**********Error al formatear, la particion '%s' no esta montada*************", data.id);
    }
    else
    {//si la particion esta montada
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        { //si existe el file
            //Lee el mbr
            Mbr mbr;
            fseek(file,0,SEEK_SET);
            fread(&mbr, sizeof(Mbr), 1, file);
            //Integers que guardan el size de la particion y su nombre
            int number_of_structures = 0;
            std::string current_partition_name;
            int current_partition_start = -1;
            //Para buscar si la particion es primaria o secundaria
            int primary_partition_index=-1;
            //verificar si la particion es primaria
            for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                /* if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,mbr.partitions[z].name))//si encuentra el nombre
                {
                    primary_partition_index=z;//indica la posicion en las primarias
                    break;
                }*/
            }
            if(primary_partition_index==-1)
            {//La particion es extendida o logica
                //Se procedera a buscar la particion logica
                int extended_partitions_index = -1;
                for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                    if(mbr.partitions[z].type=='E'||mbr.partitions[z].type=='e')//si encuentra Extendida(ya que solo hay una)
                    {
                        extended_partitions_index=z;
                    }
                }
                //se encontro una particion extendida
                if(extended_partitions_index==-1)
                {
                    printf("\n************Error, no se puede encontrar una particion extendida**************\n");
                }
                else
                {//existe la particion Extendida
                    //activador para ver si existe particion logica
                    int logic_partitions_index = -1;
                    //para guatdar el ebr
                    Ebr ebr;
                    int pointer=mbr.partitions[extended_partitions_index].start;
                    //leyendo el ebr
                    file= fopen(disk_path.c_str(), "rb+");
                    fseek(file,pointer,SEEK_SET);
                    fread(&ebr, sizeof(Ebr), 1, file);
                    //puntero en el que se guarda el proximo ebr
                    int next=ebr.next;
                    //para llevar el numero de particiones logicas
                    int logic_partitions_number=0;
                    int espaciolibre=mbr.partitions[extended_partitions_index].size;
                    espaciolibre-=32;
                    do{
                        //si tiene un ebr siguiente
                        if(ebr.next!=-1){
                            //avanza el puntero toda la estructura del ebr
                            pointer+=sizeof(Ebr);
                            pointer+=ebr.size;
                            //lee el proximo ebr
                            fseek(file,pointer,SEEK_SET);
                            fread(&ebr, sizeof(Ebr), 1, file);
                            //se traslada al proximo ebr y se aumenta el logic_partitions_number
                            next=ebr.next;
                            logic_partitions_number++;
                        }
                        //hace esto 500 veces si los ebr no se terminan
                        logic_partitions_number++;
                    }while(next!=-1 && logic_partitions_number <= 500);
                    //un array con el numero de particiones logicas
                    Ebr index[logic_partitions_number+1];
                    logic_partitions_number=0;
                    //guardando el primer ebr
                    pointer=mbr.partitions[extended_partitions_index].start;
                    fseek(file,pointer,SEEK_SET);
                    fread(&index[logic_partitions_number], sizeof(Ebr), 1, file);
                    next = -1;
                    do{
                        //iterando todas las logicas
                        if(index[logic_partitions_number].next!=-1)
                        {
                            pointer+=sizeof(Ebr);
                            pointer+=index[logic_partitions_number].size;
                            fseek(file,pointer,SEEK_SET);
                            fread(&index[logic_partitions_number+1], sizeof(Ebr), 1, file);
                            next=index[logic_partitions_number].next;
                        }
                        logic_partitions_number++;
                    }while(next!=-1);
                    //para el numero de particiones logicas si su start existe
                    for(int i=0;i<logic_partitions_number;i++){
                        if(index[i].start!=-1 ){//&& index[i].status!='0'
                            //ver si el nombre coincide y si esta montada
                            /*if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,index[i].name) && index[i].status!='0')
                            {
                                //Seteando el index en la particion extendida de la particion logica
                                logic_partitions_index=i;
                            }*/
                        }
                        //Seteando el nombre he inicio de la particion logica
                        current_partition_name=index[logic_partitions_index].name;
                        current_partition_start = index[logic_partitions_index].start;
                    }
                    if(logic_partitions_index==-1)
                    {
                        printf("\n************Error, particion no es primaria ni logica**************\n");
                        return;
                    }
                    else
                    {// si existe en las logicas
                        number_of_structures=file_system::structures_number(index[logic_partitions_index].size);
                    }
                }
            }
            else
            {//existe en las primarias
                number_of_structures=file_system::structures_number(mbr.partitions[primary_partition_index].size);
                current_partition_start = mbr.partitions[primary_partition_index].start;
            }
            //lectura del superbloque malo
            SuperBloque super;
            //leyendo Superbloque
            file= fopen(disk_path.c_str(), "rb+");
            fseek(file,current_partition_start,SEEK_SET);//puntero al inicio de la particion
            fread(&super, sizeof(SuperBloque), 1, file);// escribiendo el Superbloque
            fclose(file);
            //nos da el codigo del char vacio
            char empty_char = '\0';
            //escribiendo el bitmap del AVD
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerArbol=super.sb_ap_bitmap_arbol_directorio;//coloca el inicio del bitmap de AVD
            for(int i=0; i<(number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointerArbol,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerArbol+=sizeof(char);
            }
            fclose(file);
            //escribiendo el bitmap del DD
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerDetalleDirectorio=super.sb_ap_bitmap_detalle_directorio;//coloca el inicio del bitmap de DD
            for(int i=0; i<(number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointerDetalleDirectorio,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerDetalleDirectorio+=sizeof(char);
            }
            fclose(file);
            //Escribiendo el bitmap de los inodos
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointeri=super.sb_ap_bitmap_tabla_inodo;//coloca el inicio del bitmap de inodos
            for(int i=0; i<(5*number_of_structures); i++){//setea 0 en el arreglo de bitmaps de inodos
                fseek(file,bb_start_pointeri,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointeri+=sizeof(char);
            }
            fclose(file);
            //Escribiendo el bitmap de bloques
            file= fopen(disk_path.c_str(), "rb+");
            int bb_start_pointerBloques=super.sb_ap_bitmap_bloques;//coloca el inicio del bitmap de bloques
            for(int i=0; i<(5*4*number_of_structures); i++){//setea 0 en el arreglo de bitmaps de bloques
                fseek(file,bb_start_pointerBloques,SEEK_SET);
                fwrite(&empty_char, sizeof(char), 1, file);
                bb_start_pointerBloques+=sizeof(char);
            }
            fclose(file);
            //***********se crean los inodos***************
            int start_of_inode=super.sb_ap_tabla_inodo;
            iNodo inode;
            file= fopen(disk_path.c_str(), "rb+");
            //crea todos los inodos posibles
            for(int i=0; i<(5*number_of_structures); i++){
                //Valores por defecto para cada inodo
                inode.i_ap_indirecto=-1;
                inode.i_array_bloques[0]=-1;
                inode.i_array_bloques[1]=-1;
                inode.i_array_bloques[2]=-1;
                inode.i_array_bloques[3]=-1;
                inode.i_count_bloques_asignados=0;
                inode.i_count_inodo=i;
                inode.i_size_archivo=0;
                clean(inode.i_id_proper,10);
                strcpy(inode.i_id_proper,"root");
                fseek(file,start_of_inode,SEEK_SET);
                fwrite(&inode, sizeof(iNodo), 1, file);
                //Se pasa al inodo siguiente
                start_of_inode+=sizeof(iNodo);
            }
            //**********se crean todos los bloques posibles***********
            int start_of_blocks=super.sb_ap_bloques;
            //Objeto que nos sirce como iterador
            Bloque bloque;
            file= fopen(disk_path.c_str(), "rb+");
            for(int i=0; i<(4*5*number_of_structures); i++){
                //limpia los 25 caracteres del bloque
                clean(bloque.db_data,25);
                strcpy(bloque.db_data,"bloque_corrupto");
                fseek(file,start_of_blocks,SEEK_SET);
                fwrite(&bloque, sizeof(Bloque), 1, file);
                start_of_blocks+=sizeof(Bloque);
            }
            fclose(file);

            //se crea la raiz del arbol de directorios
            AVD avd_root;
            clean(avd_root.avd_fecha_creacion,25);
            clean(avd_root.avd_nombre_directorio,50);
            for(int i=0; i<6; i++){//setea 0 en el array de subdirectorios
                avd_root.avd_ap_array_subdirectorios[i]=-1;
            }
            avd_root.avd_ap_detalle_directorio=0;//apunta a su detalle de Diractorio Pos en el bitmap
            avd_root.avd_ap_arbol_virtual_directorio=-1;//apuntador indirecto
            clean(avd_root.avd_proper,10);
            //escribe la raiz del avd
            file= fopen(disk_path.c_str(), "rb+");
            fseek(file,super.sb_ap_arbol_directorio,SEEK_SET);
            fwrite(&avd_root, sizeof(AVD), 1, file);
            fclose(file);
            //se crea la raiz del detalle de directorio
            DD dd_root;
            dd_root.dd_ap_detalle_directorio=-1;//apntador indiracto del detalle directorio
            clean(dd_root.dd_array_files[0].dd_file_nombre,50);
            dd_root.dd_array_files[0].dd_file_ap_inodo=0;//apuntador al inodo posicion del bitmap de inodos
            clean(dd_root.dd_array_files[0].dd_file_date_creacion,25);
            clean(dd_root.dd_array_files[0].dd_file_date_modificacion,25);
            //escribe la raiz
            file= fopen(disk_path.c_str(), "rb+");
            fseek(file,super.sb_ap_detalle_directorio,SEEK_SET);
            fwrite(&dd_root, sizeof(DD), 1, file);
            fclose(file);
            //escritura del superbloque malo
            SuperBloque malo = superblock_vacio();
            //leyendo Superbloque
            file= fopen(disk_path.c_str(), "rb+");
            fseek(file,current_partition_start,SEEK_SET);//puntero al inicio de la particion
            fwrite(&malo, sizeof(SuperBloque), 1, file);// escribiendo el Superbloque
            fclose(file);
            printf("\n***********Perdida de datos exitosa en la particion='%s'************\n",data.id);
        }//Verificar si la ruta del disco existe
    }//Verificar si esta montada la particion
}

void disk::login(Metadata data)
{
    if(cookie.name[0]==0 && cookie.password[0]==0)
    {
        if(mounted_partitions.contains(QString(data.id).toUpper()))
        {
            //encontrar el path del disco de la particion montada
            std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
            //abrir el disco para escribir en el
            FILE* file= fopen(disk_path.c_str(), "rb+");
            fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
            //comprobar si no hay error al abrir el file
            if(file == nullptr)
            {
                printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
            }
            else {
                //leyendo el mbr
                fseek(file,0,SEEK_SET);
                Mbr mbr;
                fread(&mbr,sizeof (Mbr),1,file);
                //para guardar la posicion de la particion en el mbr
                int partition_index = -1;
                //recorriendo las particiones
                for (int i=0;i<4;i++) {
                    /*if(strcmp(mbr.partitions[i].name,mounted_partitions[QString(data.id).toUpper()].name)==0){
                        partition_index=i;
                        break;
                    }*/
                }
                //si encontro el nombre
                if(partition_index!=-1)
                {
                    //leyendo el superbloque de la particion
                    SuperBloque sb;
                    fseek(file,mbr.partitions[partition_index].start,SEEK_SET);
                    fread(&sb,sizeof (SuperBloque),1,file);
                    //para ver el inicio
                    int inicio = 0;
                    //cola para guardar bloques
                    QQueue<int> blocks_queue;
                    iNodo inode;
                    //marcando el primer inicio
                    inicio = sb.sb_ap_tabla_inodo;
                    do{
                        fseek(file,inicio,SEEK_SET);
                        fread(&inode,sizeof (iNodo),1,file);
                        for (int a=0;a<4;a++) {
                            if(inode.i_array_bloques[a]!=-1){
                                blocks_queue.enqueue(inode.i_array_bloques[a]);
                            }
                        }
                        //actualizando el inicio
                        inicio=sb.sb_ap_tabla_inodo+(inode.i_ap_indirecto*static_cast<int>(sizeof (iNodo)));
                    }while(inode.i_ap_indirecto!=-1);

                    //el file de usuarios.txt
                    QString usuarios;
                    Bloque block;
                    while(!blocks_queue.empty()){
                        inicio=(sb.sb_ap_bloques+(blocks_queue.front()*static_cast<int>(sizeof (Bloque))));
                        blocks_queue.dequeue();
                        fseek(file,inicio,SEEK_SET);
                        fread(&block,sizeof (Bloque),1,file);
                        for (int i=0;i<25;i++) {
                            usuarios+=block.db_data[i];
                        }
                    }
                    //lista de usuarios separados
                    /*
                     * En data.name se guarda el nombre de usuario en el analizador semantico
                     * En data.path se guarda el password en el analizador semantico
                    */
                    QStringList splitUsuarios;
                    splitUsuarios=usuarios.split("\n");
                    foreach(QString linea,splitUsuarios){
                        if(linea.toStdString().c_str()[0]!='0'){
                            if(linea.toStdString().c_str()[3]=='U'){
                                size_t number=linea.toStdString().find(data.name);
                                if(linea.toStdString().find(data.name,number+1)>7){
                                    if(linea.toStdString().find(QString(data.name).toStdString()+", "
                                                                +QString(data.path).toStdString()+" ")!=std::string::npos)
                                    {
                                        strcpy(cookie.name,data.name);
                                        strcpy(cookie.password,data.path);
                                    }
                                }
                            }
                        }
                    }
                    if(cookie.name[0]!=0 && cookie.password[0]!=0){
                        printf("\nUSUARIO '%s' HA INICIADO SESION\n",cookie.name);
                    }
                    else
                    {
                        printf("\n********nombre de usuario o password incorrecta**********\n");
                    }
                }
                else
                {
                    printf("\n************Error, el nombre de la particion no concuerda '%s'**************\n",data.name);
                }

            }
            fclose(file);
        }
        else
        {
            printf("\n************Error, particion no montada '%s'**************\n",data.name);
        }
    }
    else
    {
        printf("\n************Un usuario ya inicio sesion, por favor cerrar sesion************\n");
    }
}

void disk::logout()
{
    //verificar si existe una sesion
    if(cookie.name[0]==0 && cookie.password[0] ==0)
    {
        //no hay sesion activa
        printf("\n**********Error Logout, No existe una sesion activa***********\n");
    }
    else
    {
        clean(cookie.name,10);
        clean(cookie.password,10);
        printf("\nSE HA CERRADO SESION CON EXITO\n");
    }
}

void disk::makeGroup(Metadata data)
{
    if(mounted_partitions.contains(QString(data.id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else {
            //leyendo el mbr
            fseek(file,0,SEEK_SET);
            Mbr mbr;
            fread(&mbr,sizeof (Mbr),1,file);
            //para guardar la posicion de la particion en el mbr
            int partition_index = -1;
            //recorriendo las particiones
            for (int i=0;i<4;i++)
            {
                /*if(strcmp(mbr.partitions[i].name,mounted_partitions[QString(data.id).toUpper()].name)==0){
                    partition_index=i;
                    break;
                }*/
            }
            //si encontro el nombre
            if(partition_index!=-1)
            {
                //leyendo el superbloque de la particion
                SuperBloque sb;
                fseek(file,mbr.partitions[partition_index].start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);
                //actualizar el ultimo montaje
                strcpy(sb.sb_date_ultimo_montaje,get_current_time().c_str());
                //leyendo el detalle de directorio de la particion
                DD dd;
                fseek(file,sb.sb_ap_detalle_directorio,SEEK_SET);
                fread(&dd,sizeof (DD),1,file);
                //leyendo el inodo raiz
                iNodo inode;
                fseek(file,sb.sb_ap_tabla_inodo,SEEK_SET);
                fread(&inode,sizeof (iNodo),1,file);
                //aumenta el size del file de usuarios
                inode.i_size_archivo+=9+QString(data.name).size();
                //Escribe el nuevo size
                fseek(file,sb.sb_ap_tabla_inodo,SEEK_SET);
                fwrite(&inode,sizeof (iNodo),1,file);
                //los blockes necesarios para escribir el nuevo usuario
                int necesarios=get_block_count(inode.i_size_archivo);
                //Meter a la pila los bloques usados
                QStack<int> blocks_stack;
                for (int a=0;a<4;a++) {
                    if(inode.i_array_bloques[a]!=-1) blocks_stack.push(inode.i_array_bloques[a]);
                }
                //hacer una pila de inodos
                QStack<int> inodes_stack;
                inodes_stack.push(0);
                //el puntero de inicio de los inodos
                int inode_start;
                //mientras el inodo tenga un siguiente
                while(inode.i_ap_indirecto!=-1)
                {
                    //actualiza el inicio del puntero
                    inode_start=sb.sb_ap_tabla_inodo+(inode.i_ap_indirecto*static_cast<int>(sizeof (iNodo)));
                    inodes_stack.push(inode.i_ap_indirecto);
                    //lee el inodo actual
                    fseek(file,inode_start,SEEK_SET);
                    fread(&inode,sizeof (iNodo),1,file);
                    //mete a la pila todos los bloques del inodo si ya estan usados
                    for (int a=0;a<4;a++)
                    {
                        if(inode.i_array_bloques[a]!=-1) blocks_stack.push(inode.i_array_bloques[a]);
                    }
                }
                //Los bloques que tenemos disponibles
                int actuales=blocks_stack.size();
                //vuelve a leer el inodo raiz
                fseek(file,sb.sb_ap_tabla_inodo,SEEK_SET);
                fread(&inode,sizeof (iNodo),1,file);
                //aumenta el size del inodo raiz
                inode.i_size_archivo+=(necesarios-actuales);
                //lee de nuevo el inodo raiz
                fseek(file,sb.sb_ap_tabla_inodo,SEEK_SET);
                fread(&inode,sizeof (iNodo),1,file);
                //para grabar char vacios
                char pr='\0';
                //guarda la cantidad necesaria a escribir
                int cantidad = 0, block_start, flag = 0;
                //puntero para guardar la pila de blocks
                QStack<int> temp_block_stack;
                //para manipular un bloque
                Bloque block;
                //la diferencia indica cuantos bloques faltan
                while(cantidad<(necesarios-actuales))
                {
                    //el inicio del bitmap bloques para empezar a actualizarlo
                    block_start=sb.sb_ap_bitmap_bloques+(flag*static_cast<int>(sizeof (char)));
                    //leyendo el bitmap de bloques
                    fseek(file,block_start,SEEK_SET);
                    fread(&pr,sizeof (char),1,file);
                    //escribir un 1 para el nuevo bloque en su bitmap
                    if(pr==0)
                    {
                        pr=1;
                        //escribiendo un uno binario en el bitmap
                        fseek(file,block_start,SEEK_SET);
                        fwrite(&pr,sizeof (char),1,file);
                        blocks_stack.push(flag);
                        //la diferencia disminuye
                        cantidad++;
                    }
                    flag++;
                }
                //guarda la pila de bloques ya que se actualizara
                temp_block_stack=blocks_stack;
                //Para guardar la data de los bloques
                QString temp_data = "";
                QStringList splitted_text;
                //utilizando la cantidad de bloques guardada en la pila
                while(!blocks_stack.empty())
                {
                    //actualiza el nuevo inicio
                    block_start=(sb.sb_ap_bloques+(blocks_stack.front()*static_cast<int>(sizeof (Bloque))));
                    blocks_stack.pop();
                    //lee el bloque para ver la data que tiene escrita
                    fseek(file,block_start,SEEK_SET);
                    fread(&block,sizeof (Bloque),1,file);
                    //concatena la informaccion escrita
                    for (int i=0;i<25;i++)
                    {
                        //si no es 0 entonces tiene chars
                        if(block.db_data[i]!='\0')temp_data+=block.db_data[i];
                    }
                }
                //separar saltos de linea
                splitted_text=temp_data.split("\n");
                //para verificar si ya existe el grupo
                bool exists_group = false;
                //verificar si ya existe el grupo
                foreach(QString linea,splitted_text)
                {
                    //si la linea no esta vacia
                    if(linea.toStdString().c_str()[0]!='0'){
                        //y la linea representa un grupo
                        if(linea.toStdString().c_str()[3]=='G'){
                            //si el nombre del grupo existe en la linea
                            if(linea.toStdString().find((", "+QString(data.name).toStdString()+" "))!=string::npos)
                            {
                                exists_group=true;
                            }
                        }
                    }
                }
                //si el grupo no existe crearlo
                if(exists_group)printf("Error, el grupo ya existe: '%s'",data.name);
                else{
                    int current_inodes,necesary_inodes;
                    //restaurando la pila de bloques
                    blocks_stack=temp_block_stack;
                    //arreglando la linea a insertar
                    temp_data+=QString::number(temp_data.count("G,")+1)+", G, "+data.name+" \n";
                    //calcula los inodos necesarios para la cantidad de bloques
                    current_inodes=getCantidadInodos(actuales);
                    necesary_inodes=getCantidadInodos(necesarios);
                    //se usaran las siguientes variables
                    cantidad=0;
                    flag=0;
                    //para calcular la cantidad de inodos restantes
                    while(cantidad<(necesary_inodes-current_inodes))
                    {
                        //para guardar el lugar de inicio del inodo
                        inode_start=sb.sb_ap_bitmap_tabla_inodo+(flag*static_cast<int>(sizeof (char)));
                        fseek(file,inode_start,SEEK_SET);
                        fread(&pr,sizeof (char),1,file);
                        //si el char a escribir esta en 0
                        if(pr==0)
                        {
                            //escribir un 1 en el bitmap de inodos
                            pr=1;
                            fseek(file,inode_start,SEEK_SET);
                            fwrite(&pr,sizeof (char),1,file);
                            //aumentar la cantidad de inodos en la pila
                            inodes_stack.push(flag);
                            //reducir la diferencia de inodos a escribir
                            cantidad++;
                        }
                        //aumentar el index del inodo
                        flag++;
                    }
                    //se usaran estos valores de nuevo
                    flag=0;
                    cantidad=0;
                    int valor=0,temp_start;
                    //escribiendo todos los inodos en la pila
                    while(!inodes_stack.empty())
                    {
                        //el nuevo inicio del inodo
                        inode_start=sb.sb_ap_tabla_inodo+(inodes_stack.front()*static_cast<int>(sizeof (iNodo)));
                        inodes_stack.pop();
                        //escribiendo inodo
                        fseek(file,inode_start,SEEK_SET);
                        fread(&inode,sizeof (iNodo),1,file);
                        //para los 4 bloques del inodo
                        while(cantidad<4)
                        {
                            //mientras aun no se vacia la lista de bloques
                            if(!blocks_stack.empty())
                            {
                                //cantidad lleva el index para el array de bloques
                                inode.i_array_bloques[cantidad]=blocks_stack.front();
                                //actualizando el nuevo puntero para el bloque
                                temp_start=sb.sb_ap_bloques+(blocks_stack.front()*static_cast<int>(sizeof (Bloque)));
                                //leyendo el bloque correspondiente
                                fseek(file,temp_start,SEEK_SET);
                                fread(&block,sizeof (Bloque),1,file);
                                clean(block.db_data,25);
                                //metiendo data al bloque
                                for (int a=0;a<25;a++)
                                {
                                    //si el valor es menor de la string a escribir
                                    if(valor<=temp_data.size())
                                    {
                                        //metiendo la string al bloque
                                        block.db_data[a]=temp_data.toStdString().c_str()[valor];
                                        valor++;
                                    }
                                }
                                //escribe el bloque y el inodo
                                fseek(file,temp_start,SEEK_SET);
                                fwrite(&block,sizeof (Bloque),1,file);
                                fseek(file,inode_start,SEEK_SET);
                                fwrite(&inode,sizeof (iNodo),1,file);
                                //retira el ultimo bloque
                                blocks_stack.pop();
                            }
                            //reduce la diferencia a escribir
                            cantidad++;
                        }
                        //si aun hay inodos en la pila, crear nuevo inodo el sistema
                        if(!inodes_stack.empty()) inode.i_ap_indirecto=inodes_stack.front();
                        //guardando cambios
                        fseek(file,inode_start,SEEK_SET);
                        fwrite(&inode,sizeof (iNodo),1,file);
                        //limpiando para la iteracion
                        cantidad=0;
                        flag++;
                    }
                    //Cambio realizado
                    sb.sb_inodos_free-=(necesary_inodes-current_inodes);
                    sb.sb_bloques_free-=(necesarios-actuales);
                    sb.sb_inodos_count+=(necesary_inodes-current_inodes);
                    sb.sb_bloques_count+=(necesarios-actuales);
                    sb.sb_first_free_bit_arbol_directorio=getAVDFree(sb,data.path);
                    sb.sb_first_free_bit_detalle_directorio=getDirectorioFree(sb,data.path);
                    sb.sb_first_free_bit_tabla_inodo=getInodoFree(sb,data.path);
                    sb.sb_first_free_bit_bloques=getBloqueFree(sb,data.path);
                    strcpy(sb.sb_date_ultimo_montaje,get_current_time().c_str());
                    fseek(file,mbr.partitions[partition_index].start,SEEK_SET);
                    fwrite(&sb,sizeof (SuperBloque),1,file);
                    //Cambio realizado
                    fseek(file,sb.sb_ap_detalle_directorio,SEEK_SET);
                    fread(&dd,sizeof (DD),1,file);
                    strcpy(dd.dd_array_files[0].dd_file_date_modificacion,get_current_time().c_str());
                    fseek(file,sb.sb_ap_detalle_directorio,SEEK_SET);
                    fwrite(&dd,sizeof (DD),1,file);
                    //Leyendo la bitacora para decirle que no tiene contenido
                    Bitacora Log;
                    fseek(file,(sb.sb_ap_log+static_cast<int>(sizeof (Bitacora))),SEEK_SET);
                    fread(&Log,sizeof (Bitacora),1,file);
                    //escribiendo log actualizado
                    Log.log_contenido = 0;
                    fseek(file,(sb.sb_ap_log+static_cast<int>(sizeof (Bitacora))),SEEK_SET);
                    fwrite(&Log,sizeof (Bitacora),1,file);
                }
            }
            else{
                cout<<"Error: \"El nombre de la particion no concuerda con el montado\" "<<endl;
            }
        }

        fclose(file);
    }
    else{
        cout<<"Error: \"La particion no se encuentra montada en este momento\" "<<endl;
    }
}

void disk::makeDir(Metadata data)
{
    bool encontrado=false;
    bool p=true;
    QString insercion="";
    QStringList splitel;
    SuperBloque sb;
    AVD arbol;
    AVD hijo;
    int contador=0;
    int padre=0;
    int directorio=0;
    int posicionAvd=-2;
    int posicionBitmapA=-1;
    int posicionBitmapD=-1;
    int inicio=0;
    int inicio2=0;
    int inicioA=0;
    int inicioD=0;
    int NumStructs=0;
    char valor=2;
    int posicion=-1;
    if(mounted_partitions.contains(QString(data.id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else {
            //leyendo el mbr
            fseek(file,0,SEEK_SET);
            Mbr mbr;
            fread(&mbr,sizeof (Mbr),1,file);
            for (int i=0;i<4;i++) {
                /*if(strcmp(mbr.partitions[i].name,mounted_partitions[QString(data.id).toUpper()].name)==0){
                    posicion=i;
                }*/
            }
            if(posicion!=-1){
                fix_path(data.path,500);

                fseek(file,mbr.partitions[posicion].start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);
                NumStructs=file_system::structures_number(mbr.partitions[posicion].size);
                fseek(file,sb.sb_ap_arbol_directorio,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                inicio=sb.sb_ap_arbol_directorio;
                splitel=QString(data.path).split("/");
                if(p)
                {
                    foreach (QString linea,splitel)
                    {
                        encontrado=false;
                        if(linea!=splitel.first())
                        {
                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            contador=0;
                            while(arbol.avd_ap_array_subdirectorios[contador]!=-1){
                                inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                                fseek(file,inicio,SEEK_SET);
                                fread(&hijo,sizeof (AVD),1,file);
                                if(strcmp(hijo.avd_nombre_directorio,linea.toStdString().c_str())==0){
                                    encontrado=true;
                                    padre=arbol.avd_ap_array_subdirectorios[contador];
                                    break;
                                }
                                contador++;
                                directorio=padre;
                                if(contador>5){
                                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                                        padre=arbol.avd_ap_arbol_virtual_directorio;
                                        inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                                        fseek(file,inicio,SEEK_SET);
                                        fread(&arbol,sizeof (AVD),1,file);

                                    }
                                    else{
                                        posicionAvd=-1;
                                        encontrado=false;
                                        break;
                                    }
                                    contador=0;
                                }
                                posicionAvd=contador;

                            }
                            if(!encontrado){
                                inicioA=sb.sb_ap_bitmap_arbol_directorio;
                                fseek(file,inicioA,SEEK_SET);
                                fread(&valor,sizeof (char),1,file);
                                int i=0;
                                posicionBitmapA=-1;
                                posicionBitmapD=-1;
                                while(posicionBitmapA==-1){
                                    inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapA=i;
                                        valor=1;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                i=0;
                                while(posicionBitmapD==-1){
                                    inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapD=i;
                                        valor=1;
                                        fseek(file,inicioD,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                if(posicionAvd==-1){
                                    posicionAvd=-2;
                                    inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicio2,SEEK_SET);
                                    fread(&arbol,sizeof (AVD),1,file);
                                    //Limpieza del nuevo arbol y detalle de directorio
                                    AVD avd1;
                                    AVD avd2;
                                    clean(avd2.avd_proper,25);
                                    clean(avd2.avd_fecha_creacion,20);
                                    clean(avd2.avd_nombre_directorio,25);
                                    avd2.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd2.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd2.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle2;
                                    detalle2.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle2.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle2.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle2.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle2.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd2.avd_proper,cookie.name);
                                    strcpy(avd2.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd2.avd_nombre_directorio,arbol.avd_nombre_directorio);

                                    inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&avd1,sizeof (AVD),1,file);
                                    avd1.avd_ap_arbol_virtual_directorio=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd1,sizeof (AVD),1,file);
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    padre=posicionBitmapA;
                                    directorio=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd2,sizeof (AVD),1,file);

                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);

                                    posicionBitmapA=-1;
                                    posicionBitmapD=-1;
                                    while(posicionBitmapA==-1){
                                        inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                        fseek(file,inicioA,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            posicionBitmapA=i;
                                            valor=1;
                                            fseek(file,inicioA,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                        }
                                        i++;
                                    }
                                    if(posicionBitmapA!=-1){
                                        inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                        avd2.avd_ap_array_subdirectorios[0]=posicionBitmapA;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&avd2,sizeof (AVD),1,file);
                                    }
                                    i=0;
                                    while(posicionBitmapD==-1){
                                        inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            posicionBitmapD=i;
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                        }
                                        i++;
                                    }
                                    //Limpieza del nuevo arbol y detalle de directorio
                                    AVD avd3;
                                    clean(avd3.avd_proper,25);
                                    clean(avd3.avd_fecha_creacion,20);
                                    clean(avd3.avd_nombre_directorio,25);
                                    avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd3.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd3.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle3;
                                    detalle3.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd3.avd_proper,cookie.name);
                                    strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd3,sizeof (AVD),1,file);
                                    padre=posicionBitmapA;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle3,sizeof (DD),1,file);

                                }
                                else{//Investigar aqui
                                    //Limpieza del nuevo arbol y detalle de directorio
                                    if(posicionBitmapA!=-1){
                                        AVD avd2;
                                        inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                        fseek(file,inicioA,SEEK_SET);
                                        fread(&avd2,sizeof (AVD),1,file);
                                        avd2.avd_ap_array_subdirectorios[contador]=posicionBitmapA;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&avd2,sizeof (AVD),1,file);
                                        padre=posicionBitmapA;
                                    }
                                    AVD avd3;
                                    clean(avd3.avd_proper,25);
                                    clean(avd3.avd_fecha_creacion,20);
                                    clean(avd3.avd_nombre_directorio,25);
                                    avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd3.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd3.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle3;
                                    detalle3.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd3.avd_proper,cookie.name);
                                    strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd3,sizeof (AVD),1,file);

                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle3,sizeof (DD),1,file);
                                }
                            }
                        }
                    }
                }
                else
                {
                    int count=0;
                    foreach (QString linea,splitel) {
                        if(linea!=splitel.first()){

                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            contador=0;
                            encontrado=false;
                            while(arbol.avd_ap_array_subdirectorios[contador]!=-1)
                            {

                                inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                                fseek(file,inicio,SEEK_SET);
                                fread(&hijo,sizeof (AVD),1,file);
                                if(strcmp(hijo.avd_nombre_directorio,linea.toUtf8())==0)
                                {
                                    encontrado=true;
                                    padre=arbol.avd_ap_array_subdirectorios[contador];
                                    break;
                                }
                                contador++;
                                directorio=padre;
                                if(contador>5){
                                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                                        padre=arbol.avd_ap_arbol_virtual_directorio;
                                        inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                                        fseek(file,inicio,SEEK_SET);
                                        fread(&arbol,sizeof (AVD),1,file);
                                    }
                                    else{
                                        posicionAvd=-1;
                                        encontrado=false;
                                        break;
                                    }
                                    contador=0;
                                }
                                posicionAvd=contador;
                            }
                            if(count!=splitel.size()-1){
                                if(!encontrado){
                                    cout<<"Error:\"La carpeta "+linea.toStdString()+" no se encuentra en el sistema de files\"";
                                    break;
                                }
                                else{
                                    posicionBitmapA=padre;
                                    inicio=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicio,SEEK_SET);
                                    fread(&arbol,sizeof (AVD),1,file);
                                    posicionBitmapD=arbol.avd_ap_detalle_directorio;
                                }
                            }

                        }
                        if(count==splitel.size()-1){
                            inicioA=sb.sb_ap_bitmap_arbol_directorio;
                            fseek(file,inicioA,SEEK_SET);
                            fread(&valor,sizeof (char),1,file);
                            int i=0;
                            posicionBitmapA=-1;
                            posicionBitmapD=-1;
                            while(posicionBitmapA==-1){
                                inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                fseek(file,inicioA,SEEK_SET);
                                fread(&valor,sizeof (char),1,file);
                                if(valor==0){
                                    posicionBitmapA=i;
                                    valor=1;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&valor,sizeof (char),1,file);
                                }
                                i++;
                            }
                            i=0;
                            while(posicionBitmapD==-1){
                                inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                fseek(file,inicioD,SEEK_SET);
                                fread(&valor,sizeof (char),1,file);
                                if(valor==0){
                                    posicionBitmapD=i;
                                    valor=1;
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&valor,sizeof (char),1,file);
                                }
                                i++;
                            }
                            if(posicionAvd==-1){
                                posicionAvd=-2;
                                inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                fseek(file,inicio2,SEEK_SET);
                                fread(&arbol,sizeof (AVD),1,file);
                                //Limpieza del nuevo arbol y detalle de directorio
                                AVD avd1;
                                AVD avd2;
                                clean(avd2.avd_proper,25);
                                clean(avd2.avd_fecha_creacion,20);
                                clean(avd2.avd_nombre_directorio,25);
                                avd2.avd_ap_detalle_directorio=posicionBitmapD;
                                avd2.avd_ap_arbol_virtual_directorio=-1;
                                for (int a=0;a<6;a++) {
                                    avd2.avd_ap_array_subdirectorios[a]=-1;
                                }
                                DD detalle2;
                                detalle2.dd_ap_detalle_directorio=-1;
                                for (int a=0;a<5;a++) {
                                    detalle2.dd_array_files[a].dd_file_ap_inodo=-1;
                                    clean(detalle2.dd_array_files[a].dd_file_nombre,25);
                                    clean(detalle2.dd_array_files[a].dd_file_date_creacion, 20);
                                    clean(detalle2.dd_array_files[a].dd_file_date_modificacion,20);
                                }
                                //Fin de limpieza
                                //Llenado del arbol
                                strcpy(avd2.avd_proper,cookie.name);
                                strcpy(avd2.avd_fecha_creacion,get_current_time().c_str());
                                strcpy(avd2.avd_nombre_directorio,arbol.avd_nombre_directorio);

                                inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                fseek(file,inicioA,SEEK_SET);
                                fread(&avd1,sizeof (AVD),1,file);
                                avd1.avd_ap_arbol_virtual_directorio=posicionBitmapA;
                                fseek(file,inicioA,SEEK_SET);
                                fwrite(&avd1,sizeof (AVD),1,file);
                                inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                padre=posicionBitmapA;
                                directorio=posicionBitmapA;
                                fseek(file,inicioA,SEEK_SET);
                                fwrite(&avd2,sizeof (AVD),1,file);

                                inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                fseek(file,inicioD,SEEK_SET);
                                fwrite(&detalle2,sizeof (DD),1,file);

                                posicionBitmapA=-1;
                                posicionBitmapD=-1;
                                while(posicionBitmapA==-1){
                                    inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapA=i;
                                        valor=1;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                if(posicionBitmapA!=-1){
                                    inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    avd2.avd_ap_array_subdirectorios[0]=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd2,sizeof (AVD),1,file);
                                }
                                i=0;
                                while(posicionBitmapD==-1){
                                    inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapD=i;
                                        valor=1;
                                        fseek(file,inicioD,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                //Limpieza del nuevo arbol y detalle de directorio
                                AVD avd3;
                                clean(avd3.avd_proper,25);
                                clean(avd3.avd_fecha_creacion,20);
                                clean(avd3.avd_nombre_directorio,25);
                                avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                avd3.avd_ap_arbol_virtual_directorio=-1;
                                for (int a=0;a<6;a++) {
                                    avd3.avd_ap_array_subdirectorios[a]=-1;
                                }
                                DD detalle3;
                                detalle3.dd_ap_detalle_directorio=-1;
                                for (int a=0;a<5;a++) {
                                    detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                    clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                    clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                    clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                }
                                //Fin de limpieza
                                //Llenado del arbol
                                strcpy(avd3.avd_proper,cookie.name);
                                strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                fseek(file,inicioA,SEEK_SET);
                                fwrite(&avd3,sizeof (AVD),1,file);
                                padre=posicionBitmapA;
                                inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                fseek(file,inicioD,SEEK_SET);
                                fwrite(&detalle3,sizeof (DD),1,file);

                            }
                            else{//Investigar aqui

                                cout<<posicionBitmapA<<endl;
                                cout<<padre<<endl;
                                cout<<posicionBitmapD<<endl;
                                cout<<contador<<endl;
                                //Limpieza del nuevo arbol y detalle de directorio
                                if(posicionBitmapA!=-1){
                                    AVD avd2;
                                    inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&avd2,sizeof (AVD),1,file);
                                    avd2.avd_ap_array_subdirectorios[contador]=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd2,sizeof (AVD),1,file);
                                    padre=posicionBitmapA;
                                }
                                AVD avd3;
                                clean(avd3.avd_proper,25);
                                clean(avd3.avd_fecha_creacion,20);
                                clean(avd3.avd_nombre_directorio,25);
                                avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                avd3.avd_ap_arbol_virtual_directorio=-1;
                                for (int a=0;a<6;a++) {
                                    avd3.avd_ap_array_subdirectorios[a]=-1;
                                }
                                DD detalle3;
                                detalle3.dd_ap_detalle_directorio=-1;
                                for (int a=0;a<5;a++) {
                                    detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                    clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                    clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                    clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                }
                                //Fin de limpieza
                                //Llenado del arbol
                                strcpy(avd3.avd_proper,cookie.name);
                                strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                fseek(file,inicioA,SEEK_SET);
                                fwrite(&avd3,sizeof (AVD),1,file);

                                inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                fseek(file,inicioD,SEEK_SET);
                                fwrite(&detalle3,sizeof (DD),1,file);

                            }
                        }

                        count++;
                    }
                }

            }
            else{
                cout<<"Error: \"El nombre de la particion no concuerda con el montado\" "<<endl;
                return;
            }
            if(data.type != 'R')
            {

                Bitacora Log;
                //Limpieza
                int inicioLog=0;
                clean(Log.log_fecha,20);
                clean(Log.log_nombre,100);
                Log.log_tipo_operacion='\0';
                Log.log_tipo='\0';
                Log.log_contenido='\0';
                inicioLog=sb.sb_ap_log;
                for (int c=0;c<NumStructs;c++) {
                    fseek(file,(inicioLog),SEEK_SET);
                    fread(&Log,sizeof (Bitacora),1,file);
                    if(Log.log_tipo=='N')
                    {
                        strcpy(Log.log_fecha,get_current_time().c_str());
                        Log.log_tipo='D';
                        Log.log_contenido=1;
                        strcpy(Log.log_nombre,QString(data.path).toStdString().c_str());
                        Log.log_tipo_operacion='C';
                        fseek(file,(inicioLog),SEEK_SET);
                        fwrite(&Log,sizeof (Bitacora),1,file);
                        break;
                    }
                    inicioLog+=sizeof (Bitacora);
                }
                sb.sb_first_free_bit_arbol_directorio=getAVDFree(sb,disk_path.c_str());
                sb.sb_first_free_bit_detalle_directorio=getDirectorioFree(sb,disk_path.c_str());
                sb.sb_first_free_bit_tabla_inodo=getInodoFree(sb,disk_path.c_str());
                sb.sb_first_free_bit_bloques=getBloqueFree(sb,disk_path.c_str());
                printf("*******Directorio '%s' creado correctamente*********",data.path);
            }
            strcpy(sb.sb_date_ultimo_montaje,get_current_time().c_str());
            fseek(file,mbr.partitions[posicion].start,SEEK_SET);
            fwrite(&sb,sizeof (SuperBloque),1,file);
        }
        fclose(file);
        //de lo contrario esta adentro del recovery y no se crea log
    }
}

//complemento para el recovery
void restore_log(Bitacora log,FILE *file, int start, char id[4])
{
    Bitacora log_pointer = log;
    int start_pointer = start;
    Metadata recovery_data;
    while(log_pointer.log_tipo != 'N')
    {
        switch (log_pointer.log_tipo_operacion)
        {
        //creacion
        case 'C':
            //si es un file o un directorio
            if(log_pointer.log_tipo == 'A')
            {
                //Mandar path en el.dsk si tenia contenido y fecha
                strcpy(recovery_data.id,id);
                strcpy(recovery_data.path,log.log_nombre);
                recovery_data.type = 'A';
                recovery_data.p_dir = 1;
                disk command;
                command.makeDir(recovery_data);
                printf("\nSe recupera el file con fecha de creacion :'%s'\n",log.log_fecha);
                printf("Ruta virtual: '%s'\n",log_pointer.log_nombre);
            }
            else if(log_pointer.log_tipo == 'D')
            {

                //Mandar path en el.dsk si tenia contenido y fecha
                strcpy(recovery_data.id,id);
                strcpy(recovery_data.path,log.log_nombre);
                recovery_data.type = 'R';
                recovery_data.p_dir = 1;
                disk command;
                command.makeDir(recovery_data);
                printf("\nSe recupera el directorio con fecha de creacion :'%s'\n",log.log_fecha);
                printf("Ruta virtual: '%s'\n",log_pointer.log_nombre);
            }
            break;
            //eliminacion
        case 'E':
            //si es un file o un directorio
            if(log_pointer.log_tipo == 'A')
            {
                //Mandar path en el .dsk, si tenia contenido y fecha
            }
            else if(log_pointer.log_tipo == 'D')
            {
                //Mandar path en el.dsk si tenia contenido y fecha
            }
            break;
            //modificaion (nombre)
        case 'M':
            //si es un file o un directorio
            if(log_pointer.log_tipo == 'A')
            {
                //Mandar path en el .dsk, si tenia contenido y fecha
            }
            else if(log_pointer.log_tipo == 'D')
            {
                //Mandar path en el.dsk si tenia contenido y fecha
            }
            break;
        }
        //seguir con la operacion siguiente
        start_pointer += static_cast<int>(sizeof (Bitacora));
        //start tiene el index del log actual y se pasa al siguiente
        fseek(file,start_pointer,SEEK_SET);
        //obtiene el nuevo log
        Bitacora temp_log;
        fread(&temp_log,sizeof (Bitacora),1,file);
        log_pointer = temp_log;
    }
}

void disk::recovery(Metadata data)
{
    if(!is_mounted_partition(data.id))
    {//si la particion no esta montada
        printf("\n**********Error al restaurar, la particion '%s' no esta montada*************", data.id);
    }
    else
    {//si la particion esta montada
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        { //si existe el file
            //Lee el mbr
            Mbr mbr;
            fseek(file,0,SEEK_SET);
            fread(&mbr, sizeof(Mbr), 1, file);
            //Integers que guardan el size de la particion y su nombre
            int number_of_structures = 0;
            std::string current_partition_name;
            int current_partition_start = -1;
            //Para buscar si la particion es primaria o secundaria
            int primary_partition_index=-1;
            //verificar si la particion es primaria
            for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                /*if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,mbr.partitions[z].name))//si encuentra el nombre
                {
                    primary_partition_index=z;//indica la posicion en las primarias
                    break;
                }*/
            }
            if(primary_partition_index==-1)
            {//La particion es extendida o logica
                //Se procedera a buscar la particion logica
                int extended_partitions_index = -1;
                for(int z=0;z<4;z++){ //recorre el arreglo de partitions primarias
                    if(mbr.partitions[z].type=='E'||mbr.partitions[z].type=='e')//si encuentra Extendida(ya que solo hay una)
                    {
                        extended_partitions_index=z;
                    }
                }
                //se encontro una particion extendida
                if(extended_partitions_index==-1)
                {
                    printf("\n************Error, no se puede encontrar una particion extendida**************\n");
                }
                else
                {//existe la particion Extendida
                    //activador para ver si existe particion logica
                    int logic_partitions_index = -1;
                    int inicio=-1;
                    //para guatdar el ebr
                    Ebr ebr;
                    int pointer=mbr.partitions[extended_partitions_index].start;
                    //leyendo el ebr
                    file= fopen(disk_path.c_str(), "rb+");
                    fseek(file,pointer,SEEK_SET);
                    fread(&ebr, sizeof(Ebr), 1, file);
                    //puntero en el que se guarda el proximo ebr
                    int next=ebr.next;
                    inicio=sizeof(Ebr);
                    //para llevar el numero de particiones logicas
                    int logic_partitions_number=0;
                    int espaciolibre=mbr.partitions[extended_partitions_index].size;
                    espaciolibre-=32;
                    do{
                        //si tiene un ebr siguiente
                        if(ebr.next!=-1){
                            //avanza el puntero toda la estructura del ebr
                            pointer+=sizeof(Ebr);
                            pointer+=ebr.size;
                            //lee el proximo ebr
                            fseek(file,pointer,SEEK_SET);
                            fread(&ebr, sizeof(Ebr), 1, file);
                            //se traslada al proximo ebr y se aumenta el logic_partitions_number
                            next=ebr.next;
                            logic_partitions_number++;
                        }
                        //hace esto 500 veces si los ebr no se terminan
                        logic_partitions_number++;
                    }while(next!=-1 && logic_partitions_number <= 500);
                    //un array con el numero de particiones logicas
                    Ebr index[logic_partitions_number+1];
                    logic_partitions_number=0;
                    //guardando el primer ebr
                    pointer=mbr.partitions[extended_partitions_index].start;
                    fseek(file,pointer,SEEK_SET);
                    fread(&index[logic_partitions_number], sizeof(Ebr), 1, file);
                    next = -1;
                    do{
                        //iterando todas las logicas
                        if(index[logic_partitions_number].next!=-1)
                        {
                            pointer+=sizeof(Ebr);
                            pointer+=index[logic_partitions_number].size;
                            fseek(file,pointer,SEEK_SET);
                            fread(&index[logic_partitions_number+1], sizeof(Ebr), 1, file);
                            next=index[logic_partitions_number].next;
                        }
                        logic_partitions_number++;
                    }while(next!=-1);
                    //para el numero de particiones logicas si su start existe
                    for(int i=0;i<logic_partitions_number;i++){
                        if(index[i].start!=-1 ){//&& index[i].status!='0'
                            //ver si el nombre coincide y si esta montada
                            /*if(!strcmp(mounted_partitions[QString(data.id).toUpper()].name,index[i].name) && index[i].status!='0')
                            {
                                //Seteando el index en la particion extendida de la particion logica
                                logic_partitions_index=i;
                            }*/
                        }
                        //Seteando el nombre he inicio de la particion logica
                        current_partition_name=index[logic_partitions_index].name;
                        current_partition_start = index[logic_partitions_index].start;
                    }
                    if(logic_partitions_index==-1)
                    {
                        printf("\n************Error, particion no es primaria ni logica**************\n");
                        return;
                    }
                    else
                    {// si existe en las logicas
                        number_of_structures=file_system::structures_number(index[logic_partitions_index].size);
                    }
                }
            }
            else
            {//existe en las primarias
                number_of_structures=file_system::structures_number(mbr.partitions[primary_partition_index].size);

                current_partition_start = mbr.partitions[primary_partition_index].start;
            }
            //Empezar a hacer el recovery
            //Leer la copia del superbloque
            SuperBloque super;
            file= fopen(disk_path.c_str(), "rb+");
            //calculando el lugar donde permanece la copia del superbloque
            int iniciocopia=current_partition_start+static_cast<int>(sizeof (SuperBloque))
                    + number_of_structures
                    + (number_of_structures*static_cast<int>(sizeof (AVD)))
                    + number_of_structures
                    + (number_of_structures*static_cast<int>(sizeof (DD)))
                    + (5 * number_of_structures)
                    + (5*number_of_structures*static_cast<int>(sizeof (iNodo)))
                    + (20*number_of_structures)
                    + (20*number_of_structures*static_cast<int>(sizeof (Bloque)))
                    + (number_of_structures*static_cast<int>(sizeof(Bitacora)))
                    + 10; //bits delimitadores
            fseek(file,iniciocopia,SEEK_SET);//puntero al inicio de la copia del superbloque
            fread(&super, sizeof(SuperBloque), 1, file);// escribiendo el Superbloque

            //regenerar el super bloque
            fseek(file,current_partition_start,SEEK_SET);
            fwrite(&super,sizeof (SuperBloque),1,file);

            //obtener la bitacora
            Bitacora log;
            fseek(file,super.sb_ap_log,SEEK_SET);
            fread(&log,sizeof (Bitacora),1,file);

            restore_log(log,file,super.sb_ap_log,data.id);
            //Iterar cada registro en la bitacora
            printf("\n**********Se ha restaurado la particion con exito**********\n");
        }
    }
}

void disk::makeFile(Metadata data)
{
    bool encontrado=false;
    bool p=true;
    QString size="";
    QString insercion="";
    QStringList splitel;
    SuperBloque sb;
    AVD arbol;
    DD detalle;
    iNodo inode;
    Bloque block;
    int cantidadB=0;
    int cantidadI=0;
    int cantidadD=0;
    int cantidadC=0;
    int contador=0;
    int padre=0;
    int directorio=-1;
    int posicionAvd=-2;
    int posicionBitmapA=-1;
    int posicionBitmapD=-1;
    int posicionDetalle=-1;
    int inicio=0;
    int inicio2=0;
    int inicioA=0;
    int inicioD=0;
    int inicioI=0;
    int inicioB=0;
    int NumStructs=0;
    char valor=2;
    int necesario=0;
    QQueue<int> colaBloques;
    QQueue<int> colaBloquesA;
    QQueue<int> colaInodos;
    int posicion=-1;
    char tipoA;
    if(data.size != 0)
        tipoA = 'S';
    else tipoA = 'C';


    if(mounted_partitions.contains(QString(data.id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        {
            //leyendo el mbr
            fseek(file,0,SEEK_SET);
            Mbr mbr;
            fread(&mbr,sizeof (Mbr),1,file);

            for (int i=0;i<4;i++) {
                /*if(strcmp(mbr.partitions[i].name,mounted_partitions[QString(data.id).toUpper()].name)==0){
                    posicion=i;
                }*/
            }

            fix_path(data.path,500);

            if(posicion!=-1){
                fseek(file,mbr.partitions[posicion].start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);
                NumStructs=file_system::structures_number(mbr.partitions[posicion].size);
                fseek(file,sb.sb_ap_arbol_directorio,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                inicio=sb.sb_ap_arbol_directorio;
                splitel=QString(data.path).split("/");
                AVD hijo;
                if(p){
                    foreach(QString linea,splitel){
                        if (linea==splitel.last()) {
                            directorio=0;
                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            inicio=sb.sb_ap_detalle_directorio + (arbol.avd_ap_detalle_directorio*sizeof (DD));
                            posicionDetalle=arbol.avd_ap_detalle_directorio;
                            fseek(file,inicio,SEEK_SET);
                            fread(&detalle,sizeof (DD),1,file);
                            contador=0;
                            encontrado=false;
                            while(detalle.dd_array_files[contador].dd_file_ap_inodo!=-1){
                                if(strcmp(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str())==0){
                                    encontrado=true;
                                    break;
                                }
                                contador++;
                                if(contador==5){
                                    contador=0;
                                    if(detalle.dd_ap_detalle_directorio!=-1){
                                        posicionDetalle=detalle.dd_ap_detalle_directorio;
                                        inicioD=sb.sb_ap_detalle_directorio+(detalle.dd_ap_detalle_directorio*sizeof (DD));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&detalle,sizeof (DD),1,file);

                                    }
                                    else{
                                        directorio=-1;
                                        break;
                                    }

                                }
                            }
                            if(encontrado){
                                cout<<"El file a crear ya existe por lo que se reescribira la nueva informacion"<<endl;
                            }
                            if(directorio==-1){
                                int i=0;
                                posicionBitmapD=-1;
                                valor=2;
                                cantidadD++;
                                while(posicionBitmapD==-1){
                                    inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapD=i;
                                        valor=1;
                                        fseek(file,inicioD,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                //Limpieza de datos
                                detalle.dd_ap_detalle_directorio=posicionBitmapD;

                                inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                fseek(file,inicio,SEEK_SET);
                                fwrite(&detalle,sizeof (DD),1,file);
                                DD detalle2;
                                detalle2.dd_ap_detalle_directorio=-1;
                                for (int a=0;a<5;a++) {
                                    detalle2.dd_array_files[a].dd_file_ap_inodo=-1;
                                    clean(detalle2.dd_array_files[a].dd_file_nombre,25);
                                    clean(detalle2.dd_array_files[a].dd_file_date_creacion, 20);
                                    clean(detalle2.dd_array_files[a].dd_file_date_modificacion,20);
                                }
                                //Llenar datos
                                strcpy(detalle2.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                strcpy(detalle2.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                fseek(file,inicio,SEEK_SET);
                                fwrite(&detalle2,sizeof (DD),1,file);
                                /************************************/
                                //Nombre creado
                                for (int b=0;b<25;b++) {
                                    cout<<detalle2.dd_array_files[0].dd_file_nombre[b];
                                }
                                cout<<endl;
                                /***************************************/
                                i=0;
                                if(tipoA=='S'){
                                    necesario=get_block_count(size.toInt());
                                    cantidadB+=necesario;
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle2,sizeof (DD),1,file);
                                    /************************************/
                                    //Nombre creado
                                    detalle2.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=size.toInt();
                                        colaInodos.dequeue();
                                        i=0;
                                        char abcd[size.toInt()];
                                        clean(abcd,size.toInt());
                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);
                                                Abecedario(abcd,size.toInt());
                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<size.toInt()){

                                                        block.db_data[a]=abcd[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                                else if(tipoA=='C'){
                                    necesario=get_block_count(data.content.size());
                                    cantidadB+=necesario;
                                    data.content.replace("\"","");
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario)
                                    {
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    /************************************/
                                    //Nombre creado
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle2,sizeof (DD),1,file);
                                    detalle2.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=data.content.size();
                                        colaInodos.dequeue();
                                        i=0;

                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);

                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<data.content.size()){

                                                        block.db_data[a]=data.content.toStdString().c_str()[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                            }
                            else{
                                int i=0;
                                if(tipoA=='S'){
                                    necesario=get_block_count(size.toInt());
                                    cantidadB+=necesario;
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionDetalle*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle,sizeof (DD),1,file);
                                    //Version 2.0
                                    detalle.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    strcpy(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                    strcpy(detalle.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                    inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=size.toInt();
                                        colaInodos.dequeue();
                                        i=0;
                                        char abcd[size.toInt()];
                                        clean(abcd,size.toInt());
                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);
                                                Abecedario(abcd,size.toInt());
                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<size.toInt()){

                                                        block.db_data[a]=abcd[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                                else if(tipoA=='C'){
                                    necesario=get_block_count(data.content.size());
                                    cantidadB+=necesario;
                                    data.content.replace("\"","");
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionDetalle*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle,sizeof (DD),1,file);
                                    //Version 2.0
                                    detalle.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    strcpy(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                    strcpy(detalle.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                    inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=data.content.size();
                                        colaInodos.dequeue();
                                        i=0;

                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);

                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<data.content.size()){

                                                        block.db_data[a]=data.content.toStdString().c_str()[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                            }
                        }
                        else if(linea!=splitel.first()){
                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            contador=0;
                            while(arbol.avd_ap_array_subdirectorios[contador]!=-1){
                                inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                                fseek(file,inicio,SEEK_SET);
                                fread(&hijo,sizeof (AVD),1,file);
                                if(strcmp(hijo.avd_nombre_directorio,linea.toStdString().c_str())==0){
                                    encontrado=true;
                                    padre=arbol.avd_ap_array_subdirectorios[contador];
                                    break;
                                }
                                contador++;
                                directorio=padre;
                                if(contador>5){
                                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                                        padre=arbol.avd_ap_arbol_virtual_directorio;
                                        inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                                        fseek(file,inicio,SEEK_SET);
                                        fread(&arbol,sizeof (AVD),1,file);

                                    }
                                    else{
                                        posicionAvd=-1;
                                        encontrado=false;
                                        break;
                                    }
                                    contador=0;
                                }
                                posicionAvd=contador;

                            }
                            if(!encontrado){
                                cantidadD++;
                                cantidadC++;
                                inicioA=sb.sb_ap_bitmap_arbol_directorio;
                                fseek(file,inicioA,SEEK_SET);
                                fread(&valor,sizeof (char),1,file);
                                int i=0;
                                posicionBitmapA=-1;
                                posicionBitmapD=-1;
                                while(posicionBitmapA==-1){
                                    inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapA=i;
                                        valor=1;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                i=0;
                                while(posicionBitmapD==-1){
                                    inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapD=i;
                                        valor=1;
                                        fseek(file,inicioD,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                if(posicionAvd==-1){
                                    posicionAvd=-2;
                                    inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicio2,SEEK_SET);
                                    fread(&arbol,sizeof (AVD),1,file);
                                    //Limpieza del nuevo arbol y detalle de directorio
                                    AVD avd1;
                                    AVD avd2;
                                    clean(avd2.avd_proper,25);
                                    clean(avd2.avd_fecha_creacion,20);
                                    clean(avd2.avd_nombre_directorio,25);
                                    avd2.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd2.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd2.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle2;
                                    detalle2.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle2.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle2.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle2.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle2.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd2.avd_proper,cookie.name);
                                    strcpy(avd2.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd2.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd2.avd_nombre_directorio,arbol.avd_nombre_directorio);

                                    inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fread(&avd1,sizeof (AVD),1,file);
                                    avd1.avd_ap_arbol_virtual_directorio=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd1,sizeof (AVD),1,file);
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    padre=posicionBitmapA;
                                    directorio=posicionBitmapA;
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd2,sizeof (AVD),1,file);

                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);

                                    posicionBitmapA=-1;
                                    posicionBitmapD=-1;
                                    while(posicionBitmapA==-1){
                                        inicioA=sb.sb_ap_bitmap_arbol_directorio+(i*sizeof (char));
                                        fseek(file,inicioA,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            posicionBitmapA=i;
                                            valor=1;
                                            fseek(file,inicioA,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                        }
                                        i++;
                                    }
                                    if(posicionBitmapA!=-1){
                                        inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                        avd2.avd_ap_array_subdirectorios[0]=posicionBitmapA;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&avd2,sizeof (AVD),1,file);
                                    }
                                    i=0;
                                    while(posicionBitmapD==-1){
                                        inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            posicionBitmapD=i;
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                        }
                                        i++;
                                    }
                                    //Limpieza del nuevo arbol y detalle de directorio
                                    AVD avd3;
                                    clean(avd3.avd_proper,25);
                                    clean(avd3.avd_fecha_creacion,20);
                                    clean(avd3.avd_nombre_directorio,25);
                                    avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd3.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd3.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle3;
                                    detalle3.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd3.avd_proper,cookie.name);
                                    strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd3,sizeof (AVD),1,file);
                                    padre=posicionBitmapA;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle3,sizeof (DD),1,file);

                                }
                                else{//Investigar aqui

                                    if(posicionBitmapA!=-1){
                                        AVD avd2;
                                        inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                        fseek(file,inicioA,SEEK_SET);
                                        fread(&avd2,sizeof (AVD),1,file);
                                        avd2.avd_ap_array_subdirectorios[contador]=posicionBitmapA;
                                        fseek(file,inicioA,SEEK_SET);
                                        fwrite(&avd2,sizeof (AVD),1,file);
                                        padre=posicionBitmapA;
                                    }
                                    AVD avd3;
                                    clean(avd3.avd_proper,25);
                                    clean(avd3.avd_fecha_creacion,20);
                                    clean(avd3.avd_nombre_directorio,25);
                                    avd3.avd_ap_detalle_directorio=posicionBitmapD;
                                    avd3.avd_ap_arbol_virtual_directorio=-1;
                                    for (int a=0;a<6;a++) {
                                        avd3.avd_ap_array_subdirectorios[a]=-1;
                                    }
                                    DD detalle3;
                                    detalle3.dd_ap_detalle_directorio=-1;
                                    for (int a=0;a<5;a++) {
                                        detalle3.dd_array_files[a].dd_file_ap_inodo=-1;
                                        clean(detalle3.dd_array_files[a].dd_file_nombre,25);
                                        clean(detalle3.dd_array_files[a].dd_file_date_creacion, 20);
                                        clean(detalle3.dd_array_files[a].dd_file_date_modificacion,20);
                                    }
                                    //Fin de limpieza
                                    //Llenado del arbol
                                    strcpy(avd3.avd_proper,cookie.name);
                                    strcpy(avd3.avd_fecha_creacion,get_current_time().c_str());
                                    strcpy(avd3.avd_nombre_directorio,linea.toStdString().c_str());
                                    inicioA=sb.sb_ap_arbol_directorio+(posicionBitmapA*sizeof (AVD));
                                    fseek(file,inicioA,SEEK_SET);
                                    fwrite(&avd3,sizeof (AVD),1,file);

                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fwrite(&detalle3,sizeof (DD),1,file);

                                }
                            }
                        }
                    }
                }
                else {
                    padre=0;
                    foreach(QString linea,splitel){
                        contador=0;
                        if (linea==splitel.last()) {
                            directorio=0;
                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            inicio=sb.sb_ap_detalle_directorio + (arbol.avd_ap_detalle_directorio*sizeof (DD));
                            posicionDetalle=arbol.avd_ap_detalle_directorio;
                            fseek(file,inicio,SEEK_SET);
                            fread(&detalle,sizeof (DD),1,file);
                            contador=0;
                            encontrado=false;
                            while(detalle.dd_array_files[contador].dd_file_ap_inodo!=-1){
                                /*******************/
                                if(strcmp(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str())==0){
                                    encontrado=true;
                                    break;
                                }
                                contador++;
                                if(contador==5){
                                    contador=0;
                                    if(detalle.dd_ap_detalle_directorio!=-1){
                                        posicionDetalle=detalle.dd_ap_detalle_directorio;
                                        inicioD=sb.sb_ap_detalle_directorio+(detalle.dd_ap_detalle_directorio*sizeof (DD));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&detalle,sizeof (DD),1,file);

                                    }
                                    else{
                                        directorio=-1;
                                        break;
                                    }

                                }
                            }
                            if(encontrado){
                                cout<<"El file a crear ya existe por lo que se reescribira la nueva informacion"<<endl;
                            }
                            if(directorio==-1){
                                cantidadD++;
                                int i=0;
                                posicionBitmapD=-1;
                                valor=2;
                                while(posicionBitmapD==-1){
                                    inicioD=sb.sb_ap_bitmap_detalle_directorio+(i*sizeof (char));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&valor,sizeof (char),1,file);
                                    if(valor==0){
                                        posicionBitmapD=i;
                                        valor=1;
                                        fseek(file,inicioD,SEEK_SET);
                                        fwrite(&valor,sizeof (char),1,file);
                                    }
                                    i++;
                                }
                                //Limpieza de datos
                                detalle.dd_ap_detalle_directorio=posicionBitmapD;

                                inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                fseek(file,inicio,SEEK_SET);
                                fwrite(&detalle,sizeof (DD),1,file);
                                DD detalle2;
                                detalle2.dd_ap_detalle_directorio=-1;
                                for (int a=0;a<5;a++) {
                                    detalle2.dd_array_files[a].dd_file_ap_inodo=-1;
                                    clean(detalle2.dd_array_files[a].dd_file_nombre,25);
                                    clean(detalle2.dd_array_files[a].dd_file_date_creacion, 20);
                                    clean(detalle2.dd_array_files[a].dd_file_date_modificacion,20);
                                }
                                //Llenar datos
                                strcpy(detalle2.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                strcpy(detalle2.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                fseek(file,inicio,SEEK_SET);
                                fwrite(&detalle2,sizeof (DD),1,file);
                                /************************************/
                                //Nombre creado
                                cout<<"Nombre creado "<<endl;
                                for (int b=0;b<25;b++) {
                                    cout<<detalle2.dd_array_files[0].dd_file_nombre[b];
                                }
                                cout<<endl;
                                /***************************************/
                                i=0;
                                if(tipoA=='S'){
                                    necesario=get_block_count(size.toInt());
                                    cantidadB+=necesario;
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle2,sizeof (DD),1,file);
                                    /************************************/
                                    //Nombre creado
                                    detalle2.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=size.toInt();
                                        colaInodos.dequeue();
                                        i=0;
                                        char abcd[size.toInt()];
                                        clean(abcd,size.toInt());
                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);
                                                Abecedario(abcd,size.toInt());
                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<size.toInt()){

                                                        block.db_data[a]=abcd[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                                else if(tipoA=='C'){
                                    necesario=get_block_count(data.content.size());
                                    cantidadB+=necesario;
                                    data.content.replace("\"","");
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    /************************************/
                                    //Nombre creado
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionBitmapD*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle2,sizeof (DD),1,file);
                                    detalle2.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    inicio=sb.sb_ap_detalle_directorio + (posicionBitmapD*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle2,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=data.content.size();
                                        colaInodos.dequeue();
                                        i=0;

                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);

                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<data.content.size()){

                                                        block.db_data[a]=data.content.toStdString().c_str()[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                            }
                            else{
                                int i=0;
                                if(tipoA=='S'){
                                    necesario=get_block_count(size.toInt());
                                    cantidadB+=necesario;
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionDetalle*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle,sizeof (DD),1,file);
                                    //Version 2.0
                                    detalle.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    strcpy(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                    strcpy(detalle.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                    inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=size.toInt();
                                        colaInodos.dequeue();
                                        i=0;
                                        char abcd[size.toInt()];
                                        clean(abcd,size.toInt());
                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);
                                                Abecedario(abcd,size.toInt());
                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<size.toInt()){

                                                        block.db_data[a]=abcd[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                                else if(tipoA=='C'){
                                    necesario=get_block_count(data.content.size());
                                    cantidadB+=necesario;
                                    data.content.replace("\"","");
                                    i=0;
                                    int cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_bloques+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaBloques.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    necesario=getCantidadInodos(colaBloques.size());
                                    cantidadI+=necesario;
                                    i=0;
                                    cantidad=0;
                                    while(cantidad<necesario){
                                        inicioD=sb.sb_ap_bitmap_tabla_inodo+(i*sizeof (char));
                                        fseek(file,inicioD,SEEK_SET);
                                        fread(&valor,sizeof (char),1,file);
                                        if(valor==0){
                                            colaInodos.enqueue(i);
                                            valor=1;
                                            fseek(file,inicioD,SEEK_SET);
                                            fwrite(&valor,sizeof (char),1,file);
                                            cantidad++;
                                        }
                                        i++;
                                    }
                                    necesario=0;
                                    inicioD=sb.sb_ap_detalle_directorio+(posicionDetalle*sizeof (DD));
                                    fseek(file,inicioD,SEEK_SET);
                                    fread(&detalle,sizeof (DD),1,file);
                                    //Version 2.0
                                    detalle.dd_array_files[contador].dd_file_ap_inodo=colaInodos.front();
                                    strcpy(detalle.dd_array_files[contador].dd_file_nombre,linea.toStdString().c_str());
                                    strcpy(detalle.dd_array_files[contador].dd_file_date_creacion,get_current_time().c_str());
                                    inicio=sb.sb_ap_detalle_directorio + (posicionDetalle*sizeof (DD));
                                    fseek(file,inicio,SEEK_SET);
                                    fwrite(&detalle,sizeof (DD),1,file);
                                    int valorIB=0;
                                    while(!colaInodos.empty()){
                                        inicioI=sb.sb_ap_tabla_inodo+(colaInodos.front()*sizeof (iNodo));
                                        fseek(file,inicioI,SEEK_SET);
                                        fread(&inode,sizeof (iNodo),1,file);
                                        strcpy(inode.i_id_proper,cookie.name);
                                        inode.i_count_inodo=colaInodos.front();
                                        inode.i_count_bloques_asignados=colaBloques.size();
                                        inode.i_size_archivo=data.content.size();
                                        colaInodos.dequeue();
                                        i=0;

                                        while(i<4){
                                            if(!colaBloques.empty()){
                                                inode.i_array_bloques[i]=colaBloques.front();
                                                inicioB=sb.sb_ap_bloques+(colaBloques.front()*sizeof (Bloque));
                                                fseek(file,inicioB,SEEK_SET);
                                                fread(&block,sizeof (Bloque),1,file);

                                                clean(block.db_data,25);
                                                for (int a=0;a<25;a++) {
                                                    if(valorIB<data.content.size()){

                                                        block.db_data[a]=data.content.toStdString().c_str()[valorIB];
                                                    }
                                                    valorIB++;
                                                }
                                                fseek(file,inicioB,SEEK_SET);
                                                fwrite(&block,sizeof (Bloque),1,file);
                                                fseek(file,inicioI,SEEK_SET);
                                                fwrite(&inode,sizeof (iNodo),1,file);
                                                colaBloques.dequeue();
                                                fseek(file,inicioI,SEEK_SET);
                                                fread(&inode,sizeof (iNodo),1,file);
                                            }
                                            i++;
                                        }
                                        if(!colaInodos.empty()){
                                            inode.i_ap_indirecto=colaInodos.front();
                                        }
                                        fseek(file,inicioI,SEEK_SET);
                                        fwrite(&inode,sizeof (iNodo),1,file);
                                    }
                                }
                            }
                        }
                        else if(linea!=splitel.first()){
                            inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio2,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                            while(arbol.avd_ap_array_subdirectorios[contador]!=-1){

                                inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                                fseek(file,inicio,SEEK_SET);
                                fread(&hijo,sizeof (AVD),1,file);
                                if(strcmp(hijo.avd_nombre_directorio,linea.toStdString().c_str())==0){
                                    encontrado=true;
                                    padre=arbol.avd_ap_array_subdirectorios[contador];
                                    break;
                                }
                                contador++;
                                directorio=padre;
                                if(contador>5){
                                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                                        padre=arbol.avd_ap_arbol_virtual_directorio;
                                        inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                                        fseek(file,inicio,SEEK_SET);
                                        fread(&arbol,sizeof (AVD),1,file);

                                    }
                                    else{
                                        posicionAvd=-1;
                                        encontrado=false;
                                        break;
                                    }
                                    contador=0;
                                }
                                posicionAvd=contador;

                            }
                            if(!encontrado){
                                cout<<"Error:\"La carpeta "+linea.toStdString()+" no se encuentra en el sistema de files\"";
                                break;
                            }
                            else{
                                posicionBitmapA=padre;
                                inicio=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                                fseek(file,inicio,SEEK_SET);
                                fread(&arbol,sizeof (AVD),1,file);
                                posicionBitmapD=arbol.avd_ap_detalle_directorio;
                            }
                        }
                    }

                }
                if(data.type != 'R')
                {
                    //viene del semantico
                    Bitacora Log;
                    //Limpieza
                    int inicioLog=0;
                    clean(Log.log_fecha,20);
                    clean(Log.log_nombre,100);
                    Log.log_tipo_operacion='N';
                    Log.log_tipo='N';
                    Log.log_contenido='N';
                    inicioLog=sb.sb_ap_log;
                    for (int c=0;c<NumStructs;c++) {
                        fseek(file,(inicioLog),SEEK_SET);
                        fread(&Log,sizeof (Bitacora),1,file);
                        if(Log.log_tipo=='N'){
                            strcpy(Log.log_fecha,get_current_time().c_str());
                            Log.log_tipo='A';
                            Log.log_contenido=1;
                            strcpy(Log.log_nombre,QString(data.path).toStdString().c_str());
                            Log.log_tipo_operacion='C';
                            fseek(file,(inicioLog),SEEK_SET);
                            fwrite(&Log,sizeof (Bitacora),1,file);
                            break;
                        }
                        inicioLog+=sizeof (Bitacora);
                    }
                    sb.sb_inodos_free-=cantidadI;
                    sb.sb_bloques_free-=cantidadB;
                    sb.sb_inodos_count+=cantidadB;
                    sb.sb_bloques_count+=cantidadI;
                    sb.sb_first_free_bit_arbol_directorio=getAVDFree(sb,disk_path.c_str());
                    sb.sb_first_free_bit_detalle_directorio=getDirectorioFree(sb,disk_path.c_str());
                    sb.sb_first_free_bit_tabla_inodo=getInodoFree(sb,disk_path.c_str());
                    sb.sb_first_free_bit_bloques=getBloqueFree(sb,disk_path.c_str());
                    strcpy(sb.sb_date_ultimo_montaje,get_current_time().c_str());
                    fseek(file,mbr.partitions[posicion].start,SEEK_SET);
                    fwrite(&sb,sizeof (SuperBloque),1,file);
                    cantidadI=0;
                    cantidadB=0;
                    printf("**********file '%s' creado exitosamente*************",data.path);
                }
            }
        }
    }
}

//metodos para graficar luego
/*

QString getNodo(QList<int> valores, QString titulo,int correlativo,QList<QString> ddnames){
    QString res="";
    if(valores.size()==8){ // para directorios

        res="\"nodo"+QString::number(correlativo)+"\" [style=solid, label=\"{ \\n  "+titulo+"  \\n |{<0> "+QString::number(valores.takeLast())+"|<1> "+QString::number(valores.takeLast())+" |<2> "+QString::number(valores.takeLast())+"|<3> "+QString::number(valores.takeLast())+" |<4> "+QString::number(valores.takeLast())+" | <5> "+QString::number(valores.takeLast())+" |<6> "+QString::number(valores.takeLast())+" | <7> "+QString::number(valores.takeLast())+"}}\"];"+"\n";

    }else if(valores.size()==6){ // para detalledirectorio
        QString cadenanames="";
        for (int i=0;i<ddnames.size();i++) {
            cadenanames+=ddnames.at(i);
        }

        res="\"nodod"+QString::number(correlativo)+"\" [style=solid, label=\"{ DD | "+cadenanames+" |{<0> "+QString::number(valores.takeLast())+"|<1> "+QString::number(valores.takeLast())+" |<2> "+QString::number(valores.takeLast())+"|<3> "+QString::number(valores.takeLast())+" |<4> "+QString::number(valores.takeLast())+" | <5> "+QString::number(valores.takeLast())+"}}\"];"+"\n";

    }else if(valores.size()==5){ // para inodos
        res="\"nodoi"+QString::number(correlativo)+"\" [style=solid, label=\"{ Inodo |\\n "+titulo+" \\n \\n|{<0> "+QString::number(valores.takeLast())+"|<1> "+QString::number(valores.takeLast())+" |<2> "+QString::number(valores.takeLast())+"|<3> "+QString::number(valores.takeLast())+" |<4> "+QString::number(valores.takeLast())+"}}\"];"+"\n"; // Para Inodos

    }else if(valores.size()==1){
        res="\"nodob"+QString::number(correlativo)+"\" [style=solid, label=\""+titulo+"\"];"+"\n";

    }


    return res;
}

QString getPuntero(QString nodoizq, QString NodoDer, int posarreglo){

    QString apunta="\"nodo"+nodoizq+"\":"+QString::number(posarreglo)+" -- \"nodo"+NodoDer+"\"";
    apunta+="\n";
    return apunta;
}

void getTree(QString dot, QString pathsalida, QString extension){
    QString dotbody="graph mygraph{ \n"
                    "node [shape=record, fontsize=10, fontname=Arial]; \n"
                    "rankdir=TB; \n"
                    "ranksep=0.5; \n"
                    "rank=max; \n"
                    "splines=true; \n"
                    "overlap=false; \n"
                    "mindist=0.2; \n";
    dotbody+=dot;
    dotbody+="\n";
    dotbody+="}";

    //============= creacion del .dot y guardado temporal =====
    ofstream file;
    file.open("direcgraph.dot");
    string nuevo = dotbody.toUtf8().constData();
    file<<nuevo;
    file.close();


    //========== comando que llama a graphviz====
    string comando="";
    if(extension=="jpg"){
        comando.append("dot -Tjpg ");
        comando.append("/home/javig/direcgraph.dot ");
        comando.append("-o ");
        comando.append(pathsalida.toUtf8().constData());
        int res =system(comando.c_str());
        if(res==0){
            cout<<"\033[1;32m----------- Reporte Arbol de Directorio Creado Exitosamente -----\033[0m"<<endl;
        }

        // cout<<"dentro jpg"<<endl;
    }else if(extension=="pdf"){
        comando.append("dot -Tpdf ");
        comando.append("direcgraph.dot ");
        comando.append("-o ");
        comando.append(pathsalida.toUtf8().constData());
        int res =system(comando.c_str());
        if(res==0){
            cout<<"\033[1;32m----------- Reporte Arbol de Directorio Creado Exitosamente -----\033[0m"<<endl;
        }
    }else if(extension=="png"){
        comando.append("dot -Tpng ");
        comando.append("/home/javig/direcgraph.dot "); // genera
        comando.append("-o ");
        comando.append(pathsalida.toUtf8().constData());
        int res =system(comando.c_str());
        if(res==0){
            cout<<"\033[1;32m----------- Reporte Arbol de Directorio Creado Exitosamente -----\033[0m"<<endl;
        }
    }

}

void generate_tree_dd(QString id,QString pathT,QString pathSalida,QString extension)
{
    bool encontrado=false;
    QString path="";
    QString Nodos="";
    QString Apuntadores="";
    bool p=false;
    QString insercion="";
    QStringList splitel;
    FILE* file;
    SuperBloque sb;
    AVD arbol;
    AVD hijo;
    DD detalle;
    Mbr mbr2;
    QList<int> valores;
    QList<QString> files;
    QString titulo="";
    QString actual="";
    QString destino="";
    int correlativo=0;
    int contador=0;
    int padre=0;
    int padre2=0;
    int directorio=0;
    int inicio=0;
    int inicio2=0;
    int inicioA=0;
    int valor=2;
    int posicion=-1;

    if(mounted_partitions.contains(QString(id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        {
            //leyendo el mbr
            fseek(file,0,SEEK_SET);
            Mbr mbr;
            fread(&mbr,sizeof (Mbr),1,file);

            for (int i=0;i<4;i++) {
                if(strcmp(mbr.partitions[i].name,mounted_partitions[id.toUpper()].name)==0){
                    posicion=i;
                }
            }
            if(posicion!=-1){
                fseek(file,mbr.partitions[posicion].start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);

                fseek(file,sb.sb_ap_arbol_directorio,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                inicio=sb.sb_ap_arbol_directorio;
                pathT.replace("\"","");
                splitel=pathT.split("/");

                foreach (QString linea,splitel) {

                    if(linea!=splitel.first()){

                        inicio2=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                        fseek(file,inicio2,SEEK_SET);
                        fread(&arbol,sizeof (AVD),1,file);
                        contador=0;
                        encontrado=false;
                        while(arbol.avd_ap_array_subdirectorios[contador]!=-1){
                            inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&hijo,sizeof (AVD),1,file);
                            if(strcmp(hijo.avd_nombre_directorio,linea.toStdString().c_str())==0){
                                encontrado=true;
                                padre=arbol.avd_ap_array_subdirectorios[contador];

                                break;
                            }
                            contador++;
                            if(contador>5){
                                if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                                    padre=arbol.avd_ap_arbol_virtual_directorio;
                                    inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                                    fseek(file,inicio,SEEK_SET);
                                    fread(&arbol,sizeof (AVD),1,file);
                                }
                                else{
                                    encontrado=false;
                                    break;
                                }
                                contador=0;
                            }

                        }

                        if(!encontrado){
                            cout<<"Error:\"La carpeta "+linea.toStdString()+" no se encuentra en el sistema de files\""<<endl;
                            break;
                        }
                        else{
                            inicio=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                        }
                    }
                }
                padre2=padre;
                inicioA=sb.sb_ap_arbol_directorio+(padre*sizeof (AVD));
                fseek(file,inicioA,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                contador=0;
                valor=1;
                cout<<endl;
                while(arbol.avd_ap_array_subdirectorios[contador]!=-1){
                    inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                    fseek(file,inicio,SEEK_SET);
                    fread(&hijo,sizeof (AVD),1,file);
                    cout<<valor<<") ";
                    for (int i=0;i<25;i++) {
                        cout<<hijo.avd_nombre_directorio[i];
                    }
                    cout<<endl;
                    contador++;
                    valor++;
                    if(contador>5){
                        if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                            padre=arbol.avd_ap_arbol_virtual_directorio;
                            inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                        }
                        else{
                            break;
                        }
                        contador=0;
                    }

                }
                cin>>directorio;
                inicioA=sb.sb_ap_arbol_directorio+(padre2*sizeof (AVD));
                fseek(file,inicioA,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                contador=0;
                valor=0;
                while(valor<directorio){
                    correlativo=arbol.avd_ap_array_subdirectorios[contador];
                    inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_array_subdirectorios[contador]*sizeof (AVD));
                    fseek(file,inicio,SEEK_SET);
                    fread(&hijo,sizeof (AVD),1,file);
                    valor++;
                    contador++;
                    if(contador>5){
                        if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                            padre=arbol.avd_ap_arbol_virtual_directorio;
                            inicio=sb.sb_ap_arbol_directorio+(arbol.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&arbol,sizeof (AVD),1,file);
                        }
                        else{
                            break;
                        }
                        contador=0;
                    }
                }
                contador=0;
                for (int i=0;i<25;i++) {
                    if(hijo.avd_nombre_directorio[i]!='\0'){
                        titulo+=hijo.avd_nombre_directorio[i];
                    }

                }
                inicio2=hijo.avd_ap_detalle_directorio;
                while(contador<6){
                    valores.push_back(hijo.avd_ap_array_subdirectorios[contador]);

                    contador++;
                    if(contador>5){
                        valores.push_back(hijo.avd_ap_detalle_directorio);
                        if(hijo.avd_ap_detalle_directorio!=-1){
                            Apuntadores+=getPuntero(QString::number(correlativo),"d"+QString::number(hijo.avd_ap_detalle_directorio),6);
                        }

                        valores.push_back(hijo.avd_ap_arbol_virtual_directorio);
                        if(hijo.avd_ap_arbol_virtual_directorio!=-1){
                            Apuntadores+=getPuntero(QString::number(correlativo),QString::number(hijo.avd_ap_arbol_virtual_directorio),7);
                        }

                        Nodos+=getNodo(valores,titulo,correlativo,files);
                        valores.clear();
                        titulo="";
                        correlativo=-1;
                        files.clear();
                        if(hijo.avd_ap_arbol_virtual_directorio!=-1){
                            padre=hijo.avd_ap_arbol_virtual_directorio;
                            correlativo=hijo.avd_ap_arbol_virtual_directorio;

                            inicio=sb.sb_ap_arbol_directorio+(hijo.avd_ap_arbol_virtual_directorio*sizeof (AVD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&hijo,sizeof (AVD),1,file);
                            inicio=sb.sb_ap_detalle_directorio+(hijo.avd_ap_detalle_directorio*sizeof (DD));

                            fseek(file,inicio,SEEK_SET);
                            fread(&detalle,sizeof (DD),1,file);
                            valor=0;

                            titulo="";
                            while(valor<5){
                                for (int i=0;i<25;i++) {
                                    if(detalle.dd_array_files[valor].dd_file_nombre[i]!='\0'){
                                        titulo+=detalle.dd_array_files[valor].dd_file_nombre[i];
                                    }
                                }
                                titulo+="\\n";
                                files.push_back(titulo);
                                titulo="";
                                valores.push_back(detalle.dd_array_files[valor].dd_file_ap_inodo);
                                valor++;
                                if(valor>4){
                                    if(detalle.dd_ap_detalle_directorio!=-1){
                                        Apuntadores+=getPuntero("d"+QString::number(correlativo),"d"+QString::number(detalle.dd_ap_detalle_directorio),5);
                                    }
                                    valores.push_back(detalle.dd_ap_detalle_directorio);
                                    Nodos+=getNodo(valores,titulo,correlativo,files);
                                    valores.clear();
                                    titulo="";
                                    correlativo=-1;
                                    files.clear();
                                    if(detalle.dd_ap_detalle_directorio!=-1){
                                        correlativo=detalle.dd_ap_detalle_directorio;
                                        inicio=sb.sb_ap_detalle_directorio+(detalle.dd_ap_detalle_directorio*sizeof (DD));
                                        fseek(file,inicio,SEEK_SET);
                                        fread(&detalle,sizeof (DD),1,file);
                                        valor=0;
                                    }
                                    else{
                                        break;
                                    }
                                }
                            }

                            for (int i=0;i<25;i++) {
                                if(hijo.avd_nombre_directorio[i]!='\0'){
                                    titulo+=hijo.avd_nombre_directorio[i];
                                }
                            }
                            correlativo=hijo.avd_ap_detalle_directorio;
                        }
                        else{
                            break;
                        }
                        contador=0;
                    }
                }
                valores.clear();
                titulo="";
                correlativo=-1;
                files.clear();
                inicio=sb.sb_ap_detalle_directorio+(inicio2*sizeof (DD));
                correlativo=inicio2;
                fseek(file,inicio,SEEK_SET);
                fread(&detalle,sizeof (DD),1,file);
                valor=0;

                while(valor<5){
                    for (int i=0;i<25;i++) {
                        if(detalle.dd_array_files[valor].dd_file_nombre[i]!='\0'){
                            titulo+=detalle.dd_array_files[valor].dd_file_nombre[i];
                        }

                    }
                    titulo+=" \\n ";
                    files.push_back(titulo);
                    titulo="";
                    valores.push_back(detalle.dd_array_files[valor].dd_file_ap_inodo);
                    valor++;
                    if(valor>4){
                        if(detalle.dd_ap_detalle_directorio!=-1){
                            Apuntadores+=getPuntero("d"+QString::number(correlativo),"d"+QString::number(detalle.dd_ap_detalle_directorio),5);
                        }
                        valores.push_back(detalle.dd_ap_detalle_directorio);
                        Nodos+=getNodo(valores,titulo,correlativo,files);
                        valores.clear();
                        titulo="";
                        correlativo=-1;
                        files.clear();
                        if(detalle.dd_ap_detalle_directorio!=-1){
                            correlativo=detalle.dd_ap_detalle_directorio;
                            inicio=sb.sb_ap_detalle_directorio+(detalle.dd_ap_detalle_directorio*sizeof (DD));
                            fseek(file,inicio,SEEK_SET);
                            fread(&detalle,sizeof (DD),1,file);
                            valor=0;
                        }
                        else{
                            break;
                        }
                    }
                }
                fclose(file);
            }
            actual=Nodos;
            actual+=Apuntadores;
            getTree(actual,pathSalida,extension);
            actual="";
        }
    }
}

void RDirectorio(QString id,QString pathSalida,QString extension)
{
    QString path="";
    QString Nodos="";
    QString Apuntadores="";
    FILE* file;
    SuperBloque sb;
    AVD arbol;
    QList<int> valores;
    QList<QString> files;
    QString titulo="";
    QString actual="";
    QString destino="";
    int correlativo=0;
    int inicio=0;
    int posicion=-1;

    if(mounted_partitions.contains(QString(id).toUpper()))
    {
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        fseek(file,0,SEEK_SET);// estableciendo puntero al inicio
        //comprobar si no hay error al abrir el file
        if(file == nullptr)
        {
            printf("\n************Error al abrir el disco: '%s'***************\n",disk_path.c_str());
        }
        else
        {
            //leyendo el mbr
            fseek(file,0,SEEK_SET);
            Mbr mbr;
            fread(&mbr,sizeof (Mbr),1,file);

            for (int i=0;i<4;i++) {
                if(strcmp(mbr.partitions[i].name,mounted_partitions[id.toUpper()].name)==0){
                    posicion=i;
                }
            }

            if(posicion!=-1){
                files.clear();
                valores.clear();
                fseek(file,mbr.partitions[posicion].start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);
                correlativo=0;
                inicio=sb.sb_ap_arbol_directorio;
                fseek(file,inicio,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                while(arbol.avd_nombre_directorio[0]!='\0' && arbol.avd_ap_detalle_directorio!=-1){
                    for (int a=0;a<25;a++) {
                        if(arbol.avd_nombre_directorio[a]!='\0'){
                            titulo+=arbol.avd_nombre_directorio[a];
                        }
                    }
                    for (int b=0;b<6;b++) {
                        valores.push_back(arbol.avd_ap_array_subdirectorios[b]);
                        if(arbol.avd_ap_array_subdirectorios[b]!=-1){
                            Apuntadores+=getPuntero(QString::number(correlativo),QString::number(arbol.avd_ap_array_subdirectorios[b]),b);
                        }

                    }
                    valores.push_back(arbol.avd_ap_detalle_directorio);
                    valores.push_back(arbol.avd_ap_arbol_virtual_directorio);
                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                        Apuntadores+=getPuntero(QString::number(correlativo),QString::number(arbol.avd_ap_arbol_virtual_directorio),7);
                    }
                    Nodos+=getNodo(valores,titulo,correlativo,files);
                    valores.clear();
                    titulo="";
                    files.clear();
                    correlativo++;
                    inicio+=sizeof (AVD);
                    fseek(file,inicio,SEEK_SET);
                    fread(&arbol,sizeof (AVD),1,file);
                }

            }
            else {

            }
        }
        fclose(file);
    }
    else {

    }
    actual=Nodos;
    actual+=Apuntadores;
    getTree(actual,pathSalida,extension);
}

void Tree_Complete(QString id,QString pathSalida,QString extension){
    QString path="";
    QString Nodos="";
    QString Apuntadores="";
    FILE* file;
    SuperBloque sb;
    AVD arbol;
    DD detalle;
    iNodo inode;
    Bloque block;
    QList<int> valores;
    QList<QString> files;
    QString titulo="";
    QString actual="";
    QString destino="";
    int correlativo=0;
    int inicio=0;
    int posicion=-1;
    int cantidadD=0;
    int count=0;

    if(listaMount->Buscar2(id)){
        path=listaMount->Buscar3(id);
        file=fopen(path.toStdString().c_str(),"rb+");

        if(file==NULL){
            cout<<"Error al intentar ingresar al file"<<endl;
        }
        else {
            fseek(file,0,SEEK_SET);
            fread(&mbr2,sizeof (MbrDisk),1,file);

            for (int i=0;i<4;i++) {
                if(strcmp(mbr2.particiones[i].part_name,listaMount->BuscarName(id).toStdString().c_str())==0){
                    posicion=i;
                }
            }

            if(posicion!=-1){
                files.clear();
                valores.clear();
                fseek(file,mbr2.particiones[posicion].part_start,SEEK_SET);
                fread(&sb,sizeof (SuperBloque),1,file);
                correlativo=0;
                inicio=sb.sb_ap_arbol_directorio;
                fseek(file,inicio,SEEK_SET);
                fread(&arbol,sizeof (AVD),1,file);
                while(arbol.avd_nombre_directorio[0]!='\0' && arbol.avd_ap_detalle_directorio!=-1){
                    for (int a=0;a<25;a++) {
                        if(arbol.avd_nombre_directorio[a]!='\0'){
                            titulo+=arbol.avd_nombre_directorio[a];
                        }
                    }
                    for (int b=0;b<6;b++) {
                        valores.push_back(arbol.avd_ap_array_subdirectorios[b]);
                        if(arbol.avd_ap_array_subdirectorios[b]!=-1){
                            Apuntadores+=getPuntero(QString::number(correlativo),QString::number(arbol.avd_ap_array_subdirectorios[b]),b);
                        }

                    }
                    valores.push_back(arbol.avd_ap_detalle_directorio);
                    if(arbol.avd_ap_detalle_directorio!=-1){
                        Apuntadores+=getPuntero(QString::number(correlativo),"d"+QString::number(arbol.avd_ap_detalle_directorio),6);
                    }
                    valores.push_back(arbol.avd_ap_arbol_virtual_directorio);
                    if(arbol.avd_ap_arbol_virtual_directorio!=-1){
                        Apuntadores+=getPuntero(QString::number(correlativo),QString::number(arbol.avd_ap_arbol_virtual_directorio),7);
                    }
                    Nodos+=getNodo(valores,titulo,correlativo,files);
                    valores.clear();
                    titulo="";
                    files.clear();
                    correlativo++;
                    inicio+=sizeof (AVD);
                    fseek(file,inicio,SEEK_SET);
                    fread(&arbol,sizeof (AVD),1,file);
                }

                inicio=sb.sb_ap_detalle_directorio;
                fseek(file,inicio,SEEK_SET);
                fread(&detalle,sizeof (DD),1,file);
                correlativo=0;
                cantidadD=getDirectorioFree(sb,path)-1;
                while(count<=cantidadD){
                    for (int a=0;a<5;a++) {
                        titulo="";
                        for (int b=0;b<25;b++) {
                            if(detalle.dd_array_files[a].dd_file_nombre[b]!='\0'){
                                titulo+=detalle.dd_array_files[a].dd_file_nombre[b];
                            }
                        }
                        titulo+="\\n";
                        files.push_back(titulo);

                        valores.push_back(detalle.dd_array_files[a].dd_file_ap_inodo);
                        if(detalle.dd_array_files[a].dd_file_ap_inodo!=-1){
                            Apuntadores+=getPuntero("d"+QString::number(correlativo),"i"+QString::number(detalle.dd_array_files[a].dd_file_ap_inodo),a);
                        }
                    }
                    titulo="";
                    valores.push_back(detalle.dd_ap_detalle_directorio);
                    if(detalle.dd_ap_detalle_directorio!=-1){
                        Apuntadores+=getPuntero("d"+QString::number(correlativo),"d"+QString::number(detalle.dd_ap_detalle_directorio),5);
                    }
                    Nodos+=getNodo(valores,titulo,correlativo,files);
                    valores.clear();
                    titulo="";
                    files.clear();

                    correlativo++;
                    inicio+=sizeof (DD);
                    fseek(file,inicio,SEEK_SET);
                    fread(&detalle,sizeof (DD),1,file);
                    count++;
                }

                inicio=sb.sb_ap_tabla_inodo;
                fseek(file,inicio,SEEK_SET);
                fread(&inode,sizeof (iNodo),1,file);
                correlativo=0;
                cantidadD=0;
                cantidadD=getInodoFree(sb,path)-1;
                while(inode.i_array_bloques[0]!=-1){
                    titulo+="Size = "+QString::number(inode.i_size_archivo)+"\\nBloques = "+QString::number(inode.i_count_bloques_asignados)+"\\nPropietario = ";
                    for (int a=0;a<25;a++) {
                        if(inode.i_id_proper[a]!='\0'){
                            titulo+=inode.i_id_proper[a];
                        }
                    }
                    for (int b=0;b<4;b++) {
                        valores.push_back(inode.i_array_bloques[b]);
                        if(inode.i_array_bloques[b]!=-1){
                            Apuntadores+=getPuntero("i"+QString::number(correlativo),"b"+QString::number(inode.i_array_bloques[b]),b);
                        }
                    }
                    valores.push_back(inode.i_ap_indirecto);
                    if(inode.i_ap_indirecto!=-1){
                        Apuntadores+=getPuntero("i"+QString::number(correlativo),"i"+QString::number(inode.i_ap_indirecto),4);
                    }
                    Nodos+=getNodo(valores,titulo,correlativo,files);
                    valores.clear();
                    titulo="";
                    files.clear();
                    inicio+=sizeof (iNodo);
                    fseek(file,inicio,SEEK_SET);
                    fread(&inode,sizeof (InodoiNodo1,file);
                    correlativo++;

                }

                inicio=sb.sb_ap_bloques;
                fseek(file,inicio,SEEK_SET);
                fread(&block,sizeof (Bloque),1,file);
                correlativo=0;
                titulo="";
                cantidadD=getBloqueFree(sb,path)-1;
                while(correlativo<=cantidadD){
                    for (int a=0;a<25;a++) {
                        if(block.db_data[a]!='\0'){
                            titulo+=block.db_data[a];
                        }
                    }
                    valores.push_back(0);
                    if(titulo!=""){
                        Nodos+=getNodo(valores,titulo,correlativo,files);
                    }

                    valores.clear();
                    titulo="";
                    correlativo++;
                    files.clear();
                    inicio+=sizeof (Bloque);
                    fseek(file,inicio,SEEK_SET);
                    fread(&block,sizeof (Bloque),1,file);
                }
            }
        }
        fclose(file);
    }

    actual=Nodos;
    actual+=Apuntadores;
    getTree(actual,pathSalida,extension);
}
*/
