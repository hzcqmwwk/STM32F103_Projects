// 为了方便观察现象，将FreeRTOS的节拍频率改为20Hz，即一个时间片由1ms变为50ms
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//task1任务配置
#define TASK1_PRIO		    2	
#define STK1_SIZE 		    128  
TaskHandle_t Task1_Handler;

//task2任务配置
#define TASK2_PRIO		    2
#define STK2_SIZE 		    128  
TaskHandle_t Task2_Handler;

//start_task任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	
    //创建task1任务
    xTaskCreate((TaskFunction_t )task1,     	
                (const char*    )"task1",   	
                (uint16_t       )STK1_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_PRIO,	
                (TaskHandle_t*  )&Task1_Handler);   
    //创建task2任务
    xTaskCreate((TaskFunction_t )task2,     
                (const char*    )"task2",   
                (uint16_t       )STK2_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK2_PRIO,
                (TaskHandle_t*  )&Task2_Handler);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//task1任务函数，通过串口打印运行次数
//执行printf语句也需要时间，那么执行一次while循环的时间大于10ms，给一次时间片50ms时间，能循环4-5次
void task1(void *pvParameters)
{
    uint32_t task1_num = 0;
	while(1)
    {
        taskENTER_CRITICAL();   //打印前后进出临界区为了使打印完整，防止出现串口输出到一半被切换任务导致乱码
		printf("task1运行次数：%d\r\n", ++task1_num);
		taskEXIT_CRITICAL();
        delay_xms(10);
    }
}   

//task2任务函数，通过串口打印运行次数
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
	while(1)
    {
        taskENTER_CRITICAL();
		printf("task2运行次数：%d\r\n", ++task2_num);
		taskEXIT_CRITICAL();
        delay_xms(10);
    }
}

void process(void)
{
	//创建开始任务，参数前的括号为强制转换
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();      //开启任务调度
}
