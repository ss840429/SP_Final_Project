#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <map>
using namespace std ;

const map<string,string>OPTAB = { {"STL","14"},{"LDB","68"},{"JSUB","48"},{"LDA","00"},{"COMP","28"},{"JEQ","30"}
                                    ,{"J","3C"},{"STA","0C"},{"CLEAR","B4"},{"LDT","74"},{"TD","E0"},{"RD","D8"},{"COMPR","A0"}
                                    ,{"STCH","54"},{"TIXR","D8"},{"JLT","38"},{"STX","10"},{"RSUB","4C"},{"LDCH","50"},{"WD","DC"}};
map<string,string>SYMTAB ;

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

    /************* PROGRAM ***************/

    string LOCCTR ;


    string line ;
    while( getline( fp , line ) ) cout << line << endl ;








    return 0 ;
}

