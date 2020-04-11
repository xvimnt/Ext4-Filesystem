#include "disk.h"
//exec &path->minimos.sh
disk::disk()
{

}

//*************Definidas aqui para arreglar el file scope ************//

bool validations::validate_mount(char name[50])
{
    for(Partition part_point : mounted_partitions)
    {
        if(strcmp(part_point.name,name) == 0) return false;
    }
    return true;
}

Partition tools::get_mounted_partition(char id[4])
{
    if(mounted_partitions.contains(QString(id).toUpper()))
        return mounted_partitions[QString(id).toUpper()];

    Partition empty;
    empty.start = -1;
    return empty;
}

char tools::return_free_char(char pointer)
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

string tools::path_by_id(char unit)
{
    QString disk_path;
    foreach (char v, mounted_disks)
    {
        if (v == unit)
            disk_path = mounted_disks.key(unit);
    }
    return disk_path.toStdString();
}

void tools::read_mounted_partitions()
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

//Para generar los ID al montar los discos
//VD % LETRA % NUMERO.
QString validations::generate_ID(Metadata data)
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

//**************Metodos de manipulacion de discos ***********//

void disk::makeDisk(Metadata data)
{
    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    if(!validate_fit(data.fit)) return;
    if(!validate_unit(data.unit,&data.size,0)) return;
    if(!validate_path(data.path,true)) return;


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
        printf("Error: Al tratar de Acceder al disco %s\n",data.path);
        return;
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
    }
}

