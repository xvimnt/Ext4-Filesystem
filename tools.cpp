#include "tools.h"

tools::tools()
{

}

QString tools::get_log_report(FILE *file,int log_start)
{
    QString response = "table0 [label=<\n";
    response += "<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n";
    response += "<TR PORT=\"header\">\n";
    response += "<TD BGCOLOR=\"yellow\" COLSPAN=\"8\"> Journaling </TD>\n";
    response += "</TR>\n";
    response += "\n<TR>\n";
    response += "<TD BGCOLOR=\"red\">Tipo de Operacion</TD><TD BGCOLOR=\"red\">Ruta</TD>\n";
    response += "<TD BGCOLOR=\"red\">Contenido</TD><TD BGCOLOR=\"red\">Fecha</TD>\n";
    response += "</TR>\n";
    Bitacora pointer;
    fseek(file,log_start,SEEK_SET);
    fread(&pointer,sizeof (Bitacora),1,file);
    int count = 0;
    while(pointer.log_tipo_operacion[0] != '\0')
    {
        response += "\n<TR>\n";
        response +=  "<TD>"+QString(pointer.log_tipo_operacion)+"</TD>\n"
                + "<TD>"+QString(pointer.log_ruta)+"</TD>\n"
                + "<TD>"+QString(pointer.log_contenido)+"</TD>\n"
                + "<TD>"+get_time(pointer.log_fecha)+"</TD>\n";
        report_count++;
        count++;
        fread(&pointer,sizeof (Bitacora),1,file);
        response += "</TR>\n";
    }
    response += "</TABLE>\n>];\n\n";
    return response;
}

QString get_perm_string(int individual_perm)
{
    switch (individual_perm)
    {
    case 0:
        return "---";
    case 1:
        return "--x";
    case 2:
        return "-w-";
    case 3:
        return "-wx";
    case 4:
        return "r--";
    case 5:
        return "r-x";
    case 6:
        return "rw-";
    case 7:
        return "rwx";
    default:
        return "error";
    }
}

QString retrieve_perm(int perm)
{
    QString output = "";
    int owner = perm % 10;
    perm /= 10;
    int group = perm % 10;
    perm /= 10;
    int user = perm % 10;
    return "-"+get_perm_string(owner) + get_perm_string(group) + get_perm_string(user);
}

QString get_owner(QStringList users, int id, QString type)
{
    //buscar el usuario y su pwd
    foreach(auto line, users)
    {
        QStringList var = line.split(',');
        //si el string no esta vacio
        if(var[0] != "")
        {
            //si el id es el que buscamos
            if(var[0].simplified() == QString::number(id))
            {
                if(var[1].simplified() == type)
                {
                    if(type == "G") return var[2].simplified();
                    else return  var[3].simplified();
                }
            }
        }
    }
    return "error";
}

QString tools::get_inode_type(int type)
{
    switch (type)
    {
    case 0: return "Carpeta";
    case 1: return "Archivo";
    default: return "error";
    }
}

QString tools::get_inode_info(iNodo inode, QStringList users)
{
    return retrieve_perm(inode.i_perm) + " " + get_owner(users,inode.i_uid,"U")
            + " " + get_owner(users,inode.i_gid,"G") + " " + QString::number(inode.i_size)
            + " " + get_time(inode.i_mtime) + " " + get_inode_type(inode.i_type);
}

QString tools::get_ls(Bloque_Carpeta folder_block,FILE *file, QStringList users)
{
    QString output = "";
    iNodo inode;
    for(auto cont: folder_block.b_content)
    {
        if(cont->b_inodo != 0)
        {
            fseek(file,cont->b_inodo,SEEK_SET);
            fread(&inode,sizeof (iNodo),1,file);
            output += get_inode_info(inode,users) + " " + cont->b_name + "\n";
        }
    }
    return output;
}

QString tools::get_pointer_ls(Bloque_apuntador pointer_block,FILE *file, QStringList users, int recursevly)
{
    QString output;
    Bloque_Carpeta folder_block;
    Bloque_apuntador new_pointer;
    for (auto dir : pointer_block.b_pointers)
    {
        if(dir != 0)
        {
            switch(recursevly)
            {
            case 1:

                fseek(file,dir,SEEK_SET);
                fread(&folder_block,sizeof (Bloque_Carpeta),1,file);
                output += get_ls(folder_block,file,users);
                break;
            case 2:
                fseek(file,dir,SEEK_SET);
                fread(&new_pointer,sizeof (Bloque_apuntador),1,file);
                output += get_pointer_ls(new_pointer,file,users,1);
                break;
            case 3:
                fseek(file,dir,SEEK_SET);
                fread(&new_pointer,sizeof (Bloque_apuntador),1,file);
                output += get_pointer_ls(new_pointer,file,users,2);
                break;
            }
        }
    }
    return output;
}

void tools::add_new_log(Bitacora* log,FILE *file,int log_start)
{
    Bitacora pointer;
    fseek(file,log_start,SEEK_SET);
    fread(&pointer,sizeof (Bitacora),1,file);
    int count = 0;
    while(pointer.log_tipo_operacion[0] != '\0')
    {
        count++;
        fread(&pointer,sizeof (Bitacora),1,file);
    }
    fseek(file,log_start + count * static_cast<int>(sizeof (Bitacora)),SEEK_SET);
    fwrite(&*log,sizeof (Bitacora),1,file);
    //excribir nuevo apuntador
    log = new Bitacora();
    fseek(file,log_start + (count + 1) * static_cast<int>(sizeof (Bitacora)),SEEK_SET);
    fwrite(&*log,sizeof (Bitacora),1,file);
}

void tools::print_ls(FILE* file, iNodo inode, QStringList users)
{
    //este metodo solo sirve para inodo carpeta
    QString output = "";
    Bloque_Carpeta folder_block;
    Bloque_apuntador pointer_block;
    for (int i = 0;i <12;i++)
    {
        if(inode.i_block[i] != 0)
        {
            fseek(file,inode.i_block[i],SEEK_SET);
            fread(&folder_block,sizeof (Bloque_Carpeta),1,file);
            output += get_ls(folder_block,file,users);
        }
    }
    if(inode.i_block[12] != 0)
    {
        fseek(file,inode.i_block[12],SEEK_SET);
        fread(&pointer_block,sizeof (Bloque_apuntador),1,file);
        output += get_pointer_ls(pointer_block,file,users,1);
    }
    if(inode.i_block[13] != 0)
    {
        fseek(file,inode.i_block[13],SEEK_SET);
        fread(&pointer_block,sizeof (Bloque_apuntador),1,file);
        output += get_pointer_ls(pointer_block,file,users,2);
    }
    if(inode.i_block[14] != 0)
    {
        fseek(file,inode.i_block[14],SEEK_SET);
        fread(&pointer_block,sizeof (Bloque_apuntador),1,file);
        output += get_pointer_ls(pointer_block,file,users,3);
    }
    cout << output.toStdString() << endl;
}

