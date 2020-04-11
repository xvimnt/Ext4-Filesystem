#include "plotter.h"
#include <QString>
#include <qdebug.h>
#include <fstream>
#include <iostream>
#include "node.h"

#include <QTextStream>
#include <QFile>
#include <QFileInfo>

graficador::graficador()
{
    this->contador = 0;
}

graficador::graficador(node *raiz)
{
    this->grafo = "";
    this->raiz = raiz;
    this->contador = 0;
}

QString graficador::graficar()
{
    grafo = "digraph G{";
    grafo += "node[shape=\"box\"];";
    grafo += "node0[label=\"" + escapar("[" +raiz->tipo +"," + QString::number(raiz->tipo_)+ "("+ QString::number(raiz->linea)+","+ QString::number(raiz->columna)+")"+","+raiz->valor + "]") + "\"];\n";
    this->contador = 1;
    recorrerAST("node0", raiz);
    grafo += "}";
    return grafo;
}

void graficador::recorrerAST(QString padre, node *hijo)
{
    if(hijo == nullptr) return;
    int x = 0;
    for (x = 0 ; x < hijo->hijos.count() ; x++)
    {
        node *node = hijo->hijos[x];
        QString nombreHijo = "node" +  QString::number(contador);//  this->contador;
        grafo += nombreHijo + "[label=\"" + escapar("[" +node->tipo +"," + QString::number(node->tipo_)+ "("+ QString::number(node->linea)+","+ QString::number(node->columna)+")" + "," + node->valor + "]") + "\"];\n";
        grafo += padre + "->" + nombreHijo + ";\n";
        contador++;
        recorrerAST(nombreHijo, node);
    }
}

QString graficador::escapar(QString cadena)
{
    cadena = cadena.replace("\\", "\\\\");
    cadena = cadena.replace("\"", "\\\"");
    return cadena;
}

void graficador:: generarImagen()
{
    QFileInfo fi("temp");

    graficar();
    QString grafoDOT = this->grafo;
    QString path = fi.absolutePath() +"/";
    QFile qFile(path+"grafo.txt");
    if(qFile.open(QIODevice::WriteOnly))
    {
        QTextStream out(&qFile); out << grafoDOT;
        qFile.close();
    }
    QString cadenaComando = "dot -Tjpg grafo.txt -o grafo.jpg ";
    system(cadenaComando.toUtf8().constData());
}

void graficador::generate_SB_table_content(string output_path,SuperBloque sb){

    //HTML Rows
    QString n1=QString::number(sb.s_magic);
    QString n2=QString::number(sb.s_mtime);
    QString n3=QString::number(sb.s_umtime);
    QString n4=QString::number(sb.s_first_blo);
    QString n5=QString::number(sb.s_firts_ino);
    QString n6=QString::number(sb.s_mnt_count);
    QString n7=QString::number(sb.s_block_size);
    QString n8=QString::number(sb.s_inode_size);
    QString n9 = QString::number(sb.s_block_start);
    QString n10=QString::number(sb.s_inode_start);
    QString n11=QString::number(sb.s_blocks_count);
    QString n12 = QString::number(sb.s_inodes_count);
    QString n13=QString::number(sb.s_bm_block_start);
    QString n14= QString::number(sb.s_bm_inode_start);
    QString n15 = QString::number(sb.s_filesystem_type);
    QString n16 = QString::number(sb.s_free_blocks_count);
    QString n17 = QString::number(sb.s_free_inodes_count);

    //Adding SB attributes
    QString dot= "            <TR> \n"
                 "                <TD>sb.s_magic</TD> \n"
                 "                <TD>"
                 "                    <TABLE BORDER=\"0\"> \n"
                 "                        <TR><TD>"+n1+"</TD></TR> \n"
                                                       "                    </TABLE> \n"
                                                       "                </TD> \n"
                                                       "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_mtime</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n2+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_umtime</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n3+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_first_blo</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n4+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";

    dot+="            <TR> \n"
         "                <TD>sb.s_firts_ino</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n5+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";

    dot+="            <TR> \n"
         "                <TD>sb.s_mnt_count</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n6+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_block_size</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n7+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_inode_size</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n8+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_block_start</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n9+"</TD></TR> \n"
                                               "                    </TABLE> \n"
                                               "                </TD> \n"
                                               "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> sb.s_inode_start</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n10+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> sb.s_blocks_count </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n11+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD>sb.s_inodes_count</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n12+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> sb.s_bm_block_start</TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n13+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> s_bm_inode_start </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n14+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> s_filesystem_type </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n15+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> s_free_blocks_count </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n16+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";

    dot+="            <TR> \n"
         "                <TD> s_free_inodes_count </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+n17+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";


    QString disco=output_path.c_str();
    int slash=disco.lastIndexOf("/");
    int last = disco.length();
    disco=disco.mid(slash+1,last);
    disco=disco+"/ParticionSb/";
    generate_graphviz_table("PDF",dot,output_path,disco,"SB");

}

