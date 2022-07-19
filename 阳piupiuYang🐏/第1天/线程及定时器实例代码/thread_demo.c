#include <rtthread.h>
#include <rtdevice.h>
#include <rtconfig.h>
#include <rtdbg.h>
#include <board.h>
#include<errno.h>
#include "thread_demo.h"
#include "aht10.h"
#define LED3_PIN GET_PIN(B, 5)
#define LED1_PIN GET_PIN(A, 8)
#define AHT10_I2C_BUS_NAME          "i2c1"  /* ���������ӵ�I2C�����豸���� */

ALIGN(RT_ALIGN_SIZE)
struct rt_thread static_thread;//��̬�����̵߳ľ��
rt_uint8_t stack_stack[2048];
rt_thread_t dynamic_thread  = RT_NULL;//��̬�����̵߳ľ��
rt_timer_t test_tim;//��ʱ�����

void tm_callback01(void *parameter){
    rt_pin_write(LED1_PIN, PIN_LOW);
    rt_kprintf("tm_callback running....LED1_OFF\n");
}

void thread_static(){
    //i2c_aht10_read_temp_humi(AHT10_I2C_BUS_NAME);
    while(1){
        rt_thread_delay(5000);
        rt_kprintf("thread01....LED1_ON\n");
        rt_pin_write(LED3_PIN, PIN_HIGH);
    }
}
void thread_dynamic() {

        rt_timer_start(test_tim);//���߳��е��ö�ʱ����ʼ����

        rt_pin_write(LED1_PIN, PIN_HIGH);
        while(1){
           rt_kprintf("thread01....LED1_OFF,thread01....LED3_ON\n");
           rt_thread_delay(5000);
           rt_pin_write(LED1_PIN, PIN_HIGH);
           rt_pin_write(LED3_PIN, PIN_LOW);
           i2c_aht10_read_temp_humi(AHT10_I2C_BUS_NAME);
        }
}


void thread_test(){
    rt_pin_mode(LED3_PIN, PIN_MODE_OUTPUT);
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);

    /*��̬��ʱ����ʼ��*/
    if( (test_tim = rt_timer_create("tim_demo",
             tm_callback01,
             NULL,
             3000,
             RT_TIMER_CTRL_SET_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER )) == RT_NULL){
        LOG_E("rt_timer_create failed...\n");
        return -ENOMEM;
    }else {
        LOG_E("rt_timer_create successed...\n");
    }
    /*��̬�̳߳�ʼ��*/
    rt_thread_init(&static_thread,
            "static",
            thread_static,
            RT_NULL,
            &stack_stack[0],
            2048,
            20,
            1000);
    /*�����߳�*/
    rt_thread_startup(&static_thread);



    /*��̬�̳߳�ʼ��*/

    dynamic_thread = rt_thread_create("dynamic",
            thread_dynamic,
            RT_NULL,
            2048,
            20,
            1000);
    rt_thread_startup(dynamic_thread);


}
MSH_CMD_EXPORT(thread_test, static thread / dynamic thread sample);
