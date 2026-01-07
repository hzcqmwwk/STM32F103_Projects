#ifndef __PROCESS_H
#define __PROCESS_H

void process(void);
void start_task(void *pvParameters);
void task1(void *pvParameters);
void task2(void *pvParameters);
void PRE_SLEEP_PROCESSING(void);
void POST_SLEEP_PROCESSING(void);
 				    
#endif
