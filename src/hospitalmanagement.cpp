#include <iostream>
#include <fstream>
#include <string>
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
            int n, icu, op;
            cout << "INITIAL SETUP " << endl;
            cout << "Enter the number of beds in Ward: "; cin >> n;
            cout << "Enter the number of ICU beds: "; cin >> icu;
            cout << "Enter the number of operation theatres: "; cin >> op;
            outfile << n << endl;
            outfile << icu << endl;
            outfile << op << endl;
            outfile.close();
            cout << "SETUP COMPLETED" << endl;
        }
    }
}

// Helper function to read current resources from the file
void readResources(int &beds, int &icuBeds, int &operationTheatres) {
    ifstream infile("data.txt");
    infile >> beds >> icuBeds >> operationTheatres;
    infile.close();
}

// Helper function to update resources in the file
void updateResources(int beds, int icuBeds, int operationTheatres) {
    ofstream outfile("data.txt");
    outfile << beds << endl;
    outfile << icuBeds << endl;
    outfile << operationTheatres << endl;
    outfile.close();
}

// Manage ICU beds (admit or discharge patients)
void manageICU() {
    int icuBeds, operationTheatres, wardBeds;
    readResources(wardBeds, icuBeds, operationTheatres);

    cout << "\nICU Management: " << endl;
    cout << "Current ICU Beds Available: " << icuBeds << endl;

    int action, count;
    cout << "1. Admit to ICU\n2. Discharge from ICU\nEnter choice: "; cin >> action;
    
    if (action == 1) {
        cout << "Enter number of patients to admit: "; cin >> count;
        if (count <= icuBeds) {
            icuBeds -= count;
            cout << count << " patients admitted to ICU." << endl;
        } else {
            cout << "Not enough ICU beds available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number of patients to discharge: "; cin >> count;
        icuBeds += count;
        cout << count << " patients discharged from ICU." << endl;
    } else {
        cout << "Invalid action." << endl;
    }

    updateResources(wardBeds, icuBeds, operationTheatres);
}

// Manage Operation Theatres (schedule or finish surgeries)
void manageOperationTheatre() {
    int icuBeds, operationTheatres, wardBeds;
    readResources(wardBeds, icuBeds, operationTheatres);

    cout << "\nOperation Theatre Management: " << endl;
    cout << "Current Operation Theatres Available: " << operationTheatres << endl;

    int action, count;
    cout << "1. Schedule Surgery\n2. Finish Surgery\nEnter choice: "; cin >> action;

    if (action == 1) {
        cout << "Enter number of surgeries to schedule: "; cin >> count;
        if (count <= operationTheatres) {
            operationTheatres -= count;
            cout << count << " surgeries scheduled." << endl;
        } else {
            cout << "Not enough operation theatres available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number of surgeries completed: "; cin >> count;
        operationTheatres += count;
        cout << count << " surgeries completed." << endl;
    } else {
        cout << "Invalid action." << endl;
    }

    updateResources(wardBeds, icuBeds, operationTheatres);
}

// Manage Ward beds (admit or discharge patients)
void manageWard() {
    int icuBeds, operationTheatres, wardBeds;
    readResources(wardBeds, icuBeds, operationTheatres);

    cout << "\nWard Management: " << endl;
    cout << "Current Ward Beds Available: " << wardBeds << endl;

    int action, count;
    cout << "1. Admit to Ward\n2. Discharge from Ward\nEnter choice: "; cin >> action;

    if (action == 1) {
        cout << "Enter number of patients to admit: "; cin >> count;
        if (count <= wardBeds) {
            wardBeds -= count;
            cout << count << " patients admitted to Ward." << endl;
        } else {
            cout << "Not enough Ward beds available." << endl;
        }
    } else if (action == 2) {
        cout << "Enter number of patients to discharge: "; cin >> count;
        wardBeds += count;
        cout << count << " patients discharged from Ward." << endl;
    } else {
        cout << "Invalid action." << endl;
    }

    updateResources(wardBeds, icuBeds, operationTheatres);
}

// Display current available resources
void displayResources() {
    int icuBeds, operationTheatres, wardBeds;
    readResources(wardBeds, icuBeds, operationTheatres);

    cout << "\nCurrent Resource Availability: " << endl;
    cout << "Ward Beds: " << wardBeds << endl;
    cout << "ICU Beds: " << icuBeds << endl;
    cout << "Operation Theatres: " << operationTheatres << endl;
}
void managementhandler() {
    int choice;

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
