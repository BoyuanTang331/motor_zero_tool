
#ifndef __SERVOIPCSTRUCT_H__
#define __SERVOIPCSTRUCT_H__

#include <stdint.h>

#define EXIPC_PORT 0

typedef struct{
    uint32_t axisCount;       //position in network
    int32_t targetPosition;      //
    int32_t targetVelocity;
    int32_t powerON;    //power
}ServoControl_t;

typedef struct{
    int32_t ReqID;
    int Status;
}ServoAck_t;

typedef struct{
    int OK;
}ServoNotice_t;

typedef struct{
    uint32_t axisCount;
    int32_t curPos;
    int32_t curVel;
    int32_t curMode;
    int32_t statusWord;
    int32_t errorCode;
}ServoSta_t;

typedef struct{
    int slotCount;
    ServoSta_t sta[1];
}ServoGroupSta_t;

typedef struct{
    char name[20];
    uint32_t axisCount;
    uint32_t VID;
    uint32_t PID;
}ServoInfo_t;

typedef struct{
    int slotCount;
    ServoInfo_t info[1];
}ServoGroupInfo_t;

//**************************************commnuication**********************************
#define IPC_LEN_MASK 0x0000ffff
typedef enum{
    SERVO_NOP = 0,
    SERVO_NOTICE = 0x00810000,
    SERVO_REQ = 0x00820000,
    SERVO_ACK = 0x00830000,
    SERVO_GROUP_INFO = 0x00840000,
    SERVO_GOURP_STA = 0x00850000,
}IpcEventID_t;

#pragma pack(1)
typedef struct{
    IpcEventID_t ID;
    union{
        ServoControl_t   cntl;
        ServoAck_t       ack;
        ServoNotice_t    notice;
        ServoGroupInfo_t groupInfo;
        ServoGroupSta_t  groupSta;
    };
}IpcEvent_t;
#pragma pack()

#endif
