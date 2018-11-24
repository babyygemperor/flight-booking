//THIS IS DONE IN C++3.0
//A PROGRAM TO MANAGE ALL THE FLIGHT BOOKINGS
//INCLUDING LOGGING IN AND ALL SORTS OF STUFF
//MADE IN 32 HOURS, I guess


#include <fstream.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <dos.h>


#define true 1                        //Much better than typing return 1
#define false 0                       //Much better than typing return 0


char fileUsername[15] = "guest";                                //global username who is logged in
const char FILEPNR[8] = "pnr.txt";                              //file for storing PNR
const char FILEDB[13] = "database.txt";                         //file for storing username and password
const char FILENAME[12] = "flights.txt";                        //file for storing flight details
static const char ALPHANUM[37] = "0123456789"                   //using these characters only to generate PNR
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

int loggedAsGuest = true;             //Booleans to
int loggedAsAdmin = false;            //check who
int loggedAsUser = false;             //is logged in


struct PNR{                           //Structure for storing in FILEPNR
    char paxName[24];
    char FlightNumber[7];
    int paxCount;
    char PNR[7];
    char username[15];
};

struct Time{                          //Structure to help with arrival and departure times
    char hours[3];
    char minutes[3];
};

struct Flight{                        //Structure for storing in FILENAME                          
    char FlightNumber[7];
    char source[4];
    char destination[4];
    Time departure;
    Time arrival;
};

struct Database{                      //Structure for storing in FILEDB
    char username[15];
    char password[24];
};


int PNRExists(char PNRNumber[7]);               //returns true if PNR already exists in file, else false
int flightExists(char fNumber[6]);              //returns true if flight already exists in file, else false
int usernameAvailable(char* username);          //returns false if username already exists in file, else true


void init();                                                    //Initializes files if first run
void login();                                                   //Logs in the user
void line(int);                                                 //prints out i times lines
void userMenu();                                                //menu for regular user
void checkPNR();                                                //checks the validity of PNR and prints it
void mainMenu();                                                //master menu
void adminMenu();                                               //menu for admin ONLY
void addFlight();                                               //function available ONLY for ADMIN to add flight to file
void createUser();                                              //create a new user if not exists
void viewFlights();                                             //function to view available flights on routes
void viewAllPNRs();                                             //function available ONLY for ADMIN to see all PNRs in file
void viewBookings();                                            //function to see bookings in file
void viewAllFlights();                                          //function available ONLY for ADMIN to see all flights in file
void continueAsGuest();                                         //to continue without login or signing up
void bookFlight(Flight flight[], int);                          //function to book a flight selected
void sortFlight(Flight flight[], int);                          //function to sort flights by departure > arrival > name in asc order
void displayStruct(Flight flight[], int j);                     //function to display the flight structure in presentable format
void displayStruct(Flight flight, PNR pnr);                     //same as above but referenced with PNR
void swapFlightStruct(Flight &x, Flight &y);                    //just like swapping integers, but this time its structures
void displayPNRStruct(Flight flight, PNR pnr, int i);           //function to display the PNR in a presentable format
void loadingScreen(char *string, int times, int showLoading);   //the best part of this whole program (animates fake loading screen)


int main(){
    init();                                                     //Initializes files if first run
    mainMenu();                                                 //master menu
    getch();
    return 0;
}

int PNRExists(char PNRNumber[7]){               //returns true if PNR already exists in file, else false
    PNR pnr;
    ifstream file;
    
    file.open(FILEPNR, ios::in);

    if (!file)                                  //file does not exist, therefore no PNR exists
        return false;
    
    while (!file.eof()){
        file.read((char*) &pnr, sizeof(PNR));
        if (!strcmp(PNRNumber, pnr.PNR)){      //pnr of such kind exists
            file.close();
            return true;
        }
    }

    file.close();

    return false;                             //Kein PNR wurde gefunden
}

int flightExists(char fNumber[6]){                      //returns true if flight already exists in file, else false
    Flight flight;
    ifstream file;
    
    file.open(FILENAME, ios::in);

    if (!file)                                          //file does not exist, therefore no flight exists
        return false;

    while(!file.eof()){
        file.read((char*) &flight, sizeof(Flight));
        if (!strcmpi(fNumber, flight.FlightNumber)){    //flight of such kind exists
            file.close();
            return true;
        }
    }

    file.close();

    return false;                                       //Kein Flug wurde gefunden
}

