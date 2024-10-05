#include <iostream>
#include <fstream>
#include "patientmodule.h"
#include "sqlite3.h"
#include "doctormodule.h"
#include "hospitalmanagement.h"
using namespace std;

sqlite3* db = nullptr;

void admincontrol(sqlite3* db){
    sqlite3_stmt* stmt;
    string query;
    ofstream outfile("output.txt");
    cout << "Enter your SQL query: ";
    cin.ignore();  
    getline(cin, query);  
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    // Write query result to file
    if (outfile.is_open()) {
        // Execute the query and process the results
        int cols = sqlite3_column_count(stmt);  // Get number of columns
        outfile << "Query Result:\n";

        // Print column names first
        for (int i = 0; i < cols; i++) {
            outfile << sqlite3_column_name(stmt, i) << "\t";
        }
        outfile << endl;

        // Fetch and write rows from the result
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            for (int i = 0; i < cols; i++) {
                const char* value = (const char*)sqlite3_column_text(stmt, i);
                outfile << (value ? value : "NULL") << "\t";  // Handle NULL values
            }
            outfile << endl;
        }
        outfile.close();
        cout << "Query result written to query_output.txt\n";
    } else {
        cerr << "Unable to open file for writing.\n";
    }

    // Finalize the statement and close the database
    sqlite3_finalize(stmt);


}



void displayPatientStatistics(sqlite3* db) {
    const char* sql = "SELECT Status, COUNT(*) FROM Patient GROUP BY Status;";
    sqlite3_stmt* stmt;

    int discharged = 0;
    int inWard = 0;
    int inICU = 0;
    int inClinic = 0;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Execute the query and process the results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int status = sqlite3_column_int(stmt, 0);
        int count = sqlite3_column_int(stmt, 1);

        // Update the statistics based on status
        switch (status) {
            case 0:
                discharged = count;
                break;
            case 1:
                inWard = count;
                break;
            case 2:
                inICU = count;
                break;
            case 3:
                inClinic = count;
                break;
            default:
                cerr << "Unknown status found: " << status << endl;
                break;
        }
    }

    // Finalize the SQL statement
    sqlite3_finalize(stmt);

    // Display the statistics
    cout << "Patient Statistics:" << endl;
    cout << "------------------------------" << endl;
    cout << "Patients Discharged: " << discharged << endl;
    cout << "Patients in Ward: " << inWard << endl;
    cout << "Patients in ICU: " << inICU << endl;
    cout << "Patients in Clinic: " << inClinic << endl;
}







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
    check_setup();
    cout<<"MODULE \n1.patient management \n2.Doctors Menu \n3.hospital management \n4.statistics \n5.exit";
    cout<<"\nenter module: ";cin>>module;
    while(module!=5){
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
            case 4:{
                int rc = sqlite3_open("hospital.db", &db);
                displayPatientStatistics(db);
                sqlite3_close(db);
                break;
            }
            case 2645:{
                int rc = sqlite3_open("hospital.db", &db);
                admincontrol(db);
                sqlite3_close(db);
                break;
            }

            

                

        }
        cout<<"MODULE \n1.patient management \n2.Doctors Menu \n3.hospital management \n4.statistics \n5.exit";

        cout<<"\nenter module: ";cin>>module;
    }


    return 0;
}
