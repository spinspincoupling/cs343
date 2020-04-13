#include "printer.h"
#include <iostream>

using namespace std;

Printer::Printer( unsigned int numStudents, unsigned int numTrains, unsigned int numStops, unsigned int numCouriers )
    :numStudents{numStudents}, numTrains{numTrains}, numStops{numStops}, numCouriers{numCouriers},d{0},
    offset1{5}, offset2{offset1+numTrains}, offset3{offset2+numTrains}, offset4{offset3+numStops}, offset5{offset4+numStudents},
    total{offset5+numCouriers}, cnt{0}, written{new bool[total] {0}}, columns{new Item[total]}{
    cout << "Parent" << '\t' << "Gropoff" << '\t' << "WATOff" << '\t' << "Names" << '\t' << "Timer" << '\t';
    for(unsigned int i=0; i<numTrains; ++i){
        cout << "Train" << i << '\t';
    }
    for(unsigned int i=0; i<numTrains; ++i){
        cout << "Cond" << i << '\t';
    }
    for(unsigned int i=0; i<numStops; ++i){
        cout << "Stop" << i << '\t';
    }
    for(unsigned int i=0; i<numStudents; ++i){
        cout << "Stud" << i << '\t';
    }
    for(unsigned int i=0; i<numCouriers-1; ++i){
        cout << "WCour" << i << '\t';
    }
    cout << "WCour" << numCouriers-1 << '\n';
    for(unsigned int =0; i<total-1; ++i){
        cout << "*******" << '\t';
    }
    cout << "*******" << '\n';
}

Printer::~Printer(){
    if(cnt > 0){
        flushBuffer();
    }
    cout << "***********************" << endl;
    delete written;
    delete columns;
}

void Printer::printItem(const Item& item){
    switch(item.type){
        case 0:
            cout<< item.type;
            break;
        case 1:
            cout<< item.type << item.v1;
            break;
        case 2:
            cout<< item.type << item.v1 << ',' << item.v2;
            break;
        case 3:
            cout<< item.type << item.v1 << ',' << item.v2 << ',' << item.v3;
            break;
        case 4:
            cout<< item.type << item.c;
            break;
        case 5:
            cout<< item.type << item.v1 << ',' << item.c;
            break;
        case 6:
            cout<< item.type << item.v1 << ',' << item.v2 << ',' << item.c; 
            break;
    }
}

void Printer::flushBuffer(){
    for(unsigned int i=0; i<total; ++i) {
        if(written[i]){
            printItem(columns[i]);
            --cnt;
            written[i] = false;
            if(cnt == 0) {
                cout << '\n';
                break;
            } else {
                std::cout << '\t';
            }
        } else std::cout << '\t';
    }
}

unsigned int Printer::getOffset(Kind kind){
    switch(kind){
        case Kind::Train:
            return offset1;
        case Kind::Conductor:
            return offset2;
        case Kind::TrainStop:
            return offset3;
        case Kind::Student:
            return offset4;
        case Kind::WATCardOfficeCourier:
            return offset5;
        case Kind::Timer:
            return 4;
        case Kind::NameServer:
            return 3;
        case Kind::WATCardOffice:
            return 2;
        case Kind::Groupoff:
            return 1;
        case Kind::Parent:
            return 0;
    }
}

void Printer::insert(unsigned int index){
    if(written[index]){
        flushBuffer();
    }
    columns[index] = curr;
    written[index] = true;
    ++cnt;
}

void Printer::print( Kind kind, char state ){
    curr = Item{0, state, d, d, .v3=d};
    insert(getOffset(kind));
}

void Printer::print( Kind kind, char state, unsigned int value1 ){
    curr = Item{1, state, value1, d, .v3=d};
    insert(getOffset(kind));
}

void Printer::print( Kind kind, char state, unsigned int value1, unsigned int value2 ){
    curr = Item{2, state, value1, value2, .v3=d};
    insert(getOffset(kind));    
}

void Printer::print( Kind kind, unsigned int lid, char state ){
    curr = Item{0, state, d, d, .v3=d};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1 ){
    curr = Item{1, state, value1, d, .v3=d};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2 ){
    curr = Item{2, state, value1, value2, .v3=d};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int oid, unsigned int value1, unsigned int value2 ){
    curr = Item{3, state, oid, value1, .v3=value2};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, char c ){
    curr = Item{4, state, d, d, .c=c};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, char c ){
    curr = Item{5, state, value1, d, .c=c};
    insert(getOffset(kind)+lid);
}

void Printer::print( Kind kind, unsigned int lid, char state, unsigned int value1, unsigned int value2, char c ){
    curr = Item{6, state, value1, value2, .c=c};
    insert(getOffset(kind)+lid);
}