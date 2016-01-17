#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
using namespace std ;

int main( int argc , char* argv[] )
{
    /************ File Read **************/

    string ob_file( "default.txt" ) ;
    if( argc == 2 ) ob_file = argv[1] ;
    else if( argc > 2 ){
        cout << "Invalid command\n" ;
        return 0 ;
    }

    fstream fp( ob_file ) ;     // Open File
    if( !fp ){
        cout << "Cannot find input text file\n" ;
        return 0 ;
    }

    /************ XXXXXXXXXX **************/









    return 0 ;
}