static void GrapvizDisco(string extension, QString dot, string path){
    QString sb =
            "        digraph D {"
            "      node [fontname=\"Arial\"];"
            + dot+
            "}";
    ofstream file;
    file.open("grafo.dot");
    file<<sb.toUtf8().constData();
    file.close();
    // cout<<extension<<endl;
    string comando;
    if(extension=="jpg"){
        comando.append("dot -Tjpg ");
        comando.append("grafo.dot ");
        comando.append("-o ");
        comando.append(path);
        int res =system(comando.c_str());
        if(res==0){
            cout<<"\033[1;32m----------- Reporte Disco Creado Exitosamente -----\033[0m"<<endl;
        }

        // cout<<"dentro jpg"<<endl;
    }else if(extension=="pdf") {
        comando.append("dot -Tpdf ");
        comando.append("grafo.dot ");
        comando.append("-o ");
        comando.append(path);
        int res =system(comando.c_str());
        if(res==0)
        {
            printf("\n*********Reporte creado exitosamente**********\n");
        }
        //    cout<<"dentro pdf "<<endl;
    }else if(extension=="png"){
        comando.append("dot -Tpng ");
        comando.append("grafo.dot ");
        comando.append("-o ");
        comando.append(path);
        int res =system(comando.c_str());
        if(res==0){
            cout<<"\033[1;32m----------- Reporte Disco Creado Exitosamente -----\033[0m"<<endl;
        }
    }

}

void graficador::generate_dsk_table_content(string output_path,Mbr mbr,FILE *file){
    long tamanototal = mbr.size;
    //cout<<"tamaño del disco es "<<tamanototal<<endl;
    QString label[4];
    int numerovacias=0;int ocupado=0;
    for(int i=0;i<4;i++){
        if(mbr.partitions[i].size==0){
            numerovacias++;
        }else{
            if(mbr.partitions[i].type=='E'){
                long tam = mbr.partitions[i].size; //cout<<"mi tam"<<tam<<endl;
                long porcentaje = ((tam*100)/(tamanototal));
                ocupado+=porcentaje;
                label[i]=" { Extendida ";label[i].append(to_string(porcentaje).c_str());
                label[i].append("% |{ ");
                //logicas
                Ebr ebr;
                fseek(file,mbr.partitions[i].start,SEEK_SET);
                fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
                //buscando el nombre entre las particiones logicas
                while(ebr.next != -1 && ebr.next != 0)
                {
                    if(ebr.start == -1)
                    {
                        label[i].append("Libre |");
                    }
                    else label[i].append("EBR |");
                    long tam2 = ebr.size;
                    porcentaje = ((tam2*100)/(tam));
                    label[i].append(to_string(porcentaje).c_str());
                    label[i].append("% |");
                    fseek(file,ebr.next,SEEK_SET);
                    fread(&ebr, sizeof(Ebr), 1, file);//Lee el ebr raiz
                }

                label[i].append(" } }");
            }else {
                long tam = mbr.partitions[i].size;
                long porcentaje = ((tam*100)/(tamanototal));

                ocupado+=porcentaje;
                label[i]="Primaria ";label[i].append(to_string(porcentaje).c_str());
                label[i].append("%");
            }

        }
    }

    if(numerovacias>0){
        int cadauno=(100-ocupado-(static_cast<int>(sizeof (Mbr))*100/tamanototal))/numerovacias;
        for (int i=0;i<4;i++) {
            if(mbr.partitions[i].start==-1){
                label[i]=" LIBRE ";label[i].append(to_string(cadauno).c_str());
                label[i].append("%");
            }
        }
    }
    QString disco=output_path.c_str();
    int slash=disco.lastIndexOf("/");
    int last = disco.length();
    disco=disco.mid(slash+1,last);

    long tamanombr = sizeof (Mbr);
    long p = ((tamanombr*100)/(tamanototal));
    QString smb = "MBR ";smb.append(to_string(p).c_str());
    QString dot = "node_A [shape=record    label=\"";
    dot+=   smb+
            " |"+
            label[0]+
            " |"+
            label[1]+
            " |"+
            label[2]+
            "|"+
            label[3]+
            "\"];"+
            "\n"+
            "labelloc=\"t\"; \n"+
            "label=\""+disco+"\";";
    GrapvizDisco("png",dot,output_path);
}

