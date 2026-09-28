#ifndef __PWM_HPP
#define __PWM_HPP

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h> 
#include <string.h>
#include <time.h>


namespace ecat {

#define PERIPH_BASE     		     	0x40000000
#define MPU_AHB4_PERIPH_BASE			PERIPH_BASE + 0x10000000
#define RCC_BASE        		    	MPU_AHB4_PERIPH_BASE + 0x0000	
#define RCC_MP_AHB4ENSETR				0xa28
#define MP_APB1ENSETR                   0xa00

#define time5_base  0x40003000
#define time3_base  0x40001000
#define time2_base  0x40000000
#define time13_base 0x40007000

#define ARR         0x2c
#define CCR1        0x34
#define CR1_        0x00
#define PSC         0x28
#define CCMR1       0x18
#define CCER        0x20

#define  GPIOA_BASE     0x50002000
#define  GPIOC_BASE     0x50004000
#define  GPIOG_BASE     0x50008000
#define  MODE           0x00
#define  AFRL           0x20
#define  AFRH           0x24

struct rcc
{
    unsigned char *rcc_BASE;

    volatile unsigned int *rcc_mp;
    volatile unsigned int *mp;

    unsigned int rcc_mp_buf;
    unsigned int mp_buf;
};

struct gpio
{
    unsigned char *gpio_BASE;

    volatile unsigned int *mode;
    volatile unsigned int *afrl;

    unsigned int mode_buf;
    unsigned int afrl_buf;
};

struct reg
{
    unsigned char *tim_base;

    volatile unsigned int *arr;
    volatile unsigned int *ccr1;
    volatile unsigned int *cr1;
    volatile unsigned int *psc;
    volatile unsigned int *ccmr1;
    volatile unsigned int *ccer;
    
    unsigned int arr_buf;
    unsigned int ccr1_buf;
    unsigned int cr1_buf;
    unsigned int psc_buf;
    unsigned int ccmr1_buf;
    unsigned int ccer_buf;
};

class PWM
{
protected:
    PWM();
    ~PWM();
    static PWM pwm;

    void mapping(unsigned char **ptr, int fd, off_t offset);
public:
    static PWM& usePWM();

    int init();
    void end();

    void pwm_1_init(struct reg *regs);
    void pwm_2_init(struct reg *regs);
    void pwm_3_init(struct reg *regs);
    void pwm_4_init(struct reg *regs);
    void pwm_open(int ch);

    void charge_pwm(struct reg *regs, int cnt);
    int pwm_cnt(int ch, int cnt);

    void pwm_disable(struct reg *regs);
    void pwm_all_disable();
protected:
    struct rcc mRcc;
    struct gpio gpioA, gpioC, gpioG;
    struct reg mReg[4];

    int dev_fd;
    long page_size;
};

}


#endif