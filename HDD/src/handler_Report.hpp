#ifndef handler_Report_HPP_INCLUDED
#define handler_Report_HPP_INCLUDED

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
#include <vector>
#include <boost/filesystem/path.hpp>
#include "myIO.hpp"

//Specials
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
//Globals Varuabels

struct structRow{
    vector<string> data; //!< Row
    structRow(unsigned &);
};

class handler_Report{
    private:
        string strSpacer; //!< spacer
    protected:
        vector<string> *p_vecstr_Log; //!< keep log output in vector
        vector<structRow> *p_vecstr_formattedTXT; //!< keep data in vector
        boost::filesystem::path pathReport;//!<Keeps path to report file
        string strReportFile; //!<File to report will be written
        bool bLog; //!<To log
        bool bFormattedTxt; //!< Generate formatted txt
        bool bGenXML; //!<Generate xml
        unsigned uiMultiplySpacer; //!< Multiply spacer
        unsigned uiMaxCols; //!< Max columns
        string strSpacerChar; //!< spacer character
        void setArgs();
    public:
        handler_Report();
        virtual ~handler_Report();
        void addStatData(string&,string&,unsigned =1,unsigned =1);
        uint8_t findAndAdd(string&,string&,unsigned =1,unsigned =1);
        void FormatDataInVector();
        void SaveToDisk(boost::filesystem::path);
        string GeneratDataFromVector();
        string GenerateXMLFromVector();
};


#endif // handler_Report_HPP_INCLUDED
