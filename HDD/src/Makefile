CXX=g++
ARCH=core2
LDFLAGS= -O2 --mach=$(ARCH)
CPPFLAGS=-Wall -Werror
LDXFLAGS=-lboost_thread-mt -lboost_program_options-mt -lboost_filesystem-mt -L /usr/lib/libcrypto++ -L /usr/lib64/libcrypto++ -lcrypto++
OBJS=Globals.o main.o handler_Configuration.o handler_Report.o tester_hdd.o thread_tester_hdd.o
OBJSHPP=myConv.o myHash.o myIO.o myThreadTemplates.o myTime.o

#all: LittleBenchmarkHDD,refman

LittleBenchmarkHDD: $(OBJS)
	$(CXX) $(LDFLAGS) $(LDXFLAGS) $^ -o $@
$(OBJS): %.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
$(OBJSHPP): %.o: %.hpp
	$(CXX) $(CPPFLAGS) $(LDXFLAGS) -c $< -o $@
makedoc:
	doxygen LittleBenchmarkHDD.dox
	make -C ../doc/
clean:
	@if [ -e LittleBenchmarkHDD ]; then rm LittleBenchmarkHDD; echo "rm LittleBenchmarkHDD"; fi
	rm *.o

.PHONY: makedoc clean
