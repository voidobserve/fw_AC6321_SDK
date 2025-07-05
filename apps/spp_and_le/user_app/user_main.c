#include "system/includes.h"

void mcpwm_handle(void *p)
{    
    static u16 duty = 0;
    static u8 dir = 0;
    
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

    mcpwm_set_duty(pwm_ch0, duty);
     
}

void user_init(void *p_param)
{
#if 0
    timer_pwm_init(JL_TIMER2 , IO_PORTA_08, 1000, (u32)10000 * 25 / 100);
    // set_timer_pwm_duty(JL_TIMER2, (u32)10000 * 25 / 100);
    // set_timer_pwm_duty(JL_TIMER2, (u32)10000 * 50 / 100);
    set_timer_pwm_duty(JL_TIMER2, 0);
#endif
    struct pwm_platform_data mcpwm_param;
    mcpwm_param.pwm_aligned_mode = pwm_edge_aligned; // 边沿对齐
    mcpwm_param.pwm_ch_num = pwm_ch0;                // 通道号
    mcpwm_param.frequency = 1000;                    // 频率 1KHz
    // mcpwm_param.duty = 5000;      //占空比50%
    mcpwm_param.duty = 0;             // 占空比 0~10000 对应 0%~100%
    mcpwm_param.h_pin = IO_PORTA_08;  // 任意引脚
    mcpwm_param.l_pin = -1;           // 任意引脚,不需要就填-1
    mcpwm_param.complementary_en = 0; // 两个引脚的波形, 0: 同步,  1: 互补，互补波形的占空比体现在H引脚上
    mcpwm_init(&mcpwm_param);

    sys_s_hi_timer_add(NULL, mcpwm_handle, 1);

    while (1)
    {
        // printf("main loop\n");
        os_time_dly(1);
    }
}