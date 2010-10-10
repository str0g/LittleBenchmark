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
thread_tester_hdd<classT>::thread_tester_hdd(vector<uint64_t> &vui,
                                     boost::filesystem::path &str,
                                     unsigned &limit,unsigned &mloops, unsigned &uiThNumber, unsigned & reads,
                                     uint16_t &prec,
                                     mode_t &prem,
                                     bool dbg, bool bVerifyCache, bool ssd,
                                     classT *parent): blTT::thread_1<thread_tester_hdd<classT> >(this,NULL,dbg),
                                    pathDir(str),
                                    p_strOutFileDataBuffer(new string),
                                    p_strHashSum(new string),
                                    strSizeTest(""),
                                    uiDataLimit(limit),
                                    ui64Loop(0),
                                    uiMaxLoops(mloops),
                                    uiThreadNumber(uiThNumber),
                                    uiStatsCounter(0),
                                    uiNumberOfReads(reads),
                                    ui8Precision(prec),
                                    uiPermissions(prem),
                                    bWriteFailed(false),
                                    bVerifyCache(bVerifyCache),
                                    bCleanWorkFolder(false),
                                    bSSD(ssd),
                                    vui_Probes(vui),
                                    p_Parent(parent){

    addStats("Thread number:",blC::ToString(uiThreadNumber));
    addStats("Start date:",myTime::GetLocalTime());

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
    cerr<<"Destructor: thread: "<<uiThreadNumber<<endl;
    if (p_strOutFileDataBuffer){
        delete p_strOutFileDataBuffer;
    }
    if (p_strHashSum){
        delete p_strHashSum;
    }
    if (bCleanWorkFolder){
        cerr<<"From some reasons work folder is being cleaned in emergency way"<<endl;
        myIO::rmAll_inDir(pathDir);
        myIO::rm(pathDir);

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

template <class classT> void thread_tester_hdd<classT>::addStats(string desc,string val){
    if ( p_Parent ){
        boost::mutex::scoped_lock lock(mutex_Stats);
            if (p_Parent->findAndAdd(desc,val,uiStatsCounter,uiThreadNumber)== 0)
                ++uiStatsCounter;
    }
}

template <class classT> void thread_tester_hdd<classT>::setNewData(vector<unsigned> &vui,boost::filesystem::path &str){
    vui_Probes = vui;
    pathDir = str;
}

template <class classT> void thread_tester_hdd<classT>::setBuffer(const uint64_t *uiSize){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    boost::local_time::local_date_time *dStartt = NULL;
    uint64_t ui64tmp = 0;
    try{
            ui64tmp = p_strOutFileDataBuffer->length();
        do{
            p_strOutFileDataBuffer->append(*p_strOutFileDataBuffer);
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<< blC::Bandwidth(0, &ui64tmp,"")<<endl;
        }while(p_strOutFileDataBuffer->length() <= *uiSize-1);
        if (p_strOutFileDataBuffer->length() > *uiSize){
            ui64tmp = p_strOutFileDataBuffer->length();
            //cout<<"Buffer overflow resizing["<<ui64tmp<<"("+blC::Bandwidth(0,&ui64tmp,"")+")"<<"/"<<*uiSize<<"("+blC::Bandwidth(0,uiSize,"")+")"<<"]"<<endl;
            p_strOutFileDataBuffer->erase(*uiSize);
        }
    }catch (std::exception &e){
        bFailer = true;
        ui64tmp = p_strOutFileDataBuffer->size();
        cerr <<"Failed to Set work buffer!\n"<<endl;
        cerr << e.what()<<endl;
        cerr << p_strOutFileDataBuffer->size()<<
        "("+blC::Bandwidth(0,ui64tmp,"")<<
        ")length("<< p_strOutFileDataBuffer->length()<<")"<<endl;
        if (bFailer){
            raise(SIGABRT);
        }
    }

    ui64tmp = p_strOutFileDataBuffer->length();
    addStats(strSizeTest+"Testing files of size:",blC::ToString(ui64tmp)+"("+blC::ToString(blC::Bandwidth(0,ui64tmp,""))+")");
    addStats(strSizeTest+"Buffer has been setup in:",blC::TimeToString(myTime::TimeDiff(dStart)));

    dStartt = myTime::GetTime();
    #if COMPILE_WITH_CRYPTOPP == 1
    *p_strHashSum = Hash::SHA512(*p_strOutFileDataBuffer);
    addStats(strSizeTest+"Buffer hashsum has been counted in:",blC::TimeToString(myTime::TimeDiff(dStartt)));
    #endif
    delete dStartt;

    addStats(strSizeTest+"setBuffer executed for:",blC::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}

template <class classT> void thread_tester_hdd<classT>::Read(uint16_t mode,string desc){
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
        path_tmp /= blC::ToString(i);
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
    mode == 0 ? strtmp += "Read by char:" : strtmp += "Read by stream:";
    if(VeryfiReadFiles()){
        double dtmp = myTime::TimeDiff(dStartt);
        delete dStartt;
        addStats(desc+strtmp,blC::ToString(blC::Bandwidth(dtmp,ui64tmp)));
        if (p_list){
            list<double>::iterator it = p_list->begin();
            addStats(desc+strtmp.at(8)+"_Access Times-----","");
            addStats(desc+strtmp.at(8)+"_Min:",blC::TimeToString(*it));            ++it;
            addStats(desc+strtmp.at(8)+"_Max:",blC::TimeToString(*it));            ++it;
            addStats(desc+strtmp.at(8)+"_Avg:",blC::TimeToString(*it/ui64Loop));   ++it;
            addStats(desc+strtmp.at(8)+"_Read Times-----","");
            addStats(desc+strtmp.at(8)+"__Min:",blC::TimeToString(*it));           ++it;
            addStats(desc+strtmp.at(8)+"__Max:",blC::TimeToString(*it));           ++it;
            addStats(desc+strtmp.at(8)+"__Avg:",blC::TimeToString(*it/ui64Loop));
        }

        if((list_ReadFiles.size()%ui64Loop) != 0 or list_ReadFiles.size() == 0){
            cerr<<desc<<"Read wrong number of files! ["<<list_ReadFiles.size()<<"/"<<ui64Loop<<"]"<<endl;
            addStats(desc+strtmp.at(8)+"_Read wrong number of files:",blC::ToString(list_ReadFiles.size())+"/"+blC::ToString(ui64Loop));
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
    addStats(desc+strtmp+" executed for:",blC::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}

template <class classT> void thread_tester_hdd<classT>::Write(const uint64_t *uiSize,string desc){
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
        pathtmp /= blC::ToString(i);
        if (!myIO::SimpleWriteToFile(pathtmp,*p_strOutFileDataBuffer)){
            cerr<<"Failed to create file: "<<i<<endl;
            addStats(desc+" Failed to create file:",blC::ToString(i));
            bWriteFailed = true;
            break;
        }
    }
    double dtmp = myTime::TimeDiff(dStartt);
    addStats(desc+"Written files:",blC::ToString(ui64Loop));
    addStats(desc+"Bandwidth:",blC::Bandwidth(dtmp ,ui64tmp));
    addStats(desc+"Time:",blC::TimeToString( dtmp  ));
    addStats(desc+"Write metod executed for:",blC::TimeToString(myTime::TimeDiff(dStart)));
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
    delete list_dir;

    addStats(strSizeTest+"Copy metod executed for:",blC::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}

template <class classT> bool thread_tester_hdd<classT>::VeryfiReadFiles(){
    if ( !bVerifyCache )
        return true;

    boost::local_time::local_date_time *dStart = myTime::GetTime();
    bool bRet = true;

    #if COMPILE_WITH_CRYPTOPP == 1

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
                    blC::Bandwidth(0,uiLengthOfReadFileBuf,"")<<
                    "("<<uiLengthOfReadFileBuf<<")"<<
                    " has wrong controle sum!\nTest has been aborded!\n" <<endl;
                addStats(strSizeTest+"Fialed to verify file:",blC::ToString(uiCounter));
                addStats(strSizeTest+"Size:",blC::Bandwidth(0,uiLengthOfReadFileBuf,"")+"("+blC::ToString((**it).length())+")");
                addStats(strSizeTest+"Reason:","Bad hash sum");
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
    addStats(strSizeTest+"Files to verify",blC::ToString(list_ReadFiles.size()));
    addStats(strSizeTest+"Veryfing taken:",blC::TimeToString(myTime::TimeDiff(dStart)));
    #endif
    delete dStart;
    return bRet;
}

template <class classT> void thread_tester_hdd<classT>::Execute(){
    boost::local_time::local_date_time *dStart = myTime::GetTime();
    //
    pathDir /= blC::ToString(blTT::thread_1<thread_tester_hdd<classT> >::GetThreadID());
#if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
    if(boost::filesystem::create_directory(pathDir)){
#else
    if(mkdir(pathDir.file_string().c_str(),uiPermissions) == 0){
#endif
        bCleanWorkFolder = true;
        for(vector<uint64_t>::iterator it = vui_Probes.begin();
            it != vui_Probes.end(); ++it){
                unsigned uiI = 0;
                strSizeTest = "("+blC::Bandwidth(0,*it,"")+")";
                Write(&*it,strSizeTest);///Write test
                if ( !bWriteFailed){///Both read test
                    do{
                        Read(0,strSizeTest);
                        Read(1,strSizeTest);
                    }while(++uiI < uiNumberOfReads);
                }
                uiI = 0;
                myIO::rmAll_inDir(pathDir);///Cleaning after rw tests
                if (bSSD){///hybrird drive test which is normal test but with reversed order
                    Write(&*it,strSizeTest+"[rOrder]");///Write test
                    if ( !bWriteFailed){///Both read test
                        do{
                            Read(1,strSizeTest+"[rOrder]");
                            Read(0,strSizeTest+"[rOrder]");
                        }while(++uiI < uiNumberOfReads);
                    }
                    myIO::rmAll_inDir(pathDir);///Cleaning after rw tests
                }
                Write(&*it,strSizeTest+"[copy]");///Write test
                CopyTest();
                myIO::rmAll_inDir(pathDir);///Cleaning after rw tests
        }
        myIO::rm(pathDir);

        bCleanWorkFolder = false;
        delete p_strOutFileDataBuffer;
        p_strOutFileDataBuffer = NULL;
    }else{
        cerr<<"Failed to create work directory! ["+pathDir.file_string()+"]\n Nothing will be done."<<endl;
        addStats("Failed to create work directory!:",pathDir.file_string());
    }
    //
    addStats("Thread execution taken:",blC::TimeToString(myTime::TimeDiff(dStart)));
    delete dStart;
}