void disk::removeDisk(Metadata data)
{
    validate_path(data.path,true);
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
    if(!validate_path(data.path,true)) return;

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
            //guardara el numero de KB de la particion
            int fin=(mbr.partitions[current_partition_index].size/1024);
            //Simula 1 KB
            char buffer[1024];
            //Lo llena de 0
            clean(buffer,1024);
            //puntero al inicio de la particion
            fseek(file,mbr.partitions[current_partition_index].start,SEEK_SET);
            //Escribe todos los KB de la particion con el KB de 0
            for (int j = 0;j < fin;j++) {
                fwrite(&buffer,1024 , 1, file);
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
            return;
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
            return;
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

        if(current_partition_index == -1)
        {
            printf("Error: nombre no existente %s\n",data.name);
            return;
        }
        //arreglar el add
        if(!validate_unit(data.unit,&data.add,1)) return;
        //ponerle el nuevo size a nuestra particion
        int new_size = mbr.partitions[current_partition_index].size + data.add;
        //validar si es mayor de cero
        if(new_size <= 0){
            printf("Error: el nuevo tamano no debe de ser negativo o nulo\n");
            return;
        }
        //validar que no rebase al size del disco
        if(!is_less_than_disk(new_size,mbr,current_partition_index)){
            printf("Error: el tamano sobrepasa el del disco\n");
            return;
        }
        //setear el nuevo size de la particion
        mbr.partitions[current_partition_index].size = new_size;
        //modificara la particion
        if(data.add != 0)
        {
            //correr particiones a la derecha si tiene algunas enfrente
            for (int i = current_partition_index + 1 ; i < 4; i++) {
                //si es extendida correr todas las particiones logicas
                if(mbr.partitions[i].type == 'E')
                {
                    //si es extendida correr todas las logicas
                    //Guardar el ebr
                    Ebr ebr;
                    //int donde llevare la direccion donde empieza cada ebr
                    int current_logic_dir = mbr.partitions[current_partition_index].start;
                    //leer primer ebr
                    fseek(file,current_logic_dir,SEEK_SET);
                    fread(&ebr,sizeof (Ebr),1,file);
                    //el nuevo comienzo del ebr
                    ebr.start += new_size;
                    //correr el primer ebr aun este vacio
                    //ebr next 0 significa que nunca se ha querido escriribir una logica
                    while(ebr.next != -1 && ebr.next != 0)
                    {
                        //para guardar la direccion del proximo ebr
                        int next_ebr = ebr.next;
                        //la nueva proxima deireccion del proximo ebr
                        ebr.next += new_size;
                        //guardar cambios en el actual ebr
                        fseek(file,current_logic_dir += new_size,SEEK_SET);
                        fwrite(&ebr,sizeof (Ebr),1,file);
                        //leer el siguiente ebr donde estaba
                        current_logic_dir = next_ebr;
                        fseek(file,current_logic_dir,SEEK_SET);
                        fread(&ebr,sizeof (Ebr),1,file);
                        //el nuevo comienzo del ebr
                        ebr.start += new_size;
                    }
                    //guardar el ebr vacio de al final para sobrescribirle el comienzo
                    fseek(file,current_logic_dir += new_size,SEEK_SET);
                    fread(&ebr,sizeof (Ebr),1,file);
                }
                mbr.partitions[i].start += new_size;
            }
        }else
        {
            printf("Error: Add = 0");
            return;
        }
        //guardar cambios
        fseek(file,0,SEEK_SET);
        fwrite(&mbr,sizeof (Mbr),1,file);
        fclose(file);
        printf("Sistema: %i agregado a la particion\n",data.add);
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
    if(!validate_path(data.path,true)) return;
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

void disk::makeReport(Metadata data)
{

    set_count(0);
    //arreglar el path
    validate_path(data.path,true);
    //crear directorios
    make_directories(data.path);
    //devuelve el formato del reporte
    string format_type = get_report_format(data.path);
    if(format_type == ""){
        printf("Error: Formato del reporte erroneo");
        return;
    }
    if(mounted_partitions.contains(QString(data.id).toUpper()))
    {
        Partition current_partition = mounted_partitions[QString(data.id).toUpper()];
        //encontrar el path del disco de la particion montada
        std::string disk_path = path_by_id(QChar(data.id[2]).toUpper().toLatin1());
        //abrir el disco para escribir en el
        FILE* file= fopen(disk_path.c_str(), "rb+");
        //leyendo el superbloque de la particion
        SuperBloque sb = get_part_sb(file,current_partition.start);
        //comprobar si no hay error al abrir el file
        if(file != nullptr)
        {
            Mbr mbr = get_mbr(file);
            //switching data name (case insensitive)
            QString choice = QString(data.name).toUpper().simplified();
            if(choice == "MBR")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_mbr_table_content(data.path,mbr,format_type);
                //ver si hay particiones logicas
                int extended_partition = get_extended_partition_index(mbr);
                if(extended_partition != -1){
                    Ebr ebr;
                    //int donde llevare la direccion donde empieza cada ebr
                    int current_logic_dir = mbr.partitions[extended_partition].start;
                    //leer primer ebr
                    fseek(file,current_logic_dir,SEEK_SET);
                    fread(&ebr,sizeof (Ebr),1,file);
                    while(ebr.next != -1 && ebr.next != 0)
                    {
                        //quiere decir que ya existe un ebr mandar a graficar
                        graficador::generate_ebr_table_content(data.path,ebr,format_type);
                        //leer el siguiente
                        fseek(file,ebr.next,SEEK_SET);
                        fread(&ebr,sizeof (Ebr),1,file);
                    }
                }
            }
            else if(choice == "DISK")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_dsk_table_content(data.path,mbr,file);
            }
            else if(choice == "SB")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_SB_table_content(data.path,sb);
            }
            else if(choice == "BM_INODE")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_bitmap(sb,"bm_inode",current_partition.size,
                                            data.path,file,current_partition.type);
            }
            else if(choice == "BM_BLOCK")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_bitmap(sb,"bm_block",current_partition.size,
                                            data.path,file,current_partition.type - '0');
            }
            else if(choice == "TREE")
            {
                //Mandandolo a graficar en la path indicada
                graficador::generate_tables(data.path,QString().fromStdString(format_type),file,1,sb);
            }
            else if(choice == "INODE")
            {
                graficador::generate_tables(data.path,QString().fromStdString(format_type),file,2,sb);
            }
            else if(choice == "BLOCK")
            {
                graficador::generate_tables(data.path,QString().fromStdString(format_type),file,3,sb);
            }

            else if(choice == "FILE")
            {
                if(get_deleted())
                {
                    printf("Sistema: Ruta inexistente %s",data.ruta);
                    return;
                }
                fix_path(data.ruta,500);
                iNodo temp;
                int direction = find_inode(file,sb,QString(data.ruta));
                if(direction != 0)
                {
                    fseek(file,direction,SEEK_SET);
                    fread(&temp, sizeof(iNodo), 1, file);
                    //ver su es inodo de carpeta para mostrar el ls
                    graficador::generate_file_report(get_full_file(temp,file).toStdString(),data.path);
                }
                else
                {
                    printf("Sistema: Ruta inexistente %s",data.ruta);
                }
            }
            else if(choice == "LS")
            {
                if(get_deleted())
                {
                    printf("Sistema: Ruta inexistente %s",data.ruta);
                    return;
                }
                fix_path(data.ruta,500);
                //leer el inodo de usuarios para ver su contenido
                iNodo temp;
                int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
                fseek(file,users_dir,SEEK_SET);
                fread(&temp, sizeof(iNodo), 1, file);
                //separar los usuarios
                QStringList lines = get_full_file(temp,file).remove("\001").split('\n');
                //obtener el inodo que buscamos
                int direction = find_inode(file,sb,QString(data.ruta));
                if(direction != 0)
                {
                    fseek(file,direction,SEEK_SET);
                    fread(&temp, sizeof(iNodo), 1, file);
                    //ver su es inodo de carpeta para mostrar el ls
                    print_ls(file,temp,lines);
                }
                else
                {
                    printf("Sistema: Ruta inexistente %s",data.ruta);
                }

            }
            else if(choice == "JOURNALING")
            {
                if(sb.s_filesystem_type != 3)
                {
                    printf("Error: tipo de sistema no aplicable a esta opcion\n");
                    return;
                }
                graficador::generate_tables(data.path,QString().fromStdString(format_type),file,current_partition.start
                                            + static_cast<int>(sizeof (SuperBloque)),sb);
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

void disk::formatDisk(Metadata data)
{
    /******************************
     * VERIFICACION DE PARAMETROS
     *****************************/
    Partition current_partition = get_mounted_partition(data.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada", data.id);
        return;
    }
    if(!validate_format_type(data.format_type)) return;
    if(!validate_fs(data.fs)) return;

    /**********************************
     * CREACION DEL SISTEMA DE ARCHIVOS
     **********************************/
    //Creacion del superbloque
    SuperBloque* super = new SuperBloque(current_partition,data.fs[0] - '0');
    //Escribiendo log si es ext3
    Bitacora* Log = new Bitacora();

    /************************************
     * Escritura del sistema de arhivos
     ************************************/
    FILE *file = get_file_by_id(data.id);
    //escribiendo el superbloque de la particion
    fseek(file,current_partition.start,SEEK_SET);
    fwrite(&*super, sizeof(SuperBloque), 1, file);

    //si es ext3 escribir el journaling
    if(data.fs[0] == '3')
    {
        fseek(file,current_partition.start + static_cast<int>(sizeof (SuperBloque)),SEEK_SET);
        fwrite(&*Log, sizeof(Bitacora), 1, file);
    }
    //escribir los bitmaps y tenerlos en 0
    char empty_char = '\0', used_char = '1';
    //para ocupar 2 inodos y 3 bloques
    int count = 2;
    //bitmap de inodos
    fseek(file,super->s_bm_inode_start,SEEK_SET);
    for(int i = super->s_bm_block_start; i > super->s_bm_inode_start; i--)
    {
        //la cantidad de 0s entre la diferencia entre cada bitmap
        //de primero de escribe el bitmap de inodos
        if(count <= 0) fwrite(&empty_char, sizeof(char), 1, file);
        else fwrite(&used_char, sizeof(char), 1, file);
        count--;
    }
    count = 3;
    //bitmap de bloques
    fseek(file,super->s_bm_block_start,SEEK_SET);
    for(int i = super->s_inode_start; i > super->s_bm_block_start; i--)
    {
        //la cantidad de 0s entre la diferencia entre cada bitmap
        //de primero de escribe el bitmap de inodos
        if(count <= 0) fwrite(&empty_char, sizeof(char), 1, file);
        else fwrite(&used_char, sizeof(char), 1, file);
        count--;
    }

    //Guarda la informacion del login
    QString users = "1, G, root \n1, U, root, root, 123 \n";
    //inodo de carpetas principal
    iNodo *folder_inode = new iNodo(1,1,users.size(),0,770);
    folder_inode->i_block[0] = super->s_block_start;
    //bloque de carpeta root que apunta al inodo del archivo
    Bloque_Carpeta folder_block;
    //seteando a 0 los demas bloques y metiendo en el primero el user txt
    for(int i = 1; i < 4; i++) folder_block.b_content[i] = new content();
    folder_block.b_content[0] = new content("users.txt",super->s_inode_start + static_cast<int>(sizeof (iNodo)));
    //inodo del archivo
    iNodo *file_inode = new iNodo(1,1,users.size(),1,770);
    //bloque del archivo
    Bloque_Archivo *file_block = new Bloque_Archivo(users.toStdString());
    file_inode->i_block[0] = super->s_block_start + static_cast<int>(sizeof (Bloque_Archivo));
    //Escritura del archivo users.txt
    fseek(file,super->s_inode_start,SEEK_SET);
    fwrite(&*folder_inode, sizeof(iNodo), 1, file);
    free(folder_inode);
    fseek(file,super->s_block_start,SEEK_SET);
    fwrite(&folder_block, sizeof(Bloque_Carpeta), 1, file);
    fseek(file,super->s_inode_start + static_cast<int>(sizeof (iNodo)),SEEK_SET);
    fwrite(&*file_inode, sizeof(iNodo), 1, file);
    free(file_inode);
    fseek(file,file_inode->i_block[0],SEEK_SET);
    fwrite(&*file_block, sizeof(Bloque_Archivo), 1, file);
    free(file_block);
    //si el formateo es full escribir inodos vacios
    if(data.format_type[1] == 'U')
    {
        printf("Sistema: Formateo FULL completado\n");
    }
    else printf("Sistema: Formateo FAST completado\n");
    //liberar los punteros
    free(Log);
    free(super);
    fclose(file);
}

void disk::login(Metadata data)
{
    //si el cookie esta vacio
    if(cookie.name[0]==0 && cookie.password[0]==0)
    {
        /******************************
         * VERIFICACION DE PARAMETROS
         *****************************/
        Partition current_partition = get_mounted_partition(data.id);
        if(current_partition.start == -1)
        {//si la particion no esta montada
            printf("Error:  la particion '%s' no esta montada\n", data.id);
            return;
        }
        /************************************
         * Escritura del sistema de arhivos
         ************************************/
        //abrir el disco para escribir en el
        FILE* file = get_file_by_id(data.id);
        //leyendo el superbloque de la particion
        SuperBloque sb = get_part_sb(file,current_partition.start);
        //leer el inodo de usuarios para ver su contenido
        iNodo users;
        int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
        fseek(file,users_dir,SEEK_SET);
        fread(&users, sizeof(iNodo), 1, file);
        //separar los usuarios
        QStringList lines = get_full_file(users,file).remove("\001").split('\n');
        //buscar el usuario y su pwd
        foreach(auto line, lines)
        {
            QStringList var = line.split(',');
            //si el string no esta vacio
            if(var[0] != "")
            {
                //si es un usuario
                if(var[1].simplified() == "U")
                {
                    //si su usuario y pwd es correcta
                    if(var[3].simplified() == data.user && var[4].simplified() == data.path)
                    {
                        //guardar el usuario en ram
                        strcpy(cookie.name,data.user);
                        strcpy(cookie.password,data.path);
                        strcpy(cookie.id, data.id);
                        cookie.tag = var[0].toInt();
                        cookie.group = get_group_id_by_name(lines,var[2]);
                        printf("Sistema: El usuario %s ha iniciado sesion con exito\n",data.user);
                        return;
                    }
                }
            }
        }
        printf("Sistema: Usuario no encontrado o password incorrecta\n");
    }
    else printf("Sistema: Un usuario ha iniciado sesion, favor cerrar sesion con logout\n");
}

void disk::logout()
{
    //verificar si existe una sesion
    if(cookie.name[0]==0 && cookie.password[0] ==0)
    {
        //no hay sesion activa
        printf("Error Logout, No existe una sesion activa\n");
    }
    else
    {
        clean(cookie.name,10);
        clean(cookie.password,10);
        printf("Sistema: se ha cerrado sesion\n");
    }
}

void disk::makeGroup(Metadata data)
{
    if(QString(cookie.password).simplified() != "123"){
        printf("Sistema: solo el usuario root puede crear grupo\n");
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //quitarle las comillas al nombre
    fix_path(data.name,50);
    //validar que no sean mayor a 10
    if(!validate_number_of_chars(data.name,10,"nombre de grupo")) return;
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //leer el inodo de usuarios para ver su contenido
    iNodo users;
    int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
    fseek(file,users_dir,SEEK_SET);
    fread(&users, sizeof(iNodo), 1, file);
    //separar los usuarios
    QStringList lines = get_full_file(users,file).remove("\001").split('\n');
    //obtener el ultimo id utilizado para un grupo
    int last_id = 0;
    foreach(auto line, lines)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si es un grupo
            if(var[1].simplified() == "G")
            {
                //verificar que no existe el nombre de grupo
                if(var[2].simplified().toStdString() == data.name)
                {
                    printf("Sistema: Ya existe un grupo con este nombre %s\n", data.name);
                    fclose(file);
                    return;
                }
                last_id = var[0].simplified().toInt();
            }
        }
    }
    //armar la string a guardar
    string new_id = to_string(last_id + 1);
    new_id += ", G, ";
    new_id += QString(data.name).toStdString();
    new_id += "\n";
    //concatenar el texto en nuestro inodo
    if(!append_text_in_inode(users_dir,QString(new_id.c_str()),file,sb))
    {
        printf("Error: El archivo ha alcanzado su size maximo\n");
        return;
    }
    //guardando la bitacora
    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("mkGroup","users.txt",new_id);
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    fclose(file);
    printf("Sistema: Grupo %s creado correctamente\n",data.name);
}

