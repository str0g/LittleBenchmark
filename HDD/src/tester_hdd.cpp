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
                                        strSlash("/"),
                                        strPath(""),
                                        bThreadingPerDir(false),
                                        bDebugging(false),
                                        uiThreadsPerDir(1),
                                        uiSelfTestScenerio(0),
                                        uiDataLimit(300 * MB),
                                        uiMaxLoops(500),
                                        uiPermissions(0777),
                                        bRun(true){

    #ifdef WIN64 || _WIN64 || __WIN64__ || WIN32 || _WIN32 || __WIN32__ || _TOS_WIN__ || __WINDOWS__
    strSlash = "\\";
    #endif
    try{
        boost::program_options::options_description desc("Options");
        desc.add_options()
        ("help,h", "Print usage message and quit")
        ("Author,a","Print info about Author and quit")
        ("data-limit,l",boost::program_options::value<string>(&strDataLimit)->default_value(strDataLimit),"small data read/write limit")
        ("dir-thread,t", boost::program_options::value<bool>(&bThreadingPerDir)->default_value(bThreadingPerDir), "Run thread for every directory.\nUseful for bandwidth test of controllers or NCQ.")
        ("max-loops,m",boost::program_options::value<unsigned int>(&uiMaxLoops)->default_value(uiMaxLoops),"Limit number of loops per read/write files")
        ("output-file,o",boost::program_options::value<string>(&strSummaryFile)->default_value(strSummaryFile),"Output file")
        ("probe-size,x",boost::program_options::value<vector<string> >()->composing(),"Set size of probes")
        ("thread-per-dir,p",boost::program_options::value<unsigned int>(&uiThreadsPerDir)->default_value(uiThreadsPerDir),"Number of threads per directory.\nUseful for bandwidth test with NCQ.")
        ("version,v","Print version and quit")
        ("work-dir,w",boost::program_options::value<vector<string> >()->composing(),"Run test in specified locations or drivers (default user directory)\n \" \
                                                                                    Importent do NOT  add more then 1 directory if you specified probe sizes.")
        ;
        boost::program_options::options_description developer("Developer options");
        developer.add_options()
        ("check-defualt-probes,C","Check for default test probes and small files read/write data limit")
        ("debug,D","Verbose output for debugging only")
        ("max-allocation-size,M","Checking max size of string and char as buffer")
        ("permissions,P",boost::program_options::value<mode_t>(&uiPermissions)->default_value(uiPermissions),"Change temporary folder permission")
        ("self-test,S",boost::program_options::value<unsigned int>(&uiSelfTestScenerio)->default_value(uiSelfTestScenerio), "Run self test.\nFor Development purpose only")
        ;

        desc.add(developer);

        boost::program_options::positional_options_description pos_opt;
        pos_opt.add("work-dir", -1);
        pos_opt.add("probe-size",-1);

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::command_line_parser(ac, av).options(desc).positional(pos_opt).run(), vm);

        GetUserDir();

        std::ifstream ifs("a");// user_foler/.littlebenchmark/hdd/config.cfg
        boost::program_options::store(boost::program_options::parse_config_file(ifs, desc), vm);

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
        if (vm.count("max-allocation-size")){
            BufferingTest();
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
        if (vm.count("data-limit")){
            strDataLimit = vm["data-limit"].as<string>();
            uiDataLimit = myTime::FromString<unsigned int>(strDataLimit);
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
                    vui_Probes.push_back( myTime::FromString<unsigned int>(*it) );
                }
        }else{
            vui_Probes.push_back(4 * KB);
            vui_Probes.push_back(16 * KB);
            vui_Probes.push_back(256 *KB);
            vui_Probes.push_back(1 * MB);
            vui_Probes.push_back(25 * MB);
            vui_Probes.push_back(100 * MB);
            vui_Probes.push_back(250 * MB);
            vui_Probes.push_back(700 * MB);
            vui_Probes.push_back(GB);

        }
        if (vm.count("check-defualt-probes")){
            cout << "Data limit is set to "+strDataLimit<<endl;
            cout << "Listing probes:" << endl;
            uint64_t ui64tmp;
            for(vector<unsigned int>::iterator it = vui_Probes.begin(); it != vui_Probes.end(); ++it){
                ui64tmp = (uint64_t)*it;
                if (bDebugging){
                    cout <<"\t"<< myTime::Bandwidth(1,&ui64tmp) <<endl;
                }
            }
        }
        if (vm.count("self-test")) {
            uiSelfTestScenerio = vm["self-test"].as<unsigned int>();
            bRun = false;
            SelfTest();
        }
        if (vm.count("dir-thread")) {
            bThreadingPerDir = true;
        }
        if ("thread-per-dir"){
            uiThreadsPerDir = vm["thread-per-dir"].as<unsigned int>();
        }
        if(uiThreadsPerDir < 1){
            cerr<<"Number of threads has been corrected to 1"<<endl;
            uiThreadsPerDir = 1;
        }
        if (vm.count("output-file")){
            strSummaryFile = vm["output-file"].as<string>();
        }
    }
    catch(std::exception& e) {
        cerr << e.what() << "\n";
        exit(0);
    }
}
tester_hdd::~tester_hdd(){
    for(bst_ptrlist_it_tester_hdd = bst_ptrlist_tester_hdd.begin();
        bst_ptrlist_it_tester_hdd != bst_ptrlist_tester_hdd.end();
        ++bst_ptrlist_it_tester_hdd){
            myIO::SimpleWriteToFile(&(strPath+strSummaryFile), &bst_ptrlist_it_tester_hdd->getSummary() );
        }
        myIO::SimpleWriteToFile(&(strPath+strSummaryFile), &string("\n") );
}

void tester_hdd::GetUserDir(){
    string strRet = "";
#ifdef WIN64 || _WIN64 || __WIN64__ || WIN32 || _WIN32 || __WIN32__ || _TOS_WIN__ || __WINDOWS__
//
#else
/*MEM licking */
    struct passwd *gUserdir = getpwuid(getuid());
    if (gUserdir){
        strRet.append(gUserdir->pw_dir);
    }else{
        cout<<"Error getting user folder"<<endl;
    }

#endif
    if (strRet.length() > 0 ){///Creates program profile directory for current user
        strRet.append(strSlash+".littlebenchmark");
        myIO::createDir(&strRet);
        strRet.append(strSlash+"hdd");
        myIO::createDir(&strRet);
        strPath = strRet+strSlash;
        strRet.append(strSlash+"config.cfg");
        myIO::touch(&strRet);
        string strtmp = "\
#\n\
# Run program with -h or --help options to see which parameters you can set here\n\
# Parameter should look like param = [value]\n\
#"; // file tmp
        myIO::SimpleWriteToFile(&strRet,&strtmp);
    }
}

void tester_hdd::SelfTest(){
    switch (uiSelfTestScenerio){
        case 0:
            cout <<"Running scenerio 0 - full test" <<endl;
            uiSelfTestScenerio = 1;
            SelfTest();
            uiSelfTestScenerio = 2;
            SelfTest();
            uiSelfTestScenerio = 3;
            SelfTest();
            uiSelfTestScenerio = 4;
            SelfTest();
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
        default:
            cerr <<"Test scenerio haven't been defined yet" << endl;
            break;
    }
}

void tester_hdd::BufferingTest(){
        uint64_t ui = 100 * MB;
        char *c;
        string *p = new string;
        try{
            while (true){
                p->reserve(ui);
                ui += ui;
            }
        }catch(std::exception &e){
            ui = p->size();
            cerr<<"Max string size: "<<ui <<"("+ myTime::Bandwidth(1,&ui,"")+")"<<endl;
            delete p;
        }
        ui = 100 * MB;
        try{
            while (true){
                c = new char[ui];
                delete[] c;
                ui += ui;
            }
        }catch(std::exception &e){
            cerr<<"Max char table(char*) size: "<<ui <<"("+ myTime::Bandwidth(1,&ui,"")+")"<<endl;
            delete[] c;
        }

}

void tester_hdd::Run(){
    if (bDebugging){//Przeniesc w miejsce pierwszego uruchomienia!
        cout<<"OutFile: "<<strSummaryFile<<endl;
        cout<<"Threads per directory: "<<uiThreadsPerDir<<endl;
        cout<<"MultiThreading: "; bThreadingPerDir ? cout<<"yes"<<endl : cout<<"no"<<endl;
        cout<<"Run some test?: "; bRun ? cout<<"yes"<<endl : cout<<"Only self test"<<endl;
        cout<<"Max loops: "<<uiMaxLoops<<endl;
        cout<<"Directories: "<<endl;
        for (vitstr_WorkDirs = vstr_WorkDirs.begin();
            vitstr_WorkDirs!= vstr_WorkDirs.end();
            ++vitstr_WorkDirs){
            cout << "\t" << *vitstr_WorkDirs<<endl;
        }
    }
    for(vitstr_WorkDirs = vstr_WorkDirs.begin();
        vitstr_WorkDirs != vstr_WorkDirs.end();
        ++vitstr_WorkDirs){///Create threads and insert it to list
            bst_ptrlist_tester_hdd.push_back(new thread_tester_hdd(vui_Probes,
                                                                   *vitstr_WorkDirs,
                                                                   strSlash,
                                                                   uiDataLimit,
                                                                   uiMaxLoops,
                                                                   uiPermissions
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
}
