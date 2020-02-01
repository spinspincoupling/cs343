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
                _Resume Binsertsort<T>::Sentinel() _At binsertsort;
                try {
                    for (;;) {
                        _Enable{
                            value = binsertsort.retrieve();
                        }
                        os << value << " ";
                    }
                }
                catch (Binsertsort<T>::Sentinel &){
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
            try {
                ofstream outfile(argv[2]);
                sortFile(outfile, argv[1])
                outfile.close();
            }
            catch (...) {
                outfile.close();
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

#endif