int usernameAvailable(char* username){                      //returns false if username already exists in file, else true
    Database database;
    ifstream file;
    
    file.open(FILEDB, ios::in);
    
    if (!file)                                              //file does not exist, therefore no username exists
        return true;
    
    while(!file.eof()){
        file.read((char *) &database, sizeof(database));
        if (!strcmp(database.username, username)){          //username of such kind exists
            file.close();
            return false;
        }
    }
    
    file.close();
    return true;                                            //Kein Nutzername wurde gefunden
}


void init(){                                                //Initializes files if first run
    if (usernameAvailable((char *) "admin")){               //Sees if "admin" is available or not
        ofstream file;
        file.open(FILEDB, ios::out);
        Database database;
        strcpy(database.username, "admin");
        strcpy(database.password, "admin");
        file.write((char*) &database, sizeof(Database));
        file.close();
    }
}

void login(){                                   //Logs in the user
    int loginSuccess = false;                   //boolean to see if login successful or not
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
    for(i = 0;;){                                   //loop to enter password
        ch = getch();

        if (ch >= 33 && ch <= 126){                 //logical thing entered
            password[i++] = ch;
            cout << "*";                            //displaying password is not a good idea
        }
        
        if (ch == 127 && i >= 1){
            cout<<"\b \b";                          //mimicking backspace
            --i;                                    //removing the last index
        }
        
        if (ch == 10 && i >= 1){                    //checks if enter pressed
            password[i] = '\0';                     //adds null character and ends loop
            break;
        }
    }
    loadingScreen((char *) "Logging in", 8, false); //fake loading screen to show logging in
    
    while(!file.eof()){
        file.read((char *) &database, sizeof(Database));
        if (!strcmp(username, database.username)){          //correct username
            if (!strcmp(password, database.password)){      //correct password
                if (!strcmp(username, "admin")){            //checks if admin is logged in
                    loggedAsAdmin = true;                   //yes indeed
                    loggedAsUser = false;
                    loggedAsGuest = false;
                }
                loginSuccess = true;                //confirms login success
            }
        }
    }

    if (!loginSuccess){                                         //if login failed
        char ch;
        cout << "Incorrect username of password!\n";
        cout << "Try logging in again? (y/n)";
        ch = getch();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){                            //try logging in again
            loadingScreen((char *) "Returning to login page", 3, false);
            login();                                            //try again
        }
    }
    else {
        cout << "Logged in successfully!\n";
        if (loggedAsAdmin){                                     //opens admin page then
            strcpy(fileUsername, "admin");
            adminMenu();                                        //menu for ADMIN ONLY
        } else {                                                //opens regular user page
            strcpy(fileUsername, username);
            loggedAsUser = true;
            loggedAsAdmin = false;
            loggedAsGuest = false;
            userMenu();                                         //menu for regular user
        }
    }
}

void line(int k){                                               //prints out i times lines
    for (int i = 0; i < k; ++i)
        cout << '-';
    cout << '\n';
}

void userMenu(){                                                                //menu for regular user
    char ch;
    clrscr();
    cout << "Choose an option to continue" << '\n';
    line(30);
    cout << "1 : Book flight \n";
    cout << "2 : View bookings \n";
    cout << "3 : PNR Enquiry \n";
    cout << "0 : Logout and Quit" << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening book flights page", 6, true);
            viewFlights();                                                  //to book a flight
            break;
        case '2':
            loadingScreen((char *) "Opening view bookings page", 4, true);
            viewBookings();                                                 //to view all bookings
            break;
        case '3':
            loadingScreen((char *) "Opening PNR Enquiry page", 3, true);
            checkPNR();                                                     //to check a PNR
        case '0':
            break;
        default:
            userMenu();                                                 //refreshes the whole thing
            break;
    }
}

void checkPNR(){                                    //checks the validity of PNR and prints it
    PNR pnr;
    Flight flight;
    ifstream filePNR;
    ifstream fileFlight;
    int PNRExists = false;                          //boolean to see if PNR exists in file
    char PNRNumber[7];
    char flightNumber[7];
    
    cout << "Enter PNR : ";
    cin.getline(PNRNumber, sizeof(PNRNumber));

    filePNR.open(FILEPNR, ios::in);

    while(!filePNR.eof()){
        filePNR.read((char*) &pnr, sizeof(PNR));
        if (!strcmpi(PNRNumber, pnr.PNR)){              //PNR of such type exists
            PNRExists = true;                           //PNR Existence confirmed
            strcpy(flightNumber, pnr.FlightNumber);
            break;
        }
    }

    filePNR.close();
    
    if(!PNRExists)
        cout << "Sorry, the entered PNR is not valid!\n";
    else {
        fileFlight.open(FILENAME, ios::in);
        while(!fileFlight.eof()){                                   //To get flight info
            fileFlight.read((char*) &flight, sizeof(Flight));       //associated with
            if (!strcmp(flightNumber, flight.FlightNumber)){        //this PNR
                displayStruct(flight, pnr);                         //To display the data
                break;                                              //in a presentable format
            }
        }
    }

    fileFlight.close();

}

