#include <iostream>
#include <string>
#include <vector>
#include "patientmodule.h"
#include "sqlite3.h"
using namespace std;



sqlite3* db;


// Function to create both tables: Patient and MedicalHistory
bool createTables(sqlite3* db) {
    const char* patientTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Patient (
            PatientID INT PRIMARY KEY,
            Name TEXT NOT NULL,
            Age INT NOT NULL,
            Gender TEXT NOT NULL,
            Contact INT NOT NULL,
            Address TEXT NOT NULL,
            DoctorID INT NOT NULL,
            Status INT NOT NULL
        );
    )";

    const char* medicalHistoryTableSQL = R"(
        CREATE TABLE IF NOT EXISTS MedicalHistory (
            HistoryID INTEGER PRIMARY KEY AUTOINCREMENT,
            PatientID INT NOT NULL,
            Record TEXT NOT NULL,
            FOREIGN KEY (PatientID) REFERENCES Patient (PatientID)
        );
    )";

    char* errMsg = nullptr;

    // Execute patient table creation
    if (sqlite3_exec(db, patientTableSQL, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating Patient table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    // Execute medical history table creation
    if (sqlite3_exec(db, medicalHistoryTableSQL, 0, 0, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating MedicalHistory table: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;

}



bool insertPatient(sqlite3* db, const Patient& patient) {
    const char* sql = "INSERT INTO Patient (PatientID, Name, Age, Gender, Contact, Address, DoctorID, Status) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return false;
    }

    // Bind values from the Patient object
    sqlite3_bind_int(stmt, 1, patient.patientId);
    sqlite3_bind_text(stmt, 2, patient.name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, patient.age);
    sqlite3_bind_text(stmt, 4, patient.gender.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 5, patient.contact);
    sqlite3_bind_text(stmt, 6, patient.address.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 7, patient.doctorID);
    sqlite3_bind_int(stmt, 8, patient.status);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);

    return true;
}




bool updatePatientStatus(sqlite3* db, int patientId, int newStatus) {
    const char* sql = "UPDATE Patient SET Status = ?WHERE PatientID = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return false;
    }

    // Bind the new values
    sqlite3_bind_int(stmt, 1, newStatus);
    sqlite3_bind_int(stmt, 2, patientId);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to update data." << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

// Helper function to allow modifying the patient (adding history, updating status, etc.)
void modifyPatient(sqlite3* db, Patient& patient) {
    int action;
    cout << "\nSelected Patient: " << endl;
    patient.displayInfo();

    cout << "\nWhat would you like to do?" << endl;
    cout << "1. Add medical history" << endl;
    cout << "2. Update patient status" << endl;
    cout << "3. Cancel" << endl;
    cout << "Enter choice: ";
    cin >> action;

    switch (action) {
        case 1: {
            // Add medical history
            string history;
            cout << "Enter medical history (leave blank to finish): ";
            cin.ignore(10000, '\n');  // Clear newline
            while (true) {
                cout << ">> ";
                getline(cin, history);
                if (history.empty()) {
                    break;
                }
                patient.addMedicalHistoryToDB(db, history);
            }
            break;
        }
        case 2: {
            // Update patient status
            int newStatus;
            cout << "Enter new status (0: Discharged, 1: Admitted, 2: ICU, 3: Clinic): ";
            cin >> newStatus;
            if (updatePatientStatus(db, patient.patientId, newStatus)) {
                cout << "Status updated successfully." << endl;
            } else {
                cout << "Failed to update status." << endl;
            }
            break;
        }
        case 3:
            // Cancel operation
            cout << "Operation canceled." << endl;
            break;
        default:
            cout << "Invalid choice!" << endl;
    }
}



void searchAndModifyPatient(sqlite3* db) {
    string searchQuery;
    cout << "Enter Patient Name or ID: ";
    cin.ignore();
    getline(cin, searchQuery);

    sqlite3_stmt* stmt;
    const char* sql = "SELECT * FROM Patient WHERE Name LIKE ? OR PatientID = ?;";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement." << std::endl;
        return;
    }

    // Bind the search query for both name and ID
    sqlite3_bind_text(stmt, 1, ('%' + searchQuery + '%').c_str(), -1, SQLITE_STATIC);
    int patientId = 0;
    try {
        patientId = stoi(searchQuery);
    } catch (...) {
        patientId = 0;  // If the search query is not an integer, it will set to 0
    }
    sqlite3_bind_int(stmt, 2, patientId);

    vector<Patient> patientMatches;
    // Fetch results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        int age = sqlite3_column_int(stmt, 2);
        string gender(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        int contact = sqlite3_column_int(stmt, 4);
        string address(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));
        int doctorID = sqlite3_column_int(stmt, 6);
        int status = sqlite3_column_int(stmt, 7);

        // Create Patient object and add it to the matches
        Patient p(id, name, age, gender, contact, address, doctorID, status);
        patientMatches.push_back(p);
    }
    sqlite3_finalize(stmt);

    if (patientMatches.empty()) {
        cout << "No patient found with the given name or ID." << endl;
        return;
    }

    // If more than one patient is found
    if (patientMatches.size() > 1) {
        cout << "Multiple patients found with the same name. Please select the patient:" << endl;
        for (size_t i = 0; i < patientMatches.size(); ++i) {
            cout << i + 1 << ". " << "Patient ID: " << patientMatches[i].patientId << " | Name: " << patientMatches[i].name << endl;
        }

        int selection;
        cout << "Enter the number of the patient to select: ";
        cin >> selection;
        if (selection < 1 || selection > patientMatches.size()) {
            cout << "Invalid selection." << endl;
            return;
        }

        // Select the patient chosen by the user
        Patient& selectedPatient = patientMatches[selection - 1];

        modifyPatient(db, selectedPatient);
    } else {
        // If only one patient is found, proceed directly to modification
        modifyPatient(db, patientMatches[0]);
    }
}


