#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//task1任务配置
#define TASK1_PRIO			2	
#define STK1_SIZE 			128  
TaskHandle_t Task1_Handler;

//task2任务配置
#define TASK2_PRIO			3
#define STK2_SIZE 			128  
TaskHandle_t Task2_Handler;

QueueHandle_t semphore_handle;

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

//task1任务函数，按键扫描，key0按下时释放二值信号量
void task1(void *pvParameters)
{
    uint8_t key = 0;
	BaseType_t err;
	while(1)
    {
		key = key_scan(0);
		if(key == KEY0_PRES)
		{
			if(semphore_handle != NULL)
			{
				err = xSemaphoreGive(semphore_handle);
				if(err == pdPASS)
					printf("信号量释放成功\r\n");
				else
					printf("信号量释放失败\r\n");
			}
		}
        vTaskDelay(10);
    }
}   

//task2任务函数，获取二值信号量，若成功获取就打印信息
void task2(void *pvParameters)
{
    uint32_t i = 0;
	BaseType_t err;
	while(1)
    {
		err = xSemaphoreTake(semphore_handle, 1000);
		//err = xSemaphoreTake(semphore_handle, portMAX_DELAY);  //信号量如果没有获取到就阻塞死等，task2会让出cpu使用权
		if(err == pdTRUE)
			printf("信号量获取成功\r\n");
		else
			printf("已超时 %d\r\n", ++i);
    }
}

void process(void)
{
	//创建二值信号量
	semphore_handle = xSemaphoreCreateBinary();
	if(semphore_handle != NULL)
		printf("二值信号量创建成功\r\n");
	
	//创建开始任务，参数前的括号为强制转换
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();      //开启任务调度
}
