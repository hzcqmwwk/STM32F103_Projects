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

//list_task任务配置
#define LIST_TASK_PRIO		3
#define LIST_STK_SIZE 		128  
TaskHandle_t LISTTask_Handler;

//定义列表和3个列表项
List_t 	   TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;

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
    xTaskCreate((TaskFunction_t )list_task,     
                (const char*    )"list_task",   
                (uint16_t       )LIST_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LIST_TASK_PRIO,
                (TaskHandle_t*  )&LISTTask_Handler);
	
    vTaskDelete(NULL);          //删除开始任务
    taskEXIT_CRITICAL();        //退出临界区，退出后任务调度器才开始工作
}

//LED0任务函数，实现每500ms翻转一次
void led0_task(void *pvParameters)
{
    while(1)
    {
		LED0=~LED0;
        vTaskDelay(500);
    }
}   

//调用列表和列表项相关API函数，通过串口输出相应的信息
void list_task(void *pvParameters)
{
    //初始化列表和3个列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem1);
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	//打印列表和列表项的地址
	printf("项目\t\t\t地址：\r\n");
	printf("TestList\t\t0x%p\t\r\n", &TestList);
	printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
	printf("TestList->xListEnd\t0x%p\t\r\n", &(TestList.xListEnd));
	printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
	printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
	printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
	printf("***************结束******************\r\n");
	
	printf("列表项1插入列表\r\n");
	vListInsert(&TestList, &ListItem1);
	printf("项目\t\t\t地址：\r\n");
	printf("TestList->xListEnd->pxNext\t0x%p\r\n", TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext\t\t0x%p\r\n", ListItem1.pxNext);
	printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious\t\t0x%p\r\n", ListItem1.pxPrevious);
	printf("***************结束******************\r\n");
	
	printf("列表项3插入列表\r\n");
	vListInsert(&TestList, &ListItem3);
	printf("项目\t\t\t地址：\r\n");
	printf("TestList->xListEnd->pxNext\t0x%p\r\n", TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext\t\t0x%p\r\n", ListItem1.pxNext);
	printf("ListItem3->pxNext\t\t0x%p\r\n", ListItem3.pxNext);
	printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious\t\t0x%p\r\n", ListItem1.pxPrevious);
	printf("ListItem3->pxPrevious\t\t0x%p\r\n", ListItem3.pxPrevious);
	printf("***************结束******************\r\n");
	
	printf("列表项2尾插入列表\r\n");
	TestList.pxIndex = &ListItem1;
	vListInsertEnd(&TestList, &ListItem2);
	printf("项目\t\t\t地址：\r\n");
	printf("TestList->xListEnd->pxNext\t0x%p\r\n", TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext\t\t0x%p\r\n", ListItem1.pxNext);
	printf("ListItem2->pxNext\t\t0x%p\r\n", ListItem2.pxNext);
	printf("ListItem3->pxNext\t\t0x%p\r\n", ListItem3.pxNext);
	printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious\t\t0x%p\r\n", ListItem1.pxPrevious);
	printf("ListItem2->pxPrevious\t\t0x%p\r\n", ListItem2.pxPrevious);
	printf("ListItem3->pxPrevious\t\t0x%p\r\n", ListItem3.pxPrevious);
	printf("***************结束******************\r\n");
	
	printf("移除列表项2\r\n");
	uxListRemove(&ListItem2);
	printf("项目\t\t\t地址：\r\n");
	printf("TestList->xListEnd->pxNext\t0x%p\r\n", TestList.xListEnd.pxNext);
	printf("ListItem1->pxNext\t\t0x%p\r\n", ListItem1.pxNext);
	printf("ListItem3->pxNext\t\t0x%p\r\n", ListItem3.pxNext);
	printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", TestList.xListEnd.pxPrevious);
	printf("ListItem1->pxPrevious\t\t0x%p\r\n", ListItem1.pxPrevious);
	printf("ListItem3->pxPrevious\t\t0x%p\r\n", ListItem3.pxPrevious);
	printf("***************结束******************\r\n");
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