void disk::removeGroup(Metadata data)
{
    //ver si el usuario root esta logeado
    if(QString(cookie.password).simplified() != "123"){
        printf("Sistema: solo el usuario root puede crear grupo\n");
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //quitarle la comillas a la string
    fix_path(data.name,50);
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //leer el inodo de usuarios para ver su contenido
    iNodo users;
    int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
    fseek(file,users_dir,SEEK_SET);
    fread(&users, sizeof(iNodo), 1, file);
    //separar los usuarios
    QStringList lines = get_full_file(users,file).remove("\001").split('\n');
    //guardar la nueva string de usuarios
    QString users_txt = "";
    //para ver si se encuentra el nombre de grupo
    bool is_find = false;
    //encontrar el grupo a eliminar
    foreach(auto line, lines)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si es un grupo
            if(var[1].simplified() == "G")
            {
                if(var[2].simplified().toStdString() == data.name)
                {
                    //si el grupo ya esta eliminado mostrar que no se encuentra
                    if(var[0].toInt() == 0)
                    {
                        printf("Error: No se encuentra el grupo %s\n", data.name);
                        return;
                    }
                    line = "0, " + var[1] +", " + var[2];
                    is_find = true;
                }
            }
            users_txt += line;
            users_txt += "\n";
        }
    }
    //si no se encuentra marcar error
    if(!is_find)
    {
        printf("Error: No se encuentra el grupo %s\n", data.name);
        return;
    }
    //escribir la nueva users txt
    int number_of_blocks = (users_txt.size() / 64) + 2; // + 1 para omitir el primer bloque con nombre de archivo
    //para guardar cuantos chars quedan por escribir
    int remain_chars = users_txt.size();
    //recorrer el inodo y desasignar los bloques no usados
    for (int i = 1; i < 12; i++) // se omite el 0 ya que contiene el nombre del archivo
    {
        //las posiciones que ya no se ocupan
        if(i >= number_of_blocks)
        {
            if(users.i_block[i] != 0)
            {
                //buscar en el bitmap el bloque asignado y desasignarlo
                int bm_pos = (users.i_block[i] - sb.s_block_start) / 64;
                //tenemos la posicion del bloque en el bitmap, se procede a desabhilitarlo
                char empty = '\0';
                fseek(file,sb.s_bm_block_start + bm_pos,SEEK_SET);
                fwrite(&empty, sizeof(char), 1, file);
                //actualizamos el superbloque
                sb.s_blocks_count--;
            }
            //se deshabilita el puntero
            users.i_block[i] = 0;
            users.i_mtime = time(nullptr);
        }
        //las posiciones que se ocupan sobrescribirlas
        else
        {
            //el bloque con el que se va a sobrescribir
            Bloque_Archivo *temp_block = new Bloque_Archivo();
            //escribir el bloque completo
            for(int j = 0; j < 64; j++)
            {
                //si no se llena validar que la string no se salga de index
                if(remain_chars > 0)
                {
                    //sobrescribir el bloque
                    temp_block->b_content[j] = users_txt[(users_txt.size() - remain_chars)].toLatin1();
                    remain_chars --;
                }
            }
            //guardar el bloque sobrescrito
            fseek(file,users.i_block[i],SEEK_SET);
            fwrite(&*temp_block, sizeof(Bloque_Archivo), 1, file);
        }
    }
    //se guarda el inodo
    fseek(file,users_dir,SEEK_SET);
    fwrite(&users, sizeof(iNodo), 1, file);
    //se guarda el superbloque
    fseek(file,current_partition.start,SEEK_SET);
    fwrite(&sb, sizeof(SuperBloque), 1, file);
    //se cierra el archivo
    fclose(file);
    //mensaje para el usuario
    //guardando la bitacora
    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("removeGroup","users.txt",data.name);
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    printf("Sistema: Grupo %s borrado con exito\n",data.name);
}

