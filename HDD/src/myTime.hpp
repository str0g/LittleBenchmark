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
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/local_time/local_time.hpp"

#include "myConv.hpp"
#include "Globals.h"

namespace myTime{
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
        char buffer2 [KB]; //!< buffor do ktorego zostanie wpisany string

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strftime (buffer2,KB,strTimeFormatter.c_str(),timeinfo);

        return buffer2;
    }
    template <typename T>
    inline std::string Bandwidth(double dTime ,const T &ui64Counter,const std::string &strAppEnd = "/s"){
        dTime == 0 ? dTime = 1 : dTime;// /= 1000000;
        double dRet = 0;
        if ( ui64Counter == 0){
            std::cerr<<"\033[22;31mCan not count bandwidth from counter = 0 \033[22;30m]"<<std::endl;
            std::cerr<<"dTime:"<<dTime<<" Exp end sqc["<<strAppEnd<<"]"<<std::endl;
        }else{ dRet = ui64Counter / dTime ;}
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
        for (int it = str.length()-2; it < str.length(); it++){ //std::cout<<"FromString: "<<str<<"::"<<str.at(it)<<"::"<<std::endl;
                if(str.at(it) == 'B' or str.at(it) == 'b'){
                    str.erase(it);
                    return myConv::FromString<T>(str);
                }
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
        return 0;
    }
    inline std::string TimeScale(const double &dTime){
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
