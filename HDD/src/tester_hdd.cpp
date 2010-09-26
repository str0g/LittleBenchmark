/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public Licens
 **************************************************************/
 /**
 * Important notes for Linux:
 * * In file /etc/nsswitch.conf change passwd compat to passwd file in order to prevent memory lick
 */

///Headers
#include "tester_hdd.hpp"

tester_hdd::tester_hdd(int ac,char **av):
                                        strSummaryFile("Summary.txt"),
                                        strDataLimit("300M"),
                                        MaxCharSize("511M"),
                                        MaxStringSize("511M"),
                                        strPath(""),
                                        pathProfile(""),
                                        pathConfig(""),
                                        pathSummary(""),
                                        bThreadingPerDir(false),
                                        bDebugging(false),
                                        iTextColor(7),
                                        uiThreadsPerDir(1),
                                        uiSelfTestScenerio(15),
                                        uiDataLimit(300 * MB),
                                        uiMaxLoops(500),
                                        ui64MaxCharSize(500*MB),
                                        ui64MaxStringSize(500*MB),
                                        ui8Precision(10),
                                        uiPermissions(0777),
                                        pus_Var(new vector<profileUpdateStore*> ),
                                        bSelfTest(false),
                                        bBufferingTest(false),
                                        bRun(true)
                                        {

    #ifdef WIN64 || _WIN64 || __WIN64__ || WIN32 || _WIN32 || __WIN32__ || _TOS_WIN__ || __WINDOWS__
    strSlash = "\\";
    #endif
    //cout<<myColors::ConsoleColors_[iTextColor]<<endl;//Some consoles/terminals need it to be set at the very begining
    try{
        boost::program_options::options_description desc("Options");
        desc.add_options()
        ("help,h", "Print usage message and quit")
        ("Author,a","Print info about Author and quit")
        ("data-limit,l",boost::program_options::value<string>(&strDataLimit)->default_value(strDataLimit),"small data read/write limit")
        ("dir-thread,t", boost::program_options::value<bool>(&bThreadingPerDir)->default_value(bThreadingPerDir), "Run thread for every directory.\nUseful for bandwidth test of controllers or NCQ.")
        ("max-loops,m",boost::program_options::value<unsigned int>(&uiMaxLoops)->default_value(uiMaxLoops),"Limit number of loops per read/write files")
        ("output-file,o",boost::program_options::value<string>(&strSummaryFile)->default_value(strSummaryFile),"Output file")
        ("precision,s",boost::program_options::value<uint8_t>(&ui8Precision)->default_value(ui8Precision),"Set time precision")
        ("probe-size,x",boost::program_options::value<vector<string> >()->composing(),"Set size of probes")
        ("force-text-color",boost::program_options::value<int>(&iTextColor)->default_value(iTextColor),string("Text colors are in range of 0-"+myConv::ToString(MYCOLORS-1)+" 0 - Black, 16 - White").c_str())
        ("thread-per-dir,p",boost::program_options::value<unsigned int>(&uiThreadsPerDir)->default_value(uiThreadsPerDir),"Number of threads per directory.\nUseful for bandwidth test with NCQ.")
        ("version,v","Print version and quit")
        ("work-dir,w",boost::program_options::value<vector<string> >()->composing(),"Run test in specified locations or drivers (default user directory)\n \" \
                                                                                    Importent do NOT  add more then 1 directory if you specified probe sizes.")
        ;
        boost::program_options::options_description developer("Developer options");
        developer.add_options()
        ("check-defualt-probes,C","Check for default test probes and small files read/write data limit")
        ("debug,D","Verbose output for debugging only")
        ("max-allocation-size,M","Check and set max allocation size of string and char as buffer")
        ("max-char_size",boost::program_options::value<string>(&MaxCharSize)->default_value(MaxCharSize),"Max size of allocated char")
        ("max-string_size",boost::program_options::value<string>(&MaxStringSize)->default_value(MaxStringSize),"Max size of allocated string")
        ("permissions,P",boost::program_options::value<mode_t>(&uiPermissions)->default_value(uiPermissions),"Change temporary folder permission")
        ("self-test,S",boost::program_options::value<unsigned int>(), "Run self test.\nFor Development purpose only")//DO NOT CHANGE unless you wish to self test all the time :-)
        ;

        desc.add(developer);

        boost::program_options::positional_options_description pos_opt;
        pos_opt.add("work-dir", -1);
        pos_opt.add("probe-size",-1);

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(ac, av).options(desc).positional(pos_opt).run(), vm);

        GetUserDir();///Creates user profile before parsing args

        boost::filesystem::ifstream ifs(pathConfig);// user_folder/.littlebenchmark/hdd/config.cfg
        boost::program_options::store(boost::program_options::parse_config_file(ifs, desc), vm);
        //if (ifs.is_open()){ ifs.close(); }

        boost::program_options::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            exit(0);
        }
        if (vm.count("version")) {
            cout << "Version:" << AutoVersion::FULLVERSION_STRING <<endl;
            cout << "Build date: " << AutoVersion::DATE <<"."<<AutoVersion::MONTH <<"."<<AutoVersion::YEAR<<endl;
            exit(0);
        }
        if (vm.count("Author")){
            cout << "Author:    Łukasz Buśko"<<endl;
            cout << "Contact:   buskol.waw.pl@gmail.com"<<endl;
            cout << "Home page: http://str0g.wordpress.com"<<endl;
            cout << "License:   GNU/General Public License"<<endl;
            exit(0);
        }
        if((vm.count("max-char_size"))){
            MaxCharSize = vm["max-char_size"].as<string>();
            ui64MaxCharSize   = myConv::SizeFromString<uint64_t>( MaxCharSize   );
        }
        if((vm.count("max-string_size"))){
            MaxStringSize = vm["max-string_size"].as<string>();
            ui64MaxStringSize = myConv::SizeFromString<uint64_t>( MaxStringSize );
        }
        if (vm.count("max-allocation-size")){
            bBufferingTest = true;
        }
        if (vm.count("debug")){
            bDebugging = true;
        }
        if(vm.count("work-dir")){
            vstr_WorkDirs = vm["work-dir"].as<vector<string> >();
        }else{
            vstr_WorkDirs.push_back(strPath);
        }
        if (vm.count("permissions")){
            uiPermissions = vm["permissions"].as<mode_t>();
        }
        if ( vm.count("precision")){
            ui8Precision = vm["precision"].as<uint8_t>();
        }
        if (vm.count("data-limit")){
            strDataLimit = vm["data-limit"].as<string>();
            uiDataLimit = myConv::SizeFromString<unsigned int>(strDataLimit);
        }
        if (vm.count("max-loops")){
            uiMaxLoops = vm["max-loops"].as<unsigned int>();
        }
        if (vm.count("probe-size")){
            vector<string> vstr = vm["probe-size"].as<std::vector<string> >();
            if (vstr.size() > 0){ vui_Probes.clear(); }
            for (vector<string>::iterator it = vstr.begin();
                it != vstr.end();
                ++it){
                     if ( myConv::SizeFromString<unsigned int>(*it) <= ui64MaxStringSize){
                         vui_Probes.push_back( myConv::SizeFromString<unsigned int>(*it) );
                    }else{
                        cout<<"Probe:"<<*it<<" is to big for your system"<<endl;
                        cout<<"Current limit is:"<<MaxStringSize+"("<<ui64MaxStringSize<<")"<<endl;
                    }
                }
        }else{
            vui_Probes.push_back(4 * KB);
            vui_Probes.push_back(16 * KB);
            vui_Probes.push_back(256 *KB);
            vui_Probes.push_back(1 * MB);
            vui_Probes.push_back(25 * MB);
            vui_Probes.push_back(100 * MB);
            vui_Probes.push_back(250 * MB);
            if ( ui64MaxStringSize >= 700 * MB)
                vui_Probes.push_back(700 * MB);
            if ( ui64MaxStringSize >= GB)
                vui_Probes.push_back(GB);

        }
        if (vm.count("check-defualt-probes")){
            cout << "Data limit is set to "+strDataLimit<<endl;
            cout << "Listing probes:" << endl;
            uint64_t ui64tmp;
            for(vector<unsigned int>::iterator it = vui_Probes.begin(); it != vui_Probes.end(); ++it){
                ui64tmp = (uint64_t)*it;
                if (bDebugging){
                    cout <<"\t"<< myConv::Bandwidth(0,ui64tmp) <<endl;
                }
            }
        }
        if (vm.count("self-test")) {
            uiSelfTestScenerio = vm["self-test"].as<unsigned int>();
            bSelfTest = true;
        }
        if (vm.count("dir-thread")) {
            bThreadingPerDir = true;
        }
        if (vm.count("force-text-color")){
            iTextColor =  vm["force-text-color"].as<int>();
            myColors::TextColor = &iTextColor;
        }
        if (vm.count("thread-per-dir")){
            uiThreadsPerDir = vm["thread-per-dir"].as<unsigned int>();
        }
        if(uiThreadsPerDir < 1){
            cerr<<"Number of threads has been corrected to 1"<<endl;
            uiThreadsPerDir = 1;
        }
        if (vm.count("output-file")){
            strSummaryFile = vm["output-file"].as<string>();
        }
        pathSummary /= pathProfile.file_string() + "/"+strSummaryFile;
    }
    catch(std::exception& e) {
        cerr << e.what() << "\n";
        exit(0);
    }
}
tester_hdd::~tester_hdd(){
    if ( pus_Var ){ delete pus_Var; }
    for(bst_ptrlist_it_tester_hdd = bst_ptrlist_tester_hdd.begin();
        bst_ptrlist_it_tester_hdd != bst_ptrlist_tester_hdd.end();
        ++bst_ptrlist_it_tester_hdd){
            myIO::SimpleWriteToFile(pathSummary, bst_ptrlist_it_tester_hdd->getSummary() );
        }
        myIO::SimpleWriteToFile(pathSummary, string("\n") );
}

