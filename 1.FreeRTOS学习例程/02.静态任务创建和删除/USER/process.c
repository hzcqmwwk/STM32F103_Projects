#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "FreeRTOS.h"
#include "task.h"
#include "process.h"

//start_task任务配置
#define START_TASK_PRIO		1                   // 优先级
#define START_STK_SIZE 		128                 // 堆栈大小
TaskHandle_t StartTask_Handler;                 // 任务句柄
StackType_t start_task_stack[START_STK_SIZE];   // 任务堆栈
StaticTask_t start_task_tcb;                    // 任务控制块

//led0_task任务配置
#define LED0_TASK_PRIO		2
#define LED0_STK_SIZE 		50
TaskHandle_t LED0Task_Handler;
StackType_t led0_task_stack[LED0_STK_SIZE];
StaticTask_t led0_task_tcb;

//led1_task任务配置
#define LED1_TASK_PRIO		3
#define LED1_STK_SIZE 		50  
TaskHandle_t LED1Task_Handler;
StackType_t led1_task_stack[LED1_STK_SIZE];
StaticTask_t led1_task_tcb;

//key_task任务配置
#define KEY_TASK_PRIO		4
#define KEY_STK_SIZE 		50  
TaskHandle_t KEYTask_Handler;
StackType_t key_task_stack[KEY_STK_SIZE];
StaticTask_t key_task_tcb;

//空闲任务内存分配
StaticTask_t idle_task_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, StackType_t ** ppxIdleTaskStackBuffer, uint32_t * pulIdleTaskStackSize )
{
	*ppxIdleTaskTCBBuffer = &idle_task_tcb;
	*ppxIdleTaskStackBuffer = idle_task_stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

//软件定时器任务内存分配
StaticTask_t timer_task_tcb;
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, StackType_t ** ppxTimerTaskStackBuffer, uint32_t * pulTimerTaskStackSize )
{
	*ppxTimerTaskTCBBuffer = &timer_task_tcb;
	*ppxTimerTaskStackBuffer = timer_task_stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

//start_task任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();       //进入临界区，关闭任务切换，防止开始任务还未完全执行完 任务调度器就开始工作
	
    //创建LED0任务
    LED0Task_Handler = xTaskCreateStatic((TaskFunction_t ) led0_task,
                                         (char *         ) "led0_task",
                                         (uint32_t       ) LED0_STK_SIZE,
                                         (void *         ) NULL,
                                         (UBaseType_t    ) LED0_TASK_PRIO,
                                         (StackType_t *  ) led0_task_stack,
                                         (StaticTask_t * ) &led0_task_tcb);
    //创建LED1任务
    LED1Task_Handler = xTaskCreateStatic((TaskFunction_t ) led1_task,
                                         (char *         ) "led1_task",
                                         (uint32_t       ) LED1_STK_SIZE,
                                         (void *         ) NULL,
                                         (UBaseType_t    ) LED1_TASK_PRIO,
                                         (StackType_t *  ) led1_task_stack,
                                         (StaticTask_t * ) &led1_task_tcb);
	//创建KEY任务
	KEYTask_Handler = xTaskCreateStatic((TaskFunction_t ) key_task,
                                        (char *         ) "key_task",
                                        (uint32_t       ) KEY_STK_SIZE,
                                        (void *         ) NULL,
                                        (UBaseType_t    ) KEY_TASK_PRIO,
                                        (StackType_t *  ) key_task_stack,
                                        (StaticTask_t * ) &key_task_tcb);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//LED0任务函数，实现每500ms翻转一次
void led0_task(void *pvParameters)
{
    while(1)
    {
        printf("led0_task正在运行\r\n");
		LED0=~LED0;
        vTaskDelay(500);
    }
}   

//LED1任务函数，实现每500ms翻转一次
void led1_task(void *pvParameters)
{
    while(1)
    {
        printf("led1_task正在运行\r\n");
		LED1=~LED1;
        vTaskDelay(500);
    }
}

//KEY任务函数，判断按键key0，按下key0删除LED0任务函数，判断按键key1，按下key1删除LED1任务函数
void key_task(void *pvParameters)
{
    uint8_t key = 0;
	while(1)
	{
		printf("key_task正在运行\r\n");
		key = key_scan(0);  //不支持连按，得到键值
		printf("key = %d\r\n", key);
		if(key == KEY0_PRES)
		{
			if(LED0Task_Handler != NULL)
			{
				printf("删除led0_task\r\n");
				vTaskDelete(LED0Task_Handler);
				LED0Task_Handler = NULL;
			}
		}
		else if(key == KEY1_PRES)
		{
			if(LED1Task_Handler != NULL)
			{
				printf("删除led1_task\r\n");
				vTaskDelete(LED1Task_Handler);
				LED1Task_Handler = NULL;
			}
		}
		vTaskDelay(10);
	}
}

//执行的入口函数
void process(void)
{
	//创建开始任务，参数前的括号为强制转换
    StartTask_Handler = xTaskCreateStatic((TaskFunction_t ) start_task,       // 任务函数(指向任务函数的指针)
										  (char *         ) "start_task",     // 任务名称
                                          (uint32_t       ) START_STK_SIZE,   // 任务堆栈大小
                                          (void *         ) NULL,             // 传入给任务函数的参数(无为NULL)
                                          (UBaseType_t    ) START_TASK_PRIO,  // 任务优先级
                                          (StackType_t *  ) start_task_stack, // 任务堆栈，一般为数组，由用户自分配
                                          (StaticTask_t * ) &start_task_tcb); // 任务句柄，任务控制块指针，由用户分配(用taskhandle_t定义,传入地址)
    vTaskStartScheduler();      //开启任务调度
}
