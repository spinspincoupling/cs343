#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "config.h"
using namespace std;

void processConfigFile( const char * configFile, ConfigParms & cparms ){
    try {
        unsigned int data[11];
        bool written[11] = { false };
        int count = 0;
        ifstream file;
        file.open(configFile);
        string line, name;
        unsigned int num, pos;
        while(getline(file, line)){
            istringstream buffer(line);
            getline(buffer, line, '#');
            istringstream input(line);
            if(input >> name){
                if(!(input >> num)) throw 2;
                ++count;
                if(count >  11) throw 1;
                if(name == "StopCost"){
                    pos = 0;
                } else if(name == "NumStudents"){
                    pos = 1;
                } else if(name == "NumStops"){
                    pos = 2;
                } else if(name == "MaxNumStudents"){
                    pos = 3;
                } else if(name == "TimerDelay"){
                    pos = 4;
                } else if(name == "MaxStudentDelay"){
                    pos = 5;
                } else if(name == "MaxStudentTrips"){
                    pos = 6;
                } else if(name == "GroupoffDelay"){
                    pos = 7;
                } else if(name == "ConductorDelay"){
                    pos = 8;
                } else if(name == "ParentalDelay"){
                    pos = 9;
                } else if(name == "NumCouriers"){
                   pos = 10;
                } else throw 2;
                if(written[pos] == false){
                    written[pos] = true;
                    data[pos] = num;
                } else throw 2;
            }
        }
        if(count < 11) throw 2;
        cparms.stopCost = data[0];
        cparms.numStudents = data[1];
        cparms.numStops = data[2];
        cparms.maxNumStudents = data[3];
        cparms.timerDelay = data[4];
        cparms.maxStudentDelay = data[5];
        cparms.maxStudentTrips = data[6];
        cparms.groupoffDelay = data[7];
        cparms.conductorDelay = data[8];
        cparms.parentalDelay = data[9];
        cparms.numCouriers = data[10];
    } catch(int e){
        if(e == 1) cerr << "Error: file " << configFile << " has extraneous data." << endl;
        else cerr << "Error: file " << configFile << " is corrupt." << endl;
        exit( EXIT_FAILURE );
    }
    catch (...){
        cerr << "Error: could not open input file " << configFile << endl;
        exit( EXIT_FAILURE );
    }
}