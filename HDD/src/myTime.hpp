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
#include <string>"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"

#include "myConv.hpp"

namespace myTime{
    #define BUFFER 1024
    #define KB 1024
    #define MB 1048576
    #define GB 1073741824
    #define MS 0.001
    #define US 0.000001
    #define NS 0.000000001
    #define m_MS 1000
    #define m_US 1000000
    #define m_NS 1000000000
    //std::string strTimeFormatter = "[%Y/%m/%d %H:%M:%S]";
    inline boost::local_time::local_date_time *GetTime(const std::string &tzone = "MST-07"){
        ///Return struct with time date in specified format
        boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(tzone));
        boost::local_time::local_date_time *ldt = new boost::local_time::local_date_time( boost::local_time::local_microsec_clock::local_time(zone) );
        return ldt;
    }
    inline double TimeDiff( boost::local_time::local_date_time *Higher,\
                            boost::local_time::local_date_time *Lower){
        ///Returns time diffrence in nanoseconds for given local_time struct
         boost::posix_time::time_duration ret = *Higher-*Lower;
         return ret.total_nanoseconds()/(double)m_NS;
    }
    inline double TimeDiff( boost::local_time::local_date_time *Lower){
        ///Returns time diffrence
        boost::local_time::local_date_time *Higher = GetTime();
        boost::posix_time::time_duration ret = *Higher-*Lower;
        delete Higher;
        return ret.total_nanoseconds()/(double)m_NS;
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
        dTime == 0 ? dTime = 1 : dTime;// /= 1000000;
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
    inline std::string TimeScale(double dTime){
        if ( dTime <= 0){
            return "[Failed to measure: time diffrence to small]";
        }
        if (dTime < US){
            return myConv::ToString(m_NS * dTime)+"ns";
        }
        if(dTime < MS){
            return myConv::ToString(m_US * dTime)+"us";
        }
        if(dTime < 1){
            return myConv::ToString(m_MS * dTime)+"ms";
        }
        return myConv::ToString(dTime)+"s";
    }
}

#endif // MYTIME_HPP_INCLUDED
