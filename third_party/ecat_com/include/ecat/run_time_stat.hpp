
#pragma once

#include <string.h>
#include <cstdint>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include <time.h>

namespace ecat
{

#define MaxStatNodeNum 20

typedef struct RunTimeInfo{
    char statTitle[64] = {0};
    int64_t exeTime = 0;  // 执行的时间戳，单位 us
    int minTime = -1;
    int maxTime = -1;
    int avgTime = 0;
    int totalTime = 0;
    int cycleTimes = 0;

    int lastMaxTime = 0;
}RunTimeInfo;

typedef struct {
    char flag = 0;
    int maxDelay = 0;
    int minDelay = 0;
    int avgDelay = 0;
}RunTimeData;

class RTTools                                
{
public:
    static void usleep(int us);
    static int clock_gettime(clockid_t __clock_id, timespec *__tp);
};

class RunStat
{
public:
    RunStat(){mRTData = new RunTimeData();}
    ~RunStat(){}

    void statDotStart();   
    void statDot(int statID, const char* title);
    void statAgv();
    void statLoop();
    void setEcCycleTime(int32_t cycle){ecCycleTime = cycle;}
    
    RunTimeInfo mRunTime[MaxStatNodeNum];
    int mStatSize = 0;
    int mCycleTimes = 0;
    //int mStatCycleTimes = 1000;
    long mPeriodStartTime = 0;
    bool mNeedPrint = false;

    bool mErrorStop = false;
    bool mRecordFlag = false;       // 是否进行 操作耗时打印
    bool mRecordLoopFlag = true;    // 是否进行 通信抖动打印

    RunTimeInfo mLatency;
    int32_t ecCycleTime;

    RunTimeData* mRTData = nullptr;
};

}