void tester_hdd::BufferingTest(){
        uint64_t ui = 0;
        uint64_t ui64offset = 100;
        char *c;
        string *p = new string;
        do{
            try{
                while (true){
                    p->reserve(ui);
                    ui += ui64offset*MB;
                }
            }catch(std::exception &e){
                ui *= 0.8;
                ui64offset /= 10;
                delete p;
                p = new string;
            }
        }while(ui64offset > 1);
        delete p;
        ui64MaxStringSize = ui - MB;
        MaxStringSize =  myConv::Bandwidth(0,ui64MaxStringSize,"");
        cerr<<"Max string size: "<<ui <<"("+ MaxStringSize +")"<<endl;
        ui = 0;
        do{
            try{
                while (true){
                    c = new char[ui];
                    delete[] c;
                    c = NULL;
                    ui += ui64offset*MB;
                }
            }catch(std::exception &e){
                //delete[] c;
                ui *= 0.8;
                ui64offset /= 10;
            }
        }while(ui64offset > 1);
        if (c) { delete[] c; }
        ui64MaxCharSize = ui - MB;
        MaxCharSize = myConv::Bandwidth(0,ui64MaxCharSize,"");
        cerr<<"Max char table(char*) size: "<<ui <<"("+ MaxCharSize +")"<<endl;
        cout<<"Do you wish to update your configs?[y/n]"<<endl;
        char ca;
        cin>>ca;
        if(ca == 'y'){
            pus_Var->push_back(new profileUpdateStore("max-char_size =",MaxCharSize));
            pus_Var->push_back(new profileUpdateStore("max-string_size =",MaxStringSize));
            UpdateProfile();
        }
}

