#ifndef MYCONV_HPP_INCLUDED
#define MYCONV_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (http://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/

#include <string>
#include <sstream>
#include "Globals.h"
namespace buskol{
    namespace Conv{
        template <typename T> inline std::string ToString(const T& liczba){///Conver number to string
            std::stringstream ss;
            ss << liczba;
            return ss.str();
        }
        #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
            #ifdef _MSC_VER
            inline std::wstring stringTo_wstring(const std::string& str){///Conver string to wstring usefull only for windows
                std::wstring wstr;
                wstr.assign(str.begin(),str.end());
                return wstr;
            }
            #endif
        #endif
        template <typename T> inline T FromString(std::string s_liczba){///Convert string to number
        T liczba;
        std::stringstream(s_liczba) >> liczba;
        return liczba;
        }
        template <typename T> inline std::string Bandwidth(double dTime ,const T &ui64Counter,const std::string &strAppEnd = "/s"){///Count bandwidth or scale size
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
        template <typename T> inline T SizeFromString(std::string str){///Generate size from string with auto scaling
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

        inline std::string TimeToString(const double &dTime){///From time generates string with auto scaling
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
        template <typename T> inline T TimeFromString(std::string str, double dMulti){///Convert string which contains time to number, can also do scaling
            for (unsigned it = str.length()-2; it < str.length(); it++){ //std::cout<<"FromString: "<<str<<"::"<<str.at(it)<<"::"<<std::endl;
                if(str.at(it) == 'N' or str.at(it) == 'n' or str.at(it) == 'U' or str.at(it) == 'u' or str.at(it) == 'M' or str.at(it) == 'm' or str.at(it) == 'S' or str.at(it) == 's'){
                    str.erase(it);
                    return Conv::FromString<T>(str) * dMulti;
                }
            }
            std::cerr<<"Cannot convert unsported time unit"<<std::endl;
            return 0;
        }
    }
}
#endif // MYCONV_HPP_INCLUDED
