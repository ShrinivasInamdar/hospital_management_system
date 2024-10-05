#include <iostream>
#include <fstream>
#include <string>
#include "sqlite3.h"
#include "patientmodule.h"

using namespace std;

// Check setup or initialize the resource file
void check_setup() {
    string filename = "data.txt";
    ifstream file(filename);
    if (file) {
        return;
    } else {
        ofstream outfile(filename);
        if (outfile.is_open()) {
            int n, icu, op,a =0;
            cout << "INITIAL SETUP " << endl;
            cout << "Enter the number of beds in Ward: "; cin >> n;
            cout << "Enter the number of ICU beds: "; cin >> icu;
            cout << "Enter the number of operation theatres: "; cin >> op;
            outfile << n << endl;
            outfile << icu << endl;
            outfile << op << endl;
            outfile<<a<<endl;
            outfile<<a<<endl;
            outfile<<a<<endl;
            outfile.close();
            cout << "SETUP COMPLETED" << endl;
        }
    }
}

// Helper function to read current resources from the file
void readResources(sqlite3 *db,int &beds, int &icuBeds, int &operationTheatres,int &inWard,int &inICU,int &rebed,int &reicu, int &reop) {
    ifstream infile("data.txt");
    infile >> beds >> icuBeds >> operationTheatres>>rebed>>reicu>>reop;
   

    int discharged = 0;
    int inClinic = 0;
    inWard = 0;inICU=0;
    const char* sql = "SELECT Status, COUNT(*) FROM Patient GROUP BY Status;";
    sqlite3_stmt* stmt;


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

    infile.close();
}

// Helper function to update resources in the file
void updateResources(int beds, int icuBeds, int operationTheatres,int rebed,int reicu,int reop) {
    ofstream outfile("data.txt");
    outfile << beds << endl;
    outfile << icuBeds << endl;
    outfile << operationTheatres << endl;
    outfile<<rebed<<endl;
    outfile<<reicu<<endl;
    outfile<<reop<<endl;
    outfile.close();
}

// Manage ICU beds (admit or discharge patients)
void manageICU() {
    int rc = sqlite3_open("hospital.db", &db);
   
    int icuBeds, operationTheatres, wardBeds,inward,inicu,reward,reicu,reops;
    readResources(db,wardBeds, icuBeds, operationTheatres,inward,inicu,reward,reicu,reops);

    cout << "\nICU Management: " << endl;
    cout << "Current ICU Beds Available: " << (icuBeds-inicu+reicu) << endl;

    int action, count;
    cout << "1. Reserve  ICU\n2. Dereserve ICU\nEnter choice: "; cin >> action;
    
    if (action == 1) {
        cout << "Enter number to reserve: "; cin >> count;
        if (count <= (icuBeds-inicu+reicu)) {
            reicu -= count;
            cout << count << " reserved ICU." << endl;
        } else {
            cout << "Not enough ICU beds available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number of patients to discharge: "; cin >> count;
        reicu += count;
        cout << count << " patients discharged from ICU." << endl;
    } else {
        cout << "Invalid action." << endl;
    }
sqlite3_close(db);
    updateResources(wardBeds, icuBeds, operationTheatres,reward,reicu,reops);
}

// Manage Operation Theatres (schedule or finish surgeries)
void manageOperationTheatre() {
    int rc = sqlite3_open("hospital.db", &db);
   
     int icuBeds, operationTheatres, wardBeds,inward,inicu,reward,reicu,reops;
    readResources(db,wardBeds, icuBeds, operationTheatres,inward,inicu,reward,reicu,reops);

    

    cout << "\nOperation Theatre Management: " << endl;
    cout << "Current Operation Theatres Available: " << operationTheatres+reops << endl;

    int action, count;
    cout << "1. Schedule Surgery\n2. Finish Surgery\nEnter choice: "; cin >> action;

    if (action == 1) {
        cout << "Enter number of surgeries to schedule: "; cin >> count;
        if (count <= operationTheatres+reops) {
            reops -= count;
            cout << count << " surgeries scheduled." << endl;
        } else {
            cout << "Not enough operation theatres available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number of surgeries completed: "; cin >> count;
        reops += count;
        cout << count << " surgeries completed." << endl;
    } else {
        cout << "Invalid action." << endl;
    }
sqlite3_close(db);
  updateResources(wardBeds, icuBeds, operationTheatres,reward,reicu,reops);
}

// Manage Ward beds (admit or discharge patients)
void manageWard() {
    int rc = sqlite3_open("hospital.db", &db);
   
     int icuBeds, operationTheatres, wardBeds,inward,inicu,reward,reicu,reops;
    readResources(db,wardBeds, icuBeds, operationTheatres,inward,inicu,reward,reicu,reops);



    cout << "\nWard Management: " << endl;
    cout << "Current Ward Beds Available: " << wardBeds-inward+reward << endl;

    int action, count;
    cout << "1. Reserved Beds \n2. Dereserve Beds\nEnter choice: "; cin >> action;

    if (action == 1) {
        cout << "Enter number to reserved: "; cin >> count;
        if (count <= (wardBeds-inward+reward )) {
            reward -= count;
            cout << count << "reserved Ward beds." << endl;
        } else {
            cout << "Not enough Ward beds available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number to deserved: "; cin >> count;
        reward += count;
        cout << count << " dereserved beds." << endl;
    } else {
        cout << "Invalid action." << endl;
    }
sqlite3_close(db);
     updateResources(wardBeds, icuBeds, operationTheatres,reward,reicu,reops);
}

// Display current available resources
void displayResources() {
    int rc = sqlite3_open("hospital.db", &db);
   
    int icuBeds, operationTheatres, wardBeds,inward,inicu,reward,reicu,reops;
    readResources(db,wardBeds, icuBeds, operationTheatres,inward,inicu,reward,reicu,reops);
sqlite3_close(db);


    cout << "\nCurrent Resource Availability: " << endl;
    cout << "Total Ward Beds: " << wardBeds << endl;
    cout << "Total ICU Beds: " << icuBeds << endl;
    cout << "Total Operation Theatres: " << operationTheatres << endl;
    cout << "Free Ward Beds: "<< wardBeds+reward-inward<<endl;
    cout << "Free ICU Beds: " << icuBeds - inicu + reicu<<endl;
    cout << "Free Operation Theatres: "<< operationTheatres + reops<<endl;

}
void managementhandler() {
    int choice;
    check_setup();

    do {
        // Displaying the menu options
        cout << "\n===== Hospital Resource Management =====\n";
        cout << "1. Manage ICU\n";
        cout << "2. Manage Ward Beds\n";
        cout << "3. Manage Operation Theatres\n";
        cout << "4. Display Current Resources\n";
        cout << "5. Exit\n";
        cout << "Enter your choice (1-5): ";
        cin >> choice;

        // Menu options with appropriate function calls
        switch (choice) {
            case 1:
                manageICU();  // Manage ICU beds
                break;
            case 2:
                manageWard();  // Manage Ward beds
                break;
            case 3:
                manageOperationTheatre();  // Manage Operation Theatres
                break;
            case 4:
                displayResources();  // Display current resource status
                break;
            case 5:
                cout << "Exiting the management system.\n";
                break;
            default:
                cout << "Invalid choice! Please select a valid option.\n";
        }
    } while (choice != 5);  // Continue until the user chooses to exit
}
