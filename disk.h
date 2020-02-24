#ifndef DISK_H
#define DISK_H

#include <stdio.h>
//atoi
#include <stdlib.h>
//directorios
#include <dirent.h>
//crear directorios
#include <sys/stat.h>
//comparacion de cadenas
#include <string.h>
//timeq
#include <time.h>
#include <qdebug.h>
#include <iostream>
//para el diccionario
#include <qmap.h>
#include <qqueue.h>
#include <qstack.h>
//el fs
#include <file_system.h>
#include <node.h>
#include <plotter.h>

//********************Area de structs*******************//

typedef struct
{
    char status;
    char fit[2];
    int start;
    int size;
    int next;
    char name[16];
}Ebr;



//*************Diccionario de discos montados************//
static QMap<QString,char> mounted_disks;
static QMap<QString,Partition> mounted_partitions;
//******variable de sesion*******//
static user cookie;

class disk
{
public:
disk();
void makeDisk(Metadata data);
void fDisk(Metadata data);
void removeDisk(Metadata data);
void mountDisk(Metadata data);
void unmountDisk(QString unmount_id_partition);
void makeGroup(Metadata data);
void formatDisk(Metadata data);
void makeReport(Metadata data);
void makeDir(Metadata data);
void makeFile(Metadata data);
void recovery(Metadata data);
void loss(Metadata data);
void login(Metadata data);
void logout();

private:
void createPartition(Metadata data,FILE* file,Mbr mbr);
};

#endif // DISK_H