void mainMenu(){                                                    //master menu
    char ch;
    clrscr();
    cout << "Welcome to Generic Airline Booking! \n"; 
    line(30);
    cout << "Choose an option to continue" << '\n';
    line(30);
    cout << "1 : Login \n";
    cout << "2 : Create account \n";
    cout << "3 : Continue as guest \n";
    cout << "0 : Quit" << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening login page", 6, true);
            login();                                                        //to open login screen
            break;
        case '2':
            loadingScreen((char *) "Creating new account", 8, true);
            createUser();                                                   //create new user
            break;
        case '3':
            loadingScreen((char *) "Continuing as guest", 8, false);
            continueAsGuest();                                              //to continue as guest
            break;
        case '0':
            break;
        default:
            mainMenu();                                         //refreshes the whole thing
            break;
    }
}

void adminMenu(){                                               //menu for ADMIN ONLY
    char ch;
    clrscr();
    cout << "Choose an option to continue" << '\n';
    line(30);
    cout << "1 : Add Flights \n";
    cout << "2 : View all PNRs \n";
    cout << "3 : View all Flights \n";
    cout << "0 : Logout and Quit" << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening add flights page", 6, true);
            addFlight();                                                    //adds another flight
            break;
        case '2':
            loadingScreen((char *) "Opening view all PNRs page", 8, true);
            viewAllPNRs();                                                  //views all the PNRS
            break;
        case '3':
            loadingScreen((char *) "Opening view all flights page", 8, true);
            viewAllFlights();                                               //views all the flight
            break;
        case '0':
            break;
        default:
            adminMenu();                                                    //refreshes the whole thing
    }
}

void addFlight(){       //function available for ADMIN ONLY to add a flight to file
    ofstream file;
    Flight flight;

    file.open(FILENAME, ios::app);

    cout << "Enter FlightNumber\t: ";
    cin >> flight.FlightNumber;

    if (flightExists(flight.FlightNumber)){             //checks if flight number in use or not
        char ch;
        cout << "ERROR! Flight already exists!\n";
        file.close();
        cout << "Still add another flight? (y/n) : ";       //to continue with other flight number
        ch = getche();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){
            loadingScreen((char *) "Refreshing", 2, false);
            addFlight();                                    //refresh
        } else 
            loadingScreen((char *) "Returning to menu", 4, false);
            adminMenu();                                    //go back to admin menu
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
        cout << "Add another flight? (y/n) : ";                 //to add more flights
        ch = getch();
        cout << '\n';
        if (ch == 'y' || ch == 'Y'){
            loadingScreen((char *) "Opening add flights page", 2, true);
            addFlight();                                        //to add more flights
        } else {
            loadingScreen((char *) "Returning to menu", 2, false);
            adminMenu();                                        //back to admin menu
        }
    }
}

void createUser(){                                              //create new user if not exists
    ofstream file;
    Database database;

    clrscr();
    cout << "Create New User\n";
    line(20);
    char username[15];
    char password[25];
    char passwordRepeat[25];
    char ch;
    int i;
    cout << "Select username : ";
    cin >> username;
    if (!usernameAvailable(username) || !strcmp(username, "guest")) {       //to see if username available
        cout << "Username not available! Press 1 to try again!\n";
        char ch;
        ch = getch();
        if (ch == '1')
            createUser();
        return;
    } else
	cout << "Username available!\n";
    cout << "Password : ";
    cin >> password;
    for(i = 0;;){                       //complex algorithm to mimic password entry
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
    cout << "Reenter password : ";      //to prevent password mismatch
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
    cin >> passwordRepeat;

    if (strcmp(password, passwordRepeat)){  //password mismatch
        cout << "\nPasswords do not match!!!\n";
        cout << "Please try again!\n";
        cout << "Press 1 to try again!\n";
        char ch;
        ch = getch();
        if (ch == '1')
            createUser();
        else
            return;
    } else {    //everything good, create user
        loadingScreen((char *) "creating user", 10, true);
        strcpy(database.username, username);
        strcpy(database.password, password);
        file.open(FILEDB, ios::app);
        file.write((char *) &database, sizeof(Database));
        file.close();
        loadingScreen((char *) "User created, heading to login page", 10, true);
        login();        //continue to login page
    }
}

void viewFlights(){     //function to view flights on a particular route
    Flight flight;
    Flight send[10];        
    ifstream  file;
    int i = 0;
    int j = 0;
    int exist = false;      //to see if at least one flight exists
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
        file.read((char*) &flight, sizeof(Flight));
        if (file.eof())
            break;
        if (!strcmpi(source, flight.source)){
            if (!strcmpi(destination, flight.destination)){
                send[j++] = flight;            //stores the flights in an on that route to display
                exist = true;
            }
        }
    }

    if (exist){                             //if at least one flight exists
        int choice;
        displayStruct(send, j);             //to display structure in a presentable format
        tryAgain:
            cout << "Select S.no to book flight (0 to quit) : ";
            cin >> choice;
            if (!choice)            //quit
                return;
            else if (choice > j || choice < 0){
                cout << "Out of bounds!\n";
                goto tryAgain;
            } else
                bookFlight(send, choice);           //finalizes bookings
    }
    else    //no flights available
        cout << "\nOh oh, no flights on this route available :\\\n\n";

    file.close();

}

