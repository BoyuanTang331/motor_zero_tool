#ifndef ECAT_IPC_HPP
#define ECAT_IPC_HPP

#include "ServoIpcStruct.h"
#include <queue>
#include <necro_ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>
#include "ecat/detail/config.hpp"
#include "ecat/thread_rtlock.hpp"

namespace ecat {


class ipc
{
private:
    static ipc mInstance;
    uint8_t rxframebuf[512];
    std::queue<ServoControl_t *> qtList;
    std::queue<ServoControl_t *> alList;
    thread_rtlock lock_qt;
    thread_rtlock lock_al;
public:
    ipc();
    ~ipc();

    static ipc& use();
    int Ipc_init(int chip);
    int Ipc_deinit(int socketFd);
    int Ipc_Send(int socketFd, uint8_t *data, int len);
    int Ipc_Receive(int socketFd, uint8_t *data, int len);
    int ServoIpcPoll(int socketFd, ServoControl_t **pReq);

    void qt_push(ServoControl_t *pTmpReq);
    ServoControl_t * qt_pop();
    bool qt_isEmpty();
    int qt_size();
    // ServoControl_t *qt_front();

    void al_push(ServoControl_t *pTmpReq);
    ServoControl_t * al_pop();
    bool al_isEmpty();
    int al_size();
    // ServoControl_t *al_front();

    void mem_delete(ServoControl_t *pTmpReq);
};



}
#endif