void tester_hdd::GetUserDir(){
#ifdef WIN64 || _WIN64 || __WIN64__ || WIN32 || _WIN32 || __WIN32__ || _TOS_WIN__ || __WINDOWS__
//
#else
/*MEM leak */
    struct passwd *gUserdir = getpwuid(getuid());
    if (gUserdir){
        pathProfile /= (gUserdir->pw_dir);
    }else{
        cout<<"Error getting user folder"<<endl;
    }

#endif
    if (pathProfile.file_string().length() > 0 ){///Creates program profile directory for current user
        pathProfile /= ".littlebenchmark";
        myIO::createDir(pathProfile);
        pathProfile /= ("hdd");
        myIO::createDir(pathProfile);
        strPath = pathProfile.file_string()+"/";
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

void tester_hdd::SelfTest(){
    switch (uiSelfTestScenerio){
        case 0:
            cout <<"Running scenerio 0 - full test" <<endl;
            uiSelfTestScenerio = 5;
            SelfTest();
            bSelfTest =true;
            bRun =false;
            uiSelfTestScenerio = 1;
            SelfTest();
            bSelfTest =true;
            bRun =false;
            uiSelfTestScenerio = 2;
            SelfTest();
            bSelfTest =true;
            bRun =false;
            uiSelfTestScenerio = 3;
            SelfTest();
            bSelfTest =true;
            bRun =false;
            uiSelfTestScenerio = 4;
            SelfTest();
            bSelfTest =true;
            bRun =false;
            break;
        case 1:
            cout <<"Running scenerio 1 - 1 thread and 1 dir test" <<endl;
            Run();
            break;
        case 2:
            cout <<"Running scenerio 2 - 2 thread and 1 dir test" <<endl;
            uiThreadsPerDir = 2;
            Run();
            break;
        case 3:
            cout <<"Running scenerio 3 - 2 dir simusly (2 threads) test" <<endl;
            uiThreadsPerDir = 1;
            bThreadingPerDir = true;
            Run();
            break;
        case 4:
            cout <<"Running scenerio 3 - 2 dir simusly but every in 2 threads (4 threads) test" <<endl;
            uiThreadsPerDir = 1;
            bThreadingPerDir = true;
            Run();
            break;
        case 5:
            cout <<"Test bandwidth counter" <<endl;
            cout<<"0.1 / MB\t[";myConv::Bandwidth(0.1,MB) == "10MB/s"           ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(0.1,MB)<<endl;
            cout<<"0 / MB\t[";  myConv::Bandwidth(0,MB) == "1MB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(0,MB)<<endl;
            cout<<"1 / MB\t[";  myConv::Bandwidth(1,MB) == "1MB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(1,MB)<<endl;
            cout<<"10 / MB\t["; myConv::Bandwidth(10,MB) == "102.4KB/s"         ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(10,MB)<<endl;
            cout<<"0.1 / GB\t[";myConv::Bandwidth(0.1,GB) == "10GB/s"           ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(0.1,GB)<<endl;
            cout<<"0 / GB\t[";  myConv::Bandwidth(0,GB) == "1GB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(0,GB)<<endl;
            cout<<"1 / GB\t[";  myConv::Bandwidth(1,GB) == "1GB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(1,GB)<<endl;
            cout<<"10 / GB\t["; myConv::Bandwidth(10,GB) == "102.4MB/s"         ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::Bandwidth(10,GB)<<endl;
            cout <<"Test time scaling" <<endl;
            cout<<"1ns\t[";     myConv::TimeToString(NS) == "1ns"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeToString(NS)<<endl;
            cout<<"1us\t[";     myConv::TimeToString(US) == "1us"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeToString(US)<<endl;
            cout<<"1ms\t[";     myConv::TimeToString(MS) == "1ms"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeToString(MS)<<endl;
            cout<<"1s\t[";      myConv::TimeToString(1) == "1s"                 ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeToString(1)<<endl;
            cout<<"1ns\t[";     myConv::TimeFromString<int>("1ns",m_NS) == m_NS   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeFromString<int>("1ns",m_NS)<<endl;
            cout<<"1us\t[";     myConv::TimeFromString<int>("1us",m_US) == m_US   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeFromString<int>("1us",m_US)<<endl;
            cout<<"1ms\t[";     myConv::TimeFromString<int>("1ms",m_MS) == m_MS   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeFromString<int>("1ms",m_MS)<<endl;
            cout<<"1s\t[";      myConv::TimeFromString<int>("1s",1) == 1        ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<myConv::TimeFromString<int>("1s",1)<<endl;
            cout<<"Size from string"<<endl;
            cout<<"1b\t[";      myConv::SizeFromString<unsigned>("1B") == 1     ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1B") <<endl;
            cout<<"1K\t[";      myConv::SizeFromString<unsigned>("1K") == KB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1K") <<endl;
            cout<<"1M\t[";      myConv::SizeFromString<unsigned>("1M") == MB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1M") <<endl;
            cout<<"1G\t[";      myConv::SizeFromString<unsigned>("1G") == GB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1G") <<endl;
            cout<<"1Kb\t[";     myConv::SizeFromString<unsigned>("1Kb") == KB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1Kb") <<endl;
            cout<<"1MB\t[";     myConv::SizeFromString<unsigned>("1MB") == MB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1MB") <<endl;
            cout<<"1GB\t[";     myConv::SizeFromString<unsigned>("1GB") == GB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< myConv::SizeFromString<unsigned>("1GB") <<endl;
            bRun = false; //tmp
            break;
        case 6:
            cout<<"Testing Colors"<<endl;
            for (int i =0; i < MYCOLORS; i++){
                cout<<myColors::ConsoleColors_[i]<<i<<"."<<endl;
            }
            cout<<myColors::ConsoleColors_[*myColors::TextColor]<<"Should be "<<MYCOLORS<<" diffrent colors"<<endl;
            bRun = false; //tmp
        default:
            cerr <<"Test scenerio haven't been defined yet" << endl;
            break;
    }
}

void tester_hdd::Run(){
    if (bBufferingTest){
        BufferingTest();
        bBufferingTest = false;
    }
    if (bSelfTest){
        bSelfTest = false;
        SelfTest();
    }
    if (bDebugging){//Przeniesc w miejsce pierwszego uruchomienia!
        cout<<"PathProfile:"<<pathProfile<<endl;
        cout<<"PathConfigs:"<<pathConfig<<endl;
        cout<<"PathSummary"<<pathSummary<<endl;
        cout<<"OutFile: "<<strSummaryFile<<endl;
        cout<<"Threads per directory: "<<uiThreadsPerDir<<endl;
        cout<<"MultiThreading: "; bThreadingPerDir ? cout<<"yes"<<endl : cout<<"no"<<endl;
        cout<<"Run some test?: "; bRun ? cout<<"yes"<<endl : cout<<"no"<<endl;
        cout<<"Max loops: "<<uiMaxLoops<<endl;
        cout<<"Directories: "<<endl;
        for (vitstr_WorkDirs = vstr_WorkDirs.begin();
            vitstr_WorkDirs!= vstr_WorkDirs.end();
            ++vitstr_WorkDirs){
            cout << "\t" << *vitstr_WorkDirs<<endl;
        }
        bDebugging = false;
    }
    //second value setup...
    ui64MaxCharSize   = myConv::SizeFromString<uint64_t>( MaxCharSize   );
    ui64MaxStringSize = myConv::SizeFromString<uint64_t>( MaxStringSize );
    //
    if (bRun){
        for(vitstr_WorkDirs = vstr_WorkDirs.begin();
            vitstr_WorkDirs != vstr_WorkDirs.end();
            ++vitstr_WorkDirs){///Create threads and insert it to list
                bst_ptrlist_tester_hdd.push_back(new thread_tester_hdd(vui_Probes,
                                                                   *vitstr_WorkDirs,
                                                                   string("/"),//remove later
                                                                   uiDataLimit,
                                                                   uiMaxLoops,
                                                                   ui8Precision,
                                                                   uiPermissions,
                                                                   bDebugging
                                                                   ));
        }
        for(bst_ptrlist_it_tester_hdd = bst_ptrlist_tester_hdd.begin();
            bst_ptrlist_it_tester_hdd != bst_ptrlist_tester_hdd.end();
            ++bst_ptrlist_it_tester_hdd){///Running threads from list
                bst_ptrlist_it_tester_hdd->start();
                if(!bThreadingPerDir){///If multithreading is disable program will wait for thread to do its job before running next one
                    bst_ptrlist_it_tester_hdd->join();
                }
            }
        if(bThreadingPerDir){
            for(bst_ptrlist_it_tester_hdd = bst_ptrlist_tester_hdd.begin();
                bst_ptrlist_it_tester_hdd != bst_ptrlist_tester_hdd.end();
                ++bst_ptrlist_it_tester_hdd){///With multithreading wating for all started threads to end its jobs
                    bst_ptrlist_it_tester_hdd->join();
                }
        }
        bRun =false;
    }
}

void tester_hdd::UpdateProfile(){
    string *strBuf = new string;
    int64_t index = 0;

    myIO::simpleReadToStringByStream(pathConfig,strBuf);
    for (vector<profileUpdateStore*>::iterator it = pus_Var->begin();\
        it != pus_Var->end(); ++it
        ){
            if ( (**it).GetstrVar().length() > 3 and (**it).GetstrVal().length() > 0 ){
                index = (int64_t)strBuf->find((**it).GetstrVar());
                if ( index > 0){
                    index += (int64_t)(**it).GetstrVar().length();
                    strBuf->replace(index,\
                                    strBuf->find("\n",index),\
                                    " "+(**it).GetstrVal()
                                    );
                }else{
                    strBuf->append((**it).GetIt());
                }
            }
    }
    myIO::SimpleWriteToFile(pathConfig,*strBuf,std::ios::ate|std::ios::binary);
    //Cleaning
    if ( strBuf ){ delete strBuf; }
    for (vector<profileUpdateStore*>::iterator it = pus_Var->begin();\
        it != pus_Var->end(); ++it
        ){
        delete &(**it);
        *it = NULL;
    }
    delete pus_Var;
    pus_Var = NULL;
}
