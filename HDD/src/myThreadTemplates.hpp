#ifndef MYTHREADTEMPLATES_HPP_INCLUDED
#define MYTHREADTEMPLATES_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (https://str0g.wordpress.com)
 * License:   GNU / General Public License
 **************************************************************/
//Headers
//#include <cstdlib>
//#include <cstdio>
//#include <sys/stat.h>
#include <iostream>
#include <list>
#include <string>/*
#include <vector>*/
//boost
#include <boost/thread/thread.hpp>
#include "boost/date_time/local_time/local_time.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
//my
#include "myTime.hpp"
#include "myConv.hpp"/*
#include <boost/filesystem/path.hpp>*/
//#include "thread_tester_hdd.hpp"

//my
/*
#include "myHash.hpp"
#include "myIO.hpp"*/
//Specials
using std::list;
using std::string;/*
using std::vector;
using std::cout;
using std::cin;*/
using std::cerr;
using std::endl;
//Globals Varuabels

namespace buskol{
    namespace ThreadTemplates{
    /*! \addtogroup libbuskol
   *  Additional documentation for group libbuskol
   *  @{
   */
    ///Thread_1 is very simple thread template which constainst few controle methods and entry for statistics
    template <class ClassT> class thread_1{
        private:
            ClassT *parent; //!< Paret class adress
            boost::local_time::local_date_time *blt_ldt_Creation; //!< Keep creation time
            boost::local_time::local_date_time *blt_ldt_Start; //!< Keep thread creation time
            boost::thread m_Thread; //!< Thread
            static boost::mutex mutex_listIO; //!< Mutex for locking list IO
            list<string*> *dExTimes; //!< list for statistics
            bool bShowOverExecution; //!< Show execution time
        public:
            thread_1(ClassT *parent, list<string*> *list = NULL,bool show = false): parent(parent),\
                blt_ldt_Creation(Time::GetTime()),\
                blt_ldt_Start(NULL),\
                dExTimes(list),\
                bShowOverExecution(show){}
            void start(){///Creates Thread and links it dynamic using static method
                blt_ldt_Start = Time::GetTime();
                m_Thread = boost::thread (boost::bind(this->Execute_,parent));
            }
            void start_self_test(){///This method is only for test purpose!
                blt_ldt_Start(Time::GetTime());
                m_Thread = boost::thread (self_test);
            }
            static void self_test(){///This method is only for test purpose!
                //test code hear
            }
            static void Execute_(ClassT *p){///Static linker for dynamic method
                p->Execute();
            }
            void join(){///Join thread
                m_Thread.join();
                UpdateStats(new string("Thread has been doing something for: "+ Conv::TimeToString(Time::TimeDiff(*blt_ldt_Start))));
            }
            void join(unsigned val){///Join thread after specified time in seconds
                m_Thread.timed_join(( boost::posix_time::time_duration(0,0,val,int(boost::posix_time::time_duration::ticks_per_second() / 10))));
                UpdateStats(new string("Thread has been doing something for: "+ Conv::TimeToString(Time::TimeDiff(*blt_ldt_Start))));
            }
            string GetThreadID(){
                return Conv::ToString(m_Thread.get_id());
            }
            void UpdateStats(string *str){///Push statistic information
                boost::mutex::scoped_lock(mutex_listIO);
                if (dExTimes)
                    dExTimes->push_back(str);
                else
                    delete str;
            }
            virtual ~thread_1(){///Virtual destructor which can show time of execution
                if (bShowOverExecution)
                    cerr<<("Thread has been executed for: "+ Conv::TimeToString(Time::TimeDiff(*blt_ldt_Creation)))<<endl;
                if ( blt_ldt_Creation ) { delete blt_ldt_Creation; }
                if ( blt_ldt_Start ) { delete blt_ldt_Start; }
            }
        };
/*! @} */
    }

}

#endif // MYTHREADTEMPLATES_HPP_INCLUDED