void graficador::generate_mbr_table_content(string output_path,Mbr mbr, string choice)
{
    // genero las filas del html
    QString ssize=std::to_string(mbr.size).c_str();
    QString sdate = mbr.date;
    QString sig = std::to_string(mbr.signature).c_str();

    QString dot= "            <TR> \n"
                 "                <TD>MBR_size</TD> \n"
                 "                <TD>"
                 "                    <TABLE BORDER=\"0\"> \n"
                 "                        <TR><TD>"+ssize+"</TD></TR> \n"
                                                          "                    </TABLE> \n"
                                                          "                </TD> \n"
                                                          "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> MBR_FECHA_CREACION </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+sdate+"</TD></TR> \n"
                                                  "                    </TABLE> \n"
                                                  "                </TD> \n"
                                                  "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> MBR_DISK_SIGNATURE </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+sig+"</TD></TR> \n"
                                                "                    </TABLE> \n"
                                                "                </TD> \n"
                                                "            </TR> \n";

    for(int i=0;i<4;i++){
        // status
        QString status = std::to_string(mbr.partitions[i].status).c_str();
        // type
        string t(1,mbr.partitions[i].type);
        QString type = t.c_str();
        // fit
        QString fft = mbr.partitions[i].fit;
        //  start
        QString start = std::to_string(mbr.partitions[i].start).c_str();
        // size
        QString size = std::to_string(mbr.partitions[i].size).c_str();
        // name
        string newname=mbr.partitions[i].name;
        QString fname = newname.c_str();

        QString n1,n2,n3,n4,n5,n6;
        n1="part_status";n1.append(std::to_string(i).c_str());
        n2="part_type";n2.append(std::to_string(i).c_str());
        n3="part_fit";n3.append(std::to_string(i).c_str());
        n4="part_start";n4.append(std::to_string(i).c_str());
        n5="part_size";n5.append(std::to_string(i).c_str());
        n6="part_name";n6.append(std::to_string(i).c_str());

        dot+= "            <TR> \n"
              "                <TD>"+n1+"</TD> \n"
                                        "                <TD>"
                                        "                    <TABLE BORDER=\"0\"> \n"
                                        "                        <TR><TD>"+status+"</TD></TR> \n"
                                                                                  "                    </TABLE> \n"
                                                                                  "                </TD> \n"
                                                                                  "            </TR> \n";
        dot+="            <TR> \n"
             "                <TD>"+n2+"</TD> \n"
                                       "                <TD> \n"
                                       "                    <TABLE BORDER=\"0\"> \n"
                                       "                        <TR><TD>"+type+"</TD></TR> \n"
                                                                               "                    </TABLE> \n"
                                                                               "                </TD> \n"
                                                                               "            </TR> \n";
        dot+="            <TR> \n"
             "                <TD>"+n3+"</TD> \n"
                                       "                <TD> \n"
                                       "                    <TABLE BORDER=\"0\"> \n"
                                       "                        <TR><TD>"+fft+"</TD></TR> \n"
                                                                              "                    </TABLE> \n"
                                                                              "                </TD> \n"
                                                                              "            </TR> \n";
        dot+="            <TR> \n"
             "                <TD>"+n4+"</TD> \n"
                                       "                <TD> \n"
                                       "                    <TABLE BORDER=\"0\"> \n"
                                       "                        <TR><TD>"+start+"</TD></TR> \n"
                                                                                "                    </TABLE> \n"
                                                                                "                </TD> \n"
                                                                                "            </TR> \n";
        dot+="            <TR> \n"
             "                <TD>"+n5+"</TD> \n"
                                       "                <TD> \n"
                                       "                    <TABLE BORDER=\"0\"> \n"
                                       "                        <TR><TD>"+size+"</TD></TR> \n"
                                                                               "                    </TABLE> \n"
                                                                               "                </TD> \n"
                                                                               "            </TR> \n";
        dot+="            <TR> \n"
             "                <TD>"+n6+"</TD> \n"
                                       "                <TD> \n"
                                       "                    <TABLE BORDER=\"0\"> \n"
                                       "                        <TR><TD>"+fname+"</TD></TR> \n"
                                                                                "                    </TABLE> \n"
                                                                                "                </TD> \n"
                                                                                "            </TR> \n";
    }
    QString disco=output_path.c_str();
    int slash=disco.lastIndexOf("/");
    int last = disco.length();
    disco=disco.mid(slash+1,last);
    generate_graphviz_table(choice,dot,output_path,disco,"MBR"); // var disco es el nombre del disco
}

