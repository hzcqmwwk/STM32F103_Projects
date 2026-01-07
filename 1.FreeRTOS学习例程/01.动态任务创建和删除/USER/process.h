#ifndef __PROCESS_H
#define __PROCESS_H

void process(void);
void start_task(void *pvParameters);
void led0_task(void *pvParameters);
void led1_task(void *pvParameters);
void key_task(void *pvParameters);
 				    
#endif
