#ifndef handler_Configuration_HPP_INCLUDED
#define handler_Configuration_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers

#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
    #include <shlobj.h> /*profile path*/
#else
    #include <pwd.h> /*profile path*/
#endif

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>
//
#include "myConv.hpp"
#include "myIO.hpp"
//Specials
using std::vector;
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
namespace blIO = buskol::IO;

struct profileNode{
    string strVar;//!Keep's variable name
    string strVal;//!Keep's variable value
    profileNode(const string&,const string&);
    string getData();
};

class handler_Configuration{
    private:
        vector<profileNode> *p_vec_Nodes;
        string *p_strConfigs;
    protected:
        boost::filesystem::path pathProfile;//!<Keep's path to config file
        boost::filesystem::path pathConfig;//!<Keep's path to config file
        bool bLetUpdate;//!<
    public:
        handler_Configuration();
        virtual ~handler_Configuration();
        void setUserDir();
        void addNodeToStored(const string&,const string&);
        void clearNodes();
        void parseConfigs();
        void saveConfigs();
};/*
        template <typename T>
        friend std::ostream & operator << (T & Output, handler_Configuration &w){
            return Output<<"string Variable: "<<w.strVar<<"\nstring Value: "<<w.strVal<<"\n";
*/

#endif // handler_Configuration_HPP_INCLUDED
