# whatever flags you want to pass to the C++ compiler & linker
#set architecture march for your cpu pentium-m, core2, athlone64 , -O3 for speed optimization
#CPPFLAGS = -pedantic -Wall -march=core2 -lboost_thread-mt -lboost_program_options-mt -lboost_filesystem-mt -lcrypto++
#LDFLAGS = -lm
AM_CPPFLGAS = -pedantic -Wall -march=core2
AM_LDFLAGS = -lm -lboost_thread-mt -lboost_program_options-mt -lboost_filesystem-mt -L /usr/lib/libcrypto++ -L /usr/lib64/libcrypto++ -lcrypto++
# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = LittleBenchmarkHDD
LittleBenchmarkHDD_SOURCES =  main.cpp tester_hdd.cpp tester_hdd.hpp thread_tester_hdd.cpp thread_tester_hdd.hpp myConv.hpp myHash.hpp myTime.hpp myIO.hpp myThreadTemplates.hpp profileUpdateStore.cpp profileUpdateStore.hpp stats_keeper.cpp stats_keeper.hpp Globals.h Globals.cpp version.h
