/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
//Headers
#include "thread_tester_hdd.hpp"
template <class classT>
bool thread_tester_hdd<classT>::bFailer = false;
template <class classT>
thread_tester_hdd<classT>::thread_tester_hdd(vector<unsigned> &vui,
                                     boost::filesystem::path &str,
                                     unsigned &limit,unsigned &mloops, unsigned &uiNumber,
                                     uint8_t &prec,
                                     mode_t &prem,
                                     bool dbg, bool bVerifyCache,
                                     classT *parent): myThreadTemplates::thread_1<thread_tester_hdd<classT> >(this,NULL,dbg),
                                    vui_Probes(vui),
                                    pathDir(str),
                                    p_strOutFileDataBuffer(new string),
                                    p_strHashSum(new string),
                                    uiDataLimit(limit),
                                    ui64Loop(0),
                                    uiMaxLoops(mloops),
                                    uiThreadNumber(uiNumber),
                                    uiStatsCounter(0),
                                    ui8Precision(prec),
                                    uiPermissions(prem),
                                    bWriteFailed(false),
                                    bVerifyCache(bVerifyCache),
                                    bCleanWorkFolder(false),
                                    p_Parent(parent){

    addStats(&string("Thread number:"),&myConv::ToString(uiThreadNumber));
    addStats(&string("Start date:"),&myTime::GetLocalTime());

    p_strHashSum->reserve(SHA512LEN);

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
        cerr<<myColors::ConsoleColors_[2]<<"Unexpected length of p_strOutFileDataBuffer:"<<p_strOutFileDataBuffer->length()<<myColors::ConsoleColors_[*myColors::TextColor]<<endl;
        exit(EXIT_FAILURE);
    }
}
template <class classT>thread_tester_hdd<classT>::~thread_tester_hdd(){
    cerr<<"Disctructing: thread: "<<uiThreadNumber<<endl;
    if (p_strOutFileDataBuffer){
        delete p_strOutFileDataBuffer;
    }
    if (p_strHashSum){
        delete p_strHashSum;
    }
    if (bCleanWorkFolder){
        cerr<<"From some reasons work folder is being cleaned in emergency way"<<endl;
        myIO::rmAll(pathDir);
        rmdir(pathDir.file_string().c_str());

        cerr<<"From some reasons cache is being cleared in emergency way"<<endl;
        for(list<string*>::iterator it = list_ReadFiles.begin();
            it != list_ReadFiles.end();
            ++it){
                delete &(**it);
                *it = NULL;
            }
        list_ReadFiles.clear();
    }
}

template <class classT> void thread_tester_hdd<classT>::addStats(string *desc,string *val){
    if ( p_Parent ){
        ++uiStatsCounter;
        boost::mutex::scoped_lock lock(mutex_Stats);
        p_Parent->addStatData(desc,val,uiStatsCounter,uiThreadNumber);
    }
}

template <class classT> void thread_tester_hdd<classT>::setNewData(vector<unsigned> &vui,boost::filesystem::path &str){
    vui_Probes = vui;
    pathDir = str;
}

