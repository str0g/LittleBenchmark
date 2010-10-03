#ifndef STATS_KEEPER_HPP_INCLUDED
#define STATS_KEEPER_HPP_INCLUDED

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

struct vector_str{
    vector<string> data;
    vector_str(unsigned &);
};

class stats_keeper{
    private:
        string strSpacer; //!< spacer
    protected:
        vector<string> *p_vecstr_Log; //!< keep log output in vector
        vector<vector_str> *p_vecstr_formattedTXT; //!< keep data in vector
        bool bGenXML; //!<Generate xml
        bool bLog; //!<To log
        bool bFormattedTxt; //!< Generate formatted txt
        unsigned uiMultiplySpacer; //!< Multiply spacer
        unsigned uiMaxCols; //!< Max columns
        string strSpacerChar; //!< spacer character
        void setArgs();
    public:
        stats_keeper();
        virtual ~stats_keeper();
        void addStatData(string *,string*,unsigned =0,unsigned =0);
        void FormatDataInVector();
        void SaveToDisk(boost::filesystem::path);
        string GeneratDataFromVector();
        string GenerateXMLFromVector();
};


#endif // STATS_KEEPER_HPP_INCLUDED
