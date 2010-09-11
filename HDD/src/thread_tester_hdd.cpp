/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "thread_tester_hdd.hpp"

thread_tester_hdd::thread_tester_hdd(vector<unsigned int> vui,string str,unsigned int limit,unsigned int prem):
                                    vui_Probes(vui),
                                    strDir(str),
                                    p_strSummary(new string),
                                    p_strHashSum(new string),
                                    strSlash("/"),
                                    uiDataLimit(limit),
                                    uiPermissions(prem),
                                    bWriteFailed(false),
                                    p_strOutFileDataBuffer(new string){
    #ifdef WIN64 || _WIN64 || __WIN64__ || WIN32 || _WIN32 || __WIN32__ || _TOS_WIN__ || __WINDOWS__
    strSlash = "\\";
    #endif
    p_strHashSum->reserve(SHA512LEN);

    p_strSummary->append(myTime::GetLocalTime()+"\n");
    p_strSummary->append(strDir+"\n\n");

    p_strOutFileDataBuffer->reserve(128);
    char *p_cbuf = new char[126];
    for(int i = 0; i < 125; ++i){
        p_cbuf[i] = (char)i;
    }
    p_strOutFileDataBuffer->append(p_cbuf,125);
    p_strOutFileDataBuffer->append("\n");
    delete[] p_cbuf;
}
thread_tester_hdd::~thread_tester_hdd(){
    if (p_strSummary){
        delete p_strSummary;
    }
    if (p_strOutFileDataBuffer){
        delete p_strOutFileDataBuffer;
    }
    if (p_strHashSum){
        delete p_strHashSum;
    }
}
void thread_tester_hdd::setNewData(vector<unsigned int> vui,string str){
    vui_Probes = vui;
    strDir = str;
}

void thread_tester_hdd::setBuffer(const unsigned int *uiSize){
    double dStart = myTime::GetTime();
    uint64_t ui64tmp = 0;
    try{
            ui64tmp = p_strOutFileDataBuffer->length();
            cout<< myTime::Bandwidth(1, &ui64tmp,"")<<endl;
        do{
            p_strOutFileDataBuffer->append(*p_strOutFileDataBuffer);
            ui64tmp = p_strOutFileDataBuffer->length();
            cout<< myTime::Bandwidth(1, &ui64tmp,"")<<endl;
        }while(p_strOutFileDataBuffer->length() <= *uiSize-1);
        if (p_strOutFileDataBuffer->length() > *uiSize){
            ui64tmp = p_strOutFileDataBuffer->length();
            cout<<"Buffer overflow resizing["<<ui64tmp<<"("+myTime::Bandwidth(1,&ui64tmp,"")+")"<<"/"<<*uiSize<<"("+myTime::Bandwidth(1,uiSize,"")+")"<<"]"<<endl;
            p_strOutFileDataBuffer->erase(*uiSize);
        }
    }catch (std::exception &e){
        cerr << e.what()<<endl;
        ui64tmp = p_strOutFileDataBuffer->size();
        cerr << p_strOutFileDataBuffer->size()<<"("+myTime::Bandwidth(1,&ui64tmp,"")+")" <<endl;
    }
    p_strSummary->append("Buffer was set up in "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStart))+"s\n");
    double dStartt = myTime::GetTime();
    *p_strHashSum = Hash::SHA512(p_strOutFileDataBuffer);

    p_strSummary->append("Hashsum of buffer has been counted in "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStartt))+"s\n");
    ui64tmp = p_strOutFileDataBuffer->length();
    p_strSummary->append("\nTesting files of size: "+ myConv::ToString(p_strOutFileDataBuffer->length())+"("+myConv::ToString(myTime::Bandwidth(1,&ui64tmp,""))+")"+"\n");
    p_strSummary->append("setBuffer end in "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStart))+"s\n");
}

string &thread_tester_hdd::getSummary(){
    return *p_strSummary;
}

void thread_tester_hdd::start(){
    m_Thread = boost::thread (&thread_tester_hdd::Execute,this);
}
void thread_tester_hdd::join(){
    m_Thread.join();
}

