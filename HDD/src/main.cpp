/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
//#include <cstdlib>
//#include <iostream>
#include "tester_hdd.hpp"
/*
#include <string>
#include <list>
using std::string;
using std::cout;
using std::endl;
using std::list;
*/
///Specials
///Globals Varuabels

int main(int ac,char **av){

    tester_hdd hdd(ac,av);
    if (hdd.bRun)
        hdd.Run();

    return 0;
}
/*

-x 1K 3k -w dasd asd dsad -l 1M -C -D -S 1

*/
