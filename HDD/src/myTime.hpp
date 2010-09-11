#ifndef MYTIME_HPP_INCLUDED
#define MYTIME_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
#include <string>
#include "myConv.hpp"

namespace myTime{
    #define BUFFER 1024
    #define KB 1024
    #define MB 1048576
    #define GB 1073741824
    //std::string strTimeFormatter = "[%Y/%m/%d %H:%M:%S]";
    inline double GetTime(){
        ///Zwraca własciwie obliczony czas od właczenia programu
        return (double)clock()/CLOCKS_PER_SEC;
    }
    inline double TimeDiff(double dLiczbaA, double dLiczbaB){
        ///Zwraca różnice czasów
        return dLiczbaA - dLiczbaB;
    }
    inline std::string GetLocalTime(std::string strTimeFormatter = "[%Y/%m/%d %H:%M:%S]"){
        ///Pobiera czas wedlug zdefinowanego formatowania
        time_t rawtime; //!< pobranie czasu
        struct tm * timeinfo; //!< struktura czasu, dzieki ktorej dziala formatowanie
        char buffer2 [BUFFER]; //!< buffor do ktorego zostanie wpisany string

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strftime (buffer2,BUFFER,strTimeFormatter.c_str(),timeinfo);

        return buffer2;
    }
    template <typename T>
    inline std::string Bandwidth(double dTime ,const T *ui64Counter,std::string strAppEnd = "/s"){
        dTime == 0 ? dTime = 1 : dTime;
        double dRet = *ui64Counter / dTime ;
        if (dRet < KB ){ //B
            return myConv::ToString(dRet)+"B"+strAppEnd;
        }
        if ( dRet < MB ){//KB
            return myConv::ToString(dRet/KB)+"KB"+strAppEnd;
        }
        if( dRet < GB ){//MB
            return myConv::ToString(dRet/MB)+"MB"+strAppEnd;
        }
        return myConv::ToString(dRet/GB)+"GB"+strAppEnd;
    }
    template <typename T>
    inline T FromString(std::string str){
        for (int it = str.length()-1; -1 < it; --it){
                if(str.at(it) == 'K' or str.at(it) == 'k'){
                    str.erase(it);
                    return myConv::FromString<T>(str) * KB;
                }
                if(str.at(it) == 'M' or str.at(it) == 'm'){
                    str.erase(it);
                    return myConv::FromString<T>(str) * MB;
                }
                if(str.at(it) == 'G' or str.at(it) == 'g'){
                    str.erase(it);
                    return myConv::FromString<T>(str) * GB;
                }
        }
        return 1;
    }
}

#endif // MYTIME_HPP_INCLUDED
