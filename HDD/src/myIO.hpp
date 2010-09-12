#ifndef MYIO_HPP_INCLUDED
#define MYIO_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace myIO{
    //READ OPERATIONS
    inline bool simpleReadToStringByChar(const std::string *strFile,std::string *strBuf){
        //std::string *str = new std::string;
        if (!strBuf){
            std::cerr<<"[simpleReadToStringByChar]->"<<"failed to allocate memory for: "<<*strFile<<std::endl;
            return false;
        }
        size_t filesize;

        std::ifstream file (strFile->c_str());
        if (file.is_open()){
            filesize=file.tellg();

            strBuf->reserve(filesize);

            file.seekg(0);
            while (!file.eof()){
                *strBuf  += file.get();
            }
            return true;
        }
        std::cerr<<"[simpleReadToStringByChar]->"<<"failed to open file: "<<*strFile<<std::endl;
        return false;
    }
    inline bool simpleReadToStringByStream(const std::string *strFile, std::string *strBuf,std::ios_base::openmode mode = std::ios::in|std::ios::binary){
        std::stringstream ss;

        std::ifstream file(strFile->c_str(),mode);
        if (file.is_open()){
            ss << file.rdbuf();
            strBuf = &ss.str();
            ss.flush();
            file.close();
            return true;
        }

        return false;
    }
    //WRITE OPERATIONS
    inline bool touch(const std::string *strFileName,int int_chmod =0644){
        std::ofstream o(strFileName->c_str());
        if(o){
            o.close();
            chmod(strFileName->c_str(),int_chmod);
            return true;

        }
        return false;
    }
    inline void createDir(const std::string *strPath){
        if (!  boost::filesystem::is_directory(*strPath) ){
            boost::filesystem::create_directory(*strPath);
        }
    }
    inline void delDir(const std::string *strPath){
        std::cout<<"Removed: "<< boost::filesystem::remove(*strPath) << std::endl;
    }
    inline bool SimpleWriteToFile(const std::string *strFileName,const std::string *data,std::ios_base::openmode mode = std::ios::app|std::ios::binary){
        std::ofstream o(strFileName->c_str(),mode);
        if(o){
            o << *data;
            o.flush();
            o.close();
            return true;

        }
        return false;
    }
    inline bool appToFile(const std::string *strFileName,const std::string *data,int int_chmod =0644,int int_chmodE = 0444){
        if (chmod(strFileName->c_str(),int_chmod)==0){
            std::ofstream o(strFileName->c_str(),std::ios::app);
            if(o){
                o << *data;
                o.close();
                if (chmod(strFileName->c_str(),int_chmodE)!=0){
                    std::cerr<<"[appToFile]: "<<*strFileName<<" Premission error(after write)"<<std::endl;
                }
                return true;
            }
            std::cerr<<"[appToFile]: "<<*strFileName<<" Failed to open"<<std::endl;
        }else{
            std::cerr<<"[appToFile]: "<<*strFileName<<" Premission error (before Open)"<<std::endl;
        }
        return false;
    }
    //REMOVE OPERATIONS
    inline void rmAll(const boost::filesystem::path & directory,bool bForced = true,int intDirScanDepth =1024){
        int intDirScanCounter = 0;
        if( boost::filesystem::exists( directory ) ){
            boost::filesystem::directory_iterator end ;
            for( boost::filesystem::directory_iterator iter(directory) ; iter != end ; ++iter ){
                if ( boost::filesystem::is_directory( *iter ) ){
                    if(intDirScanCounter < intDirScanDepth){
                        ++intDirScanCounter;
                        myIO::rmAll(*iter,bForced);
                        boost::filesystem::remove(*iter);
                    }//deep scan
                }else{
                    if (bForced){
                        chmod(iter->path().file_string().c_str(),0644);
                    }
                    boost::filesystem::remove(*iter);
                }//if dir
            }//loop
        }//if exist
    }
}

#endif // MYIO_HPP_INCLUDED