void thread_tester_hdd::readByChar(uint16_t mode){
    double dStart = myTime::GetTime();
    double dStartt = myTime::GetTime();
    uint64_t ui64tmp = 0;
    string strtmp;
    bool bOK = false;
    for(uint64_t i = 0; i < ui64Loop; ++i){
        string *p_strtmp = new string;
        if (mode == 0){
            bOK = myIO::simpleReadToStringByChar(&(strDir+strSlash+myConv::ToString(i)),p_strtmp);
        }else{
            bOK = myIO::simpleReadToStringByStream(&(strDir+strSlash+myConv::ToString(i)),p_strtmp);
        }
        if (p_strtmp and bOK){
            list_ReadFiles.push_back(p_strtmp);
            ui64tmp += p_strtmp->length();
        }
    }
    if(VeryfiReadFiles()){
        dStartt = myTime::TimeDiff(myTime::GetTime(),dStartt);
        mode == 0 ? strtmp = "Read by char" : strtmp = "Read by stream";
        p_strSummary->append(strtmp+": "+myConv::ToString(myTime::Bandwidth(dStartt,&ui64tmp))+"\n");

        if((list_ReadFiles.size()%ui64Loop) != 0 or list_ReadFiles.size() == 0){
            cerr<<"Read wrong number of files! ["<<list_ReadFiles.size()<<"/"<<ui64Loop<<"]"<<endl;
            p_strSummary->append("Read wrong number of files! ["\
                             +myConv::ToString(list_ReadFiles.size()) \
                             +"/"+myConv::ToString(ui64Loop)+"]");
        }
    }
    string *p_strtmp = NULL;
    for(list<string*>::iterator it = list_ReadFiles.begin();
        it != list_ReadFiles.end();
        ++it){
            delete &(**it);
            *it = NULL;
        }
    list_ReadFiles.clear();
    p_strSummary->append(strtmp+" "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStart))+"\n\n");
}

void thread_tester_hdd::Write(const unsigned int *uiSize){
    double dStart = myTime::GetTime();
    setBuffer(uiSize);
    uint64_t ui64tmp = p_strOutFileDataBuffer->length();
    ui64Loop = (uint64_t)(uiDataLimit / *uiSize);
    //list_ReadFiles.resize(ui64Loop);
    //Write only
    double dStartt = myTime::GetTime();
    //
    for(unsigned int i = 0; i < ui64Loop; ++i){
        if (!myIO::SimpleWriteToFile(&(strDir+strSlash+myConv::ToString(i)),p_strOutFileDataBuffer)){
            cerr<<"Failed to create file: "<<i<<endl;
            p_strSummary->append("Failed to create file: "+myConv::ToString(i)+"\n");
            bWriteFailed = true;
            break;
        }
    }
    dStartt = myTime::TimeDiff(myTime::GetTime(),dStartt);
    p_strSummary->append("Written "+myConv::ToString(ui64Loop)+" files"+"\nBandwidth: "\
                        +myTime::Bandwidth(dStartt,&ui64tmp)+" ("\
                        +myConv::ToString( dStartt )+"s)\n"
                         );
    //
    p_strSummary->append("Write metod end after "+myConv::ToString( myTime::TimeDiff(myTime::GetTime(),dStart))+"s\n\n");
}

bool thread_tester_hdd::VeryfiReadFiles(){
    double dStart = myTime::GetTime();
    bool bRet = true;
    for(list<string*>::iterator it = list_ReadFiles.begin();
        it != list_ReadFiles.end();
        ++it){
            if (*p_strHashSum != Hash::SHA512(&(**it))){
                cerr<< "File has wrong controle sum!\nTest has been aborded!\n" <<endl;
                p_strSummary->append("File have wrong controle sum!\nTest has been aborded!\n");
                bRet = false;
                break;
            }
    }
    p_strSummary->append("Verfication "+ myConv::ToString(list_ReadFiles.size())+" taken "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStart))+"s\n");
    return bRet;
}

void thread_tester_hdd::Execute(){
    double dStart = myTime::GetTime();
    //
    strDir += myConv::ToString(m_Thread.get_id());
    if(mkdir(strDir.c_str(),uiPermissions) == 0){
        for(vector<unsigned int>::iterator it = vui_Probes.begin();
            it != vui_Probes.end(); ++it){
                Write(&*it);
                if ( !bWriteFailed){
                    readByChar(0);
                    readByChar(1);
                }
                myIO::rmAll(strDir);
        }
        rmdir(strDir.c_str());
    }else{
        cerr<<"Failed to create work directory!\n Nothing will be done."<<endl;
        p_strSummary->append("Failed to create work directory!\n Nothing will be done.\n");
    }
    //
    p_strSummary->append("Thread execution taken: "+myConv::ToString(myTime::TimeDiff(myTime::GetTime(),dStart))+"s\n");
}