void disk::makeUser(Metadata data)
{
    //verificar si el usuario root esta logeado
    if(QString(cookie.password).simplified() != "123"){
        printf("Sistema: solo el usuario root puede crear grupo\n");
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //quitarle las comillas al nombre
    fix_path(data.user,10);
    fix_path(data.path,50);
    fix_path(data.group,10);
    //arreglar el name
    //validar que no sean mayor a 10
    if(!validate_number_of_chars(data.user,10,"nombre de usuario")) return;
    if(!validate_number_of_chars(data.path,10,"password")) return;
    if(!validate_number_of_chars(data.group,10,"nombre de grupo")) return;
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //leer el inodo de usuarios para ver su contenido
    iNodo users;
    int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
    fseek(file,users_dir,SEEK_SET);
    fread(&users, sizeof(iNodo), 1, file);
    //separar los usuarios
    QStringList lines = get_full_file(users,file).remove("\001").split('\n');
    //Verificar si existe el grupo
    bool gp_exists = false;
    //para guardar la ultima id utilizada por un usuario
    int last_id = 0;
    foreach(auto line, lines)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si es un grupo
            if(var[1].simplified() == "G")
            {
                //verificar que exista el nombre de grupo
                if(var[2].simplified().toStdString() == data.group && var[0].toInt() != 0) gp_exists = true;
            }
            //si es un usuario buscar el ultimo id
            else if(var[1].simplified() == "U")
            {
                if(var[3].simplified() == data.user)
                {
                    printf("Sistema: Usuario con ese nombre ya existente, ingrese otro nombre\n");
                    return;
                }
                last_id = var[0].simplified().toInt();
            }
        }
    }
    if(!gp_exists)
    {
        printf("Sistema: Grupo '%s' no existente, intente con otro grupo o creelo\n",data.group);
        return;
    }
    //armar la string a guardar
    string new_id = to_string(last_id + 1);
    new_id += ", U, ";
    new_id += QString(data.group).toStdString();
    new_id += ", ";
    new_id += data.user;
    new_id += ", ";
    new_id += QString(data.path).toStdString();
    new_id += "\n";
    //concatenar el texto en nuestro inodo
    if(!append_text_in_inode(users_dir,QString(new_id.c_str()),file,sb))
    {
        printf("Error: El archivo ha alcanzado su size maximo\n");
        return;
    }
    //guardando la bitacora
    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("mkUser","users.txt",new_id);
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    fclose(file);
    printf("Sistema: Usuario %s creado correctamente\n",data.user);
}

