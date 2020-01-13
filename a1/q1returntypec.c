#include <stdio.h>
#include <stdlib.h>                                      // access: rand, srand
#include <string.h>                                      // access: strcmp
//using namespace std;
#include <unistd.h>                                     // access: getpid

struct Er1 { short int code; };
struct Er2 { int code; };
struct Er3 { long int code; };

struct return_code {
  enum {
    d_Er1, d_Er2, d_Er3, d_double
  } dtype;
  union {
    struct Er1 er1;
    struct Er2 er2;
    struct Er3 er3;
    double value;
  } data;
};

int eperiod = 10000;                                    // error period

struct return_code rtn1( double i ) {
    struct return_code returnValue;
    if ( rand() % eperiod == 0 ) {
        //struct Er1 er1 = Er1 { .code = (short int)rand() };
        returnValue.dtype = d_Er1;
        returnValue.data.er1.code =  (short int)rand() ;
    }
    else {
        returnValue.dtype = d_double;
        returnValue.data.value = i;
    }
    return returnValue;
}

struct return_code rtn2( double i ) {
    struct return_code returnValue;
    if ( rand() % eperiod == 0 ) {
        returnValue.dtype = d_Er2;
        returnValue.data.er2.code = rand();
    }
    else {
        returnValue = rtn1(i);
        if(returnValue.dtype == d_double) returnValue.data.value += i;
    }
    return returnValue;
}

struct return_code rtn3( double i ) {
    struct return_code returnValue;
    if ( rand() % eperiod == 0 ) {
        returnValue.dtype = d_Er3;
        returnValue.data.er3.code =  rand();
    }
    else {
        returnValue = rtn2(i);
        if(returnValue.dtype == d_double) returnValue.data.value += i;
    }
    return returnValue;
}

int main( int argc, char * argv[] ) {
    int times = 100000000, seed = getpid();             // default values
    // try {
        switch ( argc ) {
          case 4: if ( strcmp( argv[3], "d" ) != 0 ) {  // default ?
                seed = atoi( argv[3] ); if ( seed <= 0 ) goto L;
            } // if
          case 3: if ( strcmp( argv[2], "d" ) != 0 ) {  // default ?
                eperiod = atoi( argv[2] ); if ( eperiod <= 0 ) goto L;
            } // if
          case 2: if ( strcmp( argv[1], "d" ) != 0 ) {  // default ?
                times = atoi( argv[1] ); if ( times <= 0 ) goto L;
            } // if
          case 1: break;                                // use all defaults
          L: default: fprintf(stderr, "Usage: %s [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]\n", argv[0]);
                exit( EXIT_FAILURE );
        } // switch
    // } catch( ... ) {
        // fprintf(stderr, "Usage: %c [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]\n", argv[0]);
        // cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 ] ] ]" << endl;
        // exit( EXIT_FAILURE );
    // } // try
    srand( seed );

    double rv = 0.0;
    int ev1 = 0, ev2 = 0, ev3 = 0;
    int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

    for ( int i = 0; i < times; i += 1 ) {
        struct return_code result = rtn3( i );
        switch (result.dtype)
        {
            // analyse error
        case d_Er1:
            ev1 += result.data.er1.code; 
            ec1 += 1;
            break;
        
        case d_Er2:
            ev2 += result.data.er2.code; 
            ec2 += 1; 
            break;
        
        case d_Er3:
            ev3 += result.data.er3.code; 
            ec3 += 1; 
            break;

        case d_double:
            rv += result.data.value;
            rc += 1; 
            break;

        default:
            break;
        }
    } // for
    printf("normal result %.10e exception results %d %d %d\n", rv, ev1, ev2, ev3);
    printf("calls %d exceptions %d %d %d\n", rc, ec1, ec2, ec3);
    // cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
    // cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}