void tools::switch_deleted(bool d_var)
{
    is_deleted = d_var;
}

bool tools::get_deleted()
{
    return is_deleted;
}

QString tools::get_time(time_t rawtime)
{
    if(rawtime == 0) return "Sin fecha";
    return asctime(localtime (&rawtime));
}

QString tools::get_inode_report(iNodo inode)
{
    QString output_inode = "table"+QString::number(report_count)+" [shape=none, label=<";
    output_inode += "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">";
    output_inode +=  "<tr><td  bgcolor=\"red\"> <font color=\"white\">Inodo</font> </td></tr>\n";
    output_inode +=  "<tr><td port=\"0\" align=\"left\">i_gid:"+QString::number(inode.i_gid)+"   </td></tr>\n";
    output_inode +=  "<tr><td port=\"1\" align=\"left\">i_uid:"+QString::number(inode.i_uid)+"    </td></tr>\n";
    output_inode += "<tr><td port=\"2\" align=\"left\">i_size:"+QString::number(inode.i_size)+"    </td></tr>\n";
    output_inode += "<tr><td port=\"3\" align=\"left\">i_atime:"+get_time(inode.i_atime)+"    </td></tr>\n";
    output_inode += "<tr><td port=\"4\" align=\"left\">i_ctime:"+get_time(inode.i_ctime)+"    </td></tr>\n";
    output_inode += "<tr><td port=\"5\" align=\"left\">i_mtime:"+get_time(inode.i_mtime)+"    </td></tr>\n";
    for (int i = 0;i<15;i++) output_inode += "<tr><td port=\""
            +QString::number(6+i)+"\" align=\"left\">i_block["
            +QString::number(i)+"]:"+QString::number(inode.i_block[i])
            +"</td></tr>";
    output_inode += "<tr><td port=\"22\" align=\"left\">i_type:"+QString::number(inode.i_type)+"    </td></tr>\n";
    output_inode += "<tr><td port=\"23\" align=\"left\">i_perm:"+QString::number(inode.i_perm)+"    </td></tr>\n";
    output_inode += "</table>>];\n\n";
    report_count++;
    return  output_inode;
}

QString tools::get_full_inode_report(SuperBloque sb, FILE *file)
{
    QString output_string = "";
    iNodo inode;
    //obtengo el primer bit
    char bit;
    int count = 0;
    do
    {   //actualizando el inodo en el bm
        fseek(file,sb.s_bm_inode_start + count,SEEK_SET);
        fread(&bit, sizeof(char), 1, file);
        if(bit == '\0') break;
        //obteniendo el inodo
        fseek(file,sb.s_inode_start + count * static_cast<int>(sizeof (iNodo)),SEEK_SET);
        fread(&inode, sizeof(iNodo), 1, file);
        output_string += "table"+QString::number(report_count)
                + "-> table" + QString::number(report_count + 1) + ";\n";
        output_string += get_inode_report(inode);
        count++;
    }while(bit != '\0');
    return output_string;
}

QString tools::get_blocks_report(iNodo inode, FILE * file)
{
    QString output_file = "";
    for (int i = 0;i<12;i++)
    {
        if(inode.i_block[i] != 0)
        {
            output_file += "table"+QString::number(report_count)
                    + "-> table" + QString::number(report_count + 1) + ";\n";
            output_file += switch_block_report(inode.i_type,file,inode.i_block[i]);
        }
        // si existe un directo vacio ya no pasar a los indirectos
        else return output_file;
    }
    if(inode.i_block[12] != 0)
    {
        get_pointer_blocks_report(inode.i_block[12],file,inode.i_type,1);
    }
    if(inode.i_block[13] != 0)
    {
        get_pointer_blocks_report(inode.i_block[13],file,inode.i_type,2);
    }
    if(inode.i_block[14] != 0)
    {
        get_pointer_blocks_report(inode.i_block[14],file,inode.i_type,3);
    }
    return output_file;
}

QString tools::switch_block_report(int type,FILE *file, int dir)
{
    QString output_string = "";
    if(type == 0)
    {
        Bloque_Carpeta folder_block;
        fseek(file,dir,SEEK_SET);
        fread(&folder_block,sizeof (Bloque_Carpeta),1,file);
        output_string += get_folder_block_report(folder_block);
    }
    else
    {
        Bloque_Archivo file_block;
        fseek(file,dir,SEEK_SET);
        fread(&file_block,sizeof (Bloque_Archivo),1,file);

        output_string += get_file_block_report(file_block);
    }
    return output_string;
}

QString tools::get_full_blocks_report(SuperBloque sb, FILE *file)
{
    QString output_string = "";
    iNodo inode;
    //obtengo el primer bit
    char bit;
    int count = 0;
    do
    {   //actualizando el inodo en el bm
        fseek(file,sb.s_bm_inode_start + count,SEEK_SET);
        fread(&bit, sizeof(char), 1, file);
        if(bit == '\0') break;
        //obteniendo el inodo
        fseek(file,sb.s_inode_start + count * static_cast<int>(sizeof (iNodo)),SEEK_SET);
        fread(&inode, sizeof(iNodo), 1, file);
        output_string += get_blocks_report(inode,file);
        count++;
    }while(bit != '\0');
    return output_string;
}

void tools::set_count(int cont)
{
    report_count = cont;
}

QString tools::get_folder_block_report(Bloque_Carpeta folder_block)
{
    QString output_inode = "table"+QString::number(report_count)+" [shape=none, label=<";
    output_inode += "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">";
    output_inode +=  "<tr><td  bgcolor=\"green\"> <font color=\"white\">Bloque Carpeta</font> </td></tr>";
    int count = 0;
    for (int i = 0;i<4;i++)
    {
        if(folder_block.b_content[i]->b_inodo != 0)
        {
            output_inode +=  "<tr><td port=\""+QString::number(count)+"\" align=\"left\">b_name:"+QString(folder_block.b_content[i]->b_name)+"   </td></tr>";
            output_inode +=  "<tr><td port=\""+QString::number(count + 1)+"\" align=\"left\">b_inode:"+QString::number(folder_block.b_content[i]->b_inodo)+" </td></tr>";
        }
        else
        {
            output_inode +=  "<tr><td port=\""+QString::number(count)+"\" align=\"left\">b_name:vacio</td></tr>";
            output_inode +=  "<tr><td port=\""+QString::number(count + 1)+"\" align=\"left\">b_inode:0 </td></tr>";
        }
        count += 2;
    }
    output_inode += "</table>>];\n\n";
    report_count++;
    return  output_inode;
}

