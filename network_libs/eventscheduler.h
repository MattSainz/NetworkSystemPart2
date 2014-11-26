#ifndef __EVENTSCHDULER_H
#define __EVENTSCHDULER_H
#include <vector>
#include <algorithm>
#include <time.h>
#include <mutex>
#include <condition_variable>
#include "threadpool.h"
#define DEFAULT_MAX_EVENTS 4
#define NUM_THREADS 10
using namespace std;

class EventScheduler{
public:
    EventScheduler();
    EventScheduler(size_t maxEvents);
    ~EventScheduler();
    int eventSchedule(void evFunction(void *), void *arg, int timeout);
    void eventCancel(int eventId);
private:
    void init();
    class event{
        public:
            event(void evFunction(void *), void *arg, int timeout, int id){
                this->evFunction = evFunction;
                this->arg = arg;
                struct timespec currentTime;
                if(clock_gettime(CLOCK_REALTIME, &currentTime)){
                    exit(1);
                }
                
                //Sec
                long timeoutSec = timeout/1000;
                //Millis
                long timeoutNSec = (timeout % 1000) * 1000000;
                if(timeoutNSec > 1000000000){
                    timeoutSec += timeoutNSec/1000000000;
                    timeoutNSec %= 1000000000;
                }
                this->timeout.tv_nsec = currentTime.tv_nsec + timeoutNSec;
                this->timeout.tv_sec = currentTime.tv_sec + timeoutSec;

                //printf("Timeout: %d Sec: %ld nSec: %ld newSec: %ld newNSec: %ld\n", timeout, currentTime.tv_sec, currentTime.tv_nsec, this->timeout.tv_sec, this->timeout.tv_nsec);
                this->eventId = id;
            }
            void (*evFunction)(void *);
            void* arg;
            int eventId;
            struct timespec timeout;
    };

    struct eventCompare{
        bool operator()(const event *a, const event *b) const{
            struct timespec aTime = a->timeout;
            struct timespec bTime = b->timeout;
            if(aTime.tv_sec > bTime.tv_sec){
                return true;
            } else if (aTime.tv_sec < bTime.tv_sec){
                return false;
            }else{ 
                return aTime.tv_nsec > bTime.tv_nsec;
            }
        }
    };


    static void* execute(void* es);
    mutex idLock;
    int nextId = 0;
    mutex queueLock;
    condition_variable cv;
    size_t maxEvents;
    vector<event*> queue;
    pthread_t dispatcher;
    bool terminate = false;
    unsigned int numEvents = 0;
    ThreadPool* pool;
};

#endif
