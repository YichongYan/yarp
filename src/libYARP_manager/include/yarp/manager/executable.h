/*
 *  Yarp Modules Manager
 *  Copyright: (C) 2011 Robotics, Brain and Cognitive Sciences - Italian Institute of Technology (IIT)
 *  Authors: Ali Paikan <ali.paikan@iit.it>
 *
 *  Copy Policy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */


#ifndef YARP_MANAGER_Executable
#define YARP_MANAGER_Executable

#include <string>
#include <vector>

#include <yarp/os/RateThread.h>
#include <yarp/os/Thread.h>
#include <yarp/os/Semaphore.h>

#include <yarp/manager/ymm-types.h>
#include <yarp/manager/broker.h>
#include <yarp/manager/module.h>
#include <yarp/manager/application.h>
#include <yarp/manager/execstate.h>


namespace yarp {
namespace manager {


#define DEF_PERIOD      100  //ms
#define WDOG_PERIOD     5000 //ms

typedef enum __RSTATE {
    SUSPENDED,
    READY,
    CONNECTING,
    RUNNING,
    DYING,
    DEAD,
    STUNKNOWN
} RSTATE;


class MEvent{

public:
    MEvent() {}
    virtual ~MEvent() {}
    virtual void onExecutableStart(void* which) {}
    virtual void onExecutableStop(void* which) {}
    virtual void onExecutableDied(void* which) {}
    virtual void onExecutableFailed(void* which) {}
    virtual void onExecutableStdout(void* which, const char* msg) {}
    virtual void onCnnStablished(void* which) {}
    virtual void onCnnReleased(void* which) {}
    virtual void onCnnFailed(void* which) {}
    virtual void onError(void* which) {}
    };


class ConcurentWrapper;
class ConcurentRateWrapper;

/**
 * Class Executable
 */
class Executable : public BrokerEventSink
{
public:
    Executable(Broker* _broker, MEvent* _event, Module* module, bool bWatchDog=true);
    virtual ~Executable();

    bool start(void);
    void stop(void);
    void kill(void);

    void setID(int id) { theID = id;}
    void setCommand(const char* val) { if(val) strCommand = val; }
    void setParam(const char* val) { if(val) strParam = val; }
    void setHost(const char* val) { if(val) strHost = val; }
    void setStdio(const char* val) { if(val) strStdio = val; }
    void setWorkDir(const char* val) { if(val) strWorkdir = val; }
    void setEnv(const char* val) {if(val) strEnv = val; }    

    void addConnection(Connection &cnn) { connections.push_back(cnn); }
    CnnContainer& getConnections(void) { return connections;}
    void addResource(ResYarpPort &res) { resources.push_back(res); }
    ResourceContainer& getResources(void) { return resources; }

    RSTATE state(void);
    Broker* getBroker(void) { return broker; }
    MEvent* getEvent(void) { return event; }
    const char* getCommand(void) { return strCommand.c_str(); }
    const char* getParam(void) { return strParam.c_str(); }
    const char* getHost(void) { return strHost.c_str(); }
    const char* getStdio(void) { return strStdio.c_str(); }
    const char* getWorkDir(void) { return strWorkdir.c_str(); }
    const char* getEnv(void) { return strEnv.c_str(); }
    int getID(void) { return theID; }
    Module* getModule() { return module; }

    void setPostExecWait(double t) { waitStart = t; }
    double getPostExecWait() { return waitStart; }
    void setPostStopWait(double t) { waitStop = t; }
    double getPostStopWait() { return waitStop; }

    void enableAutoConnect(void) { bAutoConnect = true; }
    void disableAutoConnect(void) { bAutoConnect = false; }
    bool autoConnect(void) { return bAutoConnect; }

    bool startWatchDog();
    void stopWatchDog();

public: // from BrokerEventSink
    void onBrokerStdout(const char* msg) YARP_OVERRIDE;

private:
    bool bAutoConnect;
    std::string strCommand;
    std::string strParam;
    std::string strHost;
    std::string strStdio;
    std::string strWorkdir;
    std::string strEnv;
    int theID;
    double waitStart;
    double waitStop;
    bool bWatchDog;
    Broker* broker;
    MEvent* event;
    Module* module;
    CnnContainer connections;
    ResourceContainer resources;

    ExecMachine* execMachine;
    ErrorLogger* logger;
    ConcurentWrapper* startWrapper;
    ConcurentWrapper* stopWrapper;
    ConcurentWrapper* killWrapper;
    ConcurentRateWrapper* watchdogWrapper;
    yarp::os::Semaphore semInitialize;

    void startImplement(void);
    void stopImplement(void);
    void killImplement(void);
    void watchdogImplement(void);
    bool initialize(void);
};


typedef std::vector<Executable*> ExecutablePContainer;
typedef std::vector<Executable*>::iterator ExecutablePIterator;
typedef void (Executable::*ExecutableFuncPtr)(void);

class ConcurentWrapper : public yarp::os::Thread
{
public:
    ConcurentWrapper(Executable* ptrExecutable, ExecutableFuncPtr ptrLabor)
    : labor(ptrLabor), executable(ptrExecutable) { }

    virtual ~ConcurentWrapper() { if(isRunning()) stop(); }


    void run() YARP_OVERRIDE {
        if(labor && executable)
            (executable->*labor)();
    }

    //bool threadInit();
    ///void threadRelease();

private:
    ExecutableFuncPtr labor;
    Executable* executable;
};


class ConcurentRateWrapper: public yarp::os::RateThread
{
public:

    ConcurentRateWrapper(Executable* ptrExecutable, ExecutableFuncPtr ptrLabor)
    : RateThread(WDOG_PERIOD), labor(ptrLabor), executable(ptrExecutable) { }

    virtual ~ConcurentRateWrapper() { if(isRunning()) stop(); }


    void run() YARP_OVERRIDE {
        if(labor && executable)
            (executable->*labor)();
    }

    //bool threadInit();
    ///void threadRelease();

private:
    ExecutableFuncPtr labor;
    Executable* executable;
};

} // namespace yarp
} // namespace manager


#endif // __YARP_MANAGER_Executable__