QString tools::get_file_block_report(Bloque_Archivo file_block)
{
    // Leyendo el contenido del bloque archivo
    QString cont = "";
    for(int i = 0; i < 64; i++) if(file_block.b_content[i] != '\0') cont.append(file_block.b_content[i]);
    // Generando el string de graphviz
    QString output_inode = "table"+QString::number(report_count)+" [shape=none, label=<";
    output_inode += "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">";
    output_inode +=  "<tr><td  bgcolor=\"green\"> <font color=\"yellow\">Bloque Archivo</font> </td></tr>";
    output_inode += "<tr><td port=\"0\" align=\"right\">"
            +cont+"</td></tr>";

    output_inode += "</table>>];\n\n";
    report_count++;
    return  output_inode;
}

QString tools::get_pointer_string_report(Bloque_apuntador pointer)
{
    QString output_inode = "table"+QString::number(report_count)+" [shape=none, label=<";
    output_inode += "<table border=\"0\" cellborder=\"1\" cellspacing=\"0\" cellpadding=\"4\">";
    output_inode +=  "\n<tr><td  bgcolor=\"green\"> <font color=\"orange\">Bloque Apuntador</font> </td></tr>\n";
    for (int i = 0;i<16;i++)
    {
        output_inode += "<tr><td port=\"0\" align=\"right\">b_Pointers["+QString::number(i)+"]:"
                +QString::number(pointer.b_pointers[i])+"</td></tr>\n";
    }
    output_inode += "</table>>];\n\n";
    report_count++;
    return  output_inode;
}

QString tools::switch_blocks(int type,FILE *file, int dir)
{
    QString output_string = "";
    if(type == 0)
    {
        Bloque_Carpeta folder_block;
        fseek(file,dir,SEEK_SET);
        fread(&folder_block,sizeof (Bloque_Carpeta),1,file);
        output_string += get_folder_block_report(folder_block);
        //ademas del reporte tiene que reportar a lo que apuntan sus carpetas
        int current_count = report_count;
        for(int i = 0; i < 4; i++)
        {
            if(folder_block.b_content[i]->b_inodo != 0)
            {
                iNodo inode;
                fseek(file,folder_block.b_content[i]->b_inodo,SEEK_SET);
                fread(&inode,sizeof (iNodo),1,file);
                output_string += "table"+QString::number(current_count - 1)
                        +":" + QString::number(i * 2)
                        + "-> table" + QString::number(report_count) + ";\n";
                output_string += get_tree_report(inode,file);
            }
        }
    }
    else
    {
        Bloque_Archivo file_block;
        fseek(file,dir,SEEK_SET);
        fread(&file_block,sizeof (Bloque_Archivo),1,file);

        output_string += get_file_block_report(file_block);
    }
    return output_string;
}

QString tools::get_pointer_report(int dir, FILE *file,int type, int recursevely)
{
    //se carga el bloque a ram
    Bloque_apuntador temp;
    //leer el apuntador
    fseek(file,dir,SEEK_SET);
    fread(&temp,sizeof (Bloque_apuntador),1,file);
    //el reporte
    QString output_string = get_pointer_string_report(temp);
    int actual_table = report_count - 1;
    //recorrer sus 16 espacios
    for (int j = 0;j < 16; j++)
    {
        //si es distinto de 0 ver si tiene contenido vacio
        if(temp.b_pointers[j] != 0)
        {
            switch (recursevely)
            {
            case 1:
                output_string += "table"+QString::number(actual_table)
                        +":" + QString::number(j)
                        + "-> table" + QString::number(report_count) + ";\n";
                output_string += switch_blocks(type,file,temp.b_pointers[j]);
                break;
            case 2:
                output_string += get_pointer_report(temp.b_pointers[j],file,type,1);
                break;
            case 3:
                output_string += get_pointer_report(temp.b_pointers[j],file,type,2);
                break;
            }
        }
    }
    return output_string;
}

QString tools::get_pointer_blocks_report(int dir, FILE *file,int type, int recursevely)
{
    //se carga el bloque a ram
    Bloque_apuntador temp;
    //leer el apuntador
    fseek(file,dir,SEEK_SET);
    fread(&temp,sizeof (Bloque_apuntador),1,file);
    //el reporte
    QString output_string = get_pointer_string_report(temp);
    int actual_table = report_count - 1;
    //recorrer sus 16 espacios
    for (int j = 0;j < 16; j++)
    {
        //si es distinto de 0 ver si tiene contenido vacio
        if(temp.b_pointers[j] != 0)
        {
            switch (recursevely)
            {
            case 1:
                output_string += "table"+QString::number(actual_table)
                        +":" + QString::number(j)
                        + "-> table" + QString::number(report_count) + ";\n";
                output_string += switch_block_report(type,file,temp.b_pointers[j]);
                break;
            case 2:
                output_string += get_pointer_report(temp.b_pointers[j],file,type,1);
                break;
            case 3:
                output_string += get_pointer_report(temp.b_pointers[j],file,type,2);
                break;
            }
        }
    }
    return output_string;
}

