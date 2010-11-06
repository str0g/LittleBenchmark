#ifndef TESTER_HDD_HPP_INCLUDED
#define TESTER_HDD_HPP_INCLUDED

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
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <new>
#include <stdexcept>
//boost
#include <boost/program_options.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/filesystem/fstream.hpp>
//#include <boost/thread/thread.hpp>
//
#include "version.h"
//my
#include "myHash.hpp"
#include "myIO.hpp"
#include "myConv.hpp"
#include "myTime.hpp"
#include "thread_tester_hdd.hpp"
#include "thread_tester_hdd.cpp"//change it later
#include "handler_Configuration.hpp"
#include "handler_Report.hpp"

#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
    #include <windows.h>/*mem info*/
#else
    #include <sys/sysinfo.h>/*mem info*/
#endif

//Specials
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
namespace blC = buskol::Conv;
namespace blIO = buskol::IO;
//Globals Varuabels

class tester_hdd: public handler_Configuration, public handler_Report{
    private:
        string strDataLimit; //!<Data Limit as string beforre conversion to unsigned
        string MaxCharSize; //!<Limit for char size
        string MaxStringSize; //!< Limit for string size
        bool bThreadingPerDir; //!<Every dir in new thread simuisly
        bool bDebugging; //!<Debugging Enable/Disable
        bool bSelfTest;//!<Run selftest
        bool bBufferingTest;//!<Run Max alloc test
        bool bVerifyCache;//!<Use verifycation
        bool bInsufficientMemory;//!<Use memory limit
        bool bSSD;//!< SSD revers test
        int iTextColor;//!< TextColor
        unsigned uiThreadsPerDir; //!<How much threads per one directory/driver
        unsigned uiSelfTestScenerio; //!<Which test scenerio use
        unsigned uiDataLimit; //!<Data limit to count how much loop read/writes will be done for specified probes
        unsigned uiMaxLoops;//!<Max loops
        unsigned uiNumberOfReads;//!<Number of read simulations
        uint64_t ui64MaxCharSize;//!< Limit for char size
        uint64_t ui64MaxStringSize;//!< Limit for string size
        uint16_t ui8Precision;//!<Precision of cout
        mode_t uiPermissions; //!< Temporary folder premissions
        vector<boost::filesystem::path> vstr_WorkDirs; //!<Store work directories
        vector<boost::filesystem::path>::iterator vitstr_WorkDirs; //!<iterator for Stored directories
        vector<uint64_t> vui_Probes;//!<Store Probes as unsigned
        boost::ptr_list<thread_tester_hdd<tester_hdd> > bst_ptrlist_tester_hdd; //!<Store list of working threads
        boost::ptr_list<thread_tester_hdd<tester_hdd> >::iterator bst_ptrlist_it_tester_hdd; //!<iterator
        //
        void BufferingTest();
        void GetUserDir();
        void SelfTest();
        void UpdateProfile();
    public:
        bool bRun;
        tester_hdd(int,char **);
        ~tester_hdd();
        void Run();
};

#endif // TESTER_HDD_HPP_INCLUDED
