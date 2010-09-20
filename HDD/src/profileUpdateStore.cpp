/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "profileUpdateStore.hpp"

///Globals Varuabels
template<typename T>
profileUpdateStore::profileUpdateStore(string strvar,T val):strVar(new string (strvar)),\
                                                                    strVal(new string(myConv::ToString(val))){
}
profileUpdateStore::~profileUpdateStore(){
    if ( strVar ) { delete strVar; }
    if ( strVal ) { delete strVal; }
}
/*
string profileUpdateStore::*GetstrVar(){
    return strVar;
}
string profileUpdateStore::*GetstrVal(){
    return strVal;
}*/