void disk::removeUser(Metadata data)
{
    //ver si el usuario root esta logeado
    if(QString(cookie.password).simplified() != "123"){
        printf("Sistema: solo el usuario root puede crear grupo\n");
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //quitarle la comillas a la string
    fix_path(data.user,10);
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //leer el inodo de usuarios para ver su contenido
    iNodo users;
    int users_dir = sb.s_inode_start + static_cast<int>(sizeof (iNodo));
    fseek(file,users_dir,SEEK_SET);
    fread(&users, sizeof(iNodo), 1, file);
    //separar los usuarios
    QStringList lines = get_full_file(users,file).remove("\001").split('\n');
    //guardar la nueva string de usuarios
    QString users_txt = "";
    //para ver si se encuentra el nombre de usuario
    bool is_find = false;
    //encontrar el usuario a eliminar
    foreach(auto line, lines)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si es un usuario
            if(var[1].simplified() == "U")
            {
                if(var[3].simplified().toStdString() == data.user)
                {
                    //si el usuario ya esta eliminado mostrar que no se encuentra
                    if(var[0].toInt() == 0)
                    {
                        printf("Error: No se encuentra el grupo %s\n", data.user);
                        return;
                    }
                    line = "0, " + var[1] +", " + var[2]+", " + var[3]+", " + var[4];
                    is_find = true;
                }
            }
            users_txt += line;
            users_txt += "\n";
        }
    }
    //si no se encuentra marcar error
    if(!is_find)
    {
        printf("Error: No se encuentra el usuario %s\n", data.user);
        return;
    }
    //escribir la nueva users txt
    int number_of_blocks = (users_txt.size() / 64) + 2; // + 1 para omitir el primer bloque con nombre de archivo
    //para guardar cuantos chars quedan por escribir
    int remain_chars = users_txt.size();
    //recorrer el inodo y desasignar los bloques no usados
    for (int i = 1; i < 12; i++) // se omite el 0 ya que contiene el nombre del archivo
    {
        //las posiciones que ya no se ocupan
        if(i >= number_of_blocks)
        {
            if(users.i_block[i] != 0)
            {
                //buscar en el bitmap el bloque asignado y desasignarlo
                int bm_pos = (users.i_block[i] - sb.s_block_start) / 64;
                //tenemos la posicion del bloque en el bitmap, se procede a desabhilitarlo
                char empty = '\0';
                fseek(file,sb.s_bm_block_start + bm_pos,SEEK_SET);
                fwrite(&empty, sizeof(char), 1, file);
                //actualizamos el superbloque
                sb.s_blocks_count--;
            }
            //se deshabilita el puntero
            users.i_block[i] = 0;
            users.i_mtime = time(nullptr);
        }
        //las posiciones que se ocupan sobrescribirlas
        else
        {
            //el bloque con el que se va a sobrescribir
            Bloque_Archivo *temp_block = new Bloque_Archivo();
            //escribir el bloque completo
            for(int j = 0; j < 64; j++)
            {
                //si no se llena validar que la string no se salga de index
                if(remain_chars > 0)
                {
                    //sobrescribir el bloque
                    temp_block->b_content[j] = users_txt[(users_txt.size() - remain_chars)].toLatin1();
                    remain_chars --;
                }
            }
            //guardar el bloque sobrescrito
            fseek(file,users.i_block[i],SEEK_SET);
            fwrite(&*temp_block, sizeof(Bloque_Archivo), 1, file);
            //borrar los punteros
            free(temp_block);
        }
    }
    //se guarda el inodo
    fseek(file,users_dir,SEEK_SET);
    fwrite(&users, sizeof(iNodo), 1, file);
    //se guarda el superbloque
    fseek(file,current_partition.start,SEEK_SET);
    fwrite(&sb, sizeof(SuperBloque), 1, file);
    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("removeUser","users.txt",data.name);
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    //se cierra el archivo
    fclose(file);

    //mensaje para el usuario
    printf("Sistema: Usuario %s borrado con exito\n",data.user);
}

