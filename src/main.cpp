#include <iostream>
#include <fstream>
#include "patientmodule.h"
#include "sqlite3.h"
#include "doctormodule.h"
#include "hospitalmanagement.h"
using namespace std;



int main(){
    cout<<R"(
     _   _ _____ ___________ _____ _____ ___  _                    
| | | |  _  /  ___| ___ |_   _|_   _/ _ \| |                   
| |_| | | | \ `--.| |_/ / | |   | |/ /_\ | |                   
|  _  | | | |`--. |  __/  | |   | ||  _  | |                   
| | | \ \_/ /\__/ | |    _| |_  | || | | | |____               
\_| |_/\___/\____/\_|    \___/  \_/\_| |_\_____/               
                                                                                                                             
___  ___ ___  _   _  ___  _____ ________  ________ _   _ _____ 
|  \/  |/ _ \| \ | |/ _ \|  __ |  ___|  \/  |  ___| \ | |_   _|
| .  . / /_\ |  \| / /_\ | |  \| |__ | .  . | |__ |  \| | | |  
| |\/| |  _  | . ` |  _  | | __|  __|| |\/| |  __|| . ` | | |  
| |  | | | | | |\  | | | | |_\ | |___| |  | | |___| |\  | | |  
\_|  |_\_| |_\_| \_\_| |_/\____\____/\_|  |_\____/\_| \_/ \_/  
                                                                                                                             
 _______   _______ _____ ________  ___                         
/  ___\ \ / /  ___|_   _|  ___|  \/  |                         
\ `--. \ V /\ `--.  | | | |__ | .  . |                         
 `--. \ \ /  `--. \ | | |  __|| |\/| |                         
/\__/ / | | /\__/ / | | | |___| |  | |                         
\____/  \_/ \____/  \_/ \____/\_|  |_/                         
                                                               
    )";
    int module;
    cout<<"MODULE \n1.patient management \n2.Doctors Menu \n3.hospital management \n4.exit";
    cout<<"\nenter module: ";cin>>module;
    while(module!=3){
        switch(module){
            case 1:
                patienthandler();
                break;
            case 2:
                doctorHandler();
                break;
            case 3:
                managementhandler();
                break;
                

        }
        cout<<"\nenter module: ";cin>>module;
    }


    return 0;
}
