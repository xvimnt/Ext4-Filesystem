#include "io_user.h"

//***************Metodos*******************//
void ast_tree(){
    /*Instanciamos un graficador y graficamos*/
    graficador *graf = new graficador(root);
    graf->generarImagen();
    QFileInfo fi("temp");
    QString path = fi.absolutePath() +"/grafo.jpg";
}

void io_user::analize(std::string input){
    if(!input.empty()){
        //metodo que sirve para analizar una string con nuestro interprete
        YY_BUFFER_STATE buffer = yy_scan_string(input.c_str());
        if(yyparse() == 0)
        {
            //ejecutando las instrucciones
            semantic smnt = semantic();
            smnt.compute(root);
            //para graficar nuestro arbol
            //ast_tree();
        }
    }
}

void io_user::read_from_file(Metadata data){

    if(data.path[1] == '/')
    {
        data.path[0] = ' ';
        data.path[1] = '"';
    }

    //leer desde archivo
    std::string line, multi_line = "";
    std::ifstream myfile (data.path);
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            std::size_t found = line.find_last_of("\\^");
            if (found!=std::string::npos)
            {
                multi_line += line.substr(0,found - 1);
            }
            else
            {
                multi_line += line;
                if(!multi_line.empty() && multi_line.rfind("#", 0) != 0 && multi_line[0] != '\n' && multi_line[0] != '\0')
                {
                    //borra comentarios del final
                    if(multi_line.find("#") != std::string::npos)
                    {
                        QString string = QString::fromStdString(multi_line);
                        int chop_count = string.size() - string.indexOf("#");
                        string.chop(chop_count);
                        multi_line = string.toStdString();
                    }
                    printf("\n\033[1;34m[MIA]201700831_Ejecutando instruccion: \n%s\033[0m\n",multi_line.c_str());
                    analize(multi_line);
                }
                multi_line = "";
            }
        }
        myfile.close();
    }
    else printf("Error: no se pudo abrir: '%s'\n",data.path);
}

void io_user::pause_system()
{
    printf("\n*************Sistema Pausado, Presiona ENTER para continuar...***************\n");
    getchar();
}

io_user::io_user()
{

}
