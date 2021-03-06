#ifndef MYCONV_HPP_INCLUDED
#define MYCONV_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (http://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/

#include <string>
#include <iostream>
#include <sstream>
#include "Globals.h"

namespace buskol{
    namespace Conv{
/*! \addtogroup libbuskol
   *  Additional documentation for group libbuskol
   *  @{
   */
  /*!
   *  Function convert number to string
   */
        template <typename T> inline std::string ToString(const T& liczba){
            std::stringstream ss;
            ss << liczba;
            return ss.str();
        }
        #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
            #ifdef _MSC_VER
    /*!
    *  Conver string to wstring usefull only for windows
    */
            inline std::wstring stringTo_wstring(const std::string& str){
                std::wstring wstr;
                wstr.assign(str.begin(),str.end());
                return wstr;
            }
            #endif
        #endif
    /*!
    *  Convert string to number
    */
        template <typename T> inline T FromString(const std::string &s_liczba){
        T liczba;
        std::stringstream(s_liczba) >> liczba;
        return liczba;
        }
    /*!
    * Count bandwidth or scale size.
    * Output is human readable
    */
        template <typename T> inline std::string Bandwidth(double dTime ,const T &ui64Counter,const std::string &strAppEnd = "/s"){
            dTime == 0 ? dTime = 1 : dTime;// /= 1000000;
            double dRet = 0;
            if ( ui64Counter == 0){
                std::cerr<<myColors::ConsoleColors_[1]<<"Can not count bandwidth from counter = 0"<<myColors::ConsoleColors_[*myColors::TextColor]<<std::endl;
                std::cerr<<"dTime:"<<dTime<<" Exp end sqc["<<strAppEnd<<"]"<<std::endl;
            }else{ dRet = ui64Counter / dTime ;}
            if (dRet < KB ){ //B
                return Conv::ToString(dRet)+"B"+strAppEnd;
            }
            if ( dRet < MB ){//KB
                return Conv::ToString(dRet/KB)+"KB"+strAppEnd;
            }
            if( dRet < GB ){//MB
                return Conv::ToString(dRet/MB)+"MB"+strAppEnd;
            }
            return Conv::ToString(dRet/GB)+"GB"+strAppEnd;
        }
    /*!
    * Generate size from string
    */
        template <typename T> inline T SizeFromString(std::string str){
            for (unsigned it = str.length()-2; it < str.length(); it++){ //std::cout<<"FromString: "<<str<<"::"<<str.at(it)<<"::"<<std::endl;
                if(str.at(it) == 'B' or str.at(it) == 'b'){
                    str.erase(it);
                    return Conv::FromString<T>(str);
                }
                if(str.at(it) == 'K' or str.at(it) == 'k'){
                    str.erase(it);
                    return Conv::FromString<T>(str) * KB;
                }
                if(str.at(it) == 'M' or str.at(it) == 'm'){
                    str.erase(it);
                    return Conv::FromString<T>(str) * MB;
                }
                if(str.at(it) == 'G' or str.at(it) == 'g'){
                    str.erase(it);
                    return Conv::FromString<T>(str) * GB;
                }
            }
            std::cerr<<"Cannot convert unsported size unit"<<std::endl;
            return 0;
        }
    /*!
    * From time generates human readable string
    */
        inline std::string TimeToString(const double &dTime){
            if ( dTime <= 0){
                return "[Failed to measure: time diffrence to small]";
            }
            if (dTime < US){
                return Conv::ToString(m_NS * dTime)+"ns";
            }
            if(dTime < MS){
                return Conv::ToString(m_US * dTime)+"us";
            }
            if(dTime < 1){
                return Conv::ToString(m_MS * dTime)+"ms";
            }
            return Conv::ToString(dTime)+"s";
        }
    /*!
    * Convert time string to time as number, can also do scaling if multiplicand is defined
    */
        template <typename T> inline T TimeFromString(std::string str,const double &dMulti =1){
            for (unsigned it = str.length()-2; it < str.length(); it++){ //std::cout<<"FromString: "<<str<<"::"<<str.at(it)<<"::"<<std::endl;
                if(str.at(it) == 'N' or str.at(it) == 'n' or str.at(it) == 'U' or str.at(it) == 'u' or str.at(it) == 'M' or str.at(it) == 'm' or str.at(it) == 'S' or str.at(it) == 's'){
                    str.erase(it);
                    return Conv::FromString<T>(str) * dMulti;
                }
            }
            std::cerr<<"Cannot convert unsported time unit"<<std::endl;
            return 0;
        }
/*! @} */
    }
}
#endif // MYCONV_HPP_INCLUDED
