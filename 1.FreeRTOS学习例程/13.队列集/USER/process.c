#include "sys.h"
#include "delay.h"
#include "usart.h"
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
#define TASK1_PRIO		    2	
#define STK1_SIZE 		    128  
TaskHandle_t Task1_Handler;

//task12任务配置
#define TASK2_PRIO		    3
#define STK2_SIZE 		    128  
TaskHandle_t Task2_Handler;

//创建全局变量（队列集、队列的句柄）
QueueSetHandle_t queueset_handle;
QueueHandle_t key_queue;
QueueHandle_t semphr_handle;

//start_task任务函数，创建其他任务，并创建队列集、队列、信号量，将队列、信号量添加到队列集中
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	//创建队列集
	queueset_handle = xQueueCreateSet(2);
	if(queueset_handle != NULL)
		printf("队列集创建成功！！！\r\n");
	//创建队列
	key_queue = xQueueCreate(1, sizeof(uint8_t));
	if(key_queue != NULL)
		printf("key_queue队列创建成功\r\n");
	else
		printf("key_queue队列创建失败\r\n");
	//创建二值信号量
	semphr_handle = xSemaphoreCreateBinary();
	if(semphr_handle != NULL)
		printf("semphr_handle二值信号量创建成功\r\n");
	else
		printf("semphr_handle二值信号量创建失败\r\n");
	//将队列和信号量添加到队列集中
	xQueueAddToSet(key_queue, queueset_handle);
	xQueueAddToSet(semphr_handle, queueset_handle);
	
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

//task1任务函数，当按下key0，往队列写入数据，当按下key1，释放二值信号量
void task1(void *pvParameters)
{
    uint8_t key = 0;
	BaseType_t err = 0;
	while(1)
    {
		key = key_scan(0);
		if(key == KEY0_PRES)
		{
			err = xQueueSend(key_queue, &key, portMAX_DELAY);
			if(err != pdTRUE)
				printf("key_queue队列入队失败\r\n");
		}
		else if(key == KEY1_PRES)
		{
			err = xSemaphoreGive(semphr_handle);
			if(err != pdTRUE)
				printf("semphr_handle二值信号量释放失败\r\n");
		}
        vTaskDelay(10);
    }
}   

//task2任务函数，读取队列集中的消息，并打印
void task2(void *pvParameters)
{
    uint8_t key = 0;
	QueueSetMemberHandle_t member_handle;
	while(1)
    {
        member_handle = xQueueSelectFromSet(queueset_handle, portMAX_DELAY);
		if(member_handle == key_queue)
		{
			xQueueReceive(member_handle, &key, portMAX_DELAY);
			printf("获取到的队列数据为：%d\r\n", key);
		}
		else if(member_handle == semphr_handle)
		{
			xSemaphoreTake(member_handle, portMAX_DELAY);
			printf("获取信号量成功\r\n");
		}
			
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