void displayAllPatients(sqlite3* db) {
    const char* sqlAllPatients = "SELECT * FROM Patient;";
    sqlite3_stmt* stmtAllPatients;

    // Prepare the SQL statement for fetching all patient information
    if (sqlite3_prepare_v2(db, sqlAllPatients, -1, &stmtAllPatients, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement for all patients: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Iterate through each patient record
    while (sqlite3_step(stmtAllPatients) == SQLITE_ROW) {
        // Retrieve patient details
        int patientId = sqlite3_column_int(stmtAllPatients, 0);
        std::string name(reinterpret_cast<const char*>(sqlite3_column_text(stmtAllPatients, 1)));
        int age = sqlite3_column_int(stmtAllPatients, 2);
        std::string gender(reinterpret_cast<const char*>(sqlite3_column_text(stmtAllPatients, 3)));
        int contact = sqlite3_column_int(stmtAllPatients, 4);
        std::string address(reinterpret_cast<const char*>(sqlite3_column_text(stmtAllPatients, 5)));
        int doctorID = sqlite3_column_int(stmtAllPatients, 6);
        int status = sqlite3_column_int(stmtAllPatients, 7);

        // Create a Patient object
        Patient patient(patientId, name, age, gender, contact, address, doctorID, status);

        // Fetch and display medical history
        patient.fetchMedicalHistoryFromDB(db);

        // Display the patient information
        patient.displayInfo();
        std::cout << "-----------------------------------" << std::endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmtAllPatients);
}



void patienthandler( void) {
    int rc = sqlite3_open("hospital.db", &db);

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Create tables if they don't exist
    if (!createTables(db)) {
        return;
    }
    while (true){
        int choice;
        Patient p1;int a,b;
        cout<<"\n1.add Patient \n2.select patient \n3.display all \n4.update status \n5.exit"<<endl;
        cout<<"enter choice: ";cin>>choice;
        if (choice==5){
            break;
        }
        switch(choice){
            case 1:
                p1.inputDetails();
                cout<<"\nAdd medical history? (1:yes 0:no) : ";cin>>a;
                insertPatient(db,p1);
                

                if (a==1){
                    cin.ignore(10000, '\n');
                    while(true){
                        string input;
                        cout<<">> ";
                        
                        getline(cin,input);
                        if (input.empty()){break;}
                        p1.addMedicalHistoryToDB(db,input);

                    }
                }
                break;
            case 3:
                displayAllPatients(db);
                break;
            case 4:
                cout<<"Enter patient id and updated status: ";
                cin>>a>>b;
                updatePatientStatus(db,a,b);
                break;
            case 2:
            searchAndModifyPatient(db);
            break;               
        }
                
    }
    sqlite3_close(db);


}