void graficador::generate_ebr_table_content(string output_path,Ebr ebr, string choice)
{
    // genero las filas del html
    QString ssize=std::to_string(ebr.size).c_str();
    QString sstart=std::to_string(ebr.start).c_str();
    QString sstatus=std::to_string(ebr.status).c_str();
    QString snext=std::to_string(ebr.next).c_str();


    QString dot= "            <TR> \n"
                 "                <TD>EBR_size</TD> \n"
                 "                <TD>"
                 "                    <TABLE BORDER=\"0\"> \n"
                 "                        <TR><TD>"+ssize+"</TD></TR> \n"
                                                          "                    </TABLE> \n"
                                                          "                </TD> \n"
                                                          "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> EBR_FIT </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+QString(ebr.fit)+"</TD></TR> \n"
                                                             "                    </TABLE> \n"
                                                             "                </TD> \n"
                                                             "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> EBR_START </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+sstart+"</TD></TR> \n"
                                                   "                    </TABLE> \n"
                                                   "                </TD> \n"
                                                   "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> EBR_SIZE </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+ssize+"</TD></TR> \n"
                                                  "                    </TABLE> \n"
                                                  "                </TD> \n"
                                                  "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> EBR_NEXT </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+snext+"</TD></TR> \n"
                                                  "                    </TABLE> \n"
                                                  "                </TD> \n"
                                                  "            </TR> \n";
    dot+="            <TR> \n"
         "                <TD> EBR_NAME </TD> \n"
         "                <TD> \n"
         "                    <TABLE BORDER=\"0\"> \n"
         "                        <TR><TD>"+QString(ebr.name)+"</TD></TR> \n"
                                                              "                    </TABLE> \n"
                                                              "                </TD> \n"
                                                              "            </TR> \n";
    QString disco=ebr.name;
    int slash=QString(output_path.c_str()).lastIndexOf("/");
    QString new_path = QString(output_path.c_str()).mid(0,slash + 1);
    new_path.append(disco);
    new_path.append(".");
    new_path.append(QString(choice.c_str()).toLower());
    generate_graphviz_table(choice,dot,new_path.toStdString(),disco,"MBR"); // var disco es el nombre del disco
}

void graficador::generate_graphviz_table(string extension,QString dot,string path,QString disco,string tipo){
    QString sb =
            "digraph test {"
            "    graph [ratio=fill]; \n "
            "    node [label=\"\\N\", fontsize=12, shape=plaintext]; \n "
            "    graph [bb=\"0,0,352,154\"]; \n"
            "    arset [label=< \n"
            "        <TABLE ALIGN=\"LEFT\"> \n"
            "            <TR> \n"
            "                <TD bgcolor =\"red\">"+disco+"</TD> \n"
                                                          "            </TR> \n"
            + dot+
            "        </TABLE> \n"
            "    >, ];\n"
            "}";
    ofstream file;
    file.open("grafo.dot");
    file<<sb.toUtf8().constData();
    file.close();
    //  cout<<extension<<endl;
    string comando;
    if(extension=="jpg"|extension=="JPG")comando.append("dot -Tjpg ");
    else if(extension=="PDF"|extension=="pdf") comando.append("dot -Tpdf ");
    else if(extension=="png"|extension=="PNG")comando.append("dot -Tpng ");
    comando.append("grafo.dot ");
    comando.append("-o ");
    comando.append("\"");
    comando.append(path);
    comando.append("\"");
    if(system(comando.c_str())==0) cout<<"Sistema: Reporte "<<tipo<<" Creado Exitosamente"<<endl;
}

