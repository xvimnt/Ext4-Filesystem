#include <iostream>
#include <io_user.h>

int main()
{
    while(true){
        std::cout << "\033[1;34m[MIA]201700831_Ingrese un comando:\033[0m" << std::endl;
        std::string command ;
        getline(std::cin,command);
        system("clear");
        io_user::analize(command);
    }
}
