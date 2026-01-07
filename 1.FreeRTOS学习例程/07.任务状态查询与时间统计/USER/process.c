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

//task1任务配置
#define TASK1_PRIO		    2	
#define STK1_SIZE 		    128  
TaskHandle_t Task1_Handler;

//task2任务配置
#define TASK2_PRIO		    3
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

//task1任务函数，LED0每500ms闪烁一次
void task1(void *pvParameters)
{
	while(1)
    {
        LED0 = ~LED0;
		vTaskDelay(500);
    }
}   

//task2任务函数，展示任务状态信息查询和运行时间统计相关API函数的使用
void task2(void *pvParameters)
{
	UBaseType_t priority_num = 0;
	UBaseType_t task_num = 0;
	TaskStatus_t* status_array;
	TaskStatus_t* status_single;
	uint8_t i;
	char task_buf[300];
	
	
	vTaskPrioritySet(Task1_Handler, 30);
	priority_num = uxTaskPriorityGet(Task1_Handler);
	printf("task1的任务优先级为：%ld\r\n", priority_num);
	
	//任务数量为5（3个手动创建的以及初始化时系统创建的空闲任务和软件定时器任务）
	//start_task任务是动态申请内存创建的，删除自身时只是将其从所有状态列表中移除，当执行空闲任务时才会回收堆栈空间
	task_num = uxTaskGetNumberOfTasks();
	printf("任务数量：%ld\r\n", task_num);
	
	//获取系统中所有任务状态信息
	status_array = (TaskStatus_t*)pvPortMalloc(sizeof(TaskStatus_t)*task_num);
	task_num = uxTaskGetSystemState(status_array, task_num, NULL);
	printf("任务名\t\t任务优先级\t任务编号\r\n");  //任务编号表示第几个创建的
	for(i=0; i<task_num; i++)
		printf("%s\t\t%ld\t%ld\r\n",
			   status_array[i].pcTaskName,
			   status_array[i].uxCurrentPriority,
			   status_array[i].xTaskNumber);
			   
	//获取系统中单个任务状态信息
	status_single = (TaskStatus_t*)pvPortMalloc(sizeof(TaskStatus_t));
	vTaskGetInfo(Task1_Handler, status_single, pdTRUE, eInvalid);
	printf("任务名：%s\r\n", status_single->pcTaskName);
	printf("任务优先级：%ld\r\n", status_single->uxCurrentPriority);
	printf("任务编号：%ld\r\n", status_single->xTaskNumber);
	printf("任务状态：%d\r\n", status_single->eCurrentState);
	printf("任务栈历史剩余最小值：%d\r\n", status_single->usStackHighWaterMark);
	
	//以“表格”的形式获取系统中任务信息
	vTaskList(task_buf);
	printf("%s\r\n", task_buf);
	
	//以“表格”的形式统计任务的运行时间信息
	i = 0;
	while(1)
	{
		i = key_scan(0);
		if(i == KEY0_PRES)
		{
			vTaskGetRunTimeStats(task_buf);  //占据时间比较长，仅做调试用
			printf("%s\r\n", task_buf);
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
