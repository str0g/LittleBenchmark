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
//Specials
using std::string;
using std::vector;
using std::list;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
//Globals Varuabels
template <class classT> class thread_tester_hdd: public myThreadTemplates::thread_1<thread_tester_hdd<classT> >{
    private:
        boost::filesystem::path pathDir; //!<Keep work directory
        string *p_strOutFileDataBuffer; //!<buffer which keeps charactes of specified size.
        string *p_strHashSum; //!<Keeps sha512 hash sum for comparization
        string strSlash; //!<Windows compatability
        unsigned uiDataLimit; //!<Data limit to count how much loop read/writes will be done for specified probes
        uint64_t ui64Loop;//!<
        unsigned uiMaxLoops; //!<Limit number of loops
        unsigned uiThreadNumber; //!< Number of thread and column
        unsigned uiStatsCounter; //!< Count statistic message
        uint8_t ui8Precision;//!<Precision of cout
        mode_t uiPermissions; //!< Temporary folder premissions
        bool bWriteFailed;//!<
        bool bVerifyCache;//!<
        bool bCleanWorkFolder;//!<
        static bool bFailer; //!<
        vector<unsigned> vui_Probes; //!<Store Probes as unsigned
        list<string*> list_ReadFiles; //!<Stores read files to count hashsum
        list<string> it_list_RF; //!<iterator for list_ReadFiles store
        classT *p_Parent;//!< keep parent
        static boost::mutex mutex_Stats; //!< Mutex for locking list IO
        //boost::thread m_Thread; //!<Thread for magic to be happening
        void CopyTest();
        //void Execute();
        void readByChar(uint16_t);
        void readByLine();
        void Write(const unsigned *);
        bool VeryfiReadFiles();
        void addStats(string*,string*);
    public:
        thread_tester_hdd(vector<unsigned>&,
                          boost::filesystem::path &,
                          unsigned&,
                          unsigned&,
                          unsigned&,
                          uint8_t&,
                          mode_t&,
                          bool,bool,
                          classT*
                          );
        thread_tester_hdd(classT*);
        ~thread_tester_hdd();
        void setNewData(vector<unsigned>&,boost::filesystem::path&);
        void setBuffer(const unsigned *);
        string &getSummary();
        void Execute();
};
template <class classT>
boost::mutex thread_tester_hdd<classT>::mutex_Stats;
#endif // THREAD_TESTER_HDD_HPP_INCLUDED
