#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//task1任务配置
#define TASK1_PRIO		    2	
#define TASK1_STK_SIZE 		128  
TaskHandle_t Task1_Handler;

//start_task任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();             //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	
    //创建task1任务
    xTaskCreate((TaskFunction_t )task1,
                (const char*    )"task1",   	
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK1_PRIO,
                (TaskHandle_t*  )&Task1_Handler);
	
    vTaskDelete(StartTask_Handler);   //删除开始任务
    taskEXIT_CRITICAL();              //退出临界区，退出后任务调度器才开始工作
}

//task1，中断测试任务，任务中将调用关中断和开中断函数来体现对中断的管理作用
void task1(void *pvParameters)
{
    uint16_t task1_num = 0;
	while(1)
    {
        task1_num++;
		if(task1_num == 5)
		{
			task1_num = 0;
			printf("关中断！！！\r\n");
			portDISABLE_INTERRUPTS();
			//这里不能用vTaskDelay，否则会直接开中断；也不能用delay_ms，它是基于vTaskDelay实现的！！！
			//for(int i = 0; i<60000000;i++);  //也可采用简单计数延迟，但时间不确定
			delay_xms(5000);
			printf("开中断！！！\r\n");
			portENABLE_INTERRUPTS();
		}
        vTaskDelay(1000);                 //vTaskDelay函数的实现中会进入临界区，退出时会开中断
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
    vTaskStartScheduler();                              //开启任务调度
}
