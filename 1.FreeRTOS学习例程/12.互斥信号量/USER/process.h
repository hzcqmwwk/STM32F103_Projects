#ifndef __PROCESS_H
#define __PROCESS_H

void process(void);
void start_task(void *pvParameters);
void low_task(void *pvParameters);
void mid_task(void *pvParameters);
void high_task(void *pvParameters);
 				    
#endif
