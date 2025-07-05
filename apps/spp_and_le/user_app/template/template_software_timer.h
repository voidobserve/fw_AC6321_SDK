#ifndef __TEMPLATE_SOFTWARE_TIMER_H
#define __TEMPLATE_SOFTWARE_TIMER_H

#include "../../include_lib/system/timer.h"

typedef unsigned char u8, bool, BOOL;
typedef char s8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef u32 FOURCC;
typedef long long s64;
typedef unsigned long long u64;

/*
    软件定时器接口：
*/

/*
    sys_timer 是线程内定时回调，就是在那个线程内申请的就只会在这个线程内计时，超过定时值自动调用，适合不要求时间准确，处理任务较重的回调函数

    sys_s_hi_timer 是中断回调，在可以跨线程调用，适合时间要求准确，处理任务轻的

    尽量每个定时器的申请都保存它的id，防止溢出

    单次、独立申请：
    {
        u16 xxx_timer_id = 0;
        if (xxx_timer_id == 0)
        {
            xxx_timer_id = sys_s_hi_timer_add(NULL, func, 1);
        }
    }

    重复申请
    {
        u16 xxx_timer_id = 0;
        if (xxx_timer_id)
        {
            sys_s_hi_timer_del(xxx_timer_id);
            xxx_timer_id = 0;
        }
        if (xxx_timer_id == 0)
        {
            xxx_timer_id = sys_s_hi_timer_add(NULL, func, 1);
        }
    }


    timer 会重复调用传入的回调函数，
    timeout 只会调用一次传入的回调函数


    u16 xxx_timeout_id = 0;
    void func(void *priv)
    {
        xxx_timeout_id = 0;
    }


    {
        if (xxx_timeout_id)
        {
            sys_s_hi_timeout_del(xxx_timeout_id);
            xxx_timeout_id = 0;
        }
        if (xxx_timeout_id == 0)
            xxx_timeout_id = sys_s_hi_timeout_add(NULL, func, 1);
    }
    调用这个相当于：
    1、如果之前没有申请这个超时定时器，则申请；
    2、如果之前申请过这个超时定时器，则删除它（不会再执行计时），然后重新申请一个新的超时定时器，重新计时

    比如，我要保护我的FLASH，减少擦写的次数，我设计了当需要保存的数据发生变更，就调用sys_timeout_add()来达到5秒后写FLASH，
    在没达到5秒超时之前，我有变更了这个数据，再次调用，就相当于重新计时，再等5秒才写FLASH

    如果你需要做的功能是，进行一次申请timeout就让它的func一定要执行，可以不用id来做限制，直接

    void func(void *priv)
    {
    }

    {
        sys_s_hi_timeout_add(NULL, func, 1000);
    }
    比如，我就是需要将一个东西滞后1秒来处理，就不用加id来做限制了
    比方说，有个功能需要每100ms采集一次将数据放入个缓存数组buff[ ]里，然后每次采集的数据要1秒后才处理，就在func里调用buff[ ]这样


    timeout还有禁忌，不要在他的回调函数func()里调用sys_s_hi_timeout_del()，系统会自动删除的，如果在里面调用了会重复删除导致溢出
 
*/
// sys_timer
extern u16 sys_timer_add(void *priv, void (*func)(void *priv), u32 msec);
extern void sys_timer_del(u16 timer_id);
extern u16 sys_timeout_add(void *priv, void (*func)(void *priv), u32 msec);
extern void sys_timeout_del(u16 timer_id);

// sys_s_hi_timer，usr_timer
extern u16 sys_s_hi_timer_add(void *priv, void (*func)(void *priv), u32 msec);
extern void sys_s_hi_timer_del(u16 timer_id);
extern u16 sys_s_hi_timerout_add(void *priv, void (*func)(void *priv), u32 msec, u8 priority);
extern void sys_s_hi_timeout_del(u16 timer_id);

#endif
