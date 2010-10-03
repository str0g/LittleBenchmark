#ifndef PROFILEUPDATESTORE_HPP_INCLUDED
#define PROFILEUPDATESTORE_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include "myConv.hpp"
//Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

class profileUpdateStore{
    private:
        string strVar;
        string strVal;
    public:
        //profileUpdateStore(const string&,const string& ="");
        profileUpdateStore( string, string ="");
        ~profileUpdateStore();
        string GetstrVar();
        string GetstrVal();
        string GetIt();
        template <typename T>
        friend std::ostream & operator << (T & Output, profileUpdateStore &w){
            return Output<<"string Varubale: "<<w.strVar<<"\nstring Value: "<<w.strVal<<"\n";
        }
};

#endif // PROFILEUPDATESTORE_HPP_INCLUDED
