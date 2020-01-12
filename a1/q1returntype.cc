#include <iostream>
#include <cstdlib>                                      // access: rand, srand
#include <cstring>                                      // access: strcmp
using namespace std;
#include <unistd.h>                                     // access: getpid
#include <variant>

struct Er1 { short int code; };
struct Er2 { int code; };
struct Er3 { long int code; };

int eperiod = 10000;                                    // error period

variant<double, Er1> rtn1( double i ) {
    if ( rand() % eperiod == 0 ) return Er1{ (short int)rand() };
    return i;
}
variant<double, Er1, Er2> rtn2( double i  ) {
    if ( rand() % eperiod == 0 ) return Er2{ rand() };
    variant<double, Er1> rtn1_result = rtn1( i );
    if(holds_alternative<Er1>(rtn1_result)) return get<Er1>(rtn1_result);
    return get<double>(rtn1_result) + i;
}
variant<double, Er1, Er2, Er3> rtn3( double i  ) {
    if ( rand() % eperiod == 0 ) return Er3{ rand() };
    variant<double, Er1, Er2> rtn2_result = rtn2( i );
    if(holds_alternative<Er1>(rtn2_result)) return get<Er1>(rtn2_result);
    if(holds_alternative<Er2>(rtn2_result)) return get<Er2>(rtn2_result);
    return get<double>(rtn2_result) + i;
}
int main( int argc, char * argv[] ) {
    int times = 100000000, seed = getpid();             // default values
    try {
        switch ( argc ) {
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                eperiod = stoi( argv[2] ); if ( eperiod <= 0 ) throw 1;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                times = stoi( argv[1] ); if ( times <= 0 ) throw 1;
            } // if
          case 1: break;                                // use all defaults
          default: throw 1;
        } // switch
    } catch( ... ) {
        cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    srand( seed );

    double rv = 0.0;
    int ev1 = 0, ev2 = 0, ev3 = 0;
    int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

    for ( int i = 0; i < times; i += 1 ) {
        variant<double, Er1, Er2, Er3> result = rtn3( i );
        // analyse error
        if(holds_alternative<Er1>(result)){
            ev1 += get<Er1>(result).code;
            ec1 += 1;
        } else if(holds_alternative<Er2>(result)){
            ev2 += get<Er2>(result).code;
            ec2 += 1;
        } else if(holds_alternative<Er3>(result)){
            ev3 += get<Er3>(result).code;
            ec3 += 1;
        } else{
            rv += get<double>(result);
            rc += 1;
        }
    } // for
    cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
    cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