template <class classT> void thread_tester_hdd<classT>::setBuffer(const unsigned *uiSize){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    boost::local_time::local_date_time *dStartt = NULL;
    uint64_t ui64tmp = 0;
    try{
            ui64tmp = p_strOutFileDataBuffer->length();
        do{
            p_strOutFileDataBuffer->append(*p_strOutFileDataBuffer);
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<< myConv::Bandwidth(0, &ui64tmp,"")<<endl;
        }while(p_strOutFileDataBuffer->length() <= *uiSize-1);
        if (p_strOutFileDataBuffer->length() > *uiSize){
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<<"Buffer overflow resizing["<<ui64tmp<<"("+myConv::Bandwidth(0,&ui64tmp,"")+")"<<"/"<<*uiSize<<"("+myConv::Bandwidth(0,uiSize,"")+")"<<"]"<<endl;
            p_strOutFileDataBuffer->erase(*uiSize);
        }
    }catch (std::exception &e){
        bFailer = true;
        ui64tmp = p_strOutFileDataBuffer->size();
        cerr <<"Failed to Set work buffer!\n"<<endl;
        cerr << e.what()<<endl;
        cerr << p_strOutFileDataBuffer->size()<<
        "("+myConv::Bandwidth(0,ui64tmp,"")<<
        ")length("<< p_strOutFileDataBuffer->length()<<")"<<endl;
        if (bFailer){
            raise(SIGABRT);
        }
    }

    ui64tmp = p_strOutFileDataBuffer->length();
    addStats(&string("Testing files of size:"),&string(myConv::ToString(ui64tmp)+"("+myConv::ToString(myConv::Bandwidth(0,ui64tmp,""))+")"));
    addStats(&string("Buffer has been setup in:"),&string(myConv::TimeToString(myTime::TimeDiff(dStart))));

    dStartt = myTime::GetTime();
    *p_strHashSum = Hash::SHA512(*p_strOutFileDataBuffer);
    addStats(&string("Buffer hashsum has been counted in:"),&myConv::TimeToString(myTime::TimeDiff(dStartt)));
    delete dStartt;

    addStats(&string("setBuffer executed for:"),&myConv::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}

template <class classT> void thread_tester_hdd<classT>::readByChar(uint16_t mode){
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
    for(uint64_t i = 0; i < ui64Loop; ++i){
        string *p_strtmp = new string;
        boost::filesystem::path path_tmp = pathDir;
        path_tmp /= myConv::ToString(i);
        if (mode == 0){
            bOK = myIO::simpleReadToStringByChar(path_tmp,p_strtmp,p_list);
        }else{
            bOK = myIO::simpleReadToStringByStream(path_tmp,p_strtmp,p_list);
        }
        if (p_strtmp and bOK){//---------------------------
            if ( bVerifyCache )
                list_ReadFiles.push_back(p_strtmp);
            else
                delete p_strtmp;
            ui64tmp += p_strtmp->length();
        }
    }
    if(VeryfiReadFiles()){
        double dtmp = myTime::TimeDiff(dStartt);
        delete dStartt;
        mode == 0 ? strtmp = "Read by char:" : strtmp = "Read by stream:";
        addStats(&strtmp,&myConv::ToString(myConv::Bandwidth(dtmp,ui64tmp)));
        if (p_list){
            list<double>::iterator it = p_list->begin();
            addStats(&string("Access Times-----"),&string(""));
            addStats(&string("Min:"),&myConv::TimeToString(*it));            ++it;
            addStats(&string("Max:"),&myConv::TimeToString(*it));            ++it;
            addStats(&string("Avg:"),&myConv::TimeToString(*it/ui64Loop));   ++it;
            addStats(&string("Read Times-----"),&string(""));
            addStats(&string("Min:"),&myConv::TimeToString(*it));            ++it;
            addStats(&string("Max:"),&myConv::TimeToString(*it));            ++it;
            addStats(&string("Avg:"),&myConv::TimeToString(*it/ui64Loop));
        }

        if((list_ReadFiles.size()%ui64Loop) != 0 or list_ReadFiles.size() == 0){
            cerr<<strtmp<<"Read wrong number of files! ["<<list_ReadFiles.size()<<"/"<<ui64Loop<<"]"<<endl;
            addStats(&string("Read wrong number of files:"),&string(myConv::ToString(list_ReadFiles.size())+"/"+myConv::ToString(ui64Loop)));
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
    addStats(&string(strtmp+" executed for:"),&string(myConv::TimeToString(myTime::TimeDiff(dStart))));
    delete dStart;
}

template <class classT> void thread_tester_hdd<classT>::Write(const unsigned *uiSize){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    setBuffer(uiSize);
    uint64_t ui64tmp = p_strOutFileDataBuffer->length();
    ui64Loop = (uint64_t)(uiDataLimit / *uiSize);
    ui64Loop > uiMaxLoops ? ui64Loop = (uint64_t)uiMaxLoops : ui64Loop;
    //list_ReadFiles.resize(ui64Loop);
    //Write only
    boost::local_time::local_date_time *dStartt = myTime::GetTime();
    //
    for(unsigned i = 0; i < ui64Loop; ++i){
        boost::filesystem::path pathtmp = pathDir;
        pathtmp /= myConv::ToString(i);
        if (!myIO::SimpleWriteToFile(pathtmp,*p_strOutFileDataBuffer)){
            cerr<<"Failed to create file: "<<i<<endl;
            addStats(&string("Failed to create file:"),&myConv::ToString(i));
            bWriteFailed = true;
            break;
        }
    }
    double dtmp = myTime::TimeDiff(dStartt);
    addStats(&string("Written files:"),&myConv::ToString(ui64Loop));
    addStats(&string("Bandwidth:"),&myConv::Bandwidth(dtmp ,ui64tmp));
    addStats(&string("Time:"),&myConv::TimeToString( dtmp  ));
    addStats(&string("Write metod executed for:"),&myConv::TimeToString(myTime::TimeDiff(dStart)));
    //
    delete dStart;
    delete dStartt;
}

template <class classT> void thread_tester_hdd<classT>::CopyTest(){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    list<boost::filesystem::path> *list_dir = new list<boost::filesystem::path>;
    boost::filesystem::path fTo;

    myIO::lsFiles(pathDir,list_dir);
    for ( list<boost::filesystem::path>::iterator it = list_dir->begin();\
            it != list_dir->end();
            ++it ){
                fTo = *it;
                fTo = fTo.file_string()+"_cpy";
        myIO::CopyFileByChar(*it, fTo);
        }
    myIO::rmAll(pathDir);
    delete list_dir;

    addStats(&string("Copy metod executed for:"),&myConv::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}

template <class classT> bool thread_tester_hdd<classT>::VeryfiReadFiles(){
    if ( !bVerifyCache )
        return true;

    boost::local_time::local_date_time *dStart = myTime::GetTime();
    bool bRet = true;
    unsigned uiCounter = 0;
    for(list<string*>::iterator it = list_ReadFiles.begin();
        it != list_ReadFiles.end();
        it++){
            if (*p_strHashSum != Hash::SHA512((**it))){
                unsigned uiLengthOfReadFileBuf = (unsigned)(**it).length();
                unsigned uiLengthOfDataBuf = (unsigned)p_strOutFileDataBuffer->length();
                cerr<<"expected hash["<<myColors::ConsoleColors_[1]<<*p_strHashSum<<myColors::ConsoleColors_[*myColors::TextColor]<<
                "] /hash counted ["<<myColors::ConsoleColors_[2]<< Hash::SHA512((**it))<<myColors::ConsoleColors_[*myColors::TextColor]<<"]"<<endl;
                cerr<< "File ["<< uiCounter<<"] Size "<<
                    myConv::Bandwidth(0,uiLengthOfReadFileBuf,"")<<
                    "("<<uiLengthOfReadFileBuf<<")"<<
                    " has wrong controle sum!\nTest has been aborded!\n" <<endl;
                addStats(&string("Fialed to verify file:"),&myConv::ToString(uiCounter));
                addStats(&string("Size:"),&string(myConv::Bandwidth(0,uiLengthOfReadFileBuf,"")+"("+myConv::ToString((**it).length())+")"));
                addStats(&string("Reason:"),&string("Bad hash sum"));
                if (uiLengthOfDataBuf!= uiLengthOfReadFileBuf){
                    cerr<<"File size expected to be equal["<<
                    myColors::ConsoleColors_[1]<<uiLengthOfDataBuf<<myColors::ConsoleColors_[*myColors::TextColor]<<"]/["<<
                    myColors::ConsoleColors_[2]<<uiLengthOfDataBuf<<myColors::ConsoleColors_[*myColors::TextColor]<<"]"<<endl;
                }
                unsigned *uiLimit;
                uiLengthOfDataBuf < uiLengthOfReadFileBuf ? uiLimit = &uiLengthOfReadFileBuf : uiLimit = &uiLengthOfDataBuf;
                for(unsigned i=0; i < *uiLimit; i++){
                    if (i < uiLengthOfDataBuf ){
                        if (p_strOutFileDataBuffer->at(i) != (**it).at(i))
                            cerr<<"["<<myColors::ConsoleColors_[1]<<p_strOutFileDataBuffer->at(i)<<myColors::ConsoleColors_[*myColors::TextColor]<<"]"
                            <<"["<<myColors::ConsoleColors_[2]<<(**it)<<myColors::ConsoleColors_[*myColors::TextColor]<<"]"<<endl;
                    }else{
                        cerr<<" ["<<myColors::ConsoleColors_[1]<<(**it).at(i)<<myColors::ConsoleColors_[*myColors::TextColor]<<"] ";
                        if (i % 5 == 0 and i > 0)
                            cerr<<endl;
                    }
                }
                cerr<<endl;

                bRet = false;
                break;
            }
            uiCounter++;
    }
    addStats(&string("Files to verify"),&myConv::ToString(list_ReadFiles.size()));
    addStats(&string("Veryfing taken:"),&myConv::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
    return bRet;
}

template <class classT> void thread_tester_hdd<classT>::Execute(){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    //
    pathDir /= myConv::ToString(myThreadTemplates::thread_1<thread_tester_hdd<classT> >::GetThreadID());
    #ifdef __WIN32__ || __WIN64__
    if(mkdir(pathDir.file_string().c_str()) == 0){
    #else
    if(mkdir(pathDir.file_string().c_str(),uiPermissions) == 0){
    #endif
        bCleanWorkFolder = true;
        for(vector<unsigned>::iterator it = vui_Probes.begin();
            it != vui_Probes.end(); ++it){
                Write(&*it);///Write test
                if ( !bWriteFailed){///Both read test
                    readByChar(0);
                    readByChar(1);
                }
                myIO::rmAll(pathDir);///Cleaning after rw tests
                if (false){//hybrid drive
                    Write(&*it);///Write test
                    if ( !bWriteFailed){///Both read test
                        readByChar(1);
                        readByChar(0);
                    }
                    myIO::rmAll(pathDir);///Cleaning after rw tests
                }
                Write(&*it);///Write test
                CopyTest();
                myIO::rmAll(pathDir);///Cleaning after rw tests
        }
        rmdir(pathDir.file_string().c_str());
        bCleanWorkFolder = false;
        delete p_strOutFileDataBuffer;
        p_strOutFileDataBuffer = NULL;
    }else{
        cerr<<"Failed to create work directory! ["+pathDir.file_string()+"]\n Nothing will be done."<<endl;
        addStats(&string("Failed to create work directory!:"),&string(pathDir.file_string()));
    }
    //
    addStats(&string("Thread execution taken:"),&myConv::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}
