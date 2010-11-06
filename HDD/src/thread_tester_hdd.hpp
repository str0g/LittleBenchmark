#ifndef THREAD_TESTER_HDD_HPP_INCLUDED
#define THREAD_TESTER_HDD_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers

#if ( _WIN32 || _WIN64 ) || ( __WIN32__ || __WIN64__ )
    #include <windows.h>
#endif
#include <cstdlib>
//#include <cstdio>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <csignal>
//boost
#include <boost/thread/thread.hpp>
#include <boost/filesystem/path.hpp>
#include "boost/date_time/local_time/local_time.hpp"
//my
#include "myConv.hpp"
#include "myHash.hpp"
#include "myTime.hpp"
#include "myIO.hpp"
#include "myThreadTemplates.hpp"
#include "Globals.h"
//Specials
using std::string;
using std::vector;
using std::list;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
namespace blC = buskol::Conv;
namespace blTT = buskol::ThreadTemplates;
namespace blT = buskol::Time;
namespace blIO = buskol::IO;
//Globals Varuabels
template <class classT> class thread_tester_hdd: public blTT::thread_1<thread_tester_hdd<classT> >{
    private:
        boost::filesystem::path pathDir; //!<Keep work directory
        string *p_strOutFileDataBuffer; //!<buffer which keeps charactes of specified size.
        string *p_strHashSum; //!<Keeps sha512 hash sum for comparization
        string strSizeTest; //!<
        unsigned uiDataLimit; //!<Data limit to count how much loop read/writes will be done for specified probes
        uint64_t ui64Loop;//!<
        unsigned uiMaxLoops; //!<Limit number of loops
        unsigned uiThreadNumber; //!< Number of thread and column
        unsigned uiStatsCounter; //!< Count statistic message
        unsigned uiNumberOfReads;//!<
        uint16_t ui8Precision;//!<Precision of cout
        mode_t uiPermissions; //!< Temporary folder premissions
        bool bWriteFailed;//!<
        bool bVerifyCache;//!< Allow data to be cache and verified
        bool bCleanWorkFolder;//!<Avoided try to delete what wasn't created or was already deleted
        static bool bFailer; //!<Prevention to raise multiple signals
        bool bSSD;//!<
        vector<uint64_t> vui_Probes; //!<Store Probes as unsigned
        list<string*> list_ReadFiles; //!<Stores read files to count hashsum
        list<string> it_list_RF; //!<iterator for list_ReadFiles store
        classT *p_Parent;//!< keep parent
        static boost::mutex mutex_Stats; //!< Mutex for locking list IO
        void CopyTest();
        void Read(uint16_t,string ="");
        void Write(const uint64_t *,string ="");
        bool VeryfiReadFiles();
        void addStats(string,string);
        void setBuffer(const uint64_t *);
    public:
        thread_tester_hdd(vector<uint64_t>&,
                          boost::filesystem::path &,
                          unsigned&,
                          unsigned&,
                          unsigned&,
                          unsigned&,
                          uint16_t&,
                          mode_t&,
                          bool,bool,bool,
                          classT*
                          );
        thread_tester_hdd(classT*);
        ~thread_tester_hdd();
        void setNewData(vector<unsigned>&,boost::filesystem::path&);
        void Execute();
};
template <class classT>
boost::mutex thread_tester_hdd<classT>::mutex_Stats;
#endif // THREAD_TESTER_HDD_HPP_INCLUDED
