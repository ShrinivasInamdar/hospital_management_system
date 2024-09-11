#include <iostream>
#include <fstream>
#include "patientmodule.h"
#include "sqlite3.h"
using namespace std;

void check_setup(void);

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
    check_setup();
    int module;
    cout<<"MODULE \n1.patient management";
    cout<<"\nenter module: ";cin>>module;
    while(module!=3){
        switch(module){
            case 1:
                patienthandler();
                break;

        }
        cout<<"\nenter module: ";cin>>module;
    }


    return 0;
}
void check_setup(void){
    string filename = "data.txt";
    ifstream file(filename);
    if (file) {
        return;
    } else {
        ofstream outfile(filename);
        if (outfile.is_open()){
            int n,icu,op;
            cout<<"INITIAL SETUP "<<endl;
            cout<<"Enter the number of beds: ";cin>>n;
            cout<<"Enter the number of icu: ";cin>>icu;
            cout<<"Enter the number of operation theatre: ";cin>>op;
            outfile<<n<<endl;
            outfile<<icu<<endl;
            outfile<<op<<endl;outfile.close();cout<<"SETUP COMPLETED";

        }
    }
}