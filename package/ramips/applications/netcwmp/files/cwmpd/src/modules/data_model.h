#ifndef __CWMP_DATA_MODEL_H__
#define __CWMP_DATA_MODEL_H__

#include <cwmp/cwmp.h>
#include "cmdlib.h"
#include "../../../libcwmp/include/cwmp/http.h"

int get_index_after_paramname(parameter_node_t * param, const char * tag_name);
void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile);

int find_client_list_wlan(ARPTable *client_list);

#endif
