#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <iomanip>
#include <vector>
using namespace std ;

#define MAXLINE 500

const map<string,string>OPTAB = { {"STL","14"},{"LDB","68"},{"JSUB","48"},{"LDA","00"},{"COMP","28"},{"JEQ","30"}
    ,{"J","3C"},{"STA","0C"},{"CLEAR","B4"},{"LDT","74"},{"TD","E0"},{"RD","D8"},{"COMPR","A0"}
    ,{"STCH","54"},{"TIXR","D8"},{"JLT","38"},{"STX","10"},{"RSUB","4C"},{"LDCH","50"},{"WD","DC"}
};
map<string,int>SYMTAB ;


int main( int argc , char* argv[] )
{
    /************ File Read **************/

    string ob_file( "default.txt" ) ;
    if( argc == 2 ) ob_file = argv[1] ;
    else if( argc > 2 )
    {
        cout << "Invalid command\n" ;
        return 0 ;
    }

    fstream fp( ob_file ) ;     // Open File
    if( !fp )
    {
        cout << "Cannot find input text file\n" ;
        return 0 ;
    }

    fstream ofp( "result.txt" , ios::out ) ;

    /************* PROGRAM ***************/

    // For Header
    stringstream ss ;
    string FirstLine , name , tmp ;
    int START ;
    getline( fp , FirstLine ) ;
    ss << FirstLine ;
    ss >> name >> tmp >> START ;

    string command[MAXLINE] ;
    int address[MAXLINE] ;
    int numline = 0 ;

    while( getline(fp,command[numline++]) ) ;

    // Pass 1

    int BASE = START , PC = BASE , count = 0 ;

    while( count != numline )
    {
        ss.clear() ;
        address[count] = PC ;
        vector<string>S ;
        ss << command[count] ;
        while( ss >> tmp ) S.push_back(tmp) ;

        if( S.size() == 3 )
        {
            SYMTAB[S[0]] = PC ;     // add to symbol table
        }
        if( S.size() > 1 )
        {
            if( S[S.size()-2] == "END" )
            {
                address[count] = -1 ;
                numline = count+1 ;
                break ;
            }

            if( S[S.size()-2] == "RESW" )
            {
                int n = atoi(S.back().c_str()) ;
                PC += n*3 ;
            }
            else if( S[S.size()-2] == "RESB" )
            {
                int n = atoi(S.back().c_str()) ;
                PC += n ;
            }
            else if( S[S.size()-2] == "BYTE" )
            {
                int n = S.back().size()-3 ;
                if( S.back()[0] == 'X' ) n/=2 ;
                PC += n ;
            }
            else if( S[S.size()-2] == "WORD" )
            {
                PC += 3 ;
            }
            else if( S[S.size()-2] == "BASE" || S[S.size()-2] == "BASE" )
                address[count] = -1 ;
            else if( S[S.size()-2][0] == '+' ) PC += 4 ;
            else if( S[S.size()-2] == "CLEAR" || S[S.size()-2] == "TIXR"
                        || S[S.size()-2] == "COMPR" ) PC += 2 ;
            else    PC += 3 ;

        }
        else PC += 3 ;

        S.clear();
        count += 1 ;
    }

    for( int i = 0 ; i < numline ; i ++ )
    {
        cout << hex << setfill('0') << setw(4) << address[i] << "    " << command[i] << endl ;
    }

    cout << endl ;

    for( map<string,int>::iterator it = SYMTAB.begin() ; it != SYMTAB.end() ; it ++ )
    {
        cout << it->first << "  " << setw(4) << it->second << endl ;
    }

    return 0 ;
}

