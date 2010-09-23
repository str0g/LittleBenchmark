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
/*profileUpdateStore::profileUpdateStore(const string &strvar,const string &val):strVar(strvar),\
                                                                    strVal((myConv::ToString(val))){
}*/
profileUpdateStore::profileUpdateStore( string strvar, string val):strVar(strvar),\
                                                                    strVal((myConv::ToString(val))){
}
profileUpdateStore::~profileUpdateStore(){}
string profileUpdateStore::GetstrVar(){
    return strVar;
}
string profileUpdateStore::GetstrVal(){
    return strVal;
}
string profileUpdateStore::GetIt(){
    return "\n"+strVar+" "+strVal;
}
