/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/
 /**
 * Important notes for Linux:
 * * In file /etc/nsswitch.conf change passwd compat to passwd file in order to prevent memory leak
 */

//Headers
#include "tester_hdd.hpp"

tester_hdd::tester_hdd(int ac,char **av):
                                        strDataLimit("300M"),
                                        MaxCharSize("511M"),
                                        MaxStringSize("511M"),
                                        bThreadingPerDir(false),
                                        bDebugging(false),
                                        bSelfTest(false),
                                        bBufferingTest(false),
                                        bVerifyCache(true),
                                        bInsufficientMemory(false),
                                        bSSD(false),
                                        iTextColor(7),
                                        uiThreadsPerDir(1),
                                        uiSelfTestScenerio(15),
                                        uiDataLimit(300 * MB),
                                        uiMaxLoops(500),
                                        uiNumberOfReads(1),
                                        ui64MaxCharSize(500*MB),
                                        ui64MaxStringSize(500*MB),
                                        ui8Precision(10),
                                        uiPermissions(0777),
                                        bRun(true)
                                        {
    //cout<<myColors::ConsoleColors_[iTextColor]<<endl;//Some consoles/terminals need it to be set at the very begining
    try{
        setUserDir();//creates and set user directory path
        boost::program_options::options_description desc("Options");
        desc.add_options()
        ("help,h", "Print usage message and quit")
        ("Author,a","Print info about Author and quit")
        ("data-limit,l",boost::program_options::value<string>(&strDataLimit)->default_value(strDataLimit),"small data read/write limit")
        ("dir-thread,t", boost::program_options::value<bool>(&bThreadingPerDir)->default_value(bThreadingPerDir), "Run thread for every directory.\nUseful for bandwidth test of controllers or NCQ.")
        ("force-text-color",boost::program_options::value<int>(&iTextColor)->default_value(iTextColor),string("Text colors are in range of 0-"+blC::ToString(MYCOLORS-1)+" 0 - Black, 16 - White").c_str())
        ("ignore-insufficient-memory",boost::program_options::value<bool>(&bInsufficientMemory)->default_value(bInsufficientMemory),"Don't let program to run if threads*greates_probe_size > available memory")
        ("max-loops,m",boost::program_options::value<unsigned>(&uiMaxLoops)->default_value(uiMaxLoops),"Limit number of loops per read/write files")
        ("max-reads-simulation",boost::program_options::value<unsigned>(&uiNumberOfReads)->default_value(uiNumberOfReads),"Number of reads simulations usefull for hybrid drive tests")
        ("precision,s",boost::program_options::value<uint16_t>(&ui8Precision)->default_value(ui8Precision),"Set time precision")
        ("probe-size,x",boost::program_options::value<vector<string> >()->composing(),"Set size of probes")
        ("save-configs","If you wish to save all configs to configuration file")
        ("ssd",boost::program_options::value<bool>(&bSSD)->default_value(bSSD),"Enable SSD test(test normaly + reversed read oreder)")
        ("thread-per-dir,p",boost::program_options::value<unsigned>(&uiThreadsPerDir)->default_value(uiThreadsPerDir),"Number of threads per directory.\nUseful for bandwidth test with NCQ.")
        ("version,v","Print version and quit")
        ("work-dir,w",boost::program_options::value<vector<boost::filesystem::path> >()->composing(),"Run test in specified locations or drivers (default user directory)\n \" \
                                                                                    Importent do NOT  add more then 1 directory if you specified probe sizes.")
        ("Verify-and-cache", boost::program_options::value<bool>(&bVerifyCache)->default_value(bVerifyCache),"Verfiy files kept in cache (if disable cache is also disabled)")
        ;
        boost::program_options::options_description OutOptions("Output options");
        OutOptions.add_options()
        ("as-log", boost::program_options::value<bool>(&bLog)->default_value(bLog),"Output as log")
        ("as-txt", boost::program_options::value<bool>(&bFormattedTxt)->default_value(bFormattedTxt),"Output as formatted text(default)")
        ("as-xml", boost::program_options::value<bool>(&bGenXML)->default_value(bGenXML),"Output as xml")
        ("multiply-spacer",boost::program_options::value<unsigned>(&uiMultiplySpacer)->default_value(uiMultiplySpacer),"Multiplay spacer")
        ("output-file,o",boost::program_options::value<string>(&strReportFile)->default_value(strReportFile),"Output file with out extention")
        ("set-spacer",boost::program_options::value<string>(&strSpacerChar)->default_value(strSpacerChar),"Spacer should be 1 char long")
        ;
        boost::program_options::options_description developer("Developer options");
        developer.add_options()
        ("check-defualt-probes,C","Check for default test probes and small files read/write data limit")
        ("debug,D","Verbose output for debugging only")
        ("max-allocation-size,M","Check and set max allocation size of string and char as buffer")
        ("max-char_size",boost::program_options::value<string>(&MaxCharSize)->default_value(MaxCharSize),"Max size of allocated char")
        ("max-string_size",boost::program_options::value<string>(&MaxStringSize)->default_value(MaxStringSize),"Max size of allocated string")
        ("permissions,P",boost::program_options::value<mode_t>(&uiPermissions)->default_value(uiPermissions),"Change temporary folder permission")
        ("self-test,S",boost::program_options::value<unsigned>(), "Run self test.\nFor Development purpose only")//DO NOT CHANGE unless you wish to self test on every run :-)
        ;

        desc.add(developer);
        desc.add(OutOptions);

        boost::program_options::positional_options_description pos_opt;
        pos_opt.add("work-dir", pos_opt.max_total_count());
        pos_opt.add("probe-size",pos_opt.max_total_count());

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(ac, av).options(desc).positional(pos_opt).run(), vm);

        boost::filesystem::ifstream ifs(pathConfig);// user_folder/.littlebenchmark/hdd/config.cfg
        boost::program_options::store(boost::program_options::parse_config_file(ifs, desc), vm);
        //if (ifs.is_open()){ ifs.close(); }

        boost::program_options::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            raise(SIGTERM);
        }
        if (vm.count("version")) {
            cout << "Version: " << AutoVersion::FULLVERSION_STRING <<endl;
            cout << "Build date: " << AutoVersion::DATE <<"."<<AutoVersion::MONTH <<"."<<AutoVersion::YEAR<<endl;
            raise(SIGTERM);
        }
        if (vm.count("Author")){
            printf("Author:    Łukasz Buśko\n\
            		Contact:   buskol.waw.pl@gmail.com\n\
            		Home page: http://str0g.wordpress.com\n\
            		License:   GNU/General Public License\n\
            		Special thanks to:\n\
            		Robert Piszczatowski\
            		");
            raise(SIGTERM);
        }
        if((vm.count("as-log"))){
            bLog = vm["as-log"].as<bool>();
            addNodeToStored("as-log",blC::ToString(bLog));
        }

        if((vm.count("as-txt"))){
            bFormattedTxt = vm["as-txt"].as<bool>();
            addNodeToStored("as-txt",blC::ToString(bFormattedTxt));
        }

        if((vm.count("as-xml"))){
            bGenXML = vm["as-xml"].as<bool>();
            addNodeToStored("as-xml",blC::ToString(bGenXML));
        }
        if (vm.count("check-defualt-probes")){
            cout << "Data limit is set to "+strDataLimit<<endl;
            cout << "Listing probes:" << endl;
            uint64_t ui64tmp;
            for(vector<uint64_t>::iterator it = vui_Probes.begin(); it != vui_Probes.end(); ++it){
                ui64tmp = (uint64_t)*it;
                if (bDebugging){
                    cout <<"\t"<< blC::Bandwidth(0,ui64tmp) <<endl;
                }
            }
        }
        if (vm.count("data-limit")){
            strDataLimit = vm["data-limit"].as<string>();
            uiDataLimit = blC::SizeFromString<unsigned>(strDataLimit);
            addNodeToStored("data-limit",strDataLimit);
        }
        if (vm.count("debug")){
            bDebugging = true;
            addNodeToStored("debug",blC::ToString(bDebugging));
        }
        if (vm.count("dir-thread")) {
            bThreadingPerDir = true;
            addNodeToStored("dir-thread",blC::ToString(bThreadingPerDir));
        }
        if (vm.count("force-text-color")){
            iTextColor =  vm["force-text-color"].as<int>();
            myColors::TextColor = &iTextColor;
            addNodeToStored("force-text-color",blC::ToString(iTextColor));
        }
        if ((vm.count("ignore-insufficient-memory"))){
            bInsufficientMemory = vm["ignore-insufficient-memory"].as<bool>();
            addNodeToStored("ignore-insufficient-memory",blC::ToString(bInsufficientMemory));
        }

        if (vm.count("max-allocation-size")){
            bBufferingTest = true;
            addNodeToStored("max-allocation-size",blC::ToString(bBufferingTest));
        }
        if((vm.count("max-char_size"))){
            MaxCharSize = vm["max-char_size"].as<string>();
            ui64MaxCharSize   = blC::SizeFromString<uint64_t>( MaxCharSize   );
            addNodeToStored("max-char_size",MaxCharSize);
        }
        if (vm.count("max-loops")){
            uiMaxLoops = vm["max-loops"].as<unsigned>();
            addNodeToStored("max-loops",blC::ToString(uiMaxLoops));
        }
        if((vm.count("max-string_size"))){
            MaxStringSize = vm["max-string_size"].as<string>();
            ui64MaxStringSize = blC::SizeFromString<uint64_t>( MaxStringSize );
            addNodeToStored("max-string_size",MaxStringSize);
        }
        if (vm.count("max-reads-simulation")){
            uiNumberOfReads = vm["max-reads-simulation"].as<unsigned>();
            addNodeToStored("max-reads-simulation",blC::ToString(uiNumberOfReads));
        }
        if (vm.count("multiply-spacer")){
            uiMultiplySpacer = vm["multiply-spacer"].as<unsigned>();
            addNodeToStored("multiply-spacer",blC::ToString(uiMultiplySpacer));
        }
        if (vm.count("permissions")){
            uiPermissions = vm["permissions"].as<mode_t>();
            addNodeToStored("permissions",blC::ToString(uiPermissions));
        }

        if ( vm.count("precision")){
            ui8Precision = vm["precision"].as<uint16_t>();
            cout.setf(std::ios::adjustfield,std::ios::floatfield);
            cout.precision(ui8Precision);
            cerr.setf(std::ios::adjustfield,std::ios::floatfield);
            cerr.precision(ui8Precision);
            addNodeToStored("precision",blC::ToString(ui8Precision));
        }
        if(vm.count("work-dir"))
            vstr_WorkDirs = vm["work-dir"].as<vector<boost::filesystem::path> >();
        else
            vstr_WorkDirs.push_back(pathProfile);

        if (vm.count("thread-per-dir")){
            uiThreadsPerDir = vm["thread-per-dir"].as<unsigned>();
            uiMaxCols = (vstr_WorkDirs.size() * uiThreadsPerDir)+1;///Number of columns
            addNodeToStored("thread-per-dir",blC::ToString(uiThreadsPerDir));
        }
        if(uiThreadsPerDir < 1){
            cerr<<"Number of threads has been corrected to 1"<<endl;
            uiThreadsPerDir = 1;
        }
        if (vm.count("probe-size")){
            vector<string> vstr = vm["probe-size"].as<std::vector<string> >();
            if (vstr.size() > 0){ vui_Probes.clear(); }
            for (vector<string>::iterator it = vstr.begin();
                it != vstr.end();
                ++it){
                     if ( blC::SizeFromString<uint64_t>(*it) <= ui64MaxStringSize){
                         vui_Probes.push_back( blC::SizeFromString<uint64_t>(*it) );
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
        if (vm.count("save-configs"))
            bLetUpdate = true;

        if (vm.count("self-test")) {
            uiSelfTestScenerio = vm["self-test"].as<unsigned>();
            bSelfTest = true;
            addNodeToStored("self-test",blC::ToString(uiSelfTestScenerio));
        }
        if (vm.count("set-spacer"))
            strSpacerChar = vm["set-spacer"].as<string>();
            //addNodeToStored("set-spacer",strSpacerChar);
        if (vm.count("ssd")){
            bSSD = vm["ssd"].as<bool>();
            addNodeToStored("ssd",blC::ToString(bSSD));
        }
        if (vm.count("output-file"))
            strReportFile = vm["output-file"].as<string>();

        if(vm.count("Verify-and-cache"))
            bVerifyCache = vm["Verify-and-cache"].as<bool>();
            addNodeToStored("Verify-and-cache",blC::ToString(bVerifyCache));
        if (!bInsufficientMemory){
            //pobierz wolna pamiec
            uint64_t ui64FreeRam = 0;
            unsigned uiModulo = 0;
            #if ( _WIN32 ||  _WIN64) || ( __WIN32__ || __WIN64__ )
            // Get memory status
            MEMORYSTATUS theStatus;
            ZeroMemory(&theStatus,sizeof(theStatus));
            theStatus.dwLength = sizeof(theStatus);
            GlobalMemoryStatus(&theStatus);
            ui64FreeRam = theStatus.dwTotalPhys;
            #else
            struct sysinfo sys_info;
            if(sysinfo(&sys_info) != 0)
                throw std::runtime_error("Failed to get sys informations");
            ui64FreeRam =  sys_info.freeram;
            #endif
            for (vector<uint64_t>::iterator it = vui_Probes.begin();
                it != vui_Probes.end();
                it++){
                    uiModulo = *it % uiDataLimit;
                    uiModulo == 0 ? uiModulo = 1 : uiModulo;
                    if ( uint64_t ((uiMaxCols * *it) + (uiMaxCols * uiModulo)) >= ui64FreeRam)
                        throw std::runtime_error("Insufficient memory for tests ("\
                                                 +blC::Bandwidth(0,(uiMaxCols * *it) + (uiMaxCols * uiModulo),"")\
                                                 +"/"+blC::Bandwidth(0,ui64FreeRam,"")+")");
                }
        }
        pathReport /= pathProfile.file_string() + "/"+strReportFile;
        setArgs();///Set output args
    }
    catch(std::exception& e) {
        cerr << e.what() << "\n";
        raise(SIGABRT);
    }
}
tester_hdd::~tester_hdd(){
    cerr<<"Destructor: tester_hdd"<<endl;/*
    cerr<<"Destructor: tester_hdd: joining threads"<<endl;
    for(bst_ptrlist_it_tester_hdd = bst_ptrlist_tester_hdd.begin();
                bst_ptrlist_it_tester_hdd != bst_ptrlist_tester_hdd.end();
                ++bst_ptrlist_it_tester_hdd){
                    bst_ptrlist_it_tester_hdd->join(unsigned(0));
                }*/
    cerr<<"Destructor: tester_hdd: Cleaning after threads"<<endl;
    bst_ptrlist_tester_hdd.clear();
    cerr<<"Destructor: tester_hdd: Bye"<<endl;
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
                    ui64MaxStringSize = ui - MB;
                    ui += ui64offset*MB;
                }
            }catch(std::exception &e){
                ui *= 0.8;
                ui64offset /= 10;
            }
        }while(ui64offset > 1);
        if( p ){ delete p; p = NULL; }
        MaxStringSize =  blC::Bandwidth(0,ui64MaxStringSize,"");
        cerr<<"Max string size: "<<ui <<"("+ MaxStringSize +")"<<endl;
        ui = 0;
        do{
            c = new (std::nothrow) char[ui];///Using nothrow in order to avoid bad alloc signal handler in libboost and libcrypto
            if ( c) {
                    delete[] c;
                    c = NULL;
                    ui64MaxCharSize = ui - MB;
                    ui += ui64offset*MB;
            }else{
                ui *= 0.8;
                ui64offset /= 10;
            }
        }while(ui64offset > 1);
        if (c) { delete[] c; c = NULL ; }
        MaxCharSize = blC::Bandwidth(0,ui64MaxCharSize,"");
        cerr<<"Max char table(char*) size: "<<ui <<"("+ MaxCharSize +")"<<endl;
        cout<<"Do you wish to update your configs?[y/n]"<<endl;
        char ca;
        cin>>ca;
        if(ca == 'y'){
            if ( !bLetUpdate ){
                clearNodes();
                bLetUpdate = true;
            }
            addNodeToStored("max-char_size =",MaxCharSize);
            addNodeToStored("max-string_size =",MaxStringSize);
        }
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
            cout<<"0.1 / MB\t[";blC::Bandwidth(0.1,MB) == "10MB/s"           ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(0.1,MB)<<endl;
            cout<<"0 / MB\t[";  blC::Bandwidth(0,MB) == "1MB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(0,MB)<<endl;
            cout<<"1 / MB\t[";  blC::Bandwidth(1,MB) == "1MB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(1,MB)<<endl;
            cout<<"10 / MB\t["; blC::Bandwidth(10,MB) == "102.4KB/s"         ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(10,MB)<<endl;
            cout<<"0.1 / GB\t[";blC::Bandwidth(0.1,GB) == "10GB/s"           ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(0.1,GB)<<endl;
            cout<<"0 / GB\t[";  blC::Bandwidth(0,GB) == "1GB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(0,GB)<<endl;
            cout<<"1 / GB\t[";  blC::Bandwidth(1,GB) == "1GB/s"              ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(1,GB)<<endl;
            cout<<"10 / GB\t["; blC::Bandwidth(10,GB) == "102.4MB/s"         ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::Bandwidth(10,GB)<<endl;
            cout <<"Test time scaling" <<endl;
            cout<<"1ns\t[";     blC::TimeToString(NS) == "1ns"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeToString(NS)<<endl;
            cout<<"1us\t[";     blC::TimeToString(US) == "1us"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeToString(US)<<endl;
            cout<<"1ms\t[";     blC::TimeToString(MS) == "1ms"               ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeToString(MS)<<endl;
            cout<<"1s\t[";      blC::TimeToString(1) == "1s"                 ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeToString(1)<<endl;
            cout<<"1ns\t[";     blC::TimeFromString<int>("1ns",m_NS) == m_NS   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeFromString<int>("1ns",m_NS)<<endl;
            cout<<"1us\t[";     blC::TimeFromString<int>("1us",m_US) == m_US   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeFromString<int>("1us",m_US)<<endl;
            cout<<"1ms\t[";     blC::TimeFromString<int>("1ms",m_MS) == m_MS   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeFromString<int>("1ms",m_MS)<<endl;
            cout<<"1s\t[";      blC::TimeFromString<int>("1s",1) == 1        ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<<blC::TimeFromString<int>("1s",1)<<endl;
            cout<<"Size from string"<<endl;
            cout<<"1b\t[";      blC::SizeFromString<unsigned>("1B") == 1     ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1B") <<endl;
            cout<<"1K\t[";      blC::SizeFromString<unsigned>("1K") == KB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1K") <<endl;
            cout<<"1M\t[";      blC::SizeFromString<unsigned>("1M") == MB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1M") <<endl;
            cout<<"1G\t[";      blC::SizeFromString<unsigned>("1G") == GB    ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1G") <<endl;
            cout<<"1Kb\t[";     blC::SizeFromString<unsigned>("1Kb") == KB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1Kb") <<endl;
            cout<<"1MB\t[";     blC::SizeFromString<unsigned>("1MB") == MB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1MB") <<endl;
            cout<<"1GB\t[";     blC::SizeFromString<unsigned>("1GB") == GB   ? cout<<myColors::ConsoleColors_[2]<<"OK"<<myColors::ConsoleColors_[*myColors::TextColor] : cout<<myColors::ConsoleColors_[1]<<"FAIL"<<myColors::ConsoleColors_[*myColors::TextColor] ; cout<<"]"<< blC::SizeFromString<unsigned>("1GB") <<endl;
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
    parseConfigs();
    saveConfigs();
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
        cout<<"PathReport"<<pathReport<<endl;
        cout<<"OutFile: "<<strReportFile<<endl;
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
    if (bRun){
        unsigned i = 0;
        unsigned uiCounter = 0 ;
        for(vitstr_WorkDirs = vstr_WorkDirs.begin();
            vitstr_WorkDirs != vstr_WorkDirs.end();
            ++vitstr_WorkDirs){///Create threads and insert it to list
            do{
                bst_ptrlist_tester_hdd.push_back(new thread_tester_hdd<tester_hdd>(
                                                vui_Probes,
                                                *vitstr_WorkDirs,
                                                uiDataLimit,
                                                uiMaxLoops,
                                                ++uiCounter,
                                                uiNumberOfReads,
                                                ui8Precision,
                                                uiPermissions,
                                                bDebugging,
                                                bVerifyCache,
                                                bSSD,
                                                this
                                                ));
            }while( uiThreadsPerDir > ++i );
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
        SaveToDisk(pathReport);
    }
}
