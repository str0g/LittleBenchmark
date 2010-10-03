/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
//#include <cstdlib>
//#include <iostream>
#include "tester_hdd.hpp"
//#include "Globals.h"
//export ConsoleColors[];
//extern string ConsoleColors[];
#include <csignal>
//#include <string>
/*#include <list>
#include <vector>
using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::list;
*/
//Specials
//Globals Varuabels

static tester_hdd *p_hdd = NULL;
static bool bFl = false;

void clean (int param){
///Method run destructors on common signals
if(!bFl){
    bFl = true;
    cout<<"\nTerminating program...["<<param<<"]"<<endl;
    if (p_hdd){
      p_hdd->~tester_hdd();
    }
    exit(param);
  }
}

int main(int ac,char **av){

    signal (SIGSEGV, clean);
    #ifdef __WIN32__ || __WIN64__
    #else
    signal (SIGKILL,clean);
    #endif
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
