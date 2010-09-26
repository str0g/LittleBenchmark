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
#include <list>
#include <fstream>
#include <fcntl.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include "boost/date_time/local_time/local_time.hpp"
//my
#include "myTime.hpp"

namespace myIO{
    //READ OPERATIONS
    inline bool simpleReadToStringByChar(const boost::filesystem::path &strFile,\
                                         std::string *strBuf,\
                                         std::list<double> *p_listStats = NULL){
        //std::string *str = new std::string;
        if (!strBuf){
            std::cerr<<"[simpleReadToStringByChar]->"<<"Memory not allocated for: "<<strFile<<std::endl;
            return false;
        }

//        size_t filesize;
        double eTime = 0;
        std::list<double>::iterator it;
        if(p_listStats){
            it = p_listStats->begin();
        }

        boost::local_time::local_date_time *dTime = myTime::GetTime();
        std::ifstream file (strFile.file_string().c_str());
        if (file.is_open()){
            if (p_listStats){
                eTime = myTime::TimeDiff( dTime );
                *it > eTime ? *it = eTime : 0;
                ++it;
                *it < eTime ? *it = eTime : 0;
                ++it;
                *it += eTime;
                ++it;
                //std::cout<<"eTime:"<<eTime<<"["<<dTime<<"/"<<myTime::GetTime()<<"]"<<std::endl;
            }
            /*file.seekg(std::ios::end);
            filesize=file.tellg();

            strBuf->reserve(filesize);

            file.seekg(std::ios::beg);*/
            unsigned i = 0;
            while (!file.eof()){///Bug from unknow reason read 1 character more, maybe its fault of special characters. seekp(std::ios::end) returns bad file legnth probably special characters...
                *strBuf  += file.get();
                if (file.fail()){/*
                    std::cerr<<"[simpleReadToStringByChar]->Lost intergrity of the stream for file: "<<strFile<<std::endl;
                    std::cerr<<"Trying to fix situation..."<<std::endl;
                    std::cerr<<"Length of Buff: "<<strBuf->length()
                    <<"Position: "<<file.tellg()<<"Backup counter:"<<i
                    //<<"FileSize:"<<filesize
                    <<std::endl;*/
                    strBuf->erase(i);
                    break;
                }
                if (file.bad()){ std::cerr<<"[simpleReadToStringByChar]->Input error for file: "<<strFile<<std::endl; break;}
                i++;
            }
            //if (!file.eof()){ std::cerr<<"[simpleReadToStringByChar]->Read error: "<<strFile.file_string()<<std::endl; }
            file.close();
            if (p_listStats){
                eTime = myTime::TimeDiff( dTime );
                *it > eTime ? *it = eTime : 0;
                ++it;
                *it < eTime ? *it = eTime : 0;
                ++it;
                *it += eTime;
                //std::cout<<"eTime:"<<eTime<<"["<<dTime<<"/"<<myTime::GetTime()<<"]"<<std::endl;
            }
            delete dTime;
            return true;
        }
        std::cerr<<"[simpleReadToStringByChar]->"<<"failed to open file: "<<strFile<<std::endl;
        delete dTime;
        return false;
    }
    inline bool simpleReadToStringByStream(const boost::filesystem::path &pathFile, std::string *strBuf,\
                                           std::list<double> *p_listStats = NULL,\
                                           std::ios_base::openmode mode = std::ios::in|std::ios::binary){
        std::stringstream ss;
        double eTime = 0;
        std::list<double>::iterator it;
        if(p_listStats){
            it = p_listStats->begin();
        }

        boost::local_time::local_date_time *dTime = myTime::GetTime();
        std::ifstream file(pathFile.file_string().c_str(),mode);
        if (file.is_open()){
            if (p_listStats){
                eTime = myTime::TimeDiff( dTime );
                *it > eTime ? *it = eTime : 0;      ++it;
                *it < eTime ? *it = eTime : 0;      ++it;
                *it += eTime;                       ++it;
            }

            ss << file.rdbuf();
                if (ss.fail()){ std::cerr<<"[simpleReadToStringByStream]->Lost intergrity of the stream"<<std::endl; }
                if (ss.bad()){ std::cerr<<"[simpleReadToStringByStream]->Input error"<<std::endl; }
            *strBuf = ss.str();
            file.close();

            if (p_listStats){
                eTime = myTime::TimeDiff( dTime );
                *it > eTime ? *it = eTime : 0;      ++it;
                *it < eTime ? *it = eTime : 0;      ++it;
                *it += eTime;
            }
            delete dTime;
            return true;
        }
        delete dTime;
        return false;
    }
    //WRITE OPERATIONS
    inline bool touch(const boost::filesystem::path &pathTo,\
                      mode_t mode =0644){
        if ( boost::filesystem::is_regular_file(pathTo) ){
            std::cerr<<pathTo<<"::Already exist nothing to be done"<<std::endl;
            return false;
        }
        boost::filesystem::ofstream o (pathTo);
        if(o.is_open()){
            o.close();
            if (chmod(pathTo.file_string().c_str(),mode)!= 0){
                    std::cerr<<pathTo<<"::Failed to set permisions"<<std::endl;
            }
            return true;

        }
        std::cerr<<pathTo<<"::Failed to open"<<std::endl;
        return false;
    }
    inline void createDir(const boost::filesystem::path &Path){
        if (!  boost::filesystem::is_directory(Path) ){
            boost::filesystem::create_directory(Path);
        }
    }
    inline void delDir(const std::string *strPath){
        std::cout<<"Removed: "<< boost::filesystem::remove(*strPath) << std::endl;
    }
    inline bool SimpleWriteToFile(const boost::filesystem::path &pathTo,\
                                  const std::string &data,\
                                  std::ios_base::openmode mode = std::ios::app|std::ios::binary){
        boost::filesystem::ofstream o (pathTo,mode);
        //std::ofstream o (pathTo.file_string().c_str(),mode); std::cout<<*data<<std::endl;
        if(o){
            o << data;
            if( o.fail()){
                std::cerr << "Write error:"<<pathTo<<std::endl; return false;
            }
            o.close();
            return true;

        }
        return false;
    }
    inline bool appToFile(const std::string *strFileName,const std::string *data,mode_t pmode =0644,mode_t emode = 0444){
        if (chmod(strFileName->c_str(),pmode)==0){
            std::ofstream o(strFileName->c_str(),std::ios::app);
            if(o){
                o << *data;
                o.close();
                if (chmod(strFileName->c_str(),emode)!=0){
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
    //List
    inline void lsFiles(const boost::filesystem::path &bfsp_dir,\
                        std::list<boost::filesystem::path> *list_dir){
        if( boost::filesystem::exists( bfsp_dir ) ){
            boost::filesystem::directory_iterator end ;
            for( boost::filesystem::directory_iterator iter(bfsp_dir);\
                    iter != end; ++iter )
                if( boost::filesystem::is_regular_file( *iter )){
                    list_dir->push_back( iter->path() );
                }
        }else{
            std::cerr<<bfsp_dir<<" does not exist"<<std::endl;
        }
    }
    //COPY
    inline bool CopyFileByChar(const boost::filesystem::path &pathFrom,\
                               const boost::filesystem::path &pathTo){
        if ( boost::filesystem::is_directory(pathFrom) ){std::cerr<<pathFrom<<" is a directory file requered"<<std::endl; return false; }
        if ( boost::filesystem::is_directory(pathTo) ){ std::cerr<<pathFrom<<" is a directory file requered"<<std::endl; return false; }

        boost::filesystem::ifstream fileFrom (pathFrom);
        boost::filesystem::ofstream fileTo (pathTo,std::ios::ate);
        std::string strError;
        if (fileFrom.is_open() and fileTo.is_open()){
            char c;
            while (fileFrom.get(c)){
                fileTo.put(c);
                if ( fileTo.fail() ) { std::cerr << "Write error:"<<pathFrom<<std::endl; return false; }
            }
            if ( !fileFrom.eof() ) { std::cerr << "Read error:"<<pathTo<<std::endl; return false; }
            return true;
        }
        if (fileFrom.is_open()){ fileFrom.close(); } else { strError.append(" "+pathFrom.file_string()); }
        if (fileTo.is_open()){ fileTo.close(); } else { strError.append(" "+pathTo.file_string()); }
        std::cerr<<"Failed to copy files by char"<<strError<<std::endl;
        return false;

    }
    inline bool ReadByChunk(){
        return false;
    }
}

#endif // MYIO_HPP_INCLUDED
