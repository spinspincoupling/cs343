#include <iostream>
#include <string>
#include <fstream>
#include "q3floatconstant.h"
using namespace std;

void validateInput(istream& is){ //not guarded dont need to care for leading space
    string line;
    int pos, length;
    while(getline(is, line, '\n')){
                pos = 0;
                length = line.size();
                if(length == 0){
                    cout << "\"\" : Warning! Blank line." << endl;
                    continue;
                }
                FloatConstant corountine;
                while(pos < length && corountine.next(line[pos]) != FloatConstant::Status::INVALID){
                    ++pos;
                }
                cout << "\"" << line << "\" : \"" << line.substr(0, pos+1) << "\" ";
                if(corountine.getStatus() == FloatConstant::Status::MATCH){
                    cout << "yes";
                } else {
                    cout << "no";
                    if(pos < length-1 ){
                        cout << " -- extraneous characters \"" << line.substr(pos+1) << "\"" ;
                    }
                }
                cout << endl;
            }
}

int main( int argc, char * argv[] ) {
    try {
        if( argc == 2){//fstream
            ifstream file;
            try {
                file.open(argv[1]);
                validateInput(file);
                file.close();
            }
            catch (...) {
                file.close();
                cerr << "Error! Could not open input file \"" << argv[1] << "\"" << endl;
                throw;
            }
        } else if (argc == 1){// iostream
            validateInput(cin);
        } else {
            throw 1;
        }
    } catch ( int i ) {
        cerr << "Usage: " << argv[0] << " [infile-file]" << endl;
    } catch( ... ) {
        exit( EXIT_FAILURE );
    } // try
}