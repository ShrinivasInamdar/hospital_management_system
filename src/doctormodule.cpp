#include <iostream>
#include <vector>
#include <string>
#include "doctormodule.h"
#include "patientmodule.h"
#include "sqlite3.h"

using namespace std;



// Doctor class to handle doctor-related tasks
class Doctor {
public:
    int doctorId;
    string doctorName;
    string specialization;

    Doctor(int id, const string& name, const string& spec)
        : doctorId(id), doctorName(name), specialization(spec) {}

    void assignPatientToDoctor(Patient& patient) {
        patient.set_doc(db,doctorId);
        cout << "Patient " << patient.name << " has been assigned to Doctor " << doctorName << endl;
    }

    void viewPatientDetails(Patient& patient) {
        cout << "\nDoctor: " << doctorName << endl;
        patient.displayInfo();
    }

    void updatePatientStatus(Patient& patient, int status) {
        patient.set_status(status,db);
        cout << "Patient " << patient.name << "'s status updated to: " << status << endl;
    }

    void addMedicalHistory(Patient& patient) {
        string history;
        cout << "Enter medical history for patient " << patient.name << ": ";
        cin.ignore(10000, '\n');  // Clear buffer
        getline(cin, history);
        patient.addMedicalHistoryToDB(db, history);
        cout << "Medical history updated for " << patient.name << endl;
    }
};

// Helper function to create the Doctors table if it doesn't exist
bool createDoctorTable(sqlite3* db) {
    const char* doctorTableSQL = R"(
        CREATE TABLE IF NOT EXISTS Doctor (
            DoctorID INT PRIMARY KEY,
            Name TEXT NOT NULL,
            Specialization TEXT NOT NULL
        );
    )";

    char* errMsg = nullptr;

    // Execute doctor table creation
    if (sqlite3_exec(db, doctorTableSQL, 0, 0, &errMsg) != SQLITE_OK) {
        cerr << "Error creating Doctor table: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

// Function to insert a new doctor into the database
bool insertDoctor(sqlite3* db, const Doctor& doctor) {
    const char* sql = "INSERT INTO Doctor (DoctorID, Name, Specialization) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement." << endl;
        return false;
    }

    // Bind values from the Doctor object
    sqlite3_bind_int(stmt, 1, doctor.doctorId);
    sqlite3_bind_text(stmt, 2, doctor.doctorName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, doctor.specialization.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cerr << "Failed to insert data." << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(db, "COMMIT;", 0, 0, 0);

    return true;
}

// Function to display all doctors
void displayAllDoctors(sqlite3* db) {
    const char* sqlAllDoctors = "SELECT * FROM Doctor;";
    sqlite3_stmt* stmtAllDoctors;

    // Prepare the SQL statement for fetching all doctors
    if (sqlite3_prepare_v2(db, sqlAllDoctors, -1, &stmtAllDoctors, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement for all doctors: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Iterate through each doctor record
    while (sqlite3_step(stmtAllDoctors) == SQLITE_ROW) {
        // Retrieve doctor details
        int doctorId = sqlite3_column_int(stmtAllDoctors, 0);
        string name(reinterpret_cast<const char*>(sqlite3_column_text(stmtAllDoctors, 1)));
        string specialization(reinterpret_cast<const char*>(sqlite3_column_text(stmtAllDoctors, 2)));

        // Display the doctor information
        cout << "Doctor ID: " << doctorId << endl;
        cout << "Name: " << name << endl;
        cout << "Specialization: " << specialization << endl;
        cout << "-----------------------------------" << endl;
    }

    // Finalize the statement
    sqlite3_finalize(stmtAllDoctors);
}

// Function to search for a doctor by ID or name
Doctor* searchDoctorByIdOrName(sqlite3* db, const string& query) {
    const char* sql = "SELECT * FROM Doctor WHERE Name LIKE ? OR DoctorID = ?;";
    sqlite3_stmt* stmt;
    Doctor* foundDoctor = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement." << endl;
        return nullptr;
    }

    // Bind the search query for both name and ID
    sqlite3_bind_text(stmt, 1, ('%' + query + '%').c_str(), -1, SQLITE_STATIC);
    int doctorId = 0;
    try {
        doctorId = stoi(query);
    } catch (...) {
        doctorId = 0;
    }
    sqlite3_bind_int(stmt, 2, doctorId);

    // Fetch result
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        string specialization(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));

        foundDoctor = new Doctor(id, name, specialization);
    }

    sqlite3_finalize(stmt);
    return foundDoctor;
}

// Doctor handler for managing doctors
void doctorHandler() {
    
    int rc = sqlite3_open("hospital.db", &db);

    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // Create Doctor table if it doesn't exist
    if (!createDoctorTable(db)) {
        sqlite3_close(db);
        return;
    }

    while (true) {
        int choice;
        cout << "\nDoctor Panel:" << endl;
        cout << "1. Add Doctor" << endl;
        cout << "2. Assign Patient to Doctor" << endl;
        cout << "3. Display All Doctors" << endl;
        cout << "4. Search Doctor" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 5) {
            break;
        }

        switch (choice) {
            case 1: {
                int id;
                string name, specialization;
                cout << "Enter Doctor ID: ";
                cin >> id;
                cin.ignore(10000, '\n');  // Clear buffer
                cout << "Enter Doctor Name: ";
                getline(cin, name);
                cout << "Enter Specialization: ";
                getline(cin, specialization);
                Doctor doctor(id, name, specialization);
                insertDoctor(db, doctor);
                break;
            }
            case 2: {
                string query;
                cout << "Enter Doctor Name or ID: ";
                cin.ignore();
                getline(cin, query);
                Doctor* doctor = searchDoctorByIdOrName(db, query);
                if (doctor) {
                    cout << "Doctor found: " << doctor->doctorName << endl;

                    // Assign a patient to this doctor
                    cout << "Assign a patient to this doctor." << endl;
                    patienthandler();
                    // Assuming patienthandler will return a Patient object
                    // doctor->assignPatientToDoctor(patient);

                    delete doctor;
                } else {
                    cout << "Doctor not found." << endl;
                }
                break;
            }
            case 3:
                displayAllDoctors(db);
                break;
            case 4: {
                string query;
                cout << "Enter Doctor Name or ID: ";
                cin.ignore();
                getline(cin, query);
                Doctor* doctor = searchDoctorByIdOrName(db, query);
                if (doctor) {
                    cout << "Doctor ID: " << doctor->doctorId << endl;
                    cout << "Name: " << doctor->doctorName << endl;
                    cout << "Specialization: " << doctor->specialization << endl;
                    delete doctor;
                } else {
                    cout << "Doctor not found." << endl;
                }
                break;
            }
            default:
                cout << "Invalid choice!" << endl;
        }
    }

    sqlite3_close(db);
}
