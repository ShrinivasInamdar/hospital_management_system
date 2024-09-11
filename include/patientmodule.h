#pragma once
#include "sqlite3.h"
#include <vector>
#include <iostream>
using namespace std;

extern sqlite3* db;


void patienthandler();
bool updatePatientStatus(sqlite3* db, int patientId, int newStatus);
class Patient{
public:

    //status 0-discharged 1-admitted 2-icu 3-clinic
    int patientId,age,status,doctorID,contact;
    string name,gender,address;
    vector<string>medical_history;
    // Add this to your patientmodule.cpp or inline in your header file
    Patient() : patientId(0), age(0), status(0), doctorID(0), contact(0) {
    // Default constructor logic (if any)
}

    Patient(int id,string& pname, int page,string&pgender,
        int pcontact,string&paddress,int doc, int stat){ 
        patientId = id;
        age = page;
        status = stat;
        int doctorID = doc;
        contact = pcontact;
        name = pname;
        gender = pgender;
        address = paddress;
        }
    void set_status(int ,sqlite3*);
    void set_doc(sqlite3* db, int pd) {
    doctorID = pd;

    // Update the database with the new doctor ID
    const char* sql = "UPDATE Patient SET DoctorID = ? WHERE PatientID = ?;";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    // Bind the new doctorID and patientId to the SQL statement
    sqlite3_bind_int(stmt, 1, doctorID);
    sqlite3_bind_int(stmt, 2, patientId);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to update doctor ID: " << sqlite3_errmsg(db) << std::endl;
    } else {
        std::cout << "Doctor ID updated in the database successfully." << std::endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
}


   void fetchMedicalHistoryFromDB(sqlite3* db) {
        const char* sql = "SELECT Record FROM MedicalHistory WHERE PatientID = ?;";
    sqlite3_stmt* stmt;
    vector<string> historyRecords;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement." << endl;
        return;
    }

    // Bind patientId to the SQL statement
    sqlite3_bind_int(stmt, 1, patientId);

    // Loop through the results and collect medical history records
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string record(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        historyRecords.push_back(record);
    }

    sqlite3_finalize(stmt);
    medical_history = historyRecords;
    }

    
    bool addMedicalHistoryToDB(sqlite3* db, const string& record) {
        const char* sql = "INSERT INTO MedicalHistory (PatientID, Record) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    // Prepare the SQL statement
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind patientId and record to the SQL statement
    sqlite3_bind_int(stmt, 1, patientId);
    sqlite3_bind_text(stmt, 2, record.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert medical history: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    // Finalize the statement
    sqlite3_finalize(stmt);
    medical_history.push_back(record);
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);
    return true;
    }
  void inputDetails() {
    cout << "Enter Patient ID: ";
    cin >> patientId;
    cin.ignore(10000, '\n');  // Clear newline character after ID input

    cout << "Enter Patient Name: ";
    getline(cin, name);  // Get full name (including spaces)

    cout << "Enter Patient Age: ";
    cin >> age;
    cin.ignore(10000, '\n');  // Clear newline character after age input

    cout << "Enter Patient Gender (M/F): ";
    getline(cin, gender);  // Get full input for gender

    cout << "Enter Patient Contact Number: ";
    cin >> contact;
    cin.ignore(10000, '\n');  // Clear newline character after contact input

    cout << "Enter Patient Address: ";
    getline(cin, address);  // Get full address (including spaces)

    cout << "Enter Doctor ID: ";
    cin >> doctorID;
    cin.ignore(10000, '\n');  // Clear newline character after Doctor ID input

    cout << "Enter Status (0: Discharged, 1: Admitted, 2: ICU, 3: Clinic): ";
    cin >> status;
    cin.ignore(10000, '\n');  // Clear newline character after status input
}

    void displayInfo() const {
        cout << "Patient ID: " << patientId << endl;
        cout << "Name: " << name << endl;
        cout << "Age: " << age << endl;
        cout << "Gender: " << gender << endl;
        cout << "Contact: " << contact << endl;
        cout << "Address: " << address << endl;
        cout << "Doctor ID: " << doctorID << endl;

        // Display status
        cout << "Status: ";
        switch (status) {
            case 0:
                cout << "Discharged";
                break;
            case 1:
                cout << "Admitted";
                break;
            case 2:
                cout << "In ICU";
                break;
            case 3:
                cout << "Clinic";
                break;
            default:
                cout << "Unknown";
        }
        cout << endl;

        // Display medical history
        if (!medical_history.empty()) {
            cout << "Medical History:" << endl;
            for (const auto &record : medical_history) {
                cout << "- " << record << endl;
            }
        } else {
            cout << "No medical history available." << endl;
        }
    }
};
