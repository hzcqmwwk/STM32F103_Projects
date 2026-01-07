#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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

//task13任务配置
#define TASK3_PRIO		    4
#define STK3_SIZE 		    128  
TaskHandle_t Task3_Handler;

//创建全局变量（队列的句柄和一个大数组）
QueueHandle_t key_queue;
QueueHandle_t big_data_queue;
char buff[100] = "我是一个大数组，大大的数组666";

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
	//创建task3任务
    xTaskCreate((TaskFunction_t )task3,
                (const char*    )"task3",   
                (uint16_t       )STK3_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )TASK3_PRIO,
                (TaskHandle_t*  )&Task3_Handler);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//task1任务函数，当按下key0或key1，将键值拷贝到队列key_queue（入队），当按下key_up，将传输大数据，拷贝大数据的地址到队列big_data_queue中
void task1(void *pvParameters)
{
    uint8_t key = 0;
	BaseType_t err = 0;
	char * buf = &buff[0];
	while(1)
    {
		key = key_scan(0);
		if(key == KEY0_PRES || key == KEY1_PRES)
		{
			err = xQueueSend(key_queue, &key, portMAX_DELAY);
			if(err != pdTRUE)
				printf("key_queue队列入队失败\r\n");
		}
		else if(key == WKUP_PRES)
		{
			err = xQueueSend(big_data_queue, &buf, portMAX_DELAY);
			if(err != pdTRUE)
				printf("big_data_queue队列入队失败\r\n");
		}
        vTaskDelay(10);
    }
}   

//task2任务函数，读取队列key_queue中的消息（出队），打印接收到的键值
//由于task2任务优先级高于task1，所以当有数据由task1写入队列后，task2会由阻塞态抢占执行，将数据读出并删除，队列为空后再次进入阻塞
void task2(void *pvParameters)
{
    uint8_t key = 0;
	BaseType_t err = 0;
	while(1)
    {
        err = xQueueReceive(key_queue, &key, portMAX_DELAY);  //队列为空时会阻塞等待，让出cpu使用权
		if(err != pdTRUE)
			printf("key_queue队列出队失败\r\n");
		else
			printf("key_queue队列出队成功，数据：%d\r\n", key);
    }
}

//task3任务函数，从队列big_data_queue中读取大数据的地址，通过地址访问大数据
void task3(void *pvParameters)
{
	char * buf;
	BaseType_t err = 0;
	while(1)
	{
		err = xQueueReceive(big_data_queue, &buf, portMAX_DELAY);
		if(err != pdTRUE)
			printf("big_data_queue队列出队失败\r\n");
		else
			printf("big_data_queue队列出队成功，数据：%s\r\n", buf);
	}
}

void process(void)
{
	//创建队列
	key_queue = xQueueCreate(2, sizeof(uint8_t));
	if(key_queue != NULL)
		printf("key_queue队列创建成功\r\n");
	else
		printf("key_queue队列创建失败\r\n");
	big_data_queue = xQueueCreate(1, sizeof(char*));
	if(big_data_queue != NULL)
		printf("big_data_queue队列创建成功\r\n");
	else
		printf("big_data_queue队列创建失败\r\n");
	
	//创建开始任务，参数前的括号为强制转换
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();      //开启任务调度
}