void disk::chmod(Metadata data)
{
    //el comando UGO ira guardado en el parametro add
    //*******Validacion de parametros***********//
    //si es la carpeta root
    if(!validate_path(data.path,false)) return;
    if(!validate_ugo(data.add)) return;
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }

    //*******************Ejecucion***************//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //separar la ruta para obtener las carpetas
    QStringList lines = QString(data.path).split('/');
    //para iterar los niveles en el arbol
    int iterator = 0;
    //para guardar la direccion del inodo que buscamos modificar
    int direction = 0;
    //para guardar el contenido de las carpetas
    Bloque_Carpeta file_content;
    //subiendo en los niveles
    while (iterator < lines.count())
    {
        //setear a 0 para encontrar el nuevo nivel
        direction = 0;
        //recorrer todos los apuntadores directos del inodo
        for (int i = 0;i < 12;i++)
        {
            //si el apuntador directo tiene algo
            if(to_modify.i_block[i] != 0)
            {
                //leer el bloque y ver en sus 4 posiciones
                fseek(file,to_modify.i_block[i],SEEK_SET);
                fread(&file_content, sizeof(Bloque_Carpeta), 1, file);
                //recorriendo 4 posiciones del bloque
                for(auto cont : file_content.b_content)
                {
                    //se encuentra el archivo o carpeta
                    if(lines[iterator] == cont->b_name)
                    {
                        //La direccion del inodo
                        direction = cont->b_inodo;
                        //actualizando el inodo al que encontro
                        fseek(file,direction,SEEK_SET);
                        fread(&to_modify, sizeof(iNodo), 1, file);
                        //ultima fecha que se leyo el inodo sin modificarlo
                        to_modify.i_atime = time(nullptr);
                        break;
                    }
                }
                //si se encontro en el bloque ya no buscar en el siguiente
                if(direction != 0) break;
            }
        }
        //si no lo encuentra en su respectivo nivel
        if(direction == 0)
        {
            printf("Sistema: ruta inexistente: %s",data.path);
            return;
        }
        //aumentando el nivel del arbol
        iterator++;
    }
    //actualizando los permisos del inodo

    //si es el usuario root entonces modificar los permisos
    if(strcmp(cookie.name,"root") == 0)
    {
        //modificar los permisos
        to_modify.i_perm = data.add;
        //si es recursivo y es una carpeta buscar todos sus inodos y modificarlos
        if(data.recursively && to_modify.i_type == 0) change_permissions_recursively(to_modify,data.add,file,direction);
        else
        {
            //cambiar permisos
            to_modify.i_perm = data.add;
            //ultimo tiempo de modificacion
            to_modify.i_mtime = time(nullptr);
            //guardar cambios de inodo
            fseek(file,direction,SEEK_SET);
            fwrite(&to_modify, sizeof(iNodo), 1, file);
        }
    }
    //buscar si es propietario del archivo o carpeta
    else if(cookie.tag == to_modify.i_uid)
    {
        //si es recursivo y es una carpeta buscar todos sus inodos y modificarlos
        if(data.recursively && to_modify.i_type == 0) change_permissions_recursively(to_modify,data.add,file,direction);
        else
        {
            //cambiar permisos
            to_modify.i_perm = data.add;
            //ultimo tiempo de modificacion
            to_modify.i_mtime = time(nullptr);
            //guardar cambios de inodo
            fseek(file,direction,SEEK_SET);
            fwrite(&to_modify, sizeof(iNodo), 1, file);
        }
    }
    else
    {
        printf("Sistema: No se pudo modificar, usted no es propietario de este objeto: %s\n",data.path);
        return;
    }
    //mensaje de exito
    printf("Sistema: Permisos actualizados con exito\n");
}

