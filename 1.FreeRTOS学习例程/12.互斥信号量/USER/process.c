#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//low_task任务配置
#define LOW_TASK_PRIO		2	
#define LOW_STK_SIZE 		128  
TaskHandle_t LOWTask_Handler;

//mid_task任务配置
#define MID_TASK_PRIO		3
#define MID_STK_SIZE 		128  
TaskHandle_t MIDTask_Handler;

//high_task任务配置
#define HIGH_TASK_PRIO		4
#define HIGH_STK_SIZE 		128  
TaskHandle_t HIGHTask_Handler;

QueueHandle_t mutex_semphore_handle;

//start_task任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	
    //创建low_task任务
    xTaskCreate((TaskFunction_t )low_task,     	
                (const char*    )"low_task",   	
                (uint16_t       )LOW_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LOW_TASK_PRIO,	
                (TaskHandle_t*  )&LOWTask_Handler);   
    //创建mid_task任务
    xTaskCreate((TaskFunction_t )mid_task,     
                (const char*    )"mid_task",   
                (uint16_t       )MID_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )MID_TASK_PRIO,
                (TaskHandle_t*  )&MIDTask_Handler);
	//创建high_task任务
    xTaskCreate((TaskFunction_t )high_task,     
                (const char*    )"high_task",   
                (uint16_t       )HIGH_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )HIGH_TASK_PRIO,
                (TaskHandle_t*  )&HIGHTask_Handler);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//low_task任务函数
void low_task(void *pvParameters)
{
    while(1)
    {
        printf("low_task获取信号量\r\n");
		xSemaphoreTake(mutex_semphore_handle, portMAX_DELAY);
		printf("low_task正在运行\r\n");
		delay_xms(3000);
		printf("low_task释放信号量\r\n");
		xSemaphoreGive(mutex_semphore_handle);
        vTaskDelay(1000);
    }
}   

//mid_task任务函数，
void mid_task(void *pvParameters)
{
    while(1)
    {
        printf("mid_task正在运行\r\n");
        vTaskDelay(1000);
    }
}

//high_task任务函数，
void high_task(void *pvParameters)
{
	while(1)
	{
		printf("high_task获取信号量\r\n");
		xSemaphoreTake(mutex_semphore_handle, portMAX_DELAY);
		printf("high_task正在运行\r\n");
		delay_xms(1000);
		printf("high_task释放信号量\r\n");
		xSemaphoreGive(mutex_semphore_handle);
        vTaskDelay(1000);
	}
}

void process(void)
{
	mutex_semphore_handle = xSemaphoreCreateMutex();  //互斥信号量创建时会主动释放一次
	if(mutex_semphore_handle != NULL)
		printf("互斥信号量创建成功！！！\r\n");
	
	//创建开始任务，参数前的括号为强制转换
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();      //开启任务调度
}
