#include <iostream>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include "stdio_linux.h"


#define true 1
#define false 0


using std::ios;
using std::cin;
using std::cout;
using std::system;
using std::ifstream;
using std::ofstream;


char fileUsername[15] = "guest";
const char FILEPNR[8] = "pnr.txt";
const char italicsEnd[5] = "\e[0m";
const char italicsStart[5] = "\e[3m";
const char FILEDB[13] = "database.txt";
const char FILENAME[12] = "flights.txt";
static const char ALPHANUM[37] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int loggedAsGuest = true;
int loggedAsAdmin = false;
int loggedAsUser = false;


struct PNR{
    char paxName[24];
    char FlightNumber[7];
    int paxCount;
    char PNR[7];
    char username[15];
};

struct Time{
    char hours[3];
    char minutes[3];
};

struct Flight{
    char FlightNumber[7];
    char source[4];
    char destination[4];
    Time departure;
    Time arrival;
};

struct Database{
    char username[15];
    char password[24];
};


int PNRExists(char PNRNumber[7]);
int flightExists(char fNumber[6]);
int usernameAvailable(char* username);


void init();                                                    //1. Flightthingy.cpp
void login();                                                   //2. Flightthingy.cpp
void line(int);
void userMenu();
void checkPNR();                                                //a. Without_login.cpp
void mainMenu();                                                //3. Flightthingy.cpp
void adminMenu();
void addFlight();                                               //b. Without_login.cpp
void createUser();                                              //4. Flightthingy.cpp
void viewFlights();                                             //c. Without_login.cpp
void viewAllPNRs();
void viewBookings();
void viewAllFlights();                                          //d. Without_login.cpp
void continueAsGuest();                                         //5. Flightthingy.cpp
void bookFlight(Flight flight[], int);                          //e. Without_login.cpp
void sortFlight(Flight flight[], int);                          //f. Without_login.cpp
void displayStruct(Flight flight[], int j);                     //g. Without_login.cpp
void displayStruct(Flight flight, PNR pnr);                     //h. Without_login.cpp
void swapFlightStruct(Flight &x, Flight &y);                    //i. Without_login.cpp
void displayPNRStruct(Flight flight, PNR pnr, int i);
void loadingScreen(char *string, int times, int showLoading);   //6. Flightthingy.cpp


int main(){
    init();
    mainMenu();
    return 0;
}

int PNRExists(char PNRNumber[7]){
    PNR pnr;
    ifstream file;
    
    file.open(FILEPNR, ios::in);

    if (!file)
        return false;
    
    while (!file.eof()){
        file.read((char*) &pnr, sizeof(PNR));
        if (!strcmp(PNRNumber, pnr.PNR)){
            file.close();
            return true;
        }
    }

    file.close();

    return false;
}

int flightExists(char fNumber[6]){
    Flight flight;
    ifstream file;
    
    file.open(FILENAME, ios::in);

    if (!file)
        return false;

    while(!file.eof()){
        file.read((char*) &flight, sizeof(Flight));
        if (!strcasecmp(fNumber, flight.FlightNumber)){
            file.close();
            return true;
        }
    }

    file.close();

    return false;
}

int usernameAvailable(char* username){
    ifstream file;
    file.open(FILEDB, ios::in);
    
    if (!file)
        return true;

    Database database;
    
    while(!file.eof()){
        file.read((char *) &database, sizeof(database));
        if (!strcmp(database.username, username)){
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;
}


void init(){
    if (usernameAvailable((char *) "admin")){
        ofstream file;
        file.open(FILEDB, ios::out);
        Database database;
        strcpy(database.username, "admin");
        strcpy(database.password, "admin");
        file.write((char*) &database, sizeof(Database));
        file.close();
    }
}

void login(){
    int loginSuccess = false;
    ifstream file;
    Database database;
    file.open(FILEDB, ios::in);

    cout << "Login\n";
    line(28);
    char username[15];
    char password[25];
    char ch;
    int i;
    cout << "username : ";
    cin >> username;
    cout << "password : ";
    for(i = 0;;){
        ch = getch();

        if (ch >= 33 && ch <= 126){
            password[i++] = ch;
            cout << "*";
        }
        
        if (ch == 127 && i >= 1){
            cout<<"\b \b";
            --i;
        }
        
        if (ch == 10 && i >= 1){
            password[i] = '\0';
            break;
        }
    }
    loadingScreen((char *) "Logging in", 8, false);
    
    while(!file.eof()){
        file.read((char *) &database, sizeof(Database));
        if (!strcmp(username, database.username)){
            if (!strcmp(password, database.password)){
                if (!strcmp(username, "admin")){
                    loggedAsAdmin = true;
                    loggedAsUser = false;
                    loggedAsGuest = false;
                }
                loginSuccess = true;
            }
        }
    }

    if (!loginSuccess){
        char ch;
        cout << "Incorrect username of password!\n";
        cout << "Try loggin in again? (y/n)";
        ch = getch();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){
            loadingScreen((char *) "Returning to login page", 3, false);
            login();
        }
    }
    else {
        cout << "Logged in successfully!\n";
        if (loggedAsAdmin){
            strcpy(fileUsername, "admin");
            adminMenu();
        } else {
            strcpy(fileUsername, username);
            loggedAsUser = true;
            loggedAsAdmin = false;
            loggedAsGuest = false;
            userMenu();
        }
    }
}

void line(int k){
    for (int i = 0; i < k; ++i)
        cout << '-';
    cout << '\n';
}

void userMenu(){
    char ch;
    system("clear");
    cout << italicsStart << "Choose an option to continue" << italicsEnd << '\n';
    line(30);
    cout << "1 : Book flight \n";
    cout << "2 : View bookings \n";
    cout << "3 : PNR Enquiry \n";
    cout << italicsStart << "0 : Logout and Quit" << italicsEnd << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening book flights page", 6, true);
            viewFlights();
            break;
        case '2':
            loadingScreen((char *) "Opening view bookings page", 4, true);
            viewBookings();
            break;
        case '3':
            loadingScreen((char *) "Opening PNR Enquiry page", 3, true);
            checkPNR();
        case '0':
            break;
        default:
            userMenu();
            break;
    }
}

void checkPNR(){
    PNR pnr;
    Flight flight;
    ifstream filePNR;
    ifstream fileFlight;
    int PNRExists = false;
    char PNRNumber[7];
    char flightNumber[7];
    
    cout << "Enter PNR : ";
    cin.getline(PNRNumber, sizeof(PNRNumber));

    filePNR.open(FILEPNR, ios::in);  

    while(!filePNR.eof()){
        filePNR.read((char*) &pnr, sizeof(PNR));
        if (!strcasecmp(PNRNumber, pnr.PNR)){
            PNRExists = true;
            strcpy(flightNumber, pnr.FlightNumber);
            break;
        }
    }

    filePNR.close();
    
    if(!PNRExists)
        cout << "Sorry, the entered PNR is not valid!\n";
    else {
        fileFlight.open(FILENAME, ios::in);
        while(!fileFlight.eof()){
            fileFlight.read((char*) &flight, sizeof(Flight));
            if (!strcmp(flightNumber, flight.FlightNumber)){
                displayStruct(flight, pnr);
                break;
            }
        }
    }

    fileFlight.close();

}

void mainMenu(){
    char ch;
    system("clear");
    cout << "Welcome to Generic Airline Booking! \n"; 
    line(30);
    cout << italicsStart << "Choose an option to continue" << italicsEnd << '\n';
    line(30);
    cout << "1 : Login \n";
    cout << "2 : Create account \n";
    cout << "3 : Continue as guest \n";
    cout << italicsStart << "0 : Quit" << italicsEnd << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening login page", 6, true);
            login();
            break;
        case '2':
            loadingScreen((char *) "Creating new account", 8, true);
            createUser();
            break;
        case '3':
            loadingScreen((char *) "Continuing as guest", 8, false);
            continueAsGuest();
            break;
        case '0':
            break;
        default:
            mainMenu();
            break;
    }
}

void adminMenu(){
    char ch;
    system("clear");
    cout << italicsStart << "Choose an option to continue" << italicsEnd << '\n';
    line(30);
    cout << "1 : Add Flights \n";
    cout << "2 : View all PNRs \n";
    cout << "3 : View all Flights \n";
    cout << italicsStart << "0 : Logout and Quit" << italicsEnd << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening add flights page", 6, true);
            addFlight();
            break;
        case '2':
            loadingScreen((char *) "Opening view all PNRs page", 8, true);
            viewAllPNRs();
            break;
        case '3':
            loadingScreen((char *) "Opening view all flights page", 8, true);
            viewAllFlights();
            break;
        case '0':
            break;
        default:
            adminMenu();
    }
}

void addFlight(){
    ofstream file;
    Flight flight;

    file.open(FILENAME, ios::app);

    cout << "Enter FlightNumber\t: ";
    cin >> flight.FlightNumber;

    if (flightExists(flight.FlightNumber)){
        char ch;
        cout << "ERROR! Flight already exists!\n";
        file.close();
        cout << "Still add another flight? (y/n) : ";
        cin.ignore();
        ch = getche();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){
            loadingScreen((char *) "Refreshing", 2, false);
            addFlight();
        } else 
            loadingScreen((char *) "Returning to menu", 4, false);
            adminMenu();
    } else {
        cout << "Enter Source\t\t: ";
        cin >> flight.source;
        cout << "Enter destination\t: ";
        cin >> flight.destination;
        cout << "Enter Departure hours\t: ";
        cin >> flight.departure.hours;
        cout << "Enter Departure minutes\t: ";
        cin >> flight.departure.minutes;
        cout << "Enter Arrival hours\t: ";
        cin >> flight.arrival.hours;
        cout << "Enter Arrival minutes\t: ";
        cin >> flight.arrival.minutes;

        cout << flight.FlightNumber << " successfully added!\n";
        file.write((char*) &flight, sizeof(Flight));
        file.close();
        char ch;
        cout << "Add another flight? (y/n) : ";
        cin.ignore();
        ch = getch();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){
            loadingScreen((char *) "Opening add flights page", 2, true);
            addFlight();        
        } else {
            loadingScreen((char *) "Returning to menu", 2, false);
            adminMenu();
        }
    }
}

void createUser(){
    ofstream file;
    Database database;

    system("clear");
    cout << "Create New User\n";
    line(20);
    char username[15];
    char password[25];
    char passwordRepeat[25];
    char ch;
    int i;
    cout << "Select username : ";
    cin >> username;
    if (!usernameAvailable(username) || !strcmp(username, "guest")) {
        cout << "Username not available! Press 1 to try again!\n";
        char ch;
        cin.ignore();
        ch = getch();
        if (ch == '1')
            createUser();
        return;
    } else
	cout << "Username available!\n";
    cout << "Password : ";
    for(i = 0;;){
        ch = getch();

        if (ch >= 33 && ch <= 126){
            password[i++] = ch;
            cout << "*";
        }
        
        if (ch == 127 && i >= 1){
            cout<<"\b \b";
            --i;
        }
        
        if (ch == 10 && i >= 1){
            password[i] = '\0';
            break;
        }
    }
    cout << "\nReenter password : ";
    for(i = 0;;){
        ch = getch();

        if (ch >= 33 && ch <= 126){
            passwordRepeat[i++] = ch;
            cout << "*";
        }
        
        if (ch == 127 && i >= 1){
            cout<<"\b \b";
            --i;
        }
        
        if (ch == 10 && i >= 1){
            passwordRepeat[i] = '\0';
            break;
        }
    }
    if (strcmp(password, passwordRepeat)){
        cout << "\nPasswords do not match!!!\n";
        cout << "Please try again!\n";
        cout << "Press 1 to try again!\n";
        char ch;
        ch = getch();
        if (ch == '1')
            createUser();
        else
            return;
    } else {
        loadingScreen((char *) "creating user", 10, true);
        strcpy(database.username, username);
        strcpy(database.password, password);
        file.open(FILEDB, ios::app);
        file.write((char *) &database, sizeof(Database));
        file.close();
        loadingScreen((char *) "User created, heading to login page", 10, true);
        login();
    }
}

