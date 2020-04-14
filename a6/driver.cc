#include <iostream>
#include <unistd.h>
#include <cstring>
#include <cstdlib>  
#include "MPRNG.h"
#include "printer.h"
#include "timer.h"
#include "nameserver.h"
#include "trainstop.h"
#include "train.h"
#include "student.h"
#include "global.h"
#include "parent.h"
#include "groupoff.h"
#include "cardoffice.h"
#include "bank.h"
#include "config.h"
using namespace std;

int main( int argc, char * argv[] ) {
    int seed = getpid();
    char defaultFile[] = "lrt.config";
    try {        
        if(argc > 3) throw 1;                               // process command-line arguments
        else if(argc > 2){
            size_t endpos;
            seed = stoi( argv[2], &endpos);
            if ( seed <= 0 || argv[2][endpos] != '\0') throw 1;
        }
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ config-file [ Seed > 0] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    //cout << "The seed is " << seed << endl;
    mprng.set_seed(seed);

    //read file
    ConfigParms config;
    processConfigFile(argc>1? argv[1]:defaultFile, config);

    //initialize constants
    maxTripCost = config.stopCost * (config.numStops/2);
    conductorDelay = config.conductorDelay;
    const unsigned int numTrains = 2;

    //construct objects
    Printer printer(config.numStudents, numTrains, config.numStops, config.numCouriers);
    Bank bank(config.numStudents);
    WATCardOffice office(printer, bank, config.numCouriers);
    Groupoff groupoff(printer, config.numStudents, maxTripCost, config.groupoffDelay);
    Parent *parent = new Parent(printer, bank, config.numStudents, config.parentalDelay, maxTripCost);
    NameServer nameServer( printer, config.numStops, config.numStudents);
    Timer *timer = new Timer(printer, nameServer, config.timerDelay);
    TrainStop *stops[config.numStops];
    for (unsigned int i=0; i<config.numStops; ++i){
        stops[i] = new TrainStop(printer, nameServer, i, config.stopCost);
    }
    Train *trains[numTrains];
    for(unsigned int i=0; i<numTrains; ++i){
        trains[i] = new Train(printer,nameServer, i, config.maxNumStudents, config.numStops);
    }
    Student *students[config.numStudents];
    for (unsigned int i=0; i<config.numStudents; ++i){
        students[i] = new Student(printer, nameServer, office, groupoff, i, config.numStops, config.stopCost, config.maxStudentDelay, config.maxStudentTrips);
    }

    for(unsigned int i=0; i<config.numStudents; ++i){ // wait for students to finish trip
        delete students[i];
    }
    for(unsigned int i=0; i<numTrains; ++i){
        delete trains[i];
    }
    delete timer; // need to terminate before trainstop
    for (unsigned int i=0; i<config.numStops; ++i){
        delete stops[i];
    }
    delete parent; //need to terminate before bank

}