void disk::makeFile(Metadata data)
{
    //*******Validacion de parametros***********//
    if(!validate_path(data.path,false)) return;
    if(data.size < 0)
    {
        printf("Sistema: error con el size del archivo, no valido: %i",data.size);
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }

    //***************Ejecucion**********//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //separar la ruta para obtener las carpetas
    QStringList lines = QString(data.path).split('/');
    //para iterar los niveles en el arbol
    int iterator = 0;
    //para guardar la direccion del inodo que buscamos modificar
    int direction = 0, last_direction = sb.s_inode_start;
    //subiendo en los niveles
    while (iterator < lines.count() - 1)
    {
        //setear a 0 para encontrar el nuevo nivel
        direction = search_path_in_inode(to_modify,file,lines[iterator]);
        //si no lo encuentra
        if(direction == 0)
        {
            if(data.recursively)
            {
                //Se crea el folder en el inodo
                direction = make_folder_in_inode(last_direction,file,lines[iterator],data.size,sb);
                if(direction != 0)
                {
                    //actualizando el inodo al que encontro
                    fseek(file,direction,SEEK_SET);
                    fread(&to_modify, sizeof(iNodo), 1, file);
                    printf("se crea carpeta: %s\n",lines[iterator].toStdString().c_str());
                }
                //sino se logra crear es porque esta lleno el ultimo bloque carpeta
                else
                {
                    printf("Sistema: Size maximo alcanzado\n");
                    return;
                }
            }
            else
            {
                printf("Sistema: ruta inexistente: %s\n",data.path);
                return;
            }
        }
        else
        {
            //actualizar el inodo para subir de nivel
            fseek(file,direction,SEEK_SET);
            fread(&to_modify, sizeof(iNodo), 1, file);
        }
        //aumentando el nivel del arbol
        last_direction = direction;
        iterator++;
    }
    //abriendo el contenido a subir al sistema

    if(data.ruta[0] != '\0')
    {
        fix_path(data.ruta,500);
        string line,full_file = "";
        ifstream myfile (data.ruta);
        if (myfile.is_open())
        {
            while ( getline (myfile,line) )
            {
                full_file += line;
            }
            myfile.close();
            if(
                    //la direccion es la de un bloque carpeta con espacios vacios
                    make_file_in_block
                    (
                        file
                        ,get_last_folder_block_with_free_space(last_direction,file,sb)
                        ,to_modify
                        ,-1
                        ,sb,lines[iterator].toStdString().c_str()
                        , QString().fromStdString(full_file)
                        ) == 0
                    )
            {
                //si devuelve 0 no se pudo crear el archivo por falta de espacio
                printf("Error: Al crear archivo\n");
                return;
            }
            //mensaje al usuario
            printf("Sistema: Archivo creado con exito\n");
            return;
        }
        else
        {
             printf("Error: Al crear archivo, origen no encontrado %s\n",data.ruta);
             return;
        }
    }

    //probar llenando de numeros
    if(
            //la direccion es la de un bloque carpeta con espacios vacios
            make_file_in_block
            (
                file
                ,get_last_folder_block_with_free_space(last_direction,file,sb)
                ,to_modify
                ,data.size
                ,sb,lines[iterator].toStdString().c_str()
                , ""
                ) == 0
            )
    {
        //si devuelve 0 no se pudo crear el archivo por falta de espacio
        printf("Error: Al crear archivo\n");
        return;
    }
    //mensaje al usuario
    printf("Sistema: Archivo creado con exito\n");

    //se comprobaron todas las carpetas padres ahora crear el archivo

    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("mkFile",data.path,QString::number(data.recursively).toStdString());
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    fclose(file);
}

