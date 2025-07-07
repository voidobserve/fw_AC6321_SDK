#include "system/includes.h"

void mcpwm_handle(void *p)
{    
    static u16 duty = 0;
    static u8 dir = 0;
    
    #if 0
    if (0 == dir)
    {
        if (duty <= 10000)
        {
            duty++;
        }
        else
        {
            dir = 1;
        } 
    }
    else
    {
        if (duty > 0)
        {
            duty--;
        }
        else
        {
            dir = 0;
        }
    }

    duty = 10000 * 10 / 100;
    #endif 



    // mcpwm_set_duty(pwm_ch0, duty);
    // mcpwm_set_duty(pwm_ch0, 0);
    // mcpwm_set_duty(pwm_ch0, 10000);


    if (dir)
    {
        mctimer_muilty_chx_open_or_close(0x07, 1);
        dir = 0;
    }
    else
    {
        mctimer_muilty_chx_open_or_close(0x07, 0); 
        dir = 1;
    }
     
}

void user_init(void *p_param)
{ 
    struct pwm_platform_data mcpwm_param;
    mcpwm_param.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    mcpwm_param.pwm_ch_num = pwm_ch0;                // 通道号
    mcpwm_param.frequency = 1000;                    // 频率 1KHz
    mcpwm_param.duty = 5000;      //占空比50%
    // mcpwm_param.duty = 0;             // 占空比 0~10000 对应 0% ~ 100%
    // mcpwm_param.duty = 10000;             // 占空比 0~10000 对应 0% ~ 100%
    mcpwm_param.h_pin = IO_PORTA_08;  // 任意引脚
    mcpwm_param.l_pin = -1;           // 任意引脚,不需要就填-1
    mcpwm_param.complementary_en = 0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_param);

    mcpwm_param.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    mcpwm_param.pwm_ch_num = pwm_ch1;                // 通道号
    mcpwm_param.frequency = 1000;                    // 频率 1KHz
    mcpwm_param.duty = 5000;      //占空比50%
    // mcpwm_param.duty = 0;             // 占空比 0~10000 对应 0% ~ 100%
    // mcpwm_param.duty = 10000;             // 占空比 0~10000 对应 0% ~ 100%
    mcpwm_param.h_pin = IO_PORTA_07;  // 任意引脚
    mcpwm_param.l_pin = -1;           // 任意引脚,不需要就填-1
    mcpwm_param.complementary_en = 0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_param);

    mcpwm_param.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    mcpwm_param.pwm_ch_num = pwm_ch2;                // 通道号
    mcpwm_param.frequency = 1000;                    // 频率 1KHz
    mcpwm_param.duty = 5000;      //占空比50%
    // mcpwm_param.duty = 0;             // 占空比 0~10000 对应 0% ~ 100%
    // mcpwm_param.duty = 10000;             // 占空比 0~10000 对应 0% ~ 100%
    mcpwm_param.h_pin = IO_PORTA_06;  // 任意引脚
    mcpwm_param.l_pin = -1;           // 任意引脚,不需要就填-1
    mcpwm_param.complementary_en = 0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_param);


    sys_s_hi_timer_add(NULL, mcpwm_handle, 100);

    while (1)
    {
        // printf("main loop\n");
        os_time_dly(1);
    }
}