#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <iomanip>
#include <vector>
#include <math.h>
using namespace std ;

#define MAXLINE 500

map<string,string>OPTAB = { {"STL","14"},{"LDB","68"},{"JSUB","48"},{"LDA","00"},{"COMP","28"},{"JEQ","30"}
    ,{"J","3C"},{"STA","0C"},{"CLEAR","B4"},{"LDT","74"},{"TD","E0"},{"RD","D8"},{"COMPR","A0"}
    ,{"STCH","54"},{"TIXR","B8"},{"JLT","38"},{"STX","10"},{"RSUB","4C"},{"LDCH","50"},{"WD","DC"}
};
map<string,int>Register = { {"A",0},{"X",1},{"L",2},{"B",3},{"S",4},{"T",5} } ;
map<string,int>SYMTAB ;

void toupper( string& ss )
{
    for( int i = 0 ; i < ss.size() ; ++i ){
        if( ss[i] >= 'a' && ss[i] <= 'z' )
            ss[i] -= 'a' - 'A' ;
    }
}

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
    vector<int> jsubpos ;
    PC = BASE ; count = 0 ;

    while( count != numline )
    {
        PC = address[count+1] ;
        int k = 1 ;
        while( PC == -1 ){
            k ++ ;
            PC = address[count+k] ;
        }
        ss.clear() ;
        vector<string>S ;
        ss << command[count] ;
        while( ss >> tmp ) S.push_back(tmp) ;

        int n = 0 , i = 0 , x = 0 , b = 0 , p = 0 , e = 0;

        if( S.size() == 3 ) S.erase(S.begin()) ;
        if( S[0] == "JSUB" || S[0] == "+JSUB" )
            jsubpos.push_back(address[count]+1) ;

        if( S[0] == "RESW" || S[0] == "RESB" ) {
            count += 1 ;
            continue ;
        }
        else if(S[0] == "RSUB" ) object[count] << "4F0000" ;
        else if(S[0] == "BASE" ){
            BASE = SYMTAB[S[1]] ;
        }
        else if( S[0] == "CLEAR" ){
            object[count] << OPTAB[S[0]] << Register[S[1]] << "0" ;
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
        }
        else if( S[0] == "COMPR" ){
            string r1 , r2 ; r1+=S[1][0] , r2+= S[1][2] ;
            object[count] << OPTAB[S[0]] << Register[r1] << Register[r2] ;
        }
        else
        {
            if( S[0][0] == '+' ){
                e = 1 ;                 // format 4
                S[0].erase(S[0].begin()) ;
            }
            else e = 0 ;                //format 3


            if( S[1][0] == '#' ){
                n = 0 , i = 1 ;
                S[1].erase(S[1].begin()) ;
            }
            else if( S[1][0] == '@' ){
                n = 1 , i = 0 ;
                S[1].erase(S[1].begin()) ;
            }
            else{
                n = 1 , i = 1 ;
            }

            if( S[1].size() >= 2 && S[1].substr( S[1].size()-2 , 2 ) == ",X" ){
                x = 1 ;
                S[1].erase( S[1].end()-1 ) ;
                S[1].erase( S[1].end()-1 ) ;
            }

            string opcode = OPTAB[ S[0] ] ;
            opcode[1] += n*2+i ;

            if( opcode[1] > '9' && opcode[1] < 'A' )
            {
                opcode[1] -= '9' - 'A' + 1 ;
            }
            else if( opcode[1] > 'F' )
            {
                opcode[0] += 1 ;
                if( opcode[0] == '9'+1 ) opcode[0] = 'A' ;
                opcode[1] -= 'F' - '0' ;
            }

            object[count] << opcode ;

            if( e == 1 ){
                object[count] << hex << e ;
                if( i && !n ) object[count] << setfill('0') << setw(5) << hex << atoi(S[1].c_str()) ;
                else object[count] << setfill('0') << setw(5) << SYMTAB[S[1]] ;
            }
            else{
                    if( SYMTAB.find(S[1]) != SYMTAB.end()  )
                    {
                        if( abs(SYMTAB[S[1]]-PC) < 2048 ){
                            p = 1 ;
                            object[count] << hex << x*8+b*4+p*2+e ;
                            stringstream zs ;
                            zs << hex << setw(3) << setfill('0') << SYMTAB[S[1]] - PC ;
                            string zst ;
                            zs >> zst ;
                            while( zst.size() > 3 ) zst.erase( zst.begin() ) ;
                            object[count] << zst ;
                        }
                        else if ( abs(SYMTAB[S[1]]-BASE) < 4096 ){
                            b = 1 ;
                            object[count] << hex << x*8+b*4+p*2+e ;
                            object[count] << hex << setw(3) << setfill('0') <<  SYMTAB[S[1]] - BASE ;
                        }
                        else object[count] << "Error" ;
                    }
                    else
                    {
                        int shift = atoi( S[1].c_str() ) ;
                        if( abs(shift) < 2048 ){
                            p = 1 ;
                            object[count] << hex << x*8+b*4+p*2+e ;
                            object[count] << hex << setw(3) << setfill('0') << shift ;
                        }
                        else if ( shift < 4096 ){
                            b = 1 ;
                            object[count] << hex << x*8+b*4+p*2+e ;
                            object[count] << hex << setw(3) << setfill('0') << shift ;
                        }
                        else object[count] << "Error" ;
                    }
                }
        }
        count += 1 ;
        S.clear() ;
    }

    int tc = 0 , cc = 0 , last = 0 ;
    while( cc != numline )
    {
        tc += object[cc++].str().size() ;

        if( tc > 70 || command[cc].find("RESW")!=string::npos|| command[cc].find("RESB")!=string::npos || cc == numline-1  )
        {
            if( tc/2 == 0 ) continue ;

            string str ;
            while( object[last].str().size() <= 1 ) last ++ ;
            ofp << "T" << setw(6) << address[last] << hex << tc/2 ;
            for( int i = last ; i < cc ; ++i ){
                object[i] >> str ;
                if( str.size() <= 1 ) continue ;
                toupper(str) ;
                ofp << str ;
            }
            ofp << endl ;
            tc = 0 ;
            last = cc ;
        }
    }

    // For M instruction

    for( auto& e : jsubpos )
        ofp << "M" << setw(6) << e << "05" << endl ;

    // end
    ofp << "E" << setw(6) << setfill('0') << START ;

    return 0 ;
}

