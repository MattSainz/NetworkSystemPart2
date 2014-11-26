#include "eventscheduler.h" 
using namespace std;

EventScheduler::EventScheduler(){
    EventScheduler(DEFAULT_MAX_EVENTS);
}

EventScheduler::EventScheduler(size_t maxEvents){
    this->maxEvents = maxEvents; 
    init();
    
}
void EventScheduler::init(){
    pool = new ThreadPool(NUM_THREADS);
    make_heap(queue.begin(), queue.end(), eventCompare());
    pthread_create(&dispatcher, NULL, &execute, (void*)this);
}

EventScheduler::~EventScheduler(){
    queueLock.lock();
    terminate = true;
    cv.notify_all();
    queueLock.unlock();
    pthread_join(dispatcher, NULL);
    delete pool;
}

int EventScheduler::eventSchedule(void evFunction(void *), void *args, int timeout){
    if(numEvents < maxEvents){
        idLock.lock();
        int id = nextId++;
        idLock.unlock();
        event* newEvent = new event(evFunction, args, timeout, id);
        queueLock.lock();
        numEvents++;
        queue.push_back(newEvent);
        push_heap(queue.begin(), queue.end(), eventCompare());
        cv.notify_one();
        queueLock.unlock();
        return id;
    }else{
        return -1;
    }
}

void* EventScheduler::execute(void *es){
    EventScheduler* scheduler = (EventScheduler*) es;
    //printf("Got ES. maxEvents: %zu numEvents: %u\n", scheduler->maxEvents, scheduler->numEvents);
    vector<event*> *pQueue = &(scheduler->queue);
    while(!scheduler->terminate || scheduler->numEvents > 0){
        unique_lock<mutex> uLock(scheduler->queueLock);
        long differenceNsec = -1;
        event* myEvent = (event*) 0x12345;
        if(!pQueue->empty()){
            myEvent = pQueue->front();
            pop_heap(pQueue->begin(), pQueue->end(), eventCompare());
            pQueue->pop_back();
            struct timespec current;
            (void)clock_gettime(CLOCK_REALTIME, &current);
            struct timespec next = myEvent->timeout;
            differenceNsec = ((current.tv_sec - next.tv_sec) * 1000000000) + (current.tv_nsec - next.tv_nsec);
            if(differenceNsec < 0){
                pQueue->push_back(myEvent);
                push_heap(pQueue->begin(), pQueue->end(), eventCompare());
            }
        }

        if(differenceNsec < 0){
            scheduler->cv.wait_for(uLock, chrono::nanoseconds(-differenceNsec));
        }else{
            scheduler->numEvents--;
            uLock.unlock();
            scheduler->pool->dispatch_thread(myEvent->evFunction, myEvent->arg);
        }
    }
    //Meaningless return
    return (void*) NULL;
}

void EventScheduler::eventCancel(int eventId){
    queueLock.lock();
    for(vector<event*>::iterator it = queue.begin();it != queue.end();++it){
        if((*it)->eventId == eventId){
            queue.erase(it);
            make_heap(queue.begin(), queue.end(), eventCompare());
            numEvents--;
            //printf("Deleted event: %d\n", eventId);
            break;
        }
    }
    queueLock.unlock();
}
