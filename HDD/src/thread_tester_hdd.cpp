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

thread_tester_hdd::thread_tester_hdd(vector<unsigned int> vui,string str,string slash,unsigned int limit,unsigned int mloops, uint8_t prec,mode_t prem):
                                    vui_Probes(vui),
                                    strDir(str),//swap me to bfsp
                                    p_strSummary(new string),
                                    p_strHashSum(new string),
                                    strSlash(slash),//remove me later
                                    uiDataLimit(limit),
                                    ui64Loop(0),
                                    uiMaxLoops(mloops),
                                    ui8Precision(prec),
                                    uiPermissions(prem),
                                    bWriteFailed(false),
                                    p_strOutFileDataBuffer(new string){
    p_strHashSum->reserve(SHA512LEN);

    p_strSummary->append(myTime::GetLocalTime()+"\n");
    p_strSummary->append(strDir+"\n\n");

    p_strOutFileDataBuffer->reserve(ASCII);
    char *p_cbuf = new char[ASCII];
    for(int i = 0; i < (ASCII-1); ++i){
        p_cbuf[i] = (char)i;
    }
    p_strOutFileDataBuffer->append(p_cbuf,(ASCII-1));
    p_strOutFileDataBuffer->append("\n");
    delete[] p_cbuf;

    cout.setf(std::ios::adjustfield,std::ios::floatfield);
    cout.precision(ui8Precision);

    //
   if( p_strOutFileDataBuffer->length() != ASCII ){
        cerr<<myColors::ConsoleColors_[2]<<"Unexpected length of p_strOutFileDataBuffer:"<<p_strOutFileDataBuffer->length()<<myColors::ConsoleColors_[0]<<endl;
        exit(EXIT_FAILURE);
    }
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
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    boost::local_time::local_date_time *dStartt = NULL;
    uint64_t ui64tmp = 0;
    try{
            ui64tmp = p_strOutFileDataBuffer->length();
        do{
            p_strOutFileDataBuffer->append(*p_strOutFileDataBuffer);
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<< myTime::Bandwidth(0, &ui64tmp,"")<<endl;
        }while(p_strOutFileDataBuffer->length() <= *uiSize-1);
        if (p_strOutFileDataBuffer->length() > *uiSize){
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<<"Buffer overflow resizing["<<ui64tmp<<"("+myTime::Bandwidth(0,&ui64tmp,"")+")"<<"/"<<*uiSize<<"("+myTime::Bandwidth(0,uiSize,"")+")"<<"]"<<endl;
            p_strOutFileDataBuffer->erase(*uiSize);
        }
    }catch (std::exception &e){
        cerr << e.what()<<endl;
        ui64tmp = p_strOutFileDataBuffer->size();
        cerr << p_strOutFileDataBuffer->size()<<"("+myTime::Bandwidth(0,ui64tmp)<<")length("<< p_strOutFileDataBuffer->length()<<")" <<endl;
    }

    ui64tmp = p_strOutFileDataBuffer->length();
    p_strSummary->append("Testing files of size: "+ myConv::ToString(ui64tmp)+"("+myConv::ToString(myTime::Bandwidth(0,ui64tmp,""))+")"+"\n");
    p_strSummary->append("Buffer has been setup in "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n");

    dStartt = myTime::GetTime();
    *p_strHashSum = Hash::SHA512(*p_strOutFileDataBuffer);
    p_strSummary->append("Buffer hashsum has been counted in "+myTime::TimeScale(myTime::TimeDiff(dStartt))+"\n");
    delete dStartt;

    p_strSummary->append("setBuffer executed for "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n\n");
    delete dStart;
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
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    boost::local_time::local_date_time *dStartt = NULL;
    uint64_t ui64tmp = 0;
    string strtmp;
    bool bOK = false;
    list<double> *p_list = new list<double>;
    if(p_list){
        p_list->push_back(m_NS);
        p_list->push_back(0);
        p_list->push_back(0);
        p_list->push_back(m_NS);
        p_list->push_back(0);
        p_list->push_back(0);
    }

    dStartt = myTime::GetTime();
    for(uint64_t i = 0; i < ui64Loop; ++i){//Porting issue
        string *p_strtmp = new string;
        if (mode == 0){
            bOK = myIO::simpleReadToStringByChar(&(strDir+strSlash+myConv::ToString(i)),p_strtmp,p_list);
        }else{
            bOK = myIO::simpleReadToStringByStream((strDir+strSlash+myConv::ToString(i)),p_strtmp,p_list);
        }
        if (p_strtmp and bOK){//---------------------------
            list_ReadFiles.push_back(p_strtmp);
            ui64tmp += p_strtmp->length();
        }
    }
    if(VeryfiReadFiles()){
        double dtmp = myTime::TimeDiff(dStartt);
        delete dStartt;
        mode == 0 ? strtmp = "Read by char" : strtmp = "Read by stream";
        p_strSummary->append(strtmp+": "+myConv::ToString(myTime::Bandwidth(dtmp,ui64tmp))+"\n");
        if (p_list){
            list<double>::iterator it = p_list->begin();
            p_strSummary->append("Access Times-----\nMin:"+myTime::TimeScale(*it));     ++it;
            p_strSummary->append("\nMax:"+myTime::TimeScale(*it));                      ++it;
            p_strSummary->append("\nAvg:"+myTime::TimeScale(*it/ui64Loop));             ++it;
            p_strSummary->append("\nRead Times--------\nMin:"+myTime::TimeScale(*it));  ++it;
            p_strSummary->append("\nMax:"+myTime::TimeScale(*it));                      ++it;
            p_strSummary->append("\nAvg:"+myTime::TimeScale(*it/ui64Loop)+"\n");
        }

        if((list_ReadFiles.size()%ui64Loop) != 0 or list_ReadFiles.size() == 0){
            cerr<<"Read wrong number of files! ["<<list_ReadFiles.size()<<"/"<<ui64Loop<<"]"<<endl;
            p_strSummary->append("Read wrong number of files! ["\
                             +myConv::ToString(list_ReadFiles.size()) \
                             +"/"+myConv::ToString(ui64Loop)+"]");
        }
    }

    for(list<string*>::iterator it = list_ReadFiles.begin();
        it != list_ReadFiles.end();
        ++it){
            delete &(**it);
            *it = NULL;
        }
    list_ReadFiles.clear();
    if (p_list){ delete p_list; }
    p_strSummary->append(strtmp+" executed for "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n\n");
    delete dStart;
}

void thread_tester_hdd::Write(const unsigned int *uiSize){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    setBuffer(uiSize);
    uint64_t ui64tmp = p_strOutFileDataBuffer->length();
    ui64Loop = (uint64_t)(uiDataLimit / *uiSize);
    ui64Loop > uiMaxLoops ? ui64Loop = (uint64_t)uiMaxLoops : ui64Loop;
    //list_ReadFiles.resize(ui64Loop);
    //Write only
    boost::local_time::local_date_time *dStartt = myTime::GetTime();
    //
    for(unsigned int i = 0; i < ui64Loop; ++i){
        if (!myIO::SimpleWriteToFile((strDir+strSlash+myConv::ToString(i)),*p_strOutFileDataBuffer)){
            cerr<<"Failed to create file: "<<i<<endl;
            p_strSummary->append("Failed to create file: "+myConv::ToString(i)+"\n");
            bWriteFailed = true;
            break;
        }
    }
    double dtmp = myTime::TimeDiff(dStartt);
    p_strSummary->append("Written "+myConv::ToString(ui64Loop)+" files"+"\nBandwidth: "\
                        +myTime::Bandwidth(dtmp ,ui64tmp)+" ("\
                        +myTime::TimeScale( dtmp  )+")\n"\
                         );
    //
    p_strSummary->append("Write metod executed for "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n\n");
    delete dStart;
    delete dStartt;
}

void thread_tester_hdd::CopyTest(const unsigned int *uiData){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    list<boost::filesystem::path> *list_dir = new list<boost::filesystem::path>;
    boost::filesystem::path fTo;
    Write(uiData);
    myIO::lsFiles(&boost::filesystem::path(strDir),list_dir);
    for ( list<boost::filesystem::path>::iterator it = list_dir->begin();\
            it != list_dir->end();
            ++it ){
                fTo = *it;
                fTo = fTo.file_string()+"_cpy";
        myIO::CopyFileByChar(*it, fTo);
        }
    myIO::rmAll(strDir);
    delete list_dir;
    p_strSummary->append("Copy metod executed for "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n\n");
    delete dStart;
}

bool thread_tester_hdd::VeryfiReadFiles(){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    bool bRet = true;
    for(list<string*>::iterator it = list_ReadFiles.begin();
        it != list_ReadFiles.end();
        it++){
            cout<<"["<<*p_strHashSum<<"] / ["<< Hash::SHA512((**it))<<"]\n"<<endl;
            if (*p_strHashSum != Hash::SHA512((**it))){
                cerr<< "File of size "<< myTime::Bandwidth(0,(**it).length(),"")<<"("<<(**it).length()<<")"<<" has wrong controle sum!\nTest has been aborded!\n" <<endl;
                p_strSummary->append("File of size "+myTime::Bandwidth(0,(**it).length(),"")+"("+myConv::ToString((**it).length())+")"+" have wrong controle sum!\nTest has been aborded!\n");
                bRet = false;
                break;
            }
    }
    p_strSummary->append("Verfication "+ myConv::ToString(list_ReadFiles.size())+" taken "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n");
    delete dStart;
    return bRet;
}

void thread_tester_hdd::Execute(){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    //
    strDir += myConv::ToString(m_Thread.get_id());
    if(mkdir(strDir.c_str(),uiPermissions) == 0){
        for(vector<unsigned int>::iterator it = vui_Probes.begin();
            it != vui_Probes.end(); ++it){
                Write(&*it);///Write test
                if ( !bWriteFailed){///Both read test
                    readByChar(0);
                    readByChar(1);
                }
                myIO::rmAll(strDir);///Cleaning after rw tests
                CopyTest(&*it);
        }
        rmdir(strDir.c_str());
    }else{
        cerr<<"Failed to create work directory!\n Nothing will be done."<<endl;
        p_strSummary->append("Failed to create work directory!\n Nothing will be done.\n");
    }
    //
    p_strSummary->append("Thread execution taken: "+myTime::TimeScale(myTime::TimeDiff(dStart))+"\n");
    delete dStart;
}
