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
///Headers
#include <cstdlib>
//#include <cstdio>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
//boost
#include <boost/thread/thread.hpp>
//my
#include "myConv.hpp"
#include "myHash.hpp"
#include "myTime.hpp"
#include "myIO.hpp"
///Specials
using std::string;
using std::vector;
using std::list;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

class thread_tester_hdd{
    private:
        string strDir; //!<Keep work directory
        string *p_strSummary; //!<Store summary as string
        string *p_strOutFileDataBuffer; //!<Buffor which keeps charactes of specified size.
        string *p_strHashSum; //!<Keeps sha512 hash sum for comparization
        string strSlash; //!<Windows compatability
        unsigned int uiDataLimit; //!<Data limit to count how much loop read/writes will be done for specified probes
        uint64_t ui64Loop;//!<
        mode_t uiPermissions; //!< Temporary folder premissions
        bool bWriteFailed;//!<
        vector<unsigned int> vui_Probes; //!<Store Probes as unsigned int
        list<string*> list_ReadFiles; //!<Stores read files to count hashsum
        list<string> it_list_RF; //!iterator for list_ReadFiles store
        boost::thread m_Thread; //!<Thread for magic to be happening
    public:
        thread_tester_hdd(vector<unsigned int>,string,unsigned int,mode_t);
        ~thread_tester_hdd();
        void setNewData(vector<unsigned int>,string);
        void setBuffer(const unsigned int*);
        string &getSummary();
        void start();
        void join();
        void Execute();
        void readByChar(uint16_t);
        void readByLine();
        void Write(const unsigned int*);
        bool VeryfiReadFiles();
};

#endif // THREAD_TESTER_HDD_HPP_INCLUDED
