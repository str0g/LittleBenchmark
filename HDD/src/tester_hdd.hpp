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
///Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
//boost
#include <boost/program_options.hpp>
#include <boost/ptr_container/ptr_list.hpp>
//#include <boost/thread/thread.hpp>
//
#include "version.h"
//my
#include "myHash.hpp"
#include "myIO.hpp"
#include "myConv.hpp"
#include "myTime.hpp"
#include "thread_tester_hdd.hpp"

///Specials
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

class tester_hdd{
    private:
        string strSummaryFile; //!<File to summary will be written
        string strDataLimit; //!<Data Limit as string beforre conversion to unsigned int
        bool bThreadingPerDir; //!<Every dir in new thread simuisly
        bool bDebugging; //!<Debugging Enable/Disable
        unsigned int uiThreadsPerDir; //!<How much threads per one directory/driver
        unsigned int uiSelfTestScenerio; //!<Which test scenerio use
        unsigned int uiDataLimit; //!<Data limit to count how much loop read/writes will be done for specified probes
        mode_t uiPermissions; //!< Temporary folder premissions
        vector<string> vstr_WorkDirs; //!<Store work directories
        vector<string>::iterator vitstr_WorkDirs; //!<iterator for Stored directories
        vector<unsigned int> vui_Probes;//!<Store Probes as unsigned int
        boost::ptr_list<thread_tester_hdd> bst_ptrlist_tester_hdd; //!<Store list of working threads
        boost::ptr_list<thread_tester_hdd>::iterator bst_ptrlist_it_tester_hdd; //!<iterator
    public:
        bool bRun;
        tester_hdd(int,char **);
        ~tester_hdd();
        void SetBuffer(const unsigned int&);
        void WriteTest();
        void ReadTest();
        void VeryfiFilesInWorkFolder();
        void Run();
        void SelfTest();
        void BufferingTest();
};

#endif // TESTER_HDD_HPP_INCLUDED