QString tools::get_tree_report(iNodo inode,FILE* file)
{
    QString output_file = get_inode_report(inode);
    int actual_table = report_count - 1;
    for (int i = 0;i<12;i++)
    {
        if(inode.i_block[i] != 0)
        {
            output_file += "table"+QString::number(actual_table)
                    +":" + QString::number(6 + i)
                    + "-> table" + QString::number(report_count) + ";\n";
            output_file += switch_blocks(inode.i_type,file,inode.i_block[i]);
        }
        // si encuentra uno vacio entre los primeros 12 ya no pasar a ver los indirectos
        else return output_file;
    }
    if(inode.i_block[12] != 0)
    {
        output_file += get_pointer_report(inode.i_block[12],file,inode.i_type,1);
    }
    if(inode.i_block[13] != 0)
    {
        get_pointer_report(inode.i_block[13],file,inode.i_type,2);
    }
    if(inode.i_block[14] != 0)
    {
        get_pointer_report(inode.i_block[14],file,inode.i_type,3);
    }
    return output_file;
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

void append_to_full_file(string *response, Bloque_apuntador pointer, FILE* file)
{
    Bloque_Archivo content_block;
    for(auto dir : pointer.b_pointers)
    {
        if(dir != 0)
        {
            fseek(file,dir,SEEK_SET);
            fread(&content_block,sizeof (Bloque_Archivo),1,file);
            *response += content_block.b_content;
        }
    }
}

void append_pointers_to_full_file(string *response, Bloque_apuntador pointer, FILE* file)
{
    Bloque_apuntador content_block;
    for(auto dir : pointer.b_pointers)
    {
        if(dir != 0)
        {
            fseek(file,dir,SEEK_SET);
            fread(&content_block,sizeof (Bloque_apuntador),1,file);
            append_to_full_file(response,content_block,file);
        }
    }
}

int tools::make_new_direct_block(QString txt, iNodo *inode, SuperBloque sb, FILE * file)
{
    //cargando el texto a escribir
    string text;
    if(txt.size() > 64) text = txt.right(64).toStdString();
    else text = txt.toStdString();
    //creando un nuevo bloque archivo
    Bloque_Archivo *content = new Bloque_Archivo(text.c_str());
    int pos = get_free_block_index_by_bitmap(sb,file,true);
    //guardarlo
    fseek(file,pos,SEEK_SET);
    fwrite(&*content,sizeof (Bloque_Archivo),1,file);
    //apuntarlo
    for(int i = 0; i < 12; i++) if(inode->i_block[i] == 0)
    {
        inode->i_block[i] = pos;
        break;
    }
    if((txt.size() - 64) <= 0) return 0;
    else return txt.size() - 64;
}

int tools::append_text_in_block(QString txt, FILE *file, int dir)
{
    //si la direccion es vacia regresar
    if(dir == 0) return -1;
    //este metodo regresa -1 si logro concatenar todo el texto de lo contrario regresa el faltante
    //el bloque donde se quiere concatenar texto
    Bloque_Archivo content;
    fseek(file,dir,SEEK_SET);
    fread(&content,sizeof (Bloque_Archivo),1,file);
    //obtener el espacio libre en el bloque para ver si aun cabe
    int free_space = block_empty_space(content.b_content);
    //si ya esta lleno crear un nuevo bloque
    if(free_space == 0) return -1;
    int writed_space = 0;
    //escribiendo el faltante
    for (int j = 0; j < free_space; j++)
    {
        //si la string no ha terminado se concatena
        if(j < txt.size())
        {
            content.b_content[64 - free_space + j] = txt[j].toLatin1();
            //espacio escrito el el bloque es igual a free space si se lleno el bloque
            writed_space++;
        }
    }
    //guardar el bloque actualizado
    fseek(file,dir,SEEK_SET);
    fwrite(&content,sizeof (Bloque_Archivo),1,file);
    //regresa lo que no se logro escribir (0 si se escribio completa)
    return txt.size() - writed_space;
}

int tools::append_text_in_2d(QString txt, FILE *file, int dir)
{
    //leer el apuntador
    Bloque_apuntador pointer;
    fseek(file,dir,SEEK_SET);
    fread(&pointer, sizeof(Bloque_apuntador), 1, file);
    //buscar el ultimo nodo usado
    int last = 0;
    for(auto dir : pointer.b_pointers) if(dir != 0) last++;
    //si se llena el ultimo regresar lo que falta de la string
    if(last == 16) return append_text_in_pointer(txt,file,dir);
    //de lo contrario llenar todos
    else
    {
        //meter todo el texto
        int remain_text = txt.size();
        //llenar los espacios faltantes
        for(int i = last - 1; i < 16; i++)
        {
            //se escribio todo
            if(remain_text == 0) return 0;
            //lo que falta escribir del texto
            remain_text = append_text_in_pointer(txt.right(remain_text),file,pointer.b_pointers[i]);
        }
        //regresar lo que falto
        return remain_text;
    }
}

int tools::append_text_in_pointer(QString txt, FILE *file, int dir)
{
    //leer el apuntador
    Bloque_apuntador pointer;
    fseek(file,dir,SEEK_SET);
    fread(&pointer, sizeof(Bloque_apuntador), 1, file);
    //buscar el ultimo nodo usado
    int last = 0;
    for(auto dir : pointer.b_pointers) if(dir != 0) last++;
    //si se llena el ultimo regresar lo que falta de la string
    if(last == 16) return append_text_in_block(txt,file,dir);
    //de lo contrario llenar todos
    else
    {
        //meter todo el texto
        int remain_text = txt.size();
        //llenar los espacios faltantes
        for(int i = last - 1; i < 16; i++)
        {
            //se escribio todo
            if(remain_text == 0) return 0;
            //lo que falta escribir del texto
            remain_text = append_text_in_block(txt.right(remain_text),file,pointer.b_pointers[i]);
        }
        //regresar lo que falto
        return remain_text;
    }
}

int tools::append_text_in_a_new_2d(iNodo *inode, QString new_text, FILE *file, SuperBloque sb)
{
    //el nuevo apuntador
    Bloque_apuntador *pointer = new Bloque_apuntador();
    Bloque_apuntador *pointer2 = new Bloque_apuntador();
    int indirect_pos = get_free_block_index_by_bitmap(sb,file,true);
    int indirect_pos2 = get_free_block_index_by_bitmap(sb,file,true);
    //escribir un bloque entero
    Bloque_Archivo* new_blck = new Bloque_Archivo();
    for(int i = 0; i < 64; i++) if(i < new_text.size()) new_blck->b_content[i] = new_text[i].toLatin1();
    //guardarlo
    int content = get_free_block_index_by_bitmap(sb,file,true);
    fseek(file,content,SEEK_SET);
    fwrite(&*new_blck, sizeof(Bloque_Archivo), 1, file);
    //apuntar el apuntador al archivo
    pointer2->b_pointers[0] = content;
    pointer->b_pointers[0] = indirect_pos2;
    //guardar el bloque indirecto
    fseek(file,indirect_pos,SEEK_SET);
    fwrite(&*pointer, sizeof(Bloque_apuntador), 1, file);
    free(pointer);
    fseek(file,indirect_pos2,SEEK_SET);
    fwrite(&*pointer2, sizeof(Bloque_apuntador), 1, file);
    free(pointer2);
    //apuntarle el inodo al nuevo apuntador
    inode->i_block[12] = indirect_pos;
    //concatenarle el texto a los 16 bloques restantes si todavia queda
    if((new_text.size() - 64) > 0)
    {
        //todavia queda texto concatenarlo en un puntero existente
        return append_text_in_2d(new_text.right(new_text.size() - 64),file,indirect_pos);
    }
    return 0;
}

int tools::append_text_in_a_new_pointer(iNodo *inode, QString new_text, FILE *file, SuperBloque sb)
{
    //el nuevo apuntador
    Bloque_apuntador *pointer = new Bloque_apuntador();
    int indirect_pos = get_free_block_index_by_bitmap(sb,file,true);
    //escribir un bloque entero
    Bloque_Archivo* new_blck = new Bloque_Archivo();
    for(int i = 0; i < 64; i++) if(i < new_text.size()) new_blck->b_content[i] = new_text[i].toLatin1();
    //guardarlo
    int content = get_free_block_index_by_bitmap(sb,file,true);
    fseek(file,content,SEEK_SET);
    fwrite(&*new_blck, sizeof(Bloque_Archivo), 1, file);
    //apuntar el apuntador al archivo
    pointer->b_pointers[0] = content;
    //guardar el bloque indirecto
    fseek(file,indirect_pos,SEEK_SET);
    fwrite(&*pointer, sizeof(Bloque_apuntador), 1, file);
    free(pointer);
    //apuntarle el inodo al nuevo apuntador
    inode->i_block[12] = indirect_pos;
    //concatenarle el texto a los 16 bloques restantes si todavia queda
    if((new_text.size() - 64) > 0)
    {
        //todavia queda texto concatenarlo en un puntero existente
        return append_text_in_pointer(new_text.right(new_text.size() - 64),file,indirect_pos);
    }
    return 0;
}

int tools::append_text_to_last_block(iNodo *inode, QString new_text, FILE *file, SuperBloque sb)
{
    //buscar el ultimo bloque
    for (int i = 0; i < 12; i++)
    {
        //si se encuentra aqui el ultimo bloque aun es directo
        if(inode->i_block[i] != 0 && inode->i_block[i+1] == 0)
        {
            //el ultimo bloque usado es i llena el bloque
            int result = append_text_in_block(new_text,file,inode->i_block[i]);
            if(result > -1) return result;
            //si el resultado es -1 es porque se lleno el bloque y hay que crear uno nuevo
            else if(i != 11) return make_new_direct_block(new_text,inode,sb,file);
        }
    }
    //si no estaba en los directos entonces buscarlo en los indirectos
    if(inode->i_block[12] != 0 && inode->i_block[13] == 0)
    {
        //el bloque 12 esta creado entonces solo concatenar
        return append_text_in_pointer(new_text,file,inode->i_block[12]);
    }
    else if(inode->i_block[12] == 0)
    {
        return append_text_in_a_new_pointer(inode,new_text,file,sb);
    }
    else if(inode->i_block[13] != 0 && inode->i_block[14] == 0)
    {
        return append_text_in_2d(new_text,file,inode->i_block[13]);
    }
    else if(inode->i_block[13] == 0)
    {
        return append_text_in_a_new_2d(inode,new_text,file,sb);
    }
    else if (inode->i_block[14] != 0)
    {

    }
    else if (inode->i_block[14] == 0)
    {

    }
}

bool tools::append_text_in_inode(int inode_dir, QString new_text, FILE *file, SuperBloque sb)
{
    //leer el inodo
    iNodo inode;
    fseek(file,inode_dir,SEEK_SET);
    fread(&inode,sizeof (iNodo),1,file);
    //concatenar toda la cadena al inodo
    int remain_text = new_text.size();
    //concatenar toda la cadena
    while(remain_text > 0) remain_text = append_text_to_last_block(&inode,new_text.right(remain_text),file,sb);
    //guardar cambios
    inode.i_size += new_text.size();
    inode.i_mtime = time(nullptr);
    fseek(file,inode_dir,SEEK_SET);
    fwrite(&inode,sizeof (iNodo),1,file);
    return true;
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
            // concatenar solo 64 espacios
            for(int k = 0; k < 64; k++)
            {
                if(content_block.b_content[k] != '\0')
                    response += content_block.b_content[k];
            }
        }
    }
    if(inode.i_block[12] != 0)
    {
        Bloque_apuntador pointer;
        fseek(file,inode.i_block[12],SEEK_SET);
        fread(&content_block,sizeof (Bloque_Archivo),1,file);
        append_to_full_file(&response,pointer,file);
        //en la siguiente dimension
        if(inode.i_block[13] != 0)
        {
            fseek(file,inode.i_block[13],SEEK_SET);
            fread(&content_block,sizeof (Bloque_Archivo),1,file);
            append_pointers_to_full_file(&response,pointer,file);
            //en la siguiente dimension
            if(inode.i_block[14] != 0)
            {
                fseek(file,inode.i_block[14],SEEK_SET);
                fread(&content_block,sizeof (Bloque_Archivo),1,file);
                append_pointers_to_full_file(&response,pointer,file);
                Bloque_apuntador pointers2;
                for(auto dir : pointer.b_pointers)
                {
                    if(dir != 0)
                    {
                        fseek(file,dir,SEEK_SET);
                        fread(&pointers2,sizeof (Bloque_apuntador),1,file);
                        append_pointers_to_full_file(&response,pointers2,file);
                    }
                }
            }
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
        //Actualizando el superbloque
        sb.s_blocks_count++;
        sb.s_free_blocks_count--;
        sb.s_first_blo = sb.s_bm_block_start + count;
        int sb_start = sb.s_bm_inode_start - static_cast<int>(sizeof (SuperBloque));
        if(sb.s_filesystem_type == 3) sb_start -= static_cast<int>(sizeof (Bitacora));
        fseek(file,sb_start,SEEK_SET);
        fwrite(&sb, sizeof(SuperBloque), 1, file);
    }
    //regresar la posicion del bloque vacio
    return sb.s_block_start + count * static_cast<int>(sizeof (Bloque_Archivo));
}

