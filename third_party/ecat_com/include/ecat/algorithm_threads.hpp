#ifndef __ALGORITHM_THREADS_HPP
#define __ALGORITHM_THREADS_HPP

#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "ServoIpcStruct.h"
#include <queue>
#include <memory>
#include "ecat/detail/config.hpp"
#include "ecat/thread_rtlock.hpp"

namespace ecat {

typedef struct {
    void (* Function)() = nullptr;
} callback_function;


class algorithm_threads
{
public:
    callback_function op_;
    bool flag = true;
private:
    static algorithm_threads mInstance;
    thread_rtlock lock_; 
    pthread_t al_thread;
public:
    algorithm_threads();
    ~algorithm_threads();

    static algorithm_threads& use();
    friend void *al_running(void *a);

    void algorithm(void cb());
    int algorithm_create_pthread(int priority);
    void algorithm_close_pthread();
    void algorithm_break();
};


}
#endif