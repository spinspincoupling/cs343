#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "config.h"
using namespace std;

void processConfigFile( const char * configFile, ConfigParms & cparms ){
    try {
        unsigned int data[11];
        bool written[11] = {false};
        int count = 0;
        ifstream file;
        file.open(configFile);
        string line, name;
        unsigned int num, pos;
        for(;;){
            if(count >  11) throw 1;
            getline(file, line);
            istringstream buffer(line);
            getline(buffer, line, '#');
            istringstream input(line);
            if(input >> name){
                if(!(input >> num)) throw 2;
                ++count;
                if(name == "stopCost"){
                    pos = 0;
                } else if(name == "numStudents"){
                    pos = 1;
                } else if(name == "numStops"){
                    pos = 2;
                } else if(name == "maxNumStudents"){
                    pos = 3;
                } else if(name == "timerDelay":){
                    pos = 4;
                } else if(name == "maxStudentDelay"){
                    pos = 5;
                } else if(name == "maxStudentTrips"){
                    pos = 6;
                } else if(name == "groupoffDelay"){
                    pos = 7;
                } else if(name == "conductorDelay"){
                    pos = 8;
                } else if(name == "parentalDelay"){
                    pos = 9;
                } else if(name == "numCouriers"){
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