void viewFlights(){
    Flight flight[10];
    Flight send[10];
    ifstream  file;
    int i = 0;
    int j = 0;
    int exist = false;
    char source[4];
    char destination[4];

    cout << "Book a flight!\n";
    line(24);
    cout << "Enter source      : ";
    cin >> source;
    cout << "Enter destination : ";
    cin >> destination;

    file.open(FILENAME, ios::in);

    while(1){
        file.read((char*) &flight[i], sizeof(Flight));
        if (file.eof())
            break;
        if (!strcasecmp(source, flight[i].source)){
            if (!strcasecmp(destination, flight[i].destination)){
                send[j] = flight[i];
                ++j;
                exist = true;
            }
            ++i;
        }
    }

    if (exist){
        int choice;
        displayStruct(send, j);
        tryAgain:
            cout << "Select S.no to book flight (0 to quit) : ";
            cin >> choice;
            if (!choice)
                return;
            else if (choice > j || choice < 0){
                cout << "Out of bounds!\n";
                goto tryAgain;
            } else
                bookFlight(send, choice);
    }
    else
        cout << "\nOh oh, no flights on this route availabe :\\\n\n";

    file.close();

}

void viewAllPNRs(){
    PNR pnr;
    int i = 0;
    Flight flight;
    ifstream filePNR;
    ifstream fileFlight;
    char flightNumber[7];

    filePNR.open(FILEPNR, ios::in);  
    fileFlight.open(FILENAME, ios::in);

    while(!filePNR.eof()){
        filePNR.read((char*) &pnr, sizeof(PNR));
        fileFlight.seekg(0, ios::beg);
        if(filePNR.eof())
            break;
        while(!fileFlight.eof()){
            fileFlight.read((char*) &flight, sizeof(Flight));
            if (!strcmp(pnr.FlightNumber, flight.FlightNumber)){
                displayPNRStruct(flight, pnr, i++);
                break;
            }
        }
    }

    line(112);

    filePNR.close();
    fileFlight.close();
    
    cout << "Press 1 to go back" << '\n';
    char ch;
    ch = getch();
    if (ch == '1'){
        loadingScreen((char *) "Returning to menu", 2, false);
        adminMenu();
    }
}

void viewBookings(){
    PNR pnr;
    int i = 0;
    Flight flight;
    ifstream filePNR;
    ifstream fileFlight;
    char flightNumber[7];

    filePNR.open(FILEPNR, ios::in);  
    fileFlight.open(FILENAME, ios::in);

    while(!filePNR.eof()){
        filePNR.read((char*) &pnr, sizeof(PNR));
        fileFlight.seekg(0, ios::beg);
        if(filePNR.eof())
            break;
        if (!strcmp(pnr.username, fileUsername)){
            if (strcmp(pnr.username, "guest")){
                while(!fileFlight.eof()){
                    fileFlight.read((char*) &flight, sizeof(Flight));
                    if (!strcmp(pnr.FlightNumber, flight.FlightNumber)){
                        displayPNRStruct(flight, pnr, i++);
                        break;
                    }
                }
            }
        }
    }

    line(112);

    filePNR.close();
    fileFlight.close();
    
    cout << "Press 1 to go back" << '\n';
    char ch;
    ch = getch();
    if (ch == '1'){
        loadingScreen((char *) "Returning to menu", 2, false);
        userMenu();
    }
}

void viewAllFlights(){
    int i = 1;
    Flight flight;
    ifstream  file;

    file.open(FILENAME, ios::in);

    if(file){
        line(71);
        cout << "S.No\tFlightNumber\tSource\tDestionation\tDeparture\tArrival\n";
        line(71);
    }

    while(1){
        file.read((char*) &flight, sizeof(Flight));
        if (file.eof())
            break;
        cout << i++ << '\t' << flight.FlightNumber << "\t\t" << flight.source
             << '\t' << flight.destination << "\t\t" << flight.departure.hours
             << ':' << flight.departure.minutes << "\t\t"
             << flight.arrival.hours << ':' << flight.arrival.minutes 
             << '\n';
    }

    line(71);

    file.close();

    cout << "Press 1 to go back\n";
    char ch;
    ch = getch();
    if (ch == '1'){
        loadingScreen((char *) "Returning to menu", 4, false);
        adminMenu();
    }
}

