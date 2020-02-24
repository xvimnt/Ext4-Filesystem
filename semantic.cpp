#include "semantic.h"
#include <qdebug.h>

enum Choice
{
    SIZE = 1,
    FIT = 2,
    UNIT = 3,
    PATH = 4,
    TYPE = 5,
    DELETE = 6,
    NAME = 7,
    ADD = 8,
    ID = 9,
    IDEN = 10,
    NUMBER = 11,
    STRING = 12,
    ATTRIB = 13,
    ATTRIB_LIST = 14,
    MKDISK = 15,
    RMDISK = 16,
    FDISK = 17,
    MOUNT = 18,
    UNMOUNT = 19,
    COMMAND = 20,
    IDN = 21,
    EXEC = 22,
    PAUSE = 23,
    //tipos para el fs
    MAKEGROUP = 24,
    MAKEFS = 25,
    REP = 26,
    RECOVERY = 27,
    LOSS = 28,
    USR = 29,
    PWD = 30,
    LOGIN = 31,
    LOGOUT = 32,
    RUTA = 33,
    MKDIR = 34,
    PCREATION = 35,
    MKFILE = 36,
    CONTENT = 37
};

semantic::semantic()
{

}

node *semantic::compute(node *node_){
    node* result = new node();

    if (node_ == nullptr)
        return result;

    result->linea = node_->linea;
    result->columna = node_->columna;
    disk command;
    switch(node_->tipo_){
    case ATTRIB_LIST:
        //recorriendo los atributos
        foreach(node* son, node_->hijos){
            //reccorrer el arbol
            node * tempData = compute(son);
            switch (tempData->tipo_)
            {
            case SIZE:
                //colocar el size al nuevo disco
                result->metadata.size = tempData->valor.toInt();
                break;
            case PATH:
                //colocar el path al nuevo disco
                if(tempData->valor.length() > 2) memcpy(result->metadata.path, tempData->valor.toStdString().c_str(),tempData->valor.size());
                else result->metadata.path[0] = 0;
                break;
            case RUTA:
                //colocar el path al nuevo disco
                memcpy(result->metadata.ruta, tempData->valor.toStdString().c_str(),tempData->valor.size());
                break;
            case NAME:
                //colocar el nombre al nuevo disco
                memcpy(result->metadata.name, tempData->valor.toStdString().c_str(),tempData->valor.size());
                break;
            case UNIT:
                if(tempData->valor.size() == 1){
                    result->metadata.unit = tempData->valor.toUpper().toStdString().c_str()[0];
                } else result->metadata.unit = 'x';
                break;
            case TYPE:
                if(tempData->valor.size() == 1){
                    result->metadata.type = tempData->valor.toUpper().toStdString().c_str()[0];
                }
                else
                {
                    strcpy(result->metadata.format_type,tempData->valor.toStdString().c_str());
                }
                break;
            case ID:
                strcpy(result->metadata.id,tempData->valor.toStdString().c_str());
                break;
            case DELETE:
                strcpy(result->metadata.Delete,tempData->valor.toStdString().c_str());
                break;
            case USR:
                strcpy(result->metadata.name,tempData->valor.toStdString().c_str());
                break;
            case PWD:
                strcpy(result->metadata.path,tempData->valor.toStdString().c_str());
                break;
            case CONTENT:
                result->metadata.content = tempData->valor;
                break;
            case FIT:
                if(tempData->valor.length() == 2) strcpy(result->metadata.fit,tempData->valor.toUpper().toStdString().c_str());
                else result->metadata.fit[0] = 'x';
                break;
            case PCREATION:
                result->metadata.p_dir = true;
                break;
            default:
                qDebug() << "******Error Semantico, comando no aceptado******" << tempData->tipo;
                break;
            }
        }
        break;
    case ATTRIB:
        //hereda atributos de sus dos hijos
        result->tipo = compute(node_->hijos[0])->tipo;
        result->tipo_ = compute(node_->hijos[0])->tipo_;
        result->valor = compute(node_->hijos[1])->valor;
        break;
    case PAUSE:
        io_user::pause_system();
        break;
    case LOGOUT:
        command.logout();
        break;
    case COMMAND:
        compute(node_->hijos[1]);
        switch (node_->hijos[0]->tipo_)
        {
        case EXEC:
            io_user::read_from_file(compute(node_->hijos[1])->metadata);
            break;
        case RMDISK:
            //elimina el disco
            command.removeDisk(compute(node_->hijos[1])->metadata);
            break;
        case MKDISK:
            //crea el manejador de discos y crea un nuevo disco
            command.makeDisk(compute(node_->hijos[1])->metadata);
            break;
        case FDISK:
            command.fDisk(compute(node_->hijos[1])->metadata);
            break;
        case MOUNT:
            command.mountDisk(compute(node_->hijos[1])->metadata);
            break;
        case UNMOUNT:
            command.unmountDisk(compute(node_->hijos[1])->metadata.id);
            break;
            //COMANDOS PARA EL FS
        case MAKEGROUP:
            break;
        case MAKEFS:
            command.formatDisk(compute(node_->hijos[1])->metadata);
            break;
        case REP:
            command.makeReport(compute(node_->hijos[1])->metadata);
            break;
        case RECOVERY:
            command.recovery(compute(node_->hijos[1])->metadata);
            break;
        case LOSS:
            command.loss(compute(node_->hijos[1])->metadata);
            break;
        case LOGIN:
            command.login(compute(node_->hijos[1])->metadata);
            break;
        case MKDIR:
            command.makeDir(compute(node_->hijos[1])->metadata);
            break;
        case MKFILE:
            command.makeFile(compute(node_->hijos[1])->metadata);
            break;
        }
        break;
    default:
        result->hijos = node_->hijos;
        result->tipo = node_->tipo;
        result->tipo_ = node_->tipo_;
        result->valor = node_->valor;

        foreach(node* son, node_->hijos){
            //reccorrer el arbol
            compute(son);
        }
        break;
    }
    return result;
}
