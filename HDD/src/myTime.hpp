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
}

#endif // MYTIME_HPP_INCLUDED