void continueAsGuest(){
    char ch;
    system("clear");
    cout << italicsStart << "Choose an option to continue" << italicsEnd << '\n';
    line(30);
    cout << "1 : Book flight \n";
    cout << "2 : PNR Enquiry \n";
    cout << italicsStart << "0 : Quit" << italicsEnd << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening book flights page", 6, true);
            viewFlights();
            break;
        case '2':
            loadingScreen((char *) "Opening PNR Enquiry page", 3, true);
            checkPNR();
            cout << "Press 1 to go back\n";
            char choice;
            choice = getch();
            if (choice == '1'){
                loadingScreen((char *) "Returning to menu", 4, false);
                continueAsGuest();
            }
            break;
        case '0':
            break;
        default:
            continueAsGuest();
            break;
    }
}

void bookFlight(Flight flight[], int c){
    ofstream file;
    PNR pnr;
    char passengersChar;
    int paxCount, reference;
    char paxName[24];
    char PNRNumber[7];

    file.open(FILEPNR, ios::app);
    --c;

    cout << "Booking on " << flight[c].FlightNumber << '\n';
    cout << "Enter number of passengers (Maximum 10) : ";
    cin.ignore();
    passengersChar = getche();
    cout << '\n';
    paxCount = passengersChar - '0';

    cout << "Enter name of any 1 passenger : "; 
    cin.getline(paxName, sizeof(paxName));

    srand(time(NULL));

    do {
        for (int i = 0; i < 7; ++i)
            PNRNumber[i] = ALPHANUM[rand() % 37];
        PNRNumber[6] = 0;
    } while(PNRExists(PNRNumber));

    pnr.paxCount = paxCount;
    strcpy(pnr.PNR, PNRNumber);
    strcpy(pnr.paxName, paxName);
    strcpy(pnr.username, fileUsername);
    strcpy(pnr.FlightNumber, flight[c].FlightNumber);

    file.write((char*) &pnr, sizeof(pnr));

    cout << "Your PNR : " << PNRNumber << '\n';
    cout << "Please go to your nearest airport and pay against this PNR\n";
    
    file.close();
}

