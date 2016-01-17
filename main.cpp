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

map<string,string>OPTAB = { {"STL","14"},{"LDB","68"},{"JSUB","48"},{"LDA","00"},{"COMP","28"},{"JEQ","30"}
    ,{"J","3C"},{"STA","0C"},{"CLEAR","B4"},{"LDT","74"},{"TD","E0"},{"RD","D8"},{"COMPR","A0"}
    ,{"STCH","54"},{"TIXR","B8"},{"JLT","38"},{"STX","10"},{"RSUB","4C"},{"LDCH","50"},{"WD","DC"}
};
map<string,int>Register = { {"A",0},{"X",1},{"L",2},{"B",3},{"S",4},{"T",5} } ;
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

    int length = address[count]-START ;
    ofp << "H" << setw(6) << left << name << setfill('0') << hex << setw(6) << START
                            << setw(6) << setfill('0') << hex << right << length << endl ;

    // Pass 2

    stringstream object[MAXLINE] ;
    PC = BASE ; count = 0 ;
    int reg[10] = {0} ;

    while( count != numline )
    {
        PC = address[count+1] ;
        ss.clear() ;
        vector<string>S ;
        ss << command[count] ;
        while( ss >> tmp ) S.push_back(tmp) ;

        int compr = 0 ;
        int n = 0 , i = 0 , x = 0 , b = 0 , p = 0 , e = 0;

        if( S.size() == 3 ) S.erase(S.begin()) ;

        if(S[0] == "RSUB" ) object[count] << "4F0000" ;
        else if(S[0] == "BASE" ){
            BASE = atoi(S[1].c_str()) ;
        }
        else if( S[0] == "CLEAR" ){
            object[count] << OPTAB[S[0]] << Register[S[1]] << "0" ;
            reg[Register[S[1]]] = 0 ;
        }
        else if( S[0] == "BYTE" ){
            if( S[1][0] == 'C' ){
                for( int i = 2 ; i < S[1].size()-1 ; ++i )
                    object[count] << hex << static_cast<int>(S[1][i]) ;
            }
            else if(S[1][0] == 'X' ){
                 for( int i = 2 ; i < S[1].size()-1 ; ++i )
                    object[count] << S[1][i] ;
            }
        }
        else if( S[0] == "TIXR" ){
            object[count] << OPTAB[S[0]] << Register[S[1]] << "0" ;
            reg[Register[S[1]]] = reg[Register[S[1]]]+1 ;
        }
        else if( S[0] == "COMPR" ){
            string r1 , r2 ; r1+=S[1][0] , r2+= S[1][2] ;
            object[count] << OPTAB[S[0]] << Register[r1] << Register[r2] ;
            if( reg[Register[r1] ] < reg[Register[r2] ] ) compr = -1 ;
            else if( reg[Register[r1]] > reg[Register[r2]] ) compr = 1 ;
            else compr = 0 ;
        }
        else
        {
            if( S[0][0] == '+' ) e = 1 ; // format 4
            else e = 0 ;                //format 3

            if( S[1][0] == '#' ){
                n = 0 , i = 1 ;
            }
            else if( S[1][0] == '@' ){
                n = 1 , i = 0 ;
            }
            else{
                n = 1 , i = 1 ;
            }




        }



        count += 1 ;
        S.clear() ;
    }


    for( int i = 0 ; i < numline-1 ; ++i ){
        string sssss ;
        object[i] >> sssss ;
        if( !sssss.size() ) continue ;
        //ofp << "T" << setw(6) << address[i] ;
        ofp << sssss << endl ;
    }

    // end


    return 0 ;
}