void graficador::generate_bitmap(SuperBloque sb,QString choice,int partition_size,string output_path,FILE *file,int fs)
{ // el path es de la vda int star es el inicio de la particion y tambien tengo que mandar el tamaño de la particion
    //eligiendo el reporte que queremos
    if(choice=="bm_inode")
    {
        QString cuerpotdot="";
        char bm=0;
        int bmi=sb.s_bm_inode_start;
        //calculando numero de estructuras para size de la particion
        int terminabit = file_system::structures_number(fs,partition_size);
        for (int i=0;i<terminabit;i++)
        {
            if(get_deleted())
            {
                if(i!=0 && i%19==0) cuerpotdot+= '\n';
                cuerpotdot += '0';
                continue;
            }
            fseek(file,bmi,SEEK_SET);
            fread(&bm,sizeof (char),1,file);
            if(i!=0 && i%19==0) cuerpotdot+= '\n';
            if(bm == '\0') cuerpotdot += '0';
            else cuerpotdot += bm;
            bmi+=sizeof (char);
        }
        //Crear el archivo punto txt
        ofstream fileout;
        fileout.open(output_path);
        fileout<<cuerpotdot.toUtf8().constData();
        fileout.close();
        printf("Sistema: Reporte creado exitosamente\n");
    }
    else if(choice=="bm_block")
    {
        QString cuerpotdot="";
        char bm=0;
        int bmi=sb.s_bm_block_start;
        int terminabit = 3 * file_system::structures_number(fs,partition_size);
        for (int i=0;i<terminabit;i++)
        {
            if(get_deleted())
            {
                if(i!=0 && i%19==0) cuerpotdot+= '\n';
                cuerpotdot += '0';
                continue;
            }
            fseek(file,bmi,SEEK_SET);
            fread(&bm,sizeof (char),1,file);
            if(i!=0 && i%19==0) cuerpotdot+= '\n';
            if(bm == '\0') cuerpotdot += '0';
            else cuerpotdot += bm;
            bmi+=sizeof (char);
        }

        //Crear el archivo punto txt
        ofstream fileout;
        fileout.open(output_path);
        fileout<<cuerpotdot.toUtf8().constData();
        fileout.close();
        printf("Sistema: Reporte creado exitosamente\n");
    }
    fclose(file);
}

void graficador::generate_tables(string output_path,QString extension,FILE *file2, int choice,SuperBloque sb)
{
    tools tool;
    //1 para tree complete, 2 para inodos
    QString output_string = "digraph models_diagram{\n";
    output_string += "graph[rankdir=LR, overlap=false, splines=true];\n";
    output_string += "node [shape=record, fontsize=9, fontname=\"Verdana\" ];\n";
    output_string += "edge [style=dashed];\n";
    if(!get_deleted())
    {
        switch (choice)
        {
        case 1:
        {
            iNodo inode;
            fseek(file2,sb.s_inode_start,SEEK_SET);
            fread(&inode,sizeof (iNodo),1,file2);
            output_string += tool.get_tree_report(inode,file2);
        }
            break;
        case 2:
            output_string += tool.get_full_inode_report(sb,file2);
            break;
        case 3:
            output_string += tool.get_full_blocks_report(sb,file2);
            break;
        default:
            output_string += tool.get_log_report(file2,choice);
            break;
        }

    }

    output_string += "\n}";

    ofstream file;
    file.open("grafo.dot");
    file<<output_string.toUtf8().constData();
    file.close();

    //  cout<<extension<<endl;
    string comando;
    if(extension=="JPG")comando.append("dot -Tjpg ");
    else if(extension=="PDF") comando.append("dot -Tpdf ");
    else if(extension=="PNG")comando.append("dot -Tpng ");
    comando.append("grafo.dot ");
    comando.append("-o ");
    comando.append("\"");
    comando.append(output_path);
    comando.append("\"");
    if(system(comando.c_str())==0) cout<<"Sistema: Reporte creado Exitosamente"<<endl;
}

void graficador::generate_file_report(string full_file, string output_path)
{
    //Crear el archivo punto txt
    ofstream fileout;
    fileout.open(output_path);
    fileout<<full_file;
    fileout.close();
    printf("Sistema: Reporte creado exitosamente\n");
}