void sortFlight(Flight flight[], int n){
    for (int i = 0; i < n - 1; ++i){
        for (int j = 0; j < n - i - 1; ++j){
            if (flight[j].departure.hours[0] > flight[j + 1].departure.hours[0])
                swapFlightStruct(flight[j], flight[j + 1]);
            else if (flight[j].departure.hours[0] == flight[j + 1].departure.hours[0]){
                if (flight[j].departure.hours[1] > flight[j + 1].departure.hours[1])
                    swapFlightStruct(flight[j], flight[j + 1]);
                else if (flight[j].departure.hours[1] == flight[j + 1].departure.hours[1]){
                    if (flight[j].departure.minutes[0] > flight[j + 1].departure.minutes[0])
                        swapFlightStruct(flight[j], flight[j + 1]);
                    else if (flight[j].departure.minutes[0] == flight[j + 1].departure.minutes[0]){
                        if (flight[j].departure.minutes[1] > flight[j].departure.minutes[1])
                            swapFlightStruct(flight[j], flight[j + 1]);
                        else if (flight[j].departure.minutes[1] == flight[j].departure.minutes[1]){
                            if (flight[j].arrival.hours[0] > flight[j + 1].arrival.hours[0])
                                swapFlightStruct(flight[j], flight[j + 1]);
                            else if (flight[j].arrival.hours[0] == flight[j + 1].arrival.hours[0]){
                                if (flight[j].arrival.hours[1] > flight[j + 1].arrival.hours[1])
                                    swapFlightStruct(flight[j], flight[j + 1]);
                                else if (flight[j].arrival.hours[1] == flight[j + 1].arrival.hours[1]){
                                    if (flight[j].arrival.minutes[0] > flight[j + 1].arrival.minutes[0])
                                        swapFlightStruct(flight[j], flight[j + 1]);
                                    else if (flight[j].arrival.minutes[0] == flight[j + 1].arrival.minutes[0]){
                                        if (flight[j].arrival.minutes[1] > flight[j + 1].arrival.minutes[1])
                                            swapFlightStruct(flight[j], flight[j + 1]);
                                        else if (flight[j].arrival.minutes[1] == flight[j + 1].arrival.minutes[1]){
                                            for (int k = 0; k < strlen(flight[j].FlightNumber); ++k){
                                                if ((int) flight[j].FlightNumber[k]  - (int) flight[j + 1].FlightNumber[k] > 0){
                                                    swapFlightStruct(flight[j], flight[j + 1]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }                
                }
            }
        }
    }
}

void displayStruct(Flight flight[], int j){
    if (j != 1)
        sortFlight(flight, j);
    
    line(71);                       
    cout << "S.No\tFlightNumber\tSource\tDestionation\tDeparture\tArrival\n";
    line(71);                       

    for (int i = 0; i < j; ++i){
        cout << i+1 << '\t' << flight[i].FlightNumber << "\t\t" << flight[i].source
             << '\t' << flight[i].destination << "\t\t" << flight[i].departure.hours
             << ':' << flight[i].departure.minutes << "\t\t"
             << flight[i].arrival.hours << ':' << flight[i].arrival.minutes 
             << '\n';
    }

    line(71);                           
}

void displayStruct(Flight flight, PNR pnr){
    line(98);                       
    cout << "FlightNumber\tSource\tDestionation\tDeparture\tArrival\tPassengers\tName\n";
    line(98);                           

    cout << flight.FlightNumber << "\t\t" << flight.source
         << '\t' << flight.destination << "\t\t" << flight.departure.hours
         << ':' << flight.departure.minutes << "\t\t"
         << flight.arrival.hours << ':' << flight.arrival.minutes 
         << '\t' << pnr.paxCount << "\t\t" << pnr.paxName << '\n';
    
    line(98);                           
}

void swapFlightStruct(Flight &x, Flight &y){
    Flight temp = x;
    x = y;
    y = temp;
}

void displayPNRStruct(Flight flight, PNR pnr, int i){
    if (i == 0){
        line(112);                       
        cout << "PNR\tFlightNumber\tSource\tDestionation\tDeparture\tArrival\tPassengers\tName\t\tUsername\n";
        line(112);
    }

    cout << pnr.PNR << '\t' << flight.FlightNumber << "\t\t" << flight.source
         << '\t' << flight.destination << "\t\t" << flight.departure.hours
         << ':' << flight.departure.minutes << "\t\t"
         << flight.arrival.hours << ':' << flight.arrival.minutes 
         << '\t' << pnr.paxCount << "\t\t" << pnr.paxName;
    if (strlen(pnr.paxName) > 8)
        cout << '\t';
    else
        cout << "\t\t";     
    cout << pnr.username << '\n';
}

void loadingScreen(char *string, int times, int showLoading){
    system("clear");
    usleep(250000); //usleep taken in microseconds (as good as 0.25s)
    if (showLoading){
        cout << string << '\n';
        cout << "Loading...\n";
    } else {
        cout << string << "...\n";
    }
    usleep(250000);
    int loadingAnim = 0;
    int iterations = 0;
    while(iterations < times){
        system("clear");
        if(showLoading)
            cout << string << '\n';
        switch(loadingAnim){
            case 0:
                if(showLoading)
                    cout << "Loading...\\\n";
                else
                    cout << string << "...\\\n";
                break;
            case 1:
                if(showLoading)
                    cout << "Loading...|\n";
                else
                    cout << string << "...|\n";
                break;
            case 2:
                if(showLoading)
                    cout << "Loading.../\n";
                else
                    cout << string << ".../\n";
                break;
            case 3:
                if(showLoading)
                    cout << "Loading...-\n";
                else
                    cout << string << "...-\n";
                loadingAnim = -1;                
                break;
        }
        ++loadingAnim;
        ++iterations;
        usleep(250000);
    }
    system("clear");
}