void disk::makeDir(Metadata data)
{
    //*******Validacion de parametros***********//
    if(!validate_path(data.path,false)) return;
    if(data.size < 0)
    {
        printf("Sistema: error con el size del archivo, no valido: %i",data.size);
        return;
    }
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }

    //***************Ejecucion**********//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //separar la ruta para obtener las carpetas
    QStringList lines = QString(data.path).split('/');
    //para iterar los niveles en el arbol
    int iterator = 0;
    //para guardar la direccion del inodo que buscamos modificar
    int direction = 0, last_direction = sb.s_inode_start;
    //subiendo en los niveles
    while (iterator < lines.count() - 1)
    {
        //setear a 0 para encontrar el nuevo nivel
        direction = search_path_in_inode(to_modify,file,lines[iterator]);
        //si no lo encuentra
        if(direction == 0)
        {
            if(data.recursively)
            {
                //Se crea el folder en el inodo
                direction = make_folder_in_inode(last_direction,file,lines[iterator],0,sb);
                if(direction != 0)
                {
                    //actualizando el inodo al que encontro
                    fseek(file,direction,SEEK_SET);
                    fread(&to_modify, sizeof(iNodo), 1, file);
                    printf("se crea carpeta: %s\n",lines[iterator].toStdString().c_str());
                }
                //sino se logra crear es porque esta lleno el ultimo bloque carpeta
                else
                {
                    printf("Sistema: Size maximo alcanzado");
                    return;
                }
            }
            else
            {
                printf("Sistema: ruta inexistente: %s\n",data.path);
                return;
            }
        }
        else
        {
            //actualizar el inodo para subir de nivel
            fseek(file,direction,SEEK_SET);
            fread(&to_modify, sizeof(iNodo), 1, file);
        }
        //aumentando el nivel del arbol
        last_direction = direction;
        iterator++;
    }
    //se comprobaron todas las carpetas padres ahora crear el archivo
    if(
            //la direccion es la de un bloque carpeta con espacios vacios
            make_folder_in_block
            (
                file
                ,get_last_folder_block_with_free_space(last_direction,file,sb)
                ,to_modify
                ,0
                ,sb
                ,lines[iterator].toStdString().c_str()
                ) == 0
            )
        //si devuelve 0 no se pudo crear el archivo por falta de espacio
        printf("Error: Al crear carpeta\n");
    //mensaje al usuario
    printf("Sistema: Archivo creado con exito\n");
    if(sb.s_filesystem_type == 3)
    {
        Bitacora* log = new Bitacora("mkDir",data.path,QString::number(data.recursively).toStdString());
        add_new_log(log,file,current_partition.start + static_cast<int>(sizeof (SuperBloque)));
    }
    fclose(file);
}

void disk::showFile(Metadata data, bool gnt_report)
{
    //*******Validacion de parametros***********//
    if(!validate_path(data.path,false)) return;
    Partition current_partition = get_mounted_partition(cookie.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }

    //***************Ejecucion**********//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //separar la ruta para obtener las carpetas
    QStringList lines = QString(data.path).split('/');
    //para iterar los niveles en el arbol
    int iterator = 0;
    //para guardar la direccion del inodo que buscamos modificar
    int direction = 0, last_direction = sb.s_inode_start;
    //subiendo en los niveles
    while (iterator < lines.count())
    {
        //setear a 0 para encontrar el nuevo nivel
        direction = search_path_in_inode(to_modify,file,lines[iterator]);
        //si no lo encuentra
        if(direction == 0)
        {
            printf("Sistema: ruta inexistente: %s\n",data.path);
            return;
        }
        else
        {
            //actualizar el inodo para subir de nivel
            fseek(file,direction,SEEK_SET);
            fread(&to_modify, sizeof(iNodo), 1, file);
        }
        //aumentando el nivel del arbol
        last_direction = direction;
        iterator++;
    }
    //Se muestra el contenido del archivo
    string full_file = get_full_file(to_modify,file).remove("\001").toStdString();
    if(gnt_report)
    {
        fix_path(data.ruta,500);
        graficador::generate_file_report(full_file,data.ruta);
    }
    else
    {
        cout << full_file << endl;
    }
    fclose(file);
}

void disk::loss(Metadata data)
{
    Partition current_partition = get_mounted_partition(data.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //***************Ejecucion**********//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //mandando a borrar sus datos
    switch_deleted(true);
    //mensaje de usuario
    printf("Sistema: se han perdido todos los datos\n");
}

void disk::recovery(Metadata data)
{
    Partition current_partition = get_mounted_partition(data.id);
    if(current_partition.start == -1)
    {//si la particion no esta montada
        printf("Error:  la particion '%s' no esta montada\n", data.id);
        return;
    }
    //***************Ejecucion**********//
    //abrir el disco para escribir en el
    FILE* file = get_file_by_id(cookie.id);
    //leyendo el superbloque de la particion
    SuperBloque sb = get_part_sb(file,current_partition.start);
    //obteniendo el primer inodo
    iNodo to_modify;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&to_modify, sizeof(iNodo), 1, file);
    //mandando a borrar sus datos
    switch_deleted(false);
    //mensaje de usuario
    printf("Sistema: se han recuperado todos los datos\n");
}

