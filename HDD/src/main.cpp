/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/
//Headers
#include <iostream>
#include <csignal>
#include "tester_hdd.hpp"
//Globals Varuabels
static tester_hdd *p_hdd = NULL;
static bool bFl = false;
//Port
#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
#include <windows.h>
bool CtrlHandler( DWORD fdwCtrlType ){
    if(!bFl){
        bFl = true;
        cout<<"\nTerminating program...[ by X ]"<<endl;
        if (p_hdd){
        p_hdd->~tester_hdd();
        }
        return true;
    }
    return false;
}
#endif

void clean (int param){
///Method run destructors on common signals
    if(!bFl){
        bFl = true;
        std::cout<<"\nTerminating program...["<<param<<"]"<<std::endl;
        if (p_hdd){
        p_hdd->~tester_hdd();
        }
        exit(param);
    }
}

int main(int ac,char **av){

    signal (SIGSEGV, clean);
    #if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
    SetConsoleCtrlHandler((PHANDLER_ROUTINE) CtrlHandler,TRUE);
    #else
    signal (SIGKILL,clean);
    #endif
    signal (SIGTERM,clean);
    signal (SIGINT,clean);
    signal (SIGABRT,clean);

    tester_hdd hdd(ac,av);
    p_hdd = &hdd;
    hdd.Run();

    return 0;
}
/*

-x 1K 3k -w dasd asd dsad -l 1M -C -D -S 1

*/
