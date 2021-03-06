#ifndef MYIO_HPP_INCLUDED
#define MYIO_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public License
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
namespace buskol{
    namespace IO{
    //READ OPERATIONS
    /*! \addtogroup libbuskol
   *  Additional documentation for group libbuskol
   *  @{
   */
  /*!
   *  Function read file by character and feel statistics if defined
   */
    inline bool simpleReadToStringByChar(const boost::filesystem::path &strFile,\
                                         std::string *strBuf,\
                                         std::list<double> *p_listStats = NULL){
        //std::string *str = new std::string;
        if (!strBuf){
            std::cerr<<"[simpleReadToStringByChar]->"<<"Memory not allocated for: "<<strFile<<std::endl;
            return false;
        }

//        size_t filesize;
        double eTime = 0;//!< keep result of time subtraction
        std::list<double>::iterator it;
        if(p_listStats){
            it = p_listStats->begin();
        }

        boost::local_time::local_date_time *dTime = Time::GetTime(); //!< Keep start time
        std::ifstream file (strFile.file_string().c_str());
        if (file.is_open()){
            if (p_listStats){
                eTime = Time::TimeDiff( *dTime );
                *it > eTime ? *it = eTime : 0;
                ++it;
                *it < eTime ? *it = eTime : 0;
                ++it;
                *it += eTime;
                ++it;
                //std::cout<<"eTime:"<<eTime<<"["<<dTime<<"/"<<Time::GetTime()<<"]"<<std::endl;
            }
            /*file.seekg(std::ios::end);
            filesize=file.tellg();

            strBuf->reserve(filesize);

            file.seekg(std::ios::beg);*/
            unsigned i = 0; //!< counter for temporary fix
            while (!file.eof()){///Bug from unknow reason read 1 character more, maybe its special character fault, seekp(std::ios::end) return bad file legnth probably special character...
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
                eTime = Time::TimeDiff( *dTime );
                *it > eTime ? *it = eTime : 0;
                ++it;
                *it < eTime ? *it = eTime : 0;
                ++it;
                *it += eTime;
                //std::cout<<"eTime:"<<eTime<<"["<<dTime<<"/"<<Time::GetTime()<<"]"<<std::endl;
            }
            delete dTime;
            return true;
        }
        std::cerr<<"[simpleReadToStringByChar]->"<<"failed to open file: "<<strFile<<std::endl;
        delete dTime;
        return false;
    }
  /*!
   *  Function read file by stream and feel statistics if defined
   */
    inline bool simpleReadToStringByStream(const boost::filesystem::path &pathFile, std::string *strBuf,\
                                           std::list<double> *p_listStats = NULL,\
                                           std::ios_base::openmode mode = std::ios::in|std::ios::binary){
        std::stringstream ss;//!< Stream for read from file to string
        double eTime = 0;//!< keep result of time subtraction
        std::list<double>::iterator it;
        if(p_listStats){
            it = p_listStats->begin();
        }

        boost::local_time::local_date_time *dTime = Time::GetTime();//!< Keep start time
        std::ifstream file(pathFile.file_string().c_str(),mode);
        if (file.is_open()){
            if (p_listStats){
                eTime = Time::TimeDiff( *dTime );
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
                eTime = Time::TimeDiff( *dTime );
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
   /*!
   *  Function touch but can also set permission.
   */
    inline bool touch(const boost::filesystem::path &pathTo,\
                      const mode_t &mode =0644){
        if ( boost::filesystem::is_regular_file(pathTo) ){
            std::cerr<<pathTo<<"::Already exist nothing to be done"<<std::endl;
            return false;
        }
        boost::filesystem::ofstream o (pathTo);
        if(o.is_open()){
            o.close();
        #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
        #else
            if (chmod(pathTo.file_string().c_str(),mode)!= 0){
                    std::cerr<<pathTo<<"::Failed to set permisions"<<std::endl;
            }
        #endif
            return true;

        }
        std::cerr<<pathTo<<"::Failed to open"<<std::endl;
        return false;
    }
  /*!
   *  Function is equal to mkdir
   */
    inline void createDir(const boost::filesystem::path &Path,const mode_t &mode=0644){
        if (!  boost::filesystem::is_directory(Path) ){
            boost::filesystem::create_directory(Path);
        #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
        #else
            if (chmod(Path.file_string().c_str(),mode)!= 0){
                    std::cerr<<Path<<"::Failed to set permisions"<<std::endl;
            }
        #endif
        }
    }
  /*!
   *  Function write data to file by stream
   */
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

  /*!
   *  Function append data to file, support permission change.
   */
    inline bool appToFile(const boost::filesystem::path &path,
                          const std::string &data,
                          const mode_t &pmode =0644,const mode_t &emode = 0444){
    #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
        if(true){
    #else
        if (chmod(path.file_string().c_str(),pmode)==0){
    #endif
            std::ofstream o(path.file_string().c_str(),std::ios::app);
            if(o){
                o << data;
                o.close();
            #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
                return true;
            #else
                if (chmod(path.file_string().c_str(),emode)!=0){
                    std::cerr<<"[appToFile]: "<<path<<" Premission error(after write)"<<std::endl;
                }
                return true;
            #endif
            }
            std::cerr<<"[appToFile]: "<<path<<" Failed to open"<<std::endl;
        }else{
            std::cerr<<"[appToFile]: "<<path<<" Premission error (before Open)"<<std::endl;
        }
        return false;
    }
    //REMOVE OPERATIONS
  /*!
   *  Function delete all childerens in give directory by recurense
   *  Algoritm is fail proof by depth counter
   *  Support forced remove by permission change
   */
    inline void rmAll_inDir(const boost::filesystem::path & directory,bool bForced = true,const unsigned &intDirScanDepth =1024){
        unsigned intDirScanCounter = 0;
        if( boost::filesystem::exists( directory ) ){
            boost::filesystem::directory_iterator end ;
            for( boost::filesystem::directory_iterator iter(directory) ; iter != end ; ++iter ){
                if ( boost::filesystem::is_directory( *iter ) ){
                    if(intDirScanCounter < intDirScanDepth){
                        ++intDirScanCounter;
                        rmAll_inDir(*iter,bForced);
                        boost::filesystem::remove(*iter);
                    }//deep scan
                }else{
                    if (bForced){
                    #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
                    #else
                        chmod(iter->path().file_string().c_str(),0644);
                    #endif
                    }
                    boost::filesystem::remove(*iter);
                }//if dir
            }//loop
        }//if exist
    }
  /*!
   *  Function delete every thing in and give folder or empty file or folder depends on on string option
   *  Support forced remove by permission change (only for file or empty folder)
   */
    inline void rm(const boost::filesystem::path & path,const std::string &strOpt="",bool bforced = false){
        if (strOpt == "all"){///Fast remover for directory
            boost::filesystem::remove_all(path);
        }
        if (strOpt == ""){///Remove file or empty folder with premissions changing capabilities
            #if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
            #else
            if(bforced)
                if (chmod(path.file_string().c_str(),0644) != 0)
                    std::cerr<<"Failed to change Premission: "<<path<<std::endl;
            #endif
            boost::filesystem::remove(path);
        }

    }
    //List
  /*!
   *  Function list files in directory
   */
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
  /*!
   *  Function copy file by character
   */
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
  /*!
   *  Function read from file defined data size (Not implemented yet)
   */
    inline bool ReadByChunk(const boost::filesystem::path &path,
                            const unsigned &chunk){
        return false;
    }
/*! @} */
   }
}

#endif // MYIO_HPP_INCLUDED
