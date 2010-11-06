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

//#include "myConv.hpp"
#include "Globals.h"
namespace buskol{
    namespace Time{
    /*! \addtogroup libbuskol
   *  Additional documentation for group libbuskol
   *  @{
   */
  /*!
   *  Function returns pointer to local_data_time struct
   *  Time and data format can be specified
   */
    //std::string strTimeFormatter = "[%Y/%m/%d %H:%M:%S]";
    inline boost::local_time::local_date_time *GetTime(const std::string &tzone = "MST-07"){
        boost::local_time::time_zone_ptr zone(new boost::local_time::posix_time_zone(tzone));
        boost::local_time::local_date_time *ldt = new boost::local_time::local_date_time( boost::local_time::local_microsec_clock::local_time(zone) );
        return ldt;
    }
  /*!
   *  Function return time diffrence for 2 give local_data_time structers
   * Time is being scaled to seconds!
   */
    inline double TimeDiff(const boost::local_time::local_date_time &Higher,\
                           const boost::local_time::local_date_time &Lower){
         boost::posix_time::time_duration ret = Higher-Lower;
         return ret.total_nanoseconds()/(double)m_NS;
    }
  /*!
   *  Function return time diffrence for give local_data_time structer and curren time
   * Time is being scaled to seconds!
   */
    inline double TimeDiff(const boost::local_time::local_date_time &Lower){
        ///Returns time diffrence
        boost::local_time::local_date_time *Higher = GetTime();
        boost::posix_time::time_duration ret = *Higher - Lower;
        delete Higher;
        return ret.total_nanoseconds()/(double)m_NS;
    }
  /*!
   *  Function return time with defined formatting
   */
    inline std::string GetLocalTime(const std::string &strTimeFormatter = "[%Y/%m/%d %H:%M:%S]"){
        char buffer2 [KB]; //!< string buffer
        time_t rawtime;
        struct tm * timeinfo; //!< time structure for formatter

        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        strftime (buffer2,KB,strTimeFormatter.c_str(),timeinfo);

        return buffer2;
    }
/*! @} */
    }
}

#endif // MYTIME_HPP_INCLUDED