void viewAllPNRs(){                     //function available ONLY for ADMIN to see all the PNRs in file
    PNR pnr;
    int i = 0;
    Flight flight;
    ifstream filePNR;
    ifstream fileFlight;
    char flightNumber[7];

    filePNR.open(FILEPNR, ios::in);  
    fileFlight.open(FILENAME, ios::in);

    while(1){
        filePNR.read((char*) &pnr, sizeof(PNR));                    
        fileFlight.seekg(0, ios::beg);
        if(filePNR.eof())
            break;
        while(!fileFlight.eof()){
            fileFlight.read((char*) &flight, sizeof(Flight));
            if (!strcmp(pnr.FlightNumber, flight.FlightNumber)){        //sees if the flight number of pnr matches with the one in flights.txt
                displayPNRStruct(flight, pnr, i++);                     //displays the structure in a...you know it
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
        adminMenu();                        //goes back to admin menu
    }
}

void viewBookings(){                //function to see all the bookings made
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
        if (!strcmp(pnr.username, fileUsername)){   //checks the username, to only display the bookings of the logged in user
            if (strcmp(pnr.username, "guest")){
                while(!fileFlight.eof()){
                    fileFlight.read((char*) &flight, sizeof(Flight));
                    if (!strcmp(pnr.FlightNumber, flight.FlightNumber)){        //cross references the flight number
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
        userMenu();                         //back to the user menu
    }
}

void viewAllFlights(){          //function available for ADMIN ONLY to see all the flights in file
    int i = 1;
    Flight flight;
    ifstream  file;

    file.open(FILENAME, ios::in);

    if(file){           //if file is in good health
        line(71);
        cout << "S.No\tFlightNumber\tSource\tDestination\tDeparture\tArrival\n";
        line(71);
    }

    while(1){
        file.read((char*) &flight, sizeof(Flight));
        if (file.eof())
            break;
        cout << i++ << '\t' << flight.FlightNumber << "\t\t" << flight.source       //displays the
             << '\t' << flight.destination << "\t\t" << flight.departure.hours      //data in
             << ':' << flight.departure.minutes << "\t\t"                           //a presentable
             << flight.arrival.hours << ':' << flight.arrival.minutes               //format
             << '\n';
    }

    line(71);

    file.close();

    cout << "Press 1 to go back\n";
    char ch;
    ch = getch();
    if (ch == '1'){
        loadingScreen((char *) "Returning to menu", 4, false);
        adminMenu();                                    //back to the admin menu
    }
}

void continueAsGuest(){                                 //function to continue without login or signing up
    char ch;
    clrscr();
    cout << "Choose an option to continue" << '\n';
    line(30);
    cout << "1 : Book flight \n";
    cout << "2 : PNR Enquiry \n";
    cout << "0 : Quit" << '\n';
    ch = getch();
    switch (ch){
        case '1':
            loadingScreen((char *) "Opening book flights page", 6, true);
            viewFlights();                      //to book a flight
            break;
        case '2':
            loadingScreen((char *) "Opening PNR Enquiry page", 3, true);
            checkPNR();                         //to see PNR details
            cout << "Press 1 to go back\n";
            char choice;
            choice = getch();
            if (choice == '1'){
                loadingScreen((char *) "Returning to menu", 4, false);
                continueAsGuest();              //refresh
            }
            break;
        case '0':
            break;
        default:
            continueAsGuest();                  //refresh
            break;
    }
}

void bookFlight(Flight flight[], int c){        //function to book a flight selected
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
    passengersChar = getche();
    cout << '\n';
    paxCount = passengersChar - '0';

    cout << "Enter name of any 1 passenger : "; 
    gets(paxName);

    srand(time(NULL));

    do {
        for (int i = 0; i < 7; ++i)
            PNRNumber[i] = ALPHANUM[rand() % 37];       //generates random PNR
        PNRNumber[6] = 0;
    } while(PNRExists(PNRNumber));      //to see if the PNR does not already exists

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

void sortFlight(Flight flight[], int n){        //function to sort flights by departure > arrival > name in asc order
    for (int i = 0; i < n - 1; ++i){            //don't ever bother on how this works
        for (int j = 0; j < n - i - 1; ++j){    //take it for granted that it works and your life will be
            if (flight[j].departure.hours[0] > flight[j + 1].departure.hours[0])    //a whole lot simpler
                swapFlightStruct(flight[j], flight[j + 1]);                         //use this function
            else if (flight[j].departure.hours[0] == flight[j + 1].departure.hours[0]){ //without asking
                if (flight[j].departure.hours[1] > flight[j + 1].departure.hours[1])    //any questions
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
                    }               //take a moment to appreciate how beautiful it looks
                }
            }
        }
    }
}

void displayStruct(Flight flight[], int j){     //function to display the flight structure you know how
    if (j != 1)         //if more than 1 flights
        sortFlight(flight, j);                  //sort it without asking questions how the algorithm works
    
    line(71);                       
    cout << "S.No\tFlightNumber\tSource\tDestination\tDeparture\tArrival\n";
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

void displayStruct(Flight flight, PNR pnr){     //overloaded function to display one structure with PNR number
    line(98);                       
    cout << "FlightNumber\tSource\tDestination\tDeparture\tArrival\tPassengers\tName\n";
    line(98);                           

    cout << flight.FlightNumber << "\t\t" << flight.source
         << '\t' << flight.destination << "\t\t" << flight.departure.hours
         << ':' << flight.departure.minutes << "\t\t"
         << flight.arrival.hours << ':' << flight.arrival.minutes 
         << '\t' << pnr.paxCount << "\t\t" << pnr.paxName << '\n';
    
    line(98);                           
}

void swapFlightStruct(Flight &x, Flight &y){    //function to swap 2 flight identifiers
    Flight temp = x;
    x = y;
    y = temp;
}

void displayPNRStruct(Flight flight, PNR pnr, int i){   //display pnr with flight details
    if (i == 0){
        line(112);                       
        cout << "PNR\tFlightNumber\tSource\tDestination\tDeparture\tArrival\tPassengers\tName\t\tUsername\n";
        line(112);
    }

    cout << pnr.PNR << '\t' << flight.FlightNumber << "\t\t" << flight.source
         << '\t' << flight.destination << "\t\t" << flight.departure.hours
         << ':' << flight.departure.minutes << "\t\t"
         << flight.arrival.hours << ':' << flight.arrival.minutes 
         << '\t' << pnr.paxCount << "\t\t" << pnr.paxName << '\t' << pnr.username << '\n';
}

void loadingScreen(char *string, int times, int showLoading){   //the dopest part of the program
    clrscr();
    delay(250); //usleep taken in milliseconds (as good as 0.25s)
    if (showLoading){       //to show a loading text or continue with user text
        cout << string << '\n';
        cout << "Loading...\n";
    } else {
        cout << string << "...\n";
    }
    delay(250);                 //add a delay of a quarter of a second (4FPS)
    int loadingAnim = 0;
    int iterations = 0;
    while(iterations < times){      //as long as we have time to run
        clrscr();
        if(showLoading)
            cout << string << '\n';
        switch(loadingAnim){
            case 0:                                         //stage 1 of loading symbol
                if(showLoading)
                    cout << "Loading...\\\n";   
                else
                    cout << string << "...\\\n";
                break;
            case 1:                                         //stage 2 of loading symbol
                if(showLoading)
                    cout << "Loading...|\n";
                else
                    cout << string << "...|\n";
                break;
            case 2:                                         //stage 3 of loading symbol
                if(showLoading)
                    cout << "Loading.../\n";
                else
                    cout << string << ".../\n";
                break;
            case 3:                                         //stage 4 of loading symbol
                if(showLoading)
                    cout << "Loading...-\n";
                else
                    cout << string << "...-\n";
                loadingAnim = -1;                
                break;
        }
        ++loadingAnim;
        ++iterations;
        delay(250);                                        //delay of a quarter of a second to mimic 4FPS animation
    }
    clrscr();
}

