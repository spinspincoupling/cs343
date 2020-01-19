#include <iostream>
#include <string>
#include <fstream>
#include "q3floatconstant.h"
using namespace std;

void validateInput(istream& is){ //not guarded dont need to care for leading space
    string line;
    int pos, length;
    FloatConstant corountine;
    while(getline(is, line, '\n')){
                pos = 0;
                length = line.size();
                if(length == 0){
                    cout << "\"\" : Warning! Blank line." << endl;
                    continue;
                }
                while(corountine.next(line[pos]) != FloatConstant::Status::INVALID && pos < length){
                    //corountine.next(line[pos]);
                    ++pos;
                }
                cout << "\"" << line << "\" : \"" << line.substr(0, pos) << "\" ";
                if(corountine.getStatus() == FloatConstant::Status::MATCH){
                    cout << "yes";
                } else {
                    cout << "no";
                    if(pos < length ){
                        cout << " -- extraneous characters \"" << line.substr(pos) << "\"" ;
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
                file.close;
                throw;
            }
        } else if (argc == 1){// iostream
            validateInput(cin);
        } else {
            throw 1;
        }
    } catch ( int i ) {
        cerr << "Invalid number of arguments" << endl;
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
}