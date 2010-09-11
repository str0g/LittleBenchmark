/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include <cstdlib>
#include <iostream>
#include "tester_hdd.hpp"/*
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

/*
string *s = NULL;
list<string*> ls;
list <string*>::iterator ils;
for(int i=0; i<3; ++i){
    char z = i+50;
    const char *x =&z;
    string *str = new string(x);
    ls.push_back(str);
}
for(ils = ls.begin(); ils != ls.end(); ++ils){
    cout << **ils <<endl;
}

for(ils = ls.begin(); ils != ls.end(); ++ils){
    delete &(**ils);
    *ils = NULL;
}

ls.clear();
*/

    return 0;
}
/*

-x 1K 3k -w dasd asd dsad -l 1M -C -D -S 1

*/
