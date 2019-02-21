//#include "inc/cwmp_private.h"
#include <cwmp/cwmp.h>
#include "cwmp/queue.h"

 

int callback_register_task(cwmp_t * cwmp, callback_func_t callback, void *data1, void *data2)
{
   	qnode_t *node;
	
	node = data_in_queue(cwmp->queue, callback, TASK_CALLBACK_TAG);
	if(node)
		return CWMP_ERROR;

    queue_add(cwmp->queue, callback, TASK_CALLBACK_TAG, QUEUE_PRIORITY_HIGH, data1, data2);
    
    return CWMP_OK;
}

