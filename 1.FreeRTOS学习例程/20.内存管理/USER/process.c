#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//task1任务配置
#define TASK1_PRIO			2	
#define STK1_SIZE 			128  
TaskHandle_t Task1_Handler;

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
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//task1任务函数，key0按下申请内存，key1按下释放内存，打印剩余内存信息
void task1(void *pvParameters)
{
    uint8_t key = 0, size = 30;
	uint8_t * buff = NULL;
	while(1)
    {
        key = key_scan(0);
		if(key == KEY0_PRES)
		{
			if(buff == NULL)
			{
				buff = pvPortMalloc(size);
				if(buff != NULL)
					printf("申请%d字节的内存成功，剩余%d字节\r\n", size, xPortGetFreeHeapSize());
				else
					printf("申请%d字节的内存失败，剩余%d字节\r\n", size, xPortGetFreeHeapSize());
			}
			else
				printf("该内存已被申请，剩余%d字节\r\n", xPortGetFreeHeapSize());
		}
		else if(key == KEY1_PRES)
		{
			if(buff != NULL)
			{
				vPortFree(buff);
				buff = NULL;
				printf("释放%d字节的内存成功，剩余%d字节\r\n", size, xPortGetFreeHeapSize());
			}
			else
				printf("该内存已被释放，剩余%d字节\r\n", xPortGetFreeHeapSize());
		}
        vTaskDelay(10);
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
