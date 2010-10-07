/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
#include "handler_Configuration.hpp"

profileNode::profileNode(string &var,string &val):strVar(var),strVal(val){
}
string profileNode::getData(){
    return "\n"+strVar+" = "+strVal;
}

handler_Configuration::handler_Configuration(): p_vec_Nodes(new vector<profileNode>),
                                                p_strConfigs(new string),
                                                pathProfile(""),
                                                pathConfig(""),
                                                bLetUpdate(false)
                                                {}
handler_Configuration::~handler_Configuration(){
    cerr<<"Destructor: handler_Configuration"<<endl;
    delete p_vec_Nodes;
    delete p_strConfigs;
}

void handler_Configuration::addNodeToStored(string var,string val){
    p_vec_Nodes->push_back(profileNode(var,val));
}
void handler_Configuration::clearNodes(){
    p_vec_Nodes->clear();
}

void handler_Configuration::setUserDir(){
#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
  char path[ MAX_PATH ];
  if (SHGetFolderPathA( NULL, CSIDL_PROFILE, NULL, 0, path ) != S_OK)
    throw std::runtime_error( "Could not retrieve the user's home directory!");
  /*else
    cout << "Home directory = \"" << path << endl;*/

  if (SHGetFolderPathA( NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path ) != S_OK)
    throw std::runtime_error("Could not retrieve the user's application data directory!");
    /*
  else
    cout << "Application data directory = \"" << path << endl;*/

    pathProfile /= path;
#else
/*MEM leak */
    struct passwd *gUserdir = getpwuid(getuid());
    if (gUserdir)
        pathProfile /= (gUserdir->pw_dir);
    else
        throw std::runtime_error("Could not retrieve the user's home directory!");

#endif
    if (pathProfile.file_string().length() > 0 ){///Creates program profile directory for current user
        pathProfile /= ".littlebenchmark";
        myIO::createDir(pathProfile);
        pathProfile /= ("hdd");
        myIO::createDir(pathProfile);
        pathConfig /= pathProfile.file_string()+"/config.cfg";
        if (myIO::touch(pathConfig) == true){
            string strtmp = "\
#\n\
# Run program with -h or --help options to see which parameters you can set here\n\
# Parameter should look like param = [value]\n\
#"; // file tmp
            myIO::SimpleWriteToFile(pathConfig,strtmp);
        }//if touch
    }//if strRet.lenght
}

void handler_Configuration::parseConfigs(){
    if (! p_strConfigs )
        throw std::runtime_error("p_strConfigs is NULL");
    if (!bLetUpdate)
        return;
    int64_t index = 0;
    myIO::simpleReadToStringByStream(pathConfig,p_strConfigs);
    for (vector<profileNode>::iterator it = p_vec_Nodes->begin();\
        it != p_vec_Nodes->end(); it++
        ){
            if ( it->strVar.length() > 3 and it->strVal.length() > 0 ){
                index = (int64_t)p_strConfigs->find(it->strVar);
                if ( index > 0){/*
                cout<<"------------------------------"<<endl;
                cout<<*p_strConfigs<<endl;
                cout<<"------------------------------"<<endl;
                    cout<<"Replaced:"<<it->getData()<<endl;
                    cout<<"Index:"<<index<<endl ;*/
                    index += (int64_t)it->strVar.length();
                    //cout<<"Index+:"<<index<<"\nend:"<<p_strConfigs->find("\n",index)-index<<endl;
                    p_strConfigs->replace(index,\
                                    ( p_strConfigs->find("\n",index) - index ),\
                                    " = "+it->strVal
                                    );/*
                cout<<"------------------------------"<<endl;
                cout<<*p_strConfigs<<endl;
                cout<<"------------------------------"<<endl;*/
                }else{
                    p_strConfigs->append(it->getData());
                }
            }
    }
}

void handler_Configuration::saveConfigs(){
    if (bLetUpdate)
        myIO::SimpleWriteToFile(pathConfig,*p_strConfigs,std::ios::trunc);
    p_strConfigs->clear();
}
