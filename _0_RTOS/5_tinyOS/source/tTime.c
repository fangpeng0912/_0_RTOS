#include "tinyos.h"

////////////////ÈíÑÓÊ±º¯Êı
void tTaskDelay(uint32_t delay){
	uint32_t status = tTaskEnterCritical();

	tTimeTaskWait(currentTask, delay);
	tTaskSchedUnRdy(currentTask);
	
	tTaskExitCritical(status);
	
	tTaskSched();	
}