int tools::get_free_inode_index_by_bitmap(SuperBloque sb, FILE *file, bool overwrite)
{
    //obtengo el primer bit
    char bit;
    fseek(file,sb.s_bm_inode_start,SEEK_SET);
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
        fseek(file,sb.s_bm_inode_start + count,SEEK_SET);
        fwrite(&unity, sizeof(char), 1, file);

        sb.s_inodes_count++;
        sb.s_free_inodes_count--;
        sb.s_firts_ino = sb.s_bm_inode_start + count;
        int sb_start = sb.s_bm_inode_start - static_cast<int>(sizeof (SuperBloque));
        if(sb.s_filesystem_type == 3) sb_start -= static_cast<int>(sizeof (Bitacora));
        fseek(file,sb_start,SEEK_SET);
        fwrite(&sb, sizeof(SuperBloque), 1, file);
    }
    //regresar la posicion del bloque vacio
    return sb.s_inode_start + count * static_cast<int>(sizeof (iNodo));
}

int tools::block_empty_space(char b_content[64])
{
    int count = 0;
    for (int i = 0; i < 64; i++) {
        if(b_content[i] == '\0') count++;
    }
    return count;
}

int tools::get_free_block(int dir,FILE *file)
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
            return j;
    }
    return -1;
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
        else if(get_free_block(dir,file) != -1) return dir;
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
            // si el index es 0 regresar el anterior porque es el ultimo lleno
            if(i_block[i] == 0)
            {
                if(i != 0) block_index = i - 1;
                else block_index = 0;
                break;
            }
            else if(i == 11)
            {
                // si ya va por el index 11
                block_index = 11;
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
                    //si encuentra uno vacio entonces devolver el index
                    if(get_free_block(i_block[12],file) != -1) return i;
                }
                break;
            case 13:
                //si paso a este caso quiere decir que el 12 esta lleno
                //si tiene bloques libres devolverlo si no continuar
                if(i_block[13] == 0)
                {
                    block_index = i - 1;
                    break;
                }
                if(get_free_pointer(i_block[13],file) != -1) return i;
                break;
            case 14:
                //si esta vacio aun hay slots vacios en el pasado
                if(i_block[14] == 0) return i - 1;
                Bloque_apuntador temp;
                //leer el apuntador
                fseek(file,i_block[14],SEEK_SET);
                fread(&temp,sizeof (Bloque_apuntador),1,file);
                for(auto dir : temp.b_pointers)
                {
                    //si alguna de sus casillas esta vacia devolverlo
                    if(dir == 0) return i;
                    //si alguno de sus hijos tiene punteros libres devolverlo
                    else if(get_free_pointer(dir,file) != -1) return i;
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

string tools::get_report_format(QString path)
{
    QString comp = path.right(4).toUpper().simplified();

    if(comp.toUpper() == ".PDF"){
        return "PDF";
    }
    else if(comp.toUpper() == ".PNG"){
        return "PNG";
    }
    else if(comp.toUpper() == ".JPG"){
        return "JPG";
    }
    else if(comp.toUpper() == ".TXT"){
        return "TXT";
    }
    return "";
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

int tools::get_folder_direction(FILE *file,int dir, QString folder_name)
{
    Bloque_Carpeta file_content;
    //leer el bloque y ver en sus 4 posiciones
    fseek(file,dir,SEEK_SET);
    fread(&file_content, sizeof(Bloque_Carpeta), 1, file);
    //recorriendo 4 posiciones del bloque
    for(auto cont : file_content.b_content)
    {
        //si esta vacio no buscar
        if(cont->b_inodo == 0) continue;
        //se encuentra el archivo o carpeta
        if(folder_name == cont->b_name)
        {
            //La direccion del inodo
            return cont->b_inodo;
        }
    }
    //no se encontro el folder en este bloque
    return 0;
}

int tools::find_inode(FILE* file, SuperBloque sb, QString path)
{
    iNodo result;
    fseek(file,sb.s_inode_start,SEEK_SET);
    fread(&result, sizeof(iNodo), 1, file);
    //separar la ruta para obtener las carpetas
    QStringList lines = path.split('/');
    //para iterar los niveles en el arbol
    int iterator = 0;
    //para guardar la direccion del inodo que buscamos modificar
    int direction = 0;
    //subiendo en los niveles
    while (iterator < lines.count())
    {
        //no tomar en cuenta rutas vacias
        if(lines[iterator] == "")
        {
            iterator++;
            continue;
        }
        //setear a 0 para encontrar el nuevo nivel
        direction = search_path_in_inode(result,file,lines[iterator]);
        //si no lo encuentra
        if(direction == 0)
        {
            return 0;
        }
        else
        {
            //actualizar el inodo para subir de nivel
            fseek(file,direction,SEEK_SET);
            fread(&result, sizeof(iNodo), 1, file);
        }
        //aumentando el nivel del arbol
        iterator++;
    }
    //Se muestra el contenido del archivo
    return direction;
}

int tools::search_path_in_pointer(int pointer_dir,FILE *file,QString name)
{
    Bloque_apuntador pointer;
    fseek(file,pointer_dir,SEEK_SET);
    fread(&pointer,sizeof (Bloque_apuntador),1,file);
    //buscar en las 16 posiciones del puntero
    for(int dir : pointer.b_pointers)
    {
        if(dir != 0)
        {
            //buscar la direccion en los 16 bloques de carpeta
            int direction = get_folder_direction(file,dir,name);
            //si se encontro el folder en el bloque
            if(direction != 0)
            {
                return direction;
            }
        }
    }
    return 0;
}

int tools::search_path_in_2d(int pointer_dir,FILE *file,QString name)
{
    Bloque_apuntador pointer;
    fseek(file,pointer_dir,SEEK_SET);
    fread(&pointer,sizeof (Bloque_apuntador),1,file);
    //buscar en las 16 posiciones del puntero
    for(int dir : pointer.b_pointers)
    {
        if(dir != 0)
        {
            //buscar la direccion en los 16 bloques de puntero
            int direction = search_path_in_pointer(dir,file,name);
            //si se encontro el folder en el bloque
            if(direction != 0)
            {
                return direction;
            }
        }
    }
    return 0;
}

int tools::search_path_in_3d(int pointer_dir,FILE *file,QString name)
{
    Bloque_apuntador pointer;
    fseek(file,pointer_dir,SEEK_SET);
    fread(&pointer,sizeof (Bloque_apuntador),1,file);
    //buscar en las 16 posiciones del puntero
    for(int dir : pointer.b_pointers)
    {
        if(dir != 0)
        {
            //buscar la direccion en los 16 bloques de puntero
            int direction = search_path_in_2d(dir,file,name);
            //si se encontro el folder en el bloque
            if(direction != 0)
            {
                return direction;
            }
        }
    }
    return 0;
}

int tools::search_path_in_indirect_blocks(iNodo inode, FILE* file, QString name)
{
    int direction = 0;
    //si el primer indirecto no esta vacio
    if(inode.i_block[12] != 0)
    {
        //buscandolo en el primer indirecto
        direction = search_path_in_pointer(inode.i_block[12],file,name);
        if(direction != 0)
        {
            return direction;
        }
        else
        {
            if(inode.i_block[13] != 0)
            {
                //buscando en el segundo indirecto
                direction = search_path_in_2d(inode.i_block[13],file,name);
                if(direction != 0)
                {
                    return  direction;
                }
                else
                {
                    //buscando en el tercer indirecto
                    if(inode.i_block[12] != 0)
                    {
                        direction = search_path_in_3d(inode.i_block[13],file,name);
                        if(direction != 0)
                        {
                            return direction;
                        }
                    }
                }
            }
        }
    }
    return direction;
}

int tools::search_path_in_inode(iNodo inode, FILE *file, QString name)
{
    int direction = 0;
    //recorrer todos los apuntadores directos del inodo
    for (int i = 0;i < 12;i++)
    {
        //si el apuntador directo tiene algo
        if(inode.i_block[i] != 0)
        {
            direction = get_folder_direction(file,inode.i_block[i],name);
            //si se encontro en el bloque ya no buscar en el siguiente
            if(direction != 0)
            {
                return direction;
            }
        }
    }
    //si aun no se ha encontrado en los directos
    if(direction != 0)
    {
        direction = search_path_in_indirect_blocks(inode,file,name);
        if(direction != 0)
        {
            return direction;
        }
    }
    return direction;
}

int tools::make_folder_in_inode(int inode_dir, FILE *file, QString name, int size, SuperBloque sb)
{
    //buscando el ultimo bloque con contenido libre
    int free_block = get_last_folder_block_with_free_space(inode_dir,file,sb);
    //si ya no hay entonces esta lleno el inodo
    if(free_block != -1)
    {
        //leyendo el inodo
        iNodo inode;
        fseek(file,inode_dir,SEEK_SET);
        fread(&inode,sizeof (iNodo),1,file);
        int new_inode = make_folder_in_block(file,free_block,inode,size,sb,name.toStdString().c_str());
        //actualizando propiedades del inodo
        inode.i_size += size;
        inode.i_mtime = time(nullptr);
        //guardando cambios
        fseek(file,inode_dir,SEEK_SET);
        fwrite(&inode,sizeof (iNodo),1,file);
        return new_inode;
    }
    return 0;
}

int tools::make_folder_in_block(FILE *file,int dir,iNodo parent, int size, SuperBloque sb,const char * name)
{
    Bloque_Carpeta file_content;
    //leer el bloque y ver en sus 4 posiciones
    fseek(file,dir,SEEK_SET);
    fread(&file_content, sizeof(Bloque_Carpeta), 1, file);
    //recorriendo 4 posiciones del bloque
    for(auto cont : file_content.b_content)
    {
        //si esta vacio meterlo ahi
        if(cont->b_inodo == 0)
        {
            //creacion del inodo
            iNodo *new_inode = new iNodo(parent.i_uid,parent.i_gid,size,0,664);
            //direccion en el disco a guardarlo
            int inode_dir = get_free_inode_index_by_bitmap(sb,file,true);
            //metiendole el primer bloque de carpeta
            new_inode->i_block[0] = make_new_folder_block(file,sb);
            //guardando el inodo
            fseek(file,inode_dir,SEEK_SET);
            fwrite(&*new_inode, sizeof(iNodo), 1, file);
            free(new_inode);
            //apuntar el nuevo inodo
            cont->b_inodo = inode_dir;
            strcpy(cont->b_name,name);
            //guardar cambios en el bloque
            fseek(file,dir,SEEK_SET);
            fwrite(&file_content, sizeof(Bloque_Carpeta), 1, file);
            return inode_dir;
        }
    }
    return 0;
}

QString generate_text(int size)
{
    QString response = "";
    char count = '0';
    for(int i = 0; i < size; i++)
    {
        response += count;
        if(count < '9') count++;
        else count = '0';
    }
    return response;
}

int tools::make_new_folder_block(FILE *file, SuperBloque sb)
{
    //crear nuevo bloque carpeta
    Bloque_Carpeta folder_block;
    for(int i = 0; i < 4; i++) folder_block.b_content[i] = new content();
    //la nueva direccion del bloque carpeta
    int folder_dir = get_free_block_index_by_bitmap(sb,file,true);
    // guardarlo en archivo
    fseek(file,folder_dir,SEEK_SET);
    fwrite(&folder_block,sizeof (Bloque_Carpeta),1,file);
    return folder_dir;
}

int tools::make_new_pointer_block(FILE* file, SuperBloque sb)
{
    //se tiene que crearle un apuntador
    Bloque_apuntador * pointer = new Bloque_apuntador();
    int pointer_index = get_free_block_index_by_bitmap(sb,file,true);
    //apuntar un nuevo bloque de carpeta
    pointer->b_pointers[0] = make_new_folder_block(file,sb);
    //guardarlo
    fseek(file,pointer_index,SEEK_SET);
    fwrite(&*pointer,sizeof (Bloque_apuntador),1,file);
    return pointer_index;
}

bool has_free_content(int dir, FILE *file)
{
    //leer cada bloque carpeta del apuntador
    Bloque_Carpeta folder_block;
    fseek(file,dir,SEEK_SET);
    fread(&folder_block,sizeof (folder_block),1,file);
    //si tiene alguno vacio entonces devolverlo
    for(auto cont : folder_block.b_content) if(cont->b_inodo == 0) return true;
    return false;
}

int tools::get_block_with_free_content(int dir, FILE *file,SuperBloque sb, int recursevely)
{
    //tomara como libre el apuntador si encuentra uno vacio
    Bloque_apuntador temp;
    //leer el apuntador
    fseek(file,dir,SEEK_SET);
    fread(&temp,sizeof (Bloque_apuntador),1,file);
    //recorrer sus bloques carpetas y ver si alguno tiene contenido vacio
    for (int j = 0;j < 16; j++)
    {
        //si es distinto de 0 ver si tiene contenido vacio
        if(temp.b_pointers[j] != 0)
        {
            switch (recursevely)
            {
            case 1:
                if(has_free_content(temp.b_pointers[j],file)) return temp.b_pointers[j];
                break;
            case 2:
                return get_block_with_free_content(temp.b_pointers[j],file,sb,1);
            case 3:
                return get_block_with_free_content(temp.b_pointers[j],file,sb,2);
            }
        }
        // crear un bloque de carpeta en la ultima posicion libre y regresar su direccion
        else
        {
            switch (recursevely)
            {
            case 1:
                temp.b_pointers[j] = make_new_folder_block(file,sb);
                break;
            case 2:
                temp.b_pointers[j] = make_new_pointer_block(file,sb);
                break;
            case 3:
                temp.b_pointers[j] = make_new_pointer_block(file,sb);
                break;
            }
            //guardar cambios
            fseek(file,dir,SEEK_SET);
            fwrite(&temp,sizeof (Bloque_apuntador),1,file);
            return  temp.b_pointers[j];
        }
    }
    //ya esta todo lleno
    return -1;
}

int tools::get_last_folder_block_with_free_space(int inode_dir, FILE *file, SuperBloque sb)
{
    //leer el inodo
    iNodo inode;
    fseek(file,inode_dir,SEEK_SET);
    fread(&inode,sizeof (iNodo),1,file);
    //crear carpeta en el respectivo nivel
    int last_block_index = get_last_block_index(inode.i_block,file);
    //si es directo el ultimo index
    if(last_block_index < 12)
    {
        //si esta lleno el bloque carpeta en sus 4 espacios
        if(!has_free_content(inode.i_block[last_block_index],file))
        {
            if(last_block_index == 11) last_block_index++;
            else
            {
                //crear un nuevo bloque de carpeta
                int folder_dir = make_new_folder_block(file,sb);
                //apuntarlo en el inodo
                inode.i_block[last_block_index + 1] = folder_dir;
                //guardar el inodo
                fseek(file,inode_dir,SEEK_SET);
                fwrite(&inode,sizeof (iNodo),1,file);
                return folder_dir;
            }
        } else return inode.i_block[last_block_index];
    }
    if(last_block_index == 12)
    {
        if(inode.i_block[last_block_index] != 0)
        {
            //obtener el bloque con contenido libre en el apuntador
            int free_block_dir = get_block_with_free_content(inode.i_block[last_block_index],file,sb,1);
            //si esta lleno
            if(free_block_dir == -1) last_block_index++;
            else return free_block_dir;
        }
        else
        {
            //crear un nuevo bloque apuntador
            inode.i_block[last_block_index] = make_new_pointer_block(file,sb);
            //guardar el inodo
            fseek(file,inode_dir,SEEK_SET);
            fwrite(&inode,sizeof (iNodo),1,file);
            //devuelve el bloque carpeta con espacio libre
            return get_block_with_free_content(inode.i_block[last_block_index],file,sb,1);
        }
    }
    if(last_block_index == 13)
    {
        if(inode.i_block[last_block_index] != 0)
        {
            //obtener el bloque con contenido libre en el apuntador
            int free_block_dir = get_block_with_free_content(inode.i_block[last_block_index],file,sb,2);
            //si esta lleno
            if(free_block_dir == -1) last_block_index++;
            else return free_block_dir;
        }
        else
        {
            //crear un nuevo bloque apuntador
            inode.i_block[last_block_index] = make_new_pointer_block(file,sb);
            //guardar el inodo
            fseek(file,inode_dir,SEEK_SET);
            fwrite(&inode,sizeof (iNodo),1,file);
            //devuelve el bloque carpeta con espacio libre
            return get_block_with_free_content(inode.i_block[last_block_index],file,sb,2);
        }
    }
    if(last_block_index == 14)
    {
        if(inode.i_block[last_block_index] != 0)
        {
            //obtener el bloque con contenido libre en el apuntador
            int free_block_dir = get_block_with_free_content(inode.i_block[last_block_index],file,sb,3);
            //si esta lleno
            if(free_block_dir == -1) last_block_index++;
            else return free_block_dir;
        }
        else
        {
            //crear un nuevo bloque apuntador
            inode.i_block[last_block_index] = make_new_pointer_block(file,sb);
            //guardar el inodo
            fseek(file,inode_dir,SEEK_SET);
            fwrite(&inode,sizeof (iNodo),1,file);
            //devuelve el bloque carpeta con espacio libre
            return get_block_with_free_content(inode.i_block[last_block_index],file,sb,3);
        }
    }
    return 0;
}

int tools::make_file_in_block(FILE *file,int dir,iNodo parent, int size, SuperBloque sb,const char * name,QString content)
{
    Bloque_Carpeta file_content;
    //leer el bloque y ver en sus 4 posiciones
    fseek(file,dir,SEEK_SET);
    fread(&file_content, sizeof(Bloque_Carpeta), 1, file);
    //recorriendo 4 posiciones del bloque
    for(auto cont : file_content.b_content)
    {
        //si esta vacio meterlo ahi
        if(cont->b_inodo == 0)
        {
            //creacion del inodo
            iNodo *new_inode = new iNodo(parent.i_uid,parent.i_gid,size,1,664);
            //direccion en el disco a guardarlo
            int inode_dir = get_free_inode_index_by_bitmap(sb,file,true);
            //metiendole el primer bloque de archivo
            Bloque_Archivo *new_file_blck = new Bloque_Archivo();
            //espacio para guardar el primer bloque archivo
            int file_dir = get_free_block_index_by_bitmap(sb,file,true);
            //apuntando el bloque de carpeta
            new_inode->i_block[0] = file_dir;
            //guardando el inodo
            fseek(file,inode_dir,SEEK_SET);
            fwrite(&*new_inode, sizeof(iNodo), 1, file);
            free(new_inode);
            //guardando el bloque archivo
            fseek(file,file_dir,SEEK_SET);
            fwrite(&*new_file_blck, sizeof(Bloque_Carpeta), 1, file);
            free(new_file_blck);
            //llenar el archivo
            if(size > -1)
            {
                append_text_in_inode(inode_dir,generate_text(size),file,sb);
            }
            else
            {
                append_text_in_inode(inode_dir,content,file,sb);
            }
            //apuntar el nuevo inodo
            cont->b_inodo = inode_dir;
            strcpy(cont->b_name,name);
            //guardar cambios en el bloque
            fseek(file,dir,SEEK_SET);
            fwrite(&file_content, sizeof(Bloque_Carpeta), 1, file);
            return inode_dir;
        }
    }
    return 0;
}
