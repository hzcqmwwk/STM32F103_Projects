#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1         //优先级
#define START_STK_SIZE 		128       //堆栈大小
TaskHandle_t StartTask_Handler;       //任务句柄

//led0_task任务配置
#define LED0_TASK_PRIO		2	
#define LED0_STK_SIZE 		128  
TaskHandle_t LED0Task_Handler;

//led1_task任务配置
#define LED1_TASK_PRIO		3
#define LED1_STK_SIZE 		128  
TaskHandle_t LED1Task_Handler;

//key_task任务配置
#define KEY_TASK_PRIO		4
#define KEY_STK_SIZE 		128  
TaskHandle_t KEYTask_Handler;

//start_task任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	
    //创建LED0任务
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //创建LED1任务
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);
	//创建KEY任务
    xTaskCreate((TaskFunction_t )key_task,     
                (const char*    )"key_task",   
                (uint16_t       )KEY_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )KEY_TASK_PRIO,
                (TaskHandle_t*  )&KEYTask_Handler);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//LED0任务函数，实现每500ms翻转一次
void led0_task(void *pvParameters)
{
    int num0 = 1;
	while(1)
    {
        printf("num0 = %d\r\n", num0++);
		LED0=~LED0;
        vTaskDelay(500);
    }
}   

//LED1任务函数，实现每1000ms翻转一次
void led1_task(void *pvParameters)
{
    int num1 = 1;
	while(1)
    {
        printf("num1 = %d\r\n", num1++);
		LED1=~LED1;
        vTaskDelay(500);
    }
}

//KEY任务函数，按下key0挂起LED0任务，按下key1在任务中恢复LED0任务，按下key_up在中断中恢复LED0任务（外部中断线实现）
void key_task(void *pvParameters)
{
    uint8_t key = 0;
	while(1)
	{
		key = key_scan(0);  //不支持连按，得到键值
		if(key == KEY0_PRES)
		{
			printf("挂起led0_task\r\n");
			vTaskSuspend(LED0Task_Handler);
		}
		else if(key == KEY1_PRES)
		{
			printf("在任务中恢复led0_task\r\n");
			vTaskResume(LED0Task_Handler);
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
