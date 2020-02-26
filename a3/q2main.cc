#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cmath>
#include <exception>
#include "q2polymult.h"
using namespace std;

void inputpoly(char *fileA, char *fileB){
    ifstream file1, file2;
    try{
        file1.open(fileA);
        file2.open(fileB);
        int size1, size2;
        while(file1 >> size1 && file2 >> size2){
            int sizer = size1+size2-1;
            //cout << size1 << " " << size2  << " " << sizer << endl;
            int *poly1 = new int[size1];
            int *poly2 = new int[size2];
            int *polyr = new int[sizer];
            for(int i=0; i<size1; ++i){//assume sizes are the same
                file1 >> poly1[i];
                file2 >> poly2[i];
            }
            Poly c = {polyr, (size_t)sizer};
            polymultiply(Poly{poly1, (size_t)size1}, Poly{poly2, (size_t)size2}, c, floor(sqrt(sizer)));
            for(int i=size1-1; i>0; --i){
                if(poly1[i] != 0){
                    cout << poly1[i] << "x^" << i << " + "; 
                }
            }
            if(poly1[0] != 0){
                cout << poly1[0] <<"x^" << 0;
            }
            cout << endl;
            for(int i=size2-1; i>0; --i){
                if(poly2[i] != 0){
                    cout << poly2[i]<< "x^" << i << " + ";
                }
            }
            if(poly2[0] != 0){
                cout << poly2[0] <<"x^" << 0;
            }
            cout << endl;
            for(int i=sizer-1; i>0; --i){
                if(polyr[i] != 0){
                    cout << polyr[i]<< "x^" << i << " + ";
                }
            }
            if(polyr[0] != 0){
                cout << polyr[0] <<"x^" << 0;
            }
            cout << endl;
            delete poly1;
            delete poly2;
            delete polyr;
        }
        file1.close();
        file2.close();
    }
    catch(exception &e){
        file1.close();
        file2.close();
        cerr << e.what() << endl;
        cerr << "Error! Could not open input files \"" << fileA << "\" \"" << fileB << "\"" << endl;
        exit( EXIT_FAILURE );
    }
}

void randompoly(long numCoef){
    size_t sizer = (size_t)2*numCoef-1;
    int *poly1 = new int[numCoef];
    int *poly2 = new int[numCoef];
    int *polyr = new int[sizer]{};
    for (int i=0; i<numCoef; ++i){
        poly1[i] = i+1;
        poly2[i] = i+1;
    }
    Poly c = {polyr, sizer};
    polymultiply(Poly{poly1, (size_t)numCoef}, Poly{poly2, (size_t)numCoef}, c, floor(sqrt(sizer)));
    delete poly1;
    delete poly2;
    delete polyr;
}

int main( int argc, char * argv[] ) {
    try {
        int processors = 1;
        switch ( argc ) {
            case 5:
                if(strcmp( argv[1], "-p" ) == 0 ){
                    processors = stoi( argv[2] ); 
                    if ( processors <= 0 ) throw 1;
                    uProcessor p[processors-1];
                    if(strcmp( argv[3], "-t" ) == 0 ){
                    int numCoef = stoi( argv[4] ); 
                    if ( numCoef <= 0 ) throw 1;
                    randompoly(numCoef);
                    } else {
                        inputpoly(argv[3], argv[4]);
                    }
                } else throw 1;
                break;
            case 3:
                if(strcmp( argv[1], "-t" ) == 0 ){
                    int numCoef = stoi( argv[2] ); 
                    if ( numCoef <= 0 ) throw 1;
                    randompoly(numCoef);
                } else {
                    inputpoly(argv[1], argv[2]);
                }
                break;
            default: throw 1;
        }
    } catch ( int i ) {
        cerr << "Usage: " << argv[0] << " [ -p processors (>= 0) ] [ polyfileA polyfileB | -t numCoeff (>= 0) ]" << endl;
        exit( EXIT_FAILURE );
    }
}