CXX=g++
ARCH=core2
LDFLAGS= -O2 --mach=$(ARCH)
CPPFLAGS=-Wall -Werror
LDCRYPTO=-L /usr/lib/libcrypto++ -L /usr/lib64/libcrypto++ $(shell pkg-config libcrypto++ --libs)
LDBOOST=-lboost_thread-mt -lboost_program_options-mt -lboost_filesystem-mt
LDXFLAGS=$(LDCRYPTO) $(LDBOOST)

TARGET := LittleBenchmarkHDD

SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:%.cpp=%.o)
#HPPS := $(wildcard *.hpp)
#OBJSHPP := $(HPPS:%.hpp=%.o)
OBJSHPP=myConv.o myHash.o myIO.o myThreadTemplates.o myTime.o


#all: $(TARGET),refman

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $(LDXFLAGS) $^ -o $@
$(OBJS): %.o: %.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@
$(OBJSHPP): %.o: %.hpp
	$(CXX) $(CPPFLAGS) $(LDXFLAGS) -c $< -o $@
makedoc:
	doxygen $(TARGET).dox && make -C ../doc/
clean:
	@if [ -e $(TARGET) ]; then rm $(TARGET); echo "rm $(TARGET)"; fi
	rm *.o

.PHONY: makedoc clean $(TARGET)
