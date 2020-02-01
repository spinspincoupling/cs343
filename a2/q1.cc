#ifdef TYPE
#include "q1binsertsort.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
using namespace std;

void sortFile(ostream &os, char* infile){
    ifstream file;
        try {
            file.open(infile);
            //corountine call
            int length;
            while (file >> length) {
                Binsertsort<TYPE> binsertsort;
                TYPE value;
                for (int i=0; i<length; ++i) {
                    file >> value;
                     os << value << " ";
                    binsertsort.sort(value);
                }
                os << endl;
                _Resume Binsertsort<TYPE>::Sentinel() _At binsertsort;
                try {
                    for (;;) {
                        _Enable{
                            value = binsertsort.retrieve();
                        }
                        os << value << " ";
                    }
                }
                catch (Binsertsort<TYPE>::Sentinel &){
                    os << endl << endl;
                } 
            }
            file.close();
        }
        catch (...){
            file.close();
            cerr << "Error! Could not open input file \"" << infile << "\"" << endl;
            exit(EXIT_FAILURE);
        }
}

int main( int argc, char * argv[] ) {
    try {
        if( argc == 3 ){// write to file
            ofstream outfile;
            try {
                outfile.open(argv[2]);
                sortFile(outfile, argv[1]);
                outfile.close();
            }
            catch (...) {
                outfile.close();
                cerr << "Error! Could not open file \"" << argv[2] << "\"" << endl;
                exit( EXIT_FAILURE );
            }
        } else if (argc == 2){// std out
            sortFile(cout, argv[1]);
        } else {
            throw 1;
        }
    } catch ( int i ) {
        cerr << "Usage: " << argv[0] << " unsorted-file" << " [sorted-file]" << endl;
        exit( EXIT_FAILURE );
    }
}

template<typename T>
void Binsertsort<T>::main()
{
    try{
        _Enable{}
    }
    catch (Sentinel &){
        _Resume Sentinel();
    }
    T pivot = value;
    try {
        suspend();
         _Enable{}
    }
    catch (Sentinel &) { // end of value set
        suspend();
        _Resume Sentinel();
    }
    // implies vertex node
    Binsertsort<T> less, greater; // create less and greater
    try{
        for (;;) {
            if (value < pivot) {
                less.sort(value);
            } else {
                greater.sort(value);
            }
            suspend();
            _Enable{}
        }
    }
    catch (Sentinel &) { // end of value set, first retrive
    }
    _Resume Sentinel() _At less;
    try{
        for (;;) {
            _Enable{
                value = less.retrive();
            }
            suspend();
        }
    }
    catch (Sentinel &) { //exception from left child end
    }
    value = pivot;
    suspend();
    _Resume Sentinel() _At greater;
    try {
        for (;;) {
            _Enable {
                value = less.retrive();
            }
            suspend();
        }
    }
    catch (Sentinel &){ // all branch ends notify parent
    }
    _Resume Sentinel();
}

#endif