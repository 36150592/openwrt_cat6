/************************************************************************
 * Id: cwmp.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014 netcwmp group                         *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/

#include "cwmp/cwmp.h"
#include "cwmp/buffer.h"
#include "cwmp/log.h"
#include "cwmp/event.h"
#include "cwmp_private.h"
#include "cmdlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include "sys/stat.h"



#define TMP_LTE_INFO_FILE "/tmp/.lte_info"

#define CWMP_BUF_SIZE 128
#define CWMP_RENEW_SIZE CWMP_NAME_MAX

#ifdef WIN32


//#define ESA(x, y) do { (x) = (y); if( (x) == NULL) { return NULL; } } while (0)
#define ESA(x, y)   (x) = (y); if( (x) == NULL)  return NULL
#define ESN(x, y)   if((x) != (y))  return NULL
#define ESQ(x, y)   if((x) == (y))  return NULL

//not eq
#define ESNE(x,y,z)  if((y) != (z)) return (x)
//eq
#define ESE(x,y,z)  if((y) == (z)) return (x)


#else


//#define ESA(x, y) do { (x) = (y); if( (x) == NULL) { return NULL; } } while (0)
#define ESA(x, y)  do { (x) = (y); if( (x) == NULL) { printf("ASSERT: \"%s\" is NULL. file: %s , func: %s , line: %d\n", #x, __FILE__, __func__, __LINE__); return NULL; } } while(0)
#define ESN(x, y)  do { if((x) != (y))  { printf("ASSERT: \"%s\" != \"%s\"  file: %s , func: %s , line: %d\n", #x, #y, __FILE__, __func__, __LINE__); return NULL; } } while (0)
#define ESQ(x, y)    do { if((x) != (y)) return NULL ; } while(0)
//true
#define ESNE(x,y,z) do { if((y) != (z))  { printf("ASSERT: \"%s\" != \"%s\"  file: %s , func: %s , line: %d\n", #y, #z, __FILE__, __func__, __LINE__); return (x); } } while (0)
//eq
#define ESE(x,y,z)  do { if((y) == (z))  { printf("ASSERT: \"%s\" == \"%s\"  file: %s , func: %s , line: %d\n", #y, #z, __FILE__, __func__, __LINE__); return (x); } } while (0)


//#define ESA(x, y) do { (x) = (y); if( (x) == NULL) { return NULL; } } while (0)
#define ESAVOID(x, y)  do { (x) = (y); if( (x) == NULL) { printf("ASSERT: \"%s\" is NULL. file: %s , func: %s , line: %d\n", #x, __FILE__, __func__, __LINE__); return ; } } while(0)
#define ESNVOID(x, y)  do { if((x) != (y))  { printf("ASSERT: \"%s\" != \"%s\"  file: %s , func: %s , line: %d\n", #x, #y, __FILE__, __func__, __LINE__); return ; } } while (0)






#endif


char param[1024];
IGD_Entries igd_entries = {4, 1, 3, 1, 1, 1, 1, 1, 1, 1, 10, 0};
Global_Vars glb_vars;

static char SOAP_ENV[CWMP_NAME_MAX] = {0};
static char SOAP_ENC[CWMP_NAME_MAX] = {0};

static char SOAP_ENV_HEADER[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_BODY[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_ENVELOPE[CWMP_NAME_MAX] = {0};
static char SOAP_ENV_FAULT[CWMP_NAME_MAX]={0};
static char SOAP_ENC_ARRAYTYPE[CWMP_NAME_MAX] = {0};


//static parameter_node_t* gs_root_parameter_node = NULL;


#define CWMP_TYPE(x) cwmp_get_type_string(x)





static char * cwmp_get_format_string(const char * fmt, ...)
{
    va_list ap;

    static char g_cwmp_format_string[1024] = {0};
    va_start(ap, fmt);
#ifdef WIN32
    _vsnprintf(g_cwmp_format_string, 1023, fmt, ap);
#else
    vsnprintf(g_cwmp_format_string, 1023, fmt, ap);
#endif

    va_end(ap);
    return g_cwmp_format_string;
}


char * cwmp_get_type_string(int type)
{
    switch (type)
    {
    case TYPE_OBJECT:
        return "object";
    case TYPE_STRING:
        return "xsd:string";
    case TYPE_INT:
        return "xsd:int";
    case TYPE_UNSIGNEDINT:
        return "xsd:unsignedInt";
    case TYPE_STRING32:
        return "xsd:string[32]";
    case TYPE_STRING64:
        return "xsd:string[64]";
    case TYPE_STRING128:
        return "xsd:string[128]";
    case TYPE_STRING256:
        return "xsd:string[256]";
    case TYPE_STRING1024:
        return "xsd:string[1024]";
    case TYPE_BOOLEAN:
        return "xsd:bool";
    case TYPE_DATETIME:
        return "xsd:DateTime";
    default:
        return "";
    }


}



int cwmp_get_type_value(char * type)
{
//    	TYPE_OBJECT=0,	//obj
//	TYPE_INT,	//int
//	TYPE_UNSIGNEDINT, //uint
//	TYPE_STRING,  	//s
//	TYPE_STRING16,	//s16
//	TYPE_STRING32,	//s32
//	TYPE_STRING64,	//s64
//	TYPE_STRING128,	//s128
//	TYPE_STRING256,	//s256
//	TYPE_STRING1024, //s1024
//	TYPE_DATETIME,	//dt
//	TYPE_BOOLEAN,	//bool
//	TYPE_BASE64,	//base

    if(type == NULL)
    {
        return TYPE_UNKNOWN;
    }

    if(! TRstrcasecmp(type, "int"))
    {
        return TYPE_INT;
    }
    else if(! TRstrcasecmp(type, "uint"))
    {
        return TYPE_UNSIGNEDINT;
    }
    else if(! TRstrcasecmp(type, "s"))
    {
        return TYPE_STRING;
    }
    else if(! TRstrcasecmp(type, "s16"))
    {
        return TYPE_STRING16;
    }
    else if(! TRstrcasecmp(type, "s32"))
    {
        return TYPE_STRING32;
    }
    else if(! TRstrcasecmp(type, "s64"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s128"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s256"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "s1024"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "dt"))
    {
        return TYPE_STRING64;
    }
    else if(! TRstrcasecmp(type, "bool"))
    {
        return TYPE_STRING64;
    }
     else if(! TRstrcasecmp(type, "base"))
    {
        return TYPE_BASE64;
    }
    else if(! TRstrcasecmp(type, "obj"))
    {
        return TYPE_OBJECT;
    }

    return TYPE_STRING;

}




char * cwmp_get_fault_string(int code)
{
    //char * fault_string;
    switch(code)
    {
        case 9000:
            return  FAULT_STR_9000;

        case 9001:
            return  FAULT_STR_9001;

        case 9002:
            return  FAULT_STR_9002;

        case 9003:
            return  FAULT_STR_9003;

        case 9004:
            return  FAULT_STR_9004;

        case 9005:
            return  FAULT_STR_9005;

        case 9006:
            return  FAULT_STR_9006;

        case 9007:
            return  FAULT_STR_9007;

        case 9008:
            return  FAULT_STR_9008;

        case 9009:
            return  FAULT_STR_9009;

        case 9010:
            return  FAULT_STR_9010;

        case 9011:
            return  FAULT_STR_9011;

        case 9012:
            return  FAULT_STR_9012;

        case 9013:
            return  FAULT_STR_9013;

        case 9014:
            return  FAULT_STR_9014;

        case 9015:
            return  FAULT_STR_9015;

        case 9016:
            return  FAULT_STR_9016;

        case 9017:
            return  FAULT_STR_9017;

        case 9018:
            return  FAULT_STR_9018;

        case 9019:
            return  FAULT_STR_9019;
        default:
            return "";

    }

}


void get_gmtime(time_t *t, struct tm *gm)
{
	struct tm *p;
		
	p=localtime(t);
	memcpy(gm,p,sizeof(struct tm));
}

char *parse_time(time_t *t)
{
	static char buff[64];
	struct tm gm;

	if(t == NULL) return NULL;

	get_gmtime(t, &gm);
	
	sprintf(buff,
		"%04d-%02d-%02d %02d:%02d:%02d",
		gm.tm_year+1900,
		gm.tm_mon+1,
		gm.tm_mday,
		gm.tm_hour,
		gm.tm_min,
		gm.tm_sec);

	return buff;
}

char *trim_end_line(char *value, int n) {
	register int i;

	for (i = n; i >= 0; i--) {
		if (value[i] == '\0') {
			continue;
		}

		if (value[i] == '\n' || value[i] == '\r') {
			value[i] = '\0';
		} else {
			break;
		}
	}

	return value;
}


xmldoc_t * cwmp_xml_parse_buffer(pool_t * pool, char * buffer)
{
    return XmlParseBuffer(pool, buffer);

}

const char * cwmp_xml_get_node_name(xmlnode_t * node)
{
    xmlnode_t * child;
    child = XmlNodeGetFirstChild(node);
    return XmlNodeGetNodeName(child);

}


char * cwmp_xml_get_node_value(xmlnode_t * node)
{
    return XmlNodeGetNodeValue(XmlNodeGetFirstChild(node));

}

char * cwmp_xml_get_node_attribute(xmlnode_t * node, const char * name)
{
    return XmlElementGetAttribute((XmlElement*)node, name);

}


xmlnode_t * cwmp_xml_get_child_with_name(void * nodeptr, const char * nodeName)
{
    xmlnode_t * children;
    xmlnode_t * node = (xmlnode_t *)nodeptr;
    if (node == NULL)
    {
        cwmp_log_error("Invalid parameter 'param' (null)");
        return NULL;
    }

    children = XmlNodeGetFirstChild(node);
    while (children != NULL)
    {
        if (children->nodeType != XML_ELEMENT_NODE)
        {
            children = XmlNodeGetNextSibling(children);
        }
        else
        {
            if (! TRstrcasecmp(children->nodeName, nodeName))
            {
                //cwmp_log_debug("cwmp_xml_get_child_with_name found node(%p)\n", children);
                break;
            }
            else
                children = XmlNodeGetNextSibling(children);
        }
    }

    return children;
}

xmlnode_t * cwmp_xml_create_child_node(env_t * env ,  xmlnode_t * parentNode, const char * ns, const char * nodeName, const char * nodeValue)
{
    XmlElement * newNode;
    pool_t * pool = env->pool;

    assert(parentNode != NULL);
    newNode = ( XmlElement *  ) PMALLOC( sizeof( XmlElement ) );
    if ( newNode == NULL )
    {
        cwmp_log_error("cwmp_xml_create_child_node XMALLOC is error: newNode\n" );
        return NULL;
    }
    else
    {
        XmlElementInit( newNode );
        XmlElementSetTagName(pool , newNode, nodeName);
        XmlNodeSetNodeName(pool ,  (xmlnode_t *)newNode, nodeName );
        XmlNodeSetNodeValue(pool , (xmlnode_t *)newNode, nodeValue );


        XmlNodeAppendChild(parentNode, (xmlnode_t *)newNode);

    }

    return (xmlnode_t *)newNode;
}

void cwmpd_zte_base64_encode(const char* data, int data_len, char* encode, int encode_len) 
{ 
    //int data_len = strlen(data); 
    int prepare = 0; 
    int ret_len; 
    int temp = 0; 
    char *ret = NULL; 
    char *f = NULL; 
    int tmp = 0; 
    char changed[4]; 
    int i = 0; 
    const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="; 
    
    if(data == NULL)
    {
        return;
    }
    if(encode == NULL)
    {
        return;
    }
    if(data_len == 0)
    {
        return;
    }
    ret_len = data_len / 3; 
    temp = data_len % 3; 
    if (temp > 0) 
    { 
        ret_len += 1; 
    } 
    ret_len = ret_len*4 + 1; 
    ret = (char *)malloc(ret_len); 
    
    if (ret == NULL) 
    { 
        printf("No enough memory.\n"); 
        return;
    } 
    memset(ret, 0, ret_len); 
    f = ret; 
    while (tmp < data_len) 
    { 
        temp = 0; 
        prepare = 0; 
        memset(changed, '\0', 4); 
        while (temp < 3) 
        { 
            //printf("tmp = %d\n", tmp); 
            if (tmp >= data_len) 
            { 
                break; 
            } 
            prepare = ((prepare << 8) | (data[tmp] & 0xFF)); 
            tmp++; 
            temp++; 
        } 
        prepare = (prepare<<((3-temp)*8)); 
        //printf("before for : temp = %d, prepare = %d\n", temp, prepare); 
        for (i = 0; i < 4 ;i++ ) 
        { 
            if (temp < i) 
            { 
                changed[i] = 0x40; 
            } 
            else 
            { 
                changed[i] = (prepare>>((3-i)*6)) & 0x3F; 
            } 
            *f = base[(int)changed[i]]; 
            //printf("%.2X", changed[i]); 
            f++; 
        } 
    } 
    *f = '\0'; 
    strncpy(encode, ret, encode_len - 1);
    if(ret != NULL)
    {
        free(ret);
    }  
}

int cwmp_xml_set_node_attribute(env_t * env,  xmlnode_t * node, const char * name, const char * value)
{
    int rv = XmlElementSetAttribute(env->pool, (XmlElement *)node, name, value);
    if (rv == XML_OK)
        return CWMP_OK;
    else
        return CWMP_ERROR;
}

int nv_cfg_set(const char *name,const char *value)
{
	int len = strlen(name) + 3;
	int len2 = strlen(value) + 3;
	char buffer_v[len2];
	char buffer_n[len];
	
	strcpy(buffer_n,name);
	strcpy(buffer_v,value);

	

	return 1;
}

int nv_cfg_get_item(const char *name, char* buf, int bufLen)
{
	int len = strlen(name) + 3;
	char buffer_n[len];
	
	strcpy(buffer_n,name);
	
	buffer_n[strlen(buffer_n)] = '\0';
	
	return 1;
}



void cwmp_xml_copy_to_chunk_with_escape(cwmp_chunk_t * cb,   IN char *p , pool_t * pool)
{
    int i;
    size_t plen;

    if ( p == NULL )
        return;

    plen = TRstrlen( p );

    for ( i = 0; i < plen; i++ )
    {
        switch ( p[i] )
        {
        case '<':
            cwmp_chunk_write_string(cb, "&lt;", 4, pool);
            break;

        case '>':
            cwmp_chunk_write_string(cb, "&gt;", 4, pool);
            break;

        case '&':
            cwmp_chunk_write_string(cb, "&amp;", 5, pool);
            break;

        case '\'':
            cwmp_chunk_write_string(cb, "&apos;", 6, pool);
            break;

        case '\"':
            cwmp_chunk_write_string(cb, "&quot;", 6, pool);
            break;

        default:
            cwmp_chunk_write_string(cb, &p[i], 1, pool);

            break;
        }
    }
}


int cwmp_xml_dom_tree_print_to_chunk(xmlnode_t * node, cwmp_chunk_t * cb, pool_t * pool )
{
    char *nodeName = NULL;
    char *nodeValue = NULL;
    xmlnode_t *  child = NULL;
    xmlnode_t *  sibling = NULL;

    if ( node != NULL )
    {
        nodeName = ( char * )XmlNodeGetNodeName( node )? ( char * )XmlNodeGetNodeName( node ):"";
        nodeValue = XmlNodeGetNodeValue( node );
        cwmp_log_debug("dom tree xmlnode type is %d\n", XmlNodeGetNodeType( node ));
        switch ( XmlNodeGetNodeType( node ) )
        {

        case XML_TEXT_NODE:
            cwmp_xml_copy_to_chunk_with_escape(cb, nodeValue, pool);

            break;

        case XML_CDATA_SECTION_NODE:
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            break;


        case XML_PROCESSING_INSTRUCTION_NODE:
            cwmp_chunk_write_string(cb, "<?", 2, pool);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
            cwmp_chunk_write_string(cb, " ", 1, pool);
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            cwmp_chunk_write_string(cb, "?>\n", 3, pool);

            break;

        case XML_DOCUMENT_NODE:
            cwmp_xml_dom_tree_print_to_chunk(XmlNodeGetFirstChild( node ), cb, pool);

            break;

        case XML_ATTRIBUTE_NODE:
            cwmp_log_debug	("dom tree attribute: %s,%s\n", nodeName, nodeValue);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
            cwmp_chunk_write_string(cb, "=\"", 2, pool);
            cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
            cwmp_chunk_write_string(cb, "\"", 1, pool);

            if ( node->nextSibling != NULL )
            {
                cwmp_chunk_write_string(cb, " ", 1, pool);
                cwmp_xml_dom_tree_print_to_chunk(node->nextSibling, cb, pool);
            }
            break;

        case XML_ELEMENT_NODE:
            cwmp_chunk_write_string(cb, "<", 1, pool);
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);

            if ( node->firstAttr != NULL )
            {
                cwmp_chunk_write_string(cb, " ", 1, pool);
                cwmp_xml_dom_tree_print_to_chunk(node->firstAttr, cb, pool);

            }

            child = XmlNodeGetFirstChild( node );
            if ( ( child != NULL )
                    && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
            {
                cwmp_chunk_write_string(cb, ">\n", 2, pool);

                cwmp_xml_dom_tree_print_to_chunk(node, cb, pool);
                //  output the children

            }
            else
            {
                cwmp_chunk_write_string(cb, ">", 1, pool);

                if (nodeValue)
                {
                    cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);

                }
            }

            cwmp_chunk_write_string(cb, "</", 2, pool);
            // Done with children.  Output the end tag.
            cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);

            sibling = XmlNodeGetNextSibling( node );
            if ( sibling != NULL
                    && XmlNodeGetNodeType( sibling ) == XML_TEXT_NODE )
            {
                cwmp_chunk_write_string(cb, ">", 1, pool);

            }
            else
            {
                cwmp_chunk_write_string(cb, ">\n", 2, pool);
            }
            cwmp_xml_dom_tree_print_to_chunk(sibling, cb, pool);

            break;

        default:
            break;
        }
    }
    return XML_OK;
}


int cwmp_xml_print_doc_to_chunk(xmldoc_t *   doc, cwmp_chunk_t * cb, pool_t * pool )
{
    xmlnode_t * nodeptr = &doc->node;


    char *nodeName = NULL;
    char *nodeValue = NULL;
    xmlnode_t *  child = NULL;

    if ( nodeptr == NULL)
    {
        return CWMP_OK;
    }

    nodeName = ( char * )XmlNodeGetNodeName( nodeptr )? ( char * )XmlNodeGetNodeName( nodeptr ) :"";
    nodeValue = XmlNodeGetNodeValue( nodeptr );

    switch ( XmlNodeGetNodeType( nodeptr ) )
    {

    case XML_TEXT_NODE:
    case XML_CDATA_SECTION_NODE:
    case XML_PROCESSING_INSTRUCTION_NODE:
    case XML_DOCUMENT_NODE:
        cwmp_xml_dom_tree_print_to_chunk(nodeptr, cb, pool);

        break;

    case XML_ATTRIBUTE_NODE:
        cwmp_log_debug	("attribute: %s,%s\n", nodeName, nodeValue);
        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        cwmp_chunk_write_string(cb, "=\"", 2, pool);
        cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);
        cwmp_chunk_write_string(cb, "\"", 1, pool);
        break;

    case XML_ELEMENT_NODE:
        cwmp_chunk_write_string(cb, "<", 1, pool);

        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        if ( nodeptr->firstAttr != NULL )
        {
            cwmp_chunk_write_string(cb, " ", 1, pool);
            cwmp_xml_dom_tree_print_to_chunk(nodeptr->firstAttr, cb, pool);
        }




        child = XmlNodeGetFirstChild( nodeptr );
        if ( ( child != NULL )
                && ( XmlNodeGetNodeType( child ) == XML_ELEMENT_NODE ) )
        {
            cwmp_chunk_write_string(cb, ">\n", 2, pool);

            cwmp_xml_dom_tree_print_to_chunk(XmlNodeGetFirstChild( nodeptr ), cb, pool);
            //  output the children

        }
        else
        {
            cwmp_chunk_write_string(cb, ">", 1, pool);

            if (nodeValue)
            {
                cwmp_chunk_write_string(cb, nodeValue, TRstrlen(nodeValue), pool);

            }
        }



        // Done with children.  Output the end tag.
        cwmp_chunk_write_string(cb, "</", 2, pool);
        cwmp_chunk_write_string(cb, nodeName, TRstrlen(nodeName), pool);
        cwmp_chunk_write_string(cb, ">\n", 2, pool);
        break;

    default:
        break;
    }
    return CWMP_OK;
}


void cwmp_set_envelope_ns(const char * envstr, const char * encstr)
{
    const char * envs;
    const char * encs;
    char buffer[CWMP_NAME_MAX] = {0};
    if (envstr == NULL)
    {
        envs = SOAP_ENV_DEFAULT;
    }
    else
    {
        envs = envstr;
    }

    if (encstr == NULL)
    {
        encs = SOAP_ENC_DEFAULT;
    }
    else
    {
        encs = encstr;
    }

    if(TRstrcasecmp(SOAP_ENV, envs) == 0)
    {
        return;
    }

    cwmp_log_debug("ENV: %s, ENC: %s", envs, encs);
    TRstrncpy(SOAP_ENV,  envs, CWMP_NAME_MAX);
    TRstrncpy(SOAP_ENC,  encs, CWMP_NAME_MAX);


    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_HEADER);
    TRstrncpy(SOAP_ENV_HEADER, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_BODY);
    TRstrncpy(SOAP_ENV_BODY, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_FAULT);
    TRstrncpy(SOAP_ENV_FAULT, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", envs, SOAP_XML_ENVELOPE);
    TRstrncpy(SOAP_ENV_ENVELOPE, buffer, CWMP_NAME_MAX);

    TRsnprintf(buffer, CWMP_NAME_MAX, "%s:%s", encs, SOAP_TYPE_ARRAYTYPE);
    TRstrncpy(SOAP_ENC_ARRAYTYPE, buffer, CWMP_NAME_MAX);

    cwmp_log_debug("%s\n%s\n%s\n%s\n", SOAP_ENV_HEADER, SOAP_ENV_BODY, SOAP_ENV_ENVELOPE, SOAP_ENC_ARRAYTYPE);


}

void cwmp_set_faultcode(fault_code_t * fault, int code)
{
    fault->fault_code = code;
//    fault->fault_string = FAULT_STRING(code);
}


void cwmp_initialize_header(header_t * header)
{
    TRBZERO(header->id, CWMP_NAME_MAX);
    header->hold_requests = -1;
    header->no_more_requests = -1;
}


parameter_node_t * cwmp_initialize_parameter_node(env_t * env ,
        parameter_node_t * root, const char * name,
        int	rw,
        int	type,
        //const char * type,
        const char * value,
        parameter_get_handler_pt get,
        parameter_set_handler_pt set,
        parameter_notify_handler_pt notify)
{
    parameter_node_t * node;
    char * nodename;
    cwmp_log_debug("cwmp_initialize_parameter_node ...\n");
    if (cwmp_create_parameter_node(env ,  &node, name) != 0)
    {
        return NULL;
    }

    nodename = strrchr(name, '.');
    if (nodename)
    {
        node->name = XSTRDUP(nodename + 1);
    }
    else
    {
        node->name = XSTRDUP(name);
    }


    node->rw = rw;
    node->type = type;
    //node->type = type;
    node->value = XSTRDUP(value);
    if (value)
    {
        node->value_length = TRstrlen(value);
    }
    node->get = get;
    node->set = set;
    node->notify = notify;

    return node;
}





int cwmp_add_child_parameter_node(parameter_node_t * parent, parameter_node_t * child)
{
    parameter_node_t * node;
    cwmp_log_debug("cwmp_add_child_parameter_node ...\n");

    for (node = parent->child; node && node->next_sibling; node = node->next_sibling);

    if (node)
    {
        node->next_sibling = child;
    }
    else
    {
        parent->child = child;
    }

    child->prev_sibling = node;
    child->parent = parent;
    return 0;
}

int  cwmp_add_parameter_to_list(env_t * env ,  parameter_list_t * pl, parameter_t * parameter)
{
    if (pl->count >= pl->size-1)
    {
        parameter_t ** pp = XREALLOC(pl->parameters, pl->size * sizeof(parameter_t*), sizeof(parameter_t*) * (pl->size+CWMP_RENEW_SIZE));
        pl->parameters = pp;
        pl->size += CWMP_RENEW_SIZE;
    }
    pl->parameters[pl->count++] = parameter;
    return CWMP_OK;
}






void  cwmp_add_event_to_list(env_t * env ,  event_list_t * eventList, event_code_t * event)
{
    eventList->events[eventList->count++] = event;
}


int cwmp_split_parameter_name_list(char * name, char * namelist[])
{
    int i = 0;
    char * p;
    char * s = name;
    namelist[i++] = name;

    while ((p = strstr(s, ".")))
    {
        (*p) = 0;
        p ++;
        namelist[i++] = p;
    }
    namelist[i] = 0;

    return 0;
}


xmlnode_t * cwmp_get_header_node(xmldoc_t *  doc)
{
    xmlnode_t *  node;
    xmlnode_t *  root;
    ASSERT(doc != NULL);

    if (! (root = XmlNodeGetDocRoot(doc)))
    {
        cwmp_log_error("xml document root is null!");
        return NULL;
    }

    node = cwmp_xml_get_child_with_name(root, SOAP_ENV_HEADER);
    if (node == NULL)
    {
        cwmp_log_debug("xml soap header not found1!");
    }

    //lizd
    if(node == NULL){
    	if(strcmp(SOAP_ENV_HEADER, "SOAP-ENV:Header")==0){
    		node = cwmp_xml_get_child_with_name(root, "soap:Header");
		    if (node == NULL)
		    {
		        cwmp_log_info("xml soap header not found2!");
		    }
		 } 
	}
	
    return node;
}


xmlnode_t * cwmp_get_body_node(xmldoc_t *  doc)
{
    xmlnode_t *  node;
    xmlnode_t *  root;
    ASSERT(doc != NULL);
    if (! (root = XmlNodeGetDocRoot(doc)))
    {
        cwmp_log_error("xml document root is null!");
        return NULL;
    }

    node = cwmp_xml_get_child_with_name(root, SOAP_ENV_BODY);
    if (node == NULL)
    {
        cwmp_log_info("xml soap body not found1!");
    }

    //lizd
    if(node == NULL){
		if(strcmp(SOAP_ENV_BODY, "SOAP-ENV:Body")==0){
    		node = cwmp_xml_get_child_with_name(root, "soap:Body");
		    if (node == NULL)
		    {
		        cwmp_log_info("xml soap body not found2!");
		    }
		 } 
	}
	
    return node;
}


xmlnode_t *  cwmp_get_rpc_method_node(xmldoc_t *  doc)
{
    xmlnode_t * body;
    body = cwmp_get_body_node(doc);
    if (!body)
    {
        return NULL;
    }
    return XmlNodeGetFirstChild(body);
}


xmlnode_t * cwmp_get_rpc_node(xmldoc_t *   doc, const char * method)
{
    xmlnode_t * node;
    node = cwmp_get_rpc_method_node(doc);
    if (!node)
    {
        cwmp_log_error("doc get method is null!");
        return NULL;
    }
    if (TRstrcmp(node->nodeName, method))
    {
        cwmp_log_debug("doc get method(%s) is not %s!", node->nodeName, method);
        return NULL;
    }
    return node;
}

char * cwmp_get_rpc_method_name(xmldoc_t *  doc)
{
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    if (!node)
    {
        return NULL;
    }
    return node->nodeName;
}

/*
   xmlnode_t * GetRpcInform(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_INFORM);
   }

   xmlnode_t * GetRpcInformResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_INFORMRESPONSE);
   }

   xmlnode_t * GetRpcGetParameterNames(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETPARAMETERNAMES);
   }

   xmlnode_t * GetRpcGetParameterNamesResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETPARAMETERNAMESRESPONSE);
   }

   xmlnode_t * GetRpcGetParameterValues(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETPARAMETERVALUES);
   }

   xmlnode_t * GetRpcGetParameterValuesResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETPARAMETERVALUESRESPONSE);
   }

   xmlnode_t * GetRpcSetParameterValues(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_SETPARAMETERVALUES);
   }

   xmlnode_t * GetRpcSetParameterValuesResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_SETPARAMETERVALUESRESPONSE);
   }

   xmlnode_t * GetRpcGetRPCMethods(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETRPCMETHODS);
   }

   xmlnode_t * GetRpcGetRPCMethodsResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_GETRPCMETHODSRESPONSE);
   }

   xmlnode_t * GetRpcDownload(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_DOWNLOAD);
   }

   xmlnode_t * GetRpcDownloadResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_DOWNLOADRESPONSE);
   }

   xmlnode_t * GetRpcUpload(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_UPLOAD);
   }

   xmlnode_t * GetRpcUploadResponse(xmldoc_t *  doc)
   {
   return cwmp_get_rpc_node(doc, CWMP_RPC_UPLOADRESPONSE);
   }

xmlnode_t * GetRpcReboot(xmldoc_t *  doc)
{
	return cwmp_get_rpc_node(doc, CWMP_RPC_REBOOT);
}

xmlnode_t * GetRpcRebootResponse(xmldoc_t *  doc)
{
	return cwmp_get_rpc_node(doc, CWMP_RPC_REBOOTRESPONSE);
}

*/

parameter_node_t * get_node_after_paramname(parameter_node_t * param, char *name)
{
	parameter_node_t * child;
	parameter_node_t * tmp;
	parameter_node_t * next;

	if(!param) 
		return NULL; 
	
	if(TRstrcmp(param->name, name) == 0)	
		return param;
	
	child = param->child;
	if(!child)
		return NULL; 
	
	tmp = get_node_after_paramname(child, name);
	if(tmp != NULL)
		return tmp;

	next = child->next_sibling;
	while(next)
	{
		tmp = get_node_after_paramname(next, name);
		if(tmp != NULL)
			return tmp;
			
		next = next->next_sibling;
	}	
	
	return NULL; 
}


char * cwmp_get_parameter_nodename(const char * name, char * buffer)
{
    char *p = (char *)name;
    char *q = buffer;

    while (*p)
    {
        if (*p == '.')
            break;

        *q++ = *p++;
    }

    if (*p) p++;

    *q = '\0';

    return p;
}




parameter_node_t * cwmp_get_parameter_node(parameter_node_t * root, const char * param_name)
{
    parameter_node_t * node = root;
    char * dot;
    char  name[256];

    if ((!node) || (!param_name)){
    	cwmp_log_info("cwmp_get_parameter_node bad param");
        return NULL;
    }

    dot = (char*)param_name;
    while (*dot)
    {

        dot = cwmp_get_parameter_nodename(dot, name);
        while (node && node->name)
        {
            if (TRstrcmp(node->name, name) == 0)
            {

                break;
            }
            node = node->next_sibling;
        }

        if (!node)
        {
            return NULL;
        }

        if ((dot) && (*dot == 0))
        {
            break;
        }

        node = node->child;
    }
    if (!node)
    {
        cwmp_log_error("Not found param node: %s\n", param_name);
    }
    return node;
}


parameter_node_t * cwmp_get_parameter_path_node(parameter_node_t * parent, const char * param_name)
{
    parameter_node_t * param_node = parent;
    const char * dot;
    char  name[256];
    
    //tz_log_info("get path node, param_name=%s\n", param_name);

    if ((!param_node) || (!param_name))
        return NULL;

    dot = (char *)param_name;
    while (*dot)
    {

        dot = cwmp_get_parameter_nodename(dot, name);

        //tz_log_info("chknode dot=%s name=%s nodeName=%s val=%s", dot, name, param_node->name, param_node->value);

        while (param_node && param_node->name)
        {

        	//tz_log_info("    chknode name=%s nodeName=%s", name, param_node->name);
        	
	    	if(TRstrcmp(param_node->name, "{i}") == 0)
            {
				param_node = param_node->next_sibling;
				continue;
            }


            if (TRstrcmp(param_node->name, name) == 0)
            {
                //cwmp_log_debug("Found param node: %s\n", name);
                break;
            }
            if (param_node->next_sibling)
            {
                param_node = param_node->next_sibling;
            }
            else
            {
                if (*dot != 0)
                {
                    cwmp_log_error("Error param node path. %s\n", param_name);
                    return NULL;
                }
                else
                {
                    if (param_node->parent)
                    {
                        //cwmp_log_debug("Found param node path: %s.\n", param_node->parent->name);
                    }
                    else
                    {
                        cwmp_log_info("Not found param node parent path: %s.\n", param_name);
                    }
                    return param_node->parent;
                }
            }
        }

        if (!param_node)
        {
            return NULL;
        }

        if ((dot) && (*dot == 0))
        {
            break;
        }
        if (param_node->child)
        {
            param_node = param_node->child;
        }
        else
        {
            break;
        }

    }
    if (param_node)
    {
        cwmp_log_debug("Found param node path: %s.\n", param_node->name);
    }
    else
    {
        cwmp_log_error("Not found param node path: %s.\n", param_name);
    }
    return param_node;

}

int cwmp_get_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, char ** value, pool_t * pool)
{

    if (!node)
    {
        return FAULT_CODE_9000;
    }
    if (node->get)
    {
        return (*node->get)(cwmp, name, value, pool);
    }
    else
    {
        return FAULT_CODE_9000;
    }
}

int cwmp_set_parameter_node_value(cwmp_t * cwmp, parameter_node_t * node, const char * name, const char * value, int value_length)
{

    if (!node)
    {
        return CWMP_ERROR;
    }
    if (node->set)
    {
        return (*node->set)(cwmp, name,  value, value_length, callback_register_task);
    }
    else
    {
        if (node->value)
        {
            FREE(node->value);
        }

	      //it's ok , no memory less
        node->value = TRstrdup(value);
        node->value_length = value_length;
        return CWMP_OK;
    }
}


int cwmp_parse_header_node(xmlnode_t * node, header_t ** header, pool_t * pool)
{
    xmlnode_t * cwmpIdNode;
    xmlnode_t * cwmpHoldRequestsNode;
    xmlnode_t * cwmpNoMoreRequestsNode;
    char * value;

    *header  = NULL;
    if (node)
    {
        (*header) = pool_pcalloc(pool, sizeof(header_t));
        cwmpIdNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_ID);
        cwmpHoldRequestsNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_HOLDREQUESTS);
	cwmpNoMoreRequestsNode = cwmp_xml_get_child_with_name(node, CWMP_XML_HEADER_NOMOREREQUESTS);
        if (cwmpIdNode == NULL || cwmpHoldRequestsNode == NULL || cwmpNoMoreRequestsNode == NULL)
        {
            cwmp_log_debug("TR069Header cwmp:ID=%s, cwmp:HoldRequests=%s, cwmp:NoMoreRequests=%s",
                           cwmpIdNode ? cwmp_xml_get_node_value(cwmpIdNode):"null",
                           cwmpHoldRequestsNode ? cwmp_xml_get_node_value(cwmpHoldRequestsNode):"null",
                           cwmpNoMoreRequestsNode ? cwmp_xml_get_node_value(cwmpNoMoreRequestsNode):"null");
        }
        value = cwmp_xml_get_node_value(cwmpIdNode);
        if ((cwmpIdNode != NULL) || (value != NULL))
        {
            (*header)->id = pool_pcalloc(pool, CWMP_HEAD_MAX+1);
            TRstrncpy((*header)->id, value, CWMP_HEAD_MAX);
        }

        if (cwmpHoldRequestsNode != NULL)
        {
            value = cwmp_xml_get_node_value(cwmpHoldRequestsNode);
            (*header)->hold_requests = TRatoi(value);
        }

        if (cwmpNoMoreRequestsNode != NULL)
        {
            value = cwmp_xml_get_node_value(cwmpNoMoreRequestsNode);
            (*header)->no_more_requests = TRatoi(value);
        }


        return CWMP_OK;
    }

    return CWMP_ERROR;
}




int cwmp_parse_inform_response_message(xmlnode_t * node, unsigned int *max_envelopes)
{
    xmlnode_t * cwmpMaxEnvelopes;
    const char * value;
    if (node)
    {
        cwmpMaxEnvelopes = cwmp_xml_get_child_with_name(node, CWMP_XML_INFORM_MAXENVELOPES);
        if (!cwmpMaxEnvelopes)
        {
            return CWMP_ERROR;
        }
        value = XmlNodeGetNodeValue(cwmpMaxEnvelopes);
        if (!value)
        {
            cwmp_log_error("Invalid InformResponse MaxEnvelopes is null");
            return CWMP_ERROR;
        }
        (*max_envelopes) = TRatoi(value);
        return CWMP_OK;
    }

    return CWMP_ERROR;
}

//cwmp_parse_getrpcmethods_message
xmldoc_t * cwmp_parse_getrpcmethods_message(xmldoc_t *doc)
{
    return NULL;
}


int cwmp_parse_getparameternames_message(env_t * env, xmldoc_t * doc, char ** path_name, unsigned int * next_level, fault_code_t *fault)
{
    xmlnode_t * cwmpParamPath;
    xmlnode_t * cwmpNextLevel;
    const char * nl;
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);

    cwmpParamPath = cwmp_xml_get_child_with_name(node, CWMP_XML_GETPARAMETERNAMES_PARAMETERPATH);
    cwmpNextLevel = cwmp_xml_get_child_with_name(node, CWMP_XML_GETPARAMETERNAMES_NEXTLEVEL);
    if (path_name)
    {
        *path_name = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmpParamPath));

    }
    nl = cwmp_xml_get_node_value(cwmpNextLevel);
    if (!nl)
    {
        cwmp_log_error("Invalid GetParameterNames NextLevel is null");
        nl = "0";
    }
    if (TRstrcmp(nl,"true") == 0 || TRstrcmp(nl,"1") == 0)
    {
        (*next_level) = 1;
    }
    else
    {
        (*next_level) = 0;
    }
    return CWMP_OK;
}

int cwmp_parse_get_single_parameter_value(env_t * env, pool_t * pool, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault, char *name)
{
    int rc;
	parameter_node_t * pn = cwmp_get_parameter_node(root, name);
	if (!pn)
	{
		//Create Fault code;
		fault->fault_code = FAULT_CODE_9003;
		cwmp_log_error("can not find parameter %s.", name);
		return CWMP_ERROR;
	}
	else
	{
		parameter_t * parameter;// = cwmp_create_parameter(env ,  name, NULL, 0, pn->type);

		if (pn->get)
		{
			//exec get
			char * value = NULL;
			rc = (*pn->get)(env->cwmp, name, &value, pool);
			if(rc != FAULT_CODE_OK)
			{
			   fault->fault_code = rc;
			   return CWMP_ERROR;

			}
			
			parameter = cwmp_create_parameter(env ,  name, value, TRstrlen(value), pn->type);

		}
		else
		{
			parameter = cwmp_create_parameter(env ,  name, pn->value, pn->value_length, pn->type);
		}

		if (!parameter)
		{
			return CWMP_OK;
		}

		if ((*ppl)->count >= (*ppl)->size - 1)
		{
			parameter_t ** pp = XREALLOC((*ppl)->parameters, (*ppl)->size * sizeof(parameter_t*), sizeof(parameter_t*) * ((*ppl)->size + CWMP_RENEW_SIZE));
			if (!pp)
			{
				return CWMP_ERROR;
			}
			(*ppl)->parameters = pp;
			(*ppl)->size += CWMP_RENEW_SIZE;
		}
		
		parameter_t ** pv;
		pv = (*ppl)->parameters;
		*(pv + (*ppl)->count) = parameter;
		(*ppl)->count++;

	}
	
	return CWMP_OK;

}

void * cwmp_parse_walk_node(env_t * env , pool_t * pool, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault, parameter_node_t *  param_node, const char * path_name, int * count)
{
    char buffer[256];
    parameter_node_t * param_child;

    if (!param_node)
        return NULL;
	
    for (param_child = param_node->child; param_child!=NULL; param_child = param_child->next_sibling)
    {
        if(TRstrcmp(param_child->name, "{i}") == 0 || 
			TRstrcmp(param_child->name, "IPPingDiagnostics") == 0 ||
			TRstrcmp(param_child->name, "TraceRouteDiagnostics") == 0)
            continue;
		
        memset(buffer, 0, sizeof(buffer));
        if (param_child->type == TYPE_OBJECT)
        {
            sprintf(buffer, "%s%s.", path_name, param_child->name);
            //cwmp_log_debug("walk obj %s", buffer);
        	cwmp_parse_walk_node(env, pool, root, ppl, fault, param_child, buffer, count);
        }
        else
        {
            sprintf(buffer, "%s%s", path_name, param_child->name);
            //cwmp_log_debug("walk param %s", buffer);
			cwmp_parse_get_single_parameter_value(env, pool, root, ppl, fault, buffer);
        }
    }

    return NULL;
}


int cwmp_parse_getparametervalues_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
   // cwmp_buffer_t buffer;
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
	parameter_node_t *node;
	
    char name[256];
    int	count = 0;
	int rv = 0;

    parameterListNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "ParameterNames");


    if (!parameterListNode || !ppl)
    {
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env );
    ESE(CWMP_ERROR, NULL, *ppl);

    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);

    parameterNode = XmlNodeGetFirstChild(parameterListNode);
    while (parameterNode)
    {
        char *pt_name = cwmp_xml_get_node_value(parameterNode);
        if(pt_name == NULL)
            strcpy(name, "InternetGatewayDevice.");
        else
            strcpy(name, pt_name);
        // cwmp_log_debug("Name: %s\n", name);
		if (name[strlen(name)-1] == '.')
		{
			node = cwmp_get_parameter_path_node(root, name);
			if(node!=NULL)
				cwmp_parse_walk_node(env, pool, root, ppl, fault, node, name, &count);
		}
		else
		{
			rv = cwmp_parse_get_single_parameter_value(env, pool, root, ppl, fault, name);
			if (rv == CWMP_ERROR) {
				if(fault->fault_code != FAULT_CODE_OK)
				{
				   return CWMP_ERROR;
				}
				break;
			}
		}

        parameterNode = XmlNodeGetNextSibling(parameterNode);

    }

    return CWMP_OK;
}


//cwmp_parse_setparametervalues_message
int  cwmp_parse_setparametervalues_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    parameter_t ** nextpv;
    int rc = CWMP_OK;

    parameterListNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "ParameterList");


    if (!parameterListNode || !ppl)
    {
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env);
    ESE(CWMP_ERROR, NULL, *ppl);

    nextpv = (*ppl)->parameters;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);

    while (parameterNode)
    {
		xmlnode_t * pnode  = parameterNode;

		parameterNode = XmlNodeGetNextSibling(parameterNode);

        const char * name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Name"));
        const char * value = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Value"));
		cwmp_log_debug("set parameter value (%s=%s)", name, value);
		parameter_t * parameter = cwmp_create_parameter(env ,  name, value, TRstrlen(value), 0);
		
		if (!parameter)
        {
            //faild
            continue;
        }
        
        parameter_node_t * pn = cwmp_get_parameter_node(root, name);
        if (!pn)
        {
            //Create Fault code;
            parameter->fault_code = FAULT_CODE_9003;
            cwmp_log_error("can not find parameter %s.", name);
            continue;
        }
        else
        {
            parameter->type = pn->type;

            if(pn->set)
            {
				//exec set function
				parameter->fault_code =  (*pn->set)(env->cwmp, name,  value, TRstrlen(value), callback_register_task);
            }
		    else
		    {
				parameter->fault_code = FAULT_CODE_9008;
		    }

		    if(parameter->fault_code != FAULT_CODE_OK)
		    {
				cwmp_set_faultcode(fault, FAULT_CODE_9003);
				rc = CWMP_ERROR;
		    }

	        if ((*ppl)->count >= (*ppl)->size - 1)
	        {
	            parameter_t ** pp = XREALLOC((*ppl)->parameters, (*ppl)->size * sizeof(parameter_t*), sizeof(parameter_t*) * ((*ppl)->size + CWMP_RENEW_SIZE));
	            if (!pp)
	            {
	                continue;
	            }
	            
	            (*ppl)->parameters = pp;
	            (*ppl)->size += CWMP_RENEW_SIZE;
	        }
	        
			(*ppl)->count += 1;
	        *nextpv = parameter;
			nextpv++;

		}
    }

    return rc;
}

int  cwmp_parse_setparameterattributes_message(env_t * env , xmldoc_t * doc, parameter_node_t * root, parameter_list_t ** ppl, fault_code_t *fault)
{
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterNode;
    parameter_t ** nextpv;
    int rc = CWMP_OK;

    parameterListNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "ParameterList");


    if (!parameterListNode || !ppl)
    {
        return CWMP_ERROR;
    }

    *ppl = cwmp_create_parameter_list(env);
    ESE(CWMP_ERROR, NULL, *ppl);

    nextpv = (*ppl)->parameters;

    parameterNode = XmlNodeGetFirstChild(parameterListNode);

    while (parameterNode)
    {
		xmlnode_t * pnode  = parameterNode;
		xmlnode_t *pAccessListNode, *pStringNode;

		parameterNode = XmlNodeGetNextSibling(parameterNode);

        const char * name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Name"));
        const char * notificationChange = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "NotificationChange"));
        const char * notification = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "Notification"));
        const char * accessListChange = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(pnode, "AccessListChange"));
        char *accListString=NULL;

		pAccessListNode = cwmp_xml_get_child_with_name(pnode, "AccessList");
		if(pAccessListNode != NULL){
			pStringNode = cwmp_xml_get_child_with_name(pAccessListNode, "string");
			if(pStringNode != NULL){
				accListString = cwmp_xml_get_node_value(pStringNode);
			}
        }
        
		cwmp_log_debug("set attr name=%s notiChg=%s noti=%s accChg=%s str=%s", 
						name, notificationChange, notification, accessListChange, accListString);
		
		parameter_t * parameter = cwmp_create_parameter(env ,  name, NULL, 0, 0);
		
		if (!parameter)
        {
            //faild
            continue;
        }
        
        parameter_node_t * pn = cwmp_get_parameter_node(root, name);
        if (!pn)
        {
            //Create Fault code;
            parameter->fault_code = FAULT_CODE_9003;
            cwmp_log_error("can not find parameter %s.", name);
            continue;
        }
        else
        {
            parameter->type = pn->type;

            if(pn->setattr)
            {
				//exec set function
				parameter_list_t *accessList = cwmp_create_parameter_list(env);
    			ESE(CWMP_ERROR, NULL, accessList);

				parameter_t *parameter_list_node = cwmp_create_parameter(env,  "string", accListString, 0, TYPE_STRING);
    			cwmp_add_parameter_to_list(env, accessList, parameter_list_node);
    			
				parameter->fault_code =  (*pn->setattr)(env->cwmp, 
														name,  
														atoi(notificationChange), 
														atoi(notification),
														accessList,
														atoi(accessListChange), 
														callback_register_task);

														
														
            }
		    else
		    {
				parameter->fault_code = FAULT_CODE_9008;
		    }

		    if(parameter->fault_code != FAULT_CODE_OK)
		    {
				cwmp_set_faultcode(fault, FAULT_CODE_9003);
				rc = CWMP_ERROR;
		    }

	        if ((*ppl)->count >= (*ppl)->size - 1)
	        {
	            parameter_t ** pp = XREALLOC((*ppl)->parameters, (*ppl)->size * sizeof(parameter_t*), sizeof(parameter_t*) * ((*ppl)->size + CWMP_RENEW_SIZE));
	            if (!pp)
	            {
	                continue;
	            }
	            
	            (*ppl)->parameters = pp;
	            (*ppl)->size += CWMP_RENEW_SIZE;
	        }
	        
			(*ppl)->count += 1;
	        *nextpv = parameter;
			nextpv++;

		}
    }

    return rc;
}



int cwmp_parse_download_message(env_t * env , xmldoc_t *doc, download_arg_t ** pdlarg, fault_code_t *fault)
{
    //xmlnode_t * commandKeyNode;
    //xmlnode_t * cwmpNextLevel;
    //const char * nl;
    FUNCTION_TRACE();
    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    download_arg_t * dlarg = pool_pcalloc(env->pool, sizeof(download_arg_t));

   //FIXME
   dlarg->cmdkey = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "CommandKey")));
   dlarg->filetype= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileType")));
   dlarg->url = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "URL")));
   dlarg->username= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Username")));
   dlarg->password = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Password")));
   dlarg->filesize= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileSize")));

   dlarg->targetname= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "TargetFileName")));
   dlarg->delaysec= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "DelaySeconds")));
   dlarg->succurl= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "SuccessURL")));
   dlarg->failurl= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FailureURL")));

    *pdlarg = dlarg;

    return CWMP_OK;
}


int cwmp_parse_upload_message(env_t * env , xmldoc_t *doc, upload_arg_t ** pularg, fault_code_t *fault)
{

    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    upload_arg_t * ularg = pool_pcalloc(env->pool, sizeof(download_arg_t));

   ularg->cmdkey = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "CommandKey")));
   ularg->filetype= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "FileType")));
   ularg->url = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "URL")));
   ularg->username= pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Username")));
   ularg->password = pool_pstrdup(env->pool, cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "Password")));
   ularg->delaysec= TRatoi(cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "DelaySeconds")));

    *pularg = ularg;

    return CWMP_OK;
}


int cwmp_parse_addobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root,  int * instances, int* status, fault_code_t *fault)
{

    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    int fault_code;
    int instance_num;

    char * object_name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "ObjectName"));
    char * parameter_key = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "ParameterKey"));
	cwmp_log_info("parse_addobject_message name=%s", object_name);
	cwmp_log_info("parse_addobject_message key=%s", parameter_key);

    parameter_node_t * param = cwmp_get_parameter_path_node(root, object_name);
    if(!param)
    {
        cwmp_log_error("can't find AddObject parameter ObjectName %s\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }
    parameter_node_t * child_param = param->child;
    cwmp_log_info("name=%s type=%d rw=%d childName=%s",
    	param->name, param->type, param->rw, child_param->name);
    if( (param->type != TYPE_OBJECT) || (param->rw  != 1) || (TRstrcmp(child_param->name, "{i}") != 0))
    {
        cwmp_log_error("AddObject parameter ObjectName %s is invalid or not writable\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }
    if(! param->add)
    {
        cwmp_log_error("could not find %s add object function\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    fault_code = param->add(env->cwmp, param, &instance_num, callback_register_task);

    if(fault_code != FAULT_CODE_OK)
    {
        cwmp_log_error("exec %s add object function failed\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    *instances = instance_num;

    return CWMP_OK;
}

extern void walk_xmlDoc_node_tree(XmlNode * param, int level);
extern void _walk_parameter_node_tree(parameter_node_t * param, int level);;
int cwmp_parse_deleteobject_message(env_t * env , xmldoc_t *doc, parameter_node_t * root, int* status, fault_code_t *fault)
{

    xmlnode_t * node = cwmp_get_rpc_method_node(doc);
    int fault_code;
    int instance_num;

    char * object_name = cwmp_xml_get_node_value(cwmp_xml_get_child_with_name(node, "ObjectName"));


    parameter_node_t * param = cwmp_get_parameter_path_node(root, object_name);
    if(!param)
    {
        cwmp_log_error("can't find DeleteObject parameter ObjectName %s\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    if((param->type == TYPE_OBJECT && object_name[strlen(object_name)-1] != '.') ||  param->type != TYPE_OBJECT)
    {
        cwmp_log_error("DeleteObject parameter ObjectName %s is invalid\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    if(is_digit(param->name) != 0)
    {
        cwmp_log_error("DeleteObject parameter ObjectName %s is not digit\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    parameter_node_t * parent = param->parent;
    if(! parent->del)
    {
        cwmp_log_error("could not find %s delete object function\n", object_name);
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }

    instance_num = TRatoi(param->name);

    fault_code = param->parent->del(env->cwmp, param, instance_num, callback_register_task);

    if(fault_code != FAULT_CODE_OK)
    {
        cwmp_set_faultcode(fault, FAULT_CODE_9005);
        return CWMP_ERROR;
    }


    return CWMP_OK;
}


int cwmp_parse_reboot_message(env_t * env , xmldoc_t *doc, char ** key, fault_code_t *fault)
{
    xmlnode_t * commandKeyNode;




    commandKeyNode = cwmp_xml_get_child_with_name(cwmp_get_rpc_method_node(doc), "CommandKey");

    if (!commandKeyNode)
    {
        return CWMP_ERROR;
    }

    *key = cwmp_xml_get_node_value(commandKeyNode);

    return CWMP_OK;
}

download_arg_t * cwmp_clone_download_arg(download_arg_t * dlarg)
{
	if(!dlarg)
	{
		return NULL;
	}
	download_arg_t * newdl = MALLOC(sizeof(download_arg_t));
	if(!newdl)
	{
		return NULL;
	}
	newdl->cmdkey = TRstrdup(dlarg->cmdkey);
	newdl->filetype = TRstrdup(dlarg->filetype);
	newdl->url = TRstrdup(dlarg->url);
	newdl->username = TRstrdup(dlarg->username);
	newdl->password = TRstrdup(dlarg->password);
	newdl->targetname = TRstrdup(dlarg->targetname);
	newdl->succurl = TRstrdup(dlarg->succurl);
	newdl->failurl = TRstrdup(dlarg->failurl);
	newdl->delaysec = dlarg->delaysec;
	newdl->filesize = dlarg->filesize;

	cwmp_log_debug("download arg: %s, %s, %s, %s, %s, targetname:%s,%s,%s, delaysecond:%d,%d",
		newdl->cmdkey?newdl->cmdkey:"null",
		newdl->filetype?newdl->filetype:"null",
		newdl->url?newdl->url:"null",
		newdl->username?newdl->username:"null",
		newdl->password?newdl->password:"null",
		newdl->targetname?newdl->targetname:"null",
		newdl->succurl?newdl->succurl:"null",
		newdl->failurl?newdl->cmdkey:"null",
		newdl->delaysec, newdl->filesize);



	return newdl;

}

upload_arg_t * cwmp_clone_upload_arg(upload_arg_t * ularg)
{
	if(!ularg)
	{
		return NULL;
	}
	upload_arg_t * newul = MALLOC(sizeof(upload_arg_t));
	if(!newul)
	{
		return NULL;
	}
	newul->cmdkey = TRstrdup(ularg->cmdkey);
	newul->filetype = TRstrdup(ularg->filetype);
	newul->url = TRstrdup(ularg->url);
	newul->username = TRstrdup(ularg->username);
	newul->password = TRstrdup(ularg->password);
	newul->delaysec = ularg->delaysec;


	return newul;

}




parameter_t* cwmp_create_parameter(env_t * env ,  const char * name, const char * value, size_t value_length, int type)
{
    parameter_t * pv = XMALLOC(sizeof(parameter_t));
    if (!pv)
    {
        return NULL;
    }

    pv->name = XSTRDUP(name);

    pv->value = XSTRDUP(value);
    pv->value_length = value_length;

    pv->type = type;
    pv->fault_code = 0;


    return pv;
}

event_list_t * cwmp_create_event_list(env_t * env, int size )
{
    event_list_t * el;
    el = cwmp_event_list_create(env->pool, size);

    return el;
}

event_code_t * cwmp_create_event_code(env_t * env )
{
    event_code_t * ev;
    ev = cwmp_event_code_create(env->pool);
    return ev;
}

parameter_list_t* cwmp_create_parameter_list(env_t * env )
{
    parameter_list_t * pl;
    pl = XMALLOC(sizeof(parameter_list_t));
    if (!pl)
    {
        return NULL;
    }
    TRBZERO(pl, sizeof(parameter_list_t));

    pl->parameters = XMALLOC(sizeof(parameter_t*) * CWMP_RENEW_SIZE);
    pl->count = 0;
    pl->size = CWMP_RENEW_SIZE;

    return pl;
}



xmlnode_t * cwmp_create_current_time_node(env_t * env ,   xmlnode_t * parent, const datatime_t *currentt)
{
    char buffer[CWMP_BUF_SIZE];

    xmlnode_t * currTimeNode;
    FUNCTION_TRACE();
    TRsnprintf(buffer, CWMP_BUF_SIZE, "%4d-%02d-%02dT%02d:%02d:%02dZ",
               currentt->year,
               currentt->month,
               currentt->day,
               currentt->hour,
               currentt->min,
               currentt->sec);

    ESA(currTimeNode, cwmp_xml_create_child_node(env ,  parent, NULL, "CurrentTime", buffer));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  currTimeNode, SOAP_XSI_TYPE, SOAP_XSD_DATETIME));

    return currTimeNode;
}


xmlnode_t * cwmp_create_event_node(env_t * env ,  xmlnode_t * parent, const event_list_t * eventlist)
{
    xmlnode_t * eventNode, *eventStructNode,  * eventCodeNode, * eventCommandKeyNode;

    int count = 0;


    event_code_t ** pe = eventlist->events;

    FUNCTION_TRACE();
    ESA(eventNode, cwmp_xml_create_child_node(env ,  parent, NULL, "Event", NULL));

    while (count < eventlist->count)
    {
        ESA(eventStructNode, cwmp_xml_create_child_node(env ,  eventNode, NULL, "EventStruct", NULL));

        ESA(eventCodeNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "EventCode", pe[count]->code));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  eventCodeNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));

		if (pe[count]->event == INFORM_MREBOOT ) //|| pe[count]->event == INFORM_BOOTSTRAP)
		{
	        ESA(eventCommandKeyNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "CommandKey", pe[count]->command_key));
		}
		else
		{
			ESA(eventCommandKeyNode, cwmp_xml_create_child_node(env ,  eventStructNode, NULL, "CommandKey", NULL));
		}
        ++count ;
    }


    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  eventNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:EventStruct[%d]", count)));

    return eventNode;
}


xmlnode_t * cwmp_create_header_node(env_t * env ,   xmlnode_t * root, header_t * header)
{
    xmlnode_t * headerNode;
    xmlnode_t * idNode;
    //xmlnode_t * holdRequestsNode;
    FUNCTION_TRACE();
    ESA(headerNode, cwmp_xml_create_child_node(env ,  root, NULL, SOAP_ENV_HEADER, NULL));
    ESA(idNode, cwmp_xml_create_child_node(env ,  headerNode, NULL, CWMP_XML_HEADER_ID, header->id));
    //ESA(holdRequestsNode, cwmp_xml_create_child_node(env ,  headerNode, NULL, CWMP_XML_HEADER_HOLDREQUESTS, NULL));

    ESN(XML_OK, cwmp_xml_set_node_attribute(env , idNode, cwmp_get_format_string("%s:%s", SOAP_ENV, CWMP_XML_MUSTUNDERSTAND), "1"));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  holdRequestsNode, XMLNS_APPEND(SOAP_ENV, CWMP_XML_MUSTUNDERSTAND), "1"));

    return headerNode;
}

xmlnode_t * cwmp_create_body_node(env_t * env ,  xmlnode_t * root)
{
    xmlnode_t * bodyNode;
    FUNCTION_TRACE();
    ESA(bodyNode, cwmp_xml_create_child_node(env ,  root, NULL, SOAP_ENV_BODY, NULL));
    return bodyNode;
}

xmlnode_t * cwmp_create_envelope_node(env_t * env ,  xmlnode_t * parent)
{

    xmlnode_t * envelopeNode;
    FUNCTION_TRACE();
    cwmp_log_debug("ENV: %s, ENC: %s\n", SOAP_ENV, SOAP_ENC);
    ESA(envelopeNode, cwmp_xml_create_child_node(env ,  parent, NULL, SOAP_ENV_ENVELOPE, NULL));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENV), SOAP_ENV_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENC), SOAP_ENC_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "xsi"), SOAP_XSI_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "xsd"), SOAP_XSD_NS));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  envelopeNode, cwmp_get_format_string("%s:%s", "xmlns", "cwmp"), SOAP_CWMP_NS));

    return envelopeNode;
}

xmlnode_t * cwmp_create_device_id_node(env_t * env ,  xmlnode_t * parent, const device_id_t * deviceid)
{
    xmlnode_t * deviceIdNode;
    xmlnode_t * mf;
    xmlnode_t * oui;
    xmlnode_t * pc;
    xmlnode_t * sn;

    FUNCTION_TRACE();

    ESA(deviceIdNode, cwmp_xml_create_child_node(env ,  parent, NULL, "DeviceId", NULL));

    ESA(mf, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "Manufacturer", env->cwmp->cpe_mf));

    ESA(oui, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "OUI", env->cwmp->cpe_oui));

    ESA(pc, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "ProductClass", env->cwmp->cpe_pc));

    ESA(sn, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "SerialNumber",  env->cwmp->cpe_sn));


    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  deviceIdNode, SOAP_XSI_TYPE, "cwmp:DeviceIdStruct"));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_NAME, deviceId->name));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  oui, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  pc, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  sn, SOAP_XSI_TYPE, SOAP_XSD_STRING));

    return deviceIdNode;
}
/*
xmlnode_t * cwmp_create_device_id_node(env_t * env ,  xmlnode_t * parent, const device_id_t * deviceid)
{
    xmlnode_t * deviceIdNode;
    xmlnode_t * mf;
    xmlnode_t * oui;
    xmlnode_t * pc;
    xmlnode_t * sn;

    FUNCTION_TRACE();

	char * value;
    ESA(deviceIdNode, cwmp_xml_create_child_node(env ,  parent, NULL, "DeviceId", NULL));

    get_sys_parameter(X_SYS_Manufacturer, &value);
    ESA(mf, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "Manufacturer", value));

	get_sys_parameter(X_SYS_ManufacturerOUI, &value);
    ESA(oui, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "OUI", value));

    get_sys_parameter(X_SYS_ProductClass, &value);
    ESA(pc, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "ProductClass", value));

    get_sys_parameter(X_SYS_SerialNumber, &value);
    ESA(sn, cwmp_xml_create_child_node(env ,  deviceIdNode, NULL, "SerialNumber",  value));


    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  deviceIdNode, SOAP_XSI_TYPE, "cwmp:DeviceIdStruct"));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  mf, SOAP_XSI_NAME, deviceId->name));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  oui, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  pc, SOAP_XSI_TYPE, SOAP_XSD_STRING));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  sn, SOAP_XSI_TYPE, SOAP_XSD_STRING));


    return deviceIdNode;
}
*/



xmlnode_t * cwmp_create_max_envelope_node(env_t * env ,  xmlnode_t * parent, unsigned int max_envelope)
{
    xmlnode_t * maxEnvNode;

    FUNCTION_TRACE();

    ESA(maxEnvNode, cwmp_xml_create_child_node(env ,  parent, NULL, CWMP_XML_INFORM_MAXENVELOPES, cwmp_get_format_string("%d", max_envelope)));

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  maxEnvNode, SOAP_XSI_TYPE, SOAP_XSD_UNSIGNEDINT));

    return maxEnvNode;
}


xmlnode_t * cwmp_create_retry_count_node(env_t * env ,  xmlnode_t * parent, unsigned int retry_count)
{
    xmlnode_t * retryCountNode;
    FUNCTION_TRACE();

    ESA(retryCountNode, cwmp_xml_create_child_node(env ,  parent, NULL, "RetryCount", cwmp_get_format_string("%d", retry_count)));

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  retryCountNode, SOAP_XSI_TYPE, SOAP_XSD_UNSIGNEDINT));

    return retryCountNode;
}


int cwmp_create_parameter_node(env_t * env ,  parameter_node_t ** news, const char * name)
{
    parameter_node_t * param_node = XMALLOC(sizeof(parameter_node_t));
    cwmp_log_debug("cwmp_create_parameter_node ...\n");
    if (!param_node)
    {
        return CWMP_ERROR;
    }
    //param_node->full_name = XSTRDUP(name);
    param_node->name = NULL;
    param_node->rw = 0;
    param_node->type = 0;
    //param_node->type = NULL;
    param_node->value = NULL;
    param_node->child = param_node->next_sibling = param_node->prev_sibling = param_node->parent = NULL;

    (*news) = param_node;

    return CWMP_OK;
}


xmlnode_t * cwmp_create_parameter_list_node(env_t * env ,  xmlnode_t * parent, parameter_list_t * pl)
{
    xmlnode_t * parameterValueStructNode;
    parameter_t ** ps;
    parameter_t* pv;
    xmlnode_t * parameterListNode, *nameNode, *valueNode;
    int i = 0;

    FUNCTION_TRACE();

    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  parent, NULL, "ParameterList", NULL));
    ps = pl->parameters;

    while (i < pl->count)
    {
        pv = *ps;
        ESA(parameterValueStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterValueStruct", NULL));

        ESA(nameNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Name", pv->name));
        ESA(valueNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Value", pv->value));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  nameNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  valueNode, SOAP_XSI_TYPE, SOAP_XSD_STRING));

        parameterValueStructNode = NULL;
        ps ++;
        i++;
    }


    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterValueStruct[%d]", pl->count) ));
    // cwmp_log_debug("created parameter list: [%d]\n", i);
    return parameterListNode;
}





xmldoc_t* cwmp_create_inform_message(env_t * env ,  header_t * header,
                                     device_id_t * deviceid,
                                     event_list_t *events,
                                     datatime_t * currentt,
                                     unsigned int max_envelope,
                                     unsigned int retry_count,
                                     parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * informNode;
    xmlnode_t * headerNode;
    xmlnode_t * deviceIdNode, *eventsNode, *maxenvNode, *currtimeNode, *retryCountNode, *paramlistNode;



    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    FUNCTION_TRACE();
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);



    ESA(informNode,     cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_INFORM, NULL));
    ESA(deviceIdNode    , cwmp_create_device_id_node(env ,  informNode, deviceid));
    ESA(eventsNode      , cwmp_create_event_node(env ,  informNode, events));
    ESA(maxenvNode      , cwmp_create_max_envelope_node(env ,  informNode, max_envelope));
    ESA(currtimeNode    , cwmp_create_current_time_node(env ,  informNode, currentt));
    ESA(retryCountNode  , cwmp_create_retry_count_node(env ,  informNode, retry_count));
    ESA(paramlistNode   , cwmp_create_parameter_list_node(env ,  informNode, pl));

    return doc;
}




//cwmp_create_getrpcmethods_response_message
xmldoc_t * cwmp_create_getrpcmethods_response_message(env_t * env ,  header_t * header, char ** methods, unsigned int count)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * methodListNode;
    xmlnode_t * methodNode;

    xmldoc_t * doc;
    char ** method;
    int num = 0;
    if (!methods)
    {
        return NULL;
    }

    doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETRPCMETHODSRESPONSE, NULL));
    ESA(methodListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "MethodList", NULL));

    method = (methods);
    num = 0;
    while ((num < count) && method && (*method))
    {
        ESA(methodNode, cwmp_xml_create_child_node(env ,  methodListNode, NULL, "string", *method));

        method ++;
        num ++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  methodListNode, cwmp_get_format_string("%s:%s", "xmlns", SOAP_ENV), SOAP_ENV_NS ));
    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  methodListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("xsd:string[%d]", count) ));

    return doc;
}






void * cwmp_create_getparameternames_response_all_parameter_names(env_t * env , xmlnode_t * parent_node, const char * path_name, parameter_node_t *  param_node, int * count)
{
    cwmp_buffer_t buffer;
    xmlnode_t * parameterInfoStructNode;
    xmlnode_t * parameterWritableNode;
    xmlnode_t * parameterNameNode;
    parameter_node_t * param_child;

    if (!param_node)
        return NULL;

    //if(param_node->type != TYPE_OBJECT)
    if(path_name != NULL && *path_name != 0)
    {
        ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parent_node, NULL, "ParameterInfoStruct", NULL));
        ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", path_name ));
        ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", param_node->rw==0? "0" : "1"));
        (*count) ++;
    }
    for (param_child = param_node->child; param_child; param_child = param_child->next_sibling)
    {
        if(TRstrcmp(param_child->name, "{i}") == 0)
            continue;
        cwmp_buffer_init(&buffer);
        if (param_child->type == TYPE_OBJECT)
        {
            cwmp_buffer_write_format_string(&buffer,"%s%s.", path_name, param_child->name);
        }
        else
        {
            cwmp_buffer_write_format_string(&buffer,"%s%s", path_name, param_child->name);

        }
        cwmp_create_getparameternames_response_all_parameter_names(env, parent_node, cwmp_buffer_string(&buffer), param_child, count);

    }

    return NULL;
}


//cwmp_create_getparameternames_response_message
xmldoc_t* cwmp_create_getparameternames_response_message(env_t * env ,
        header_t * header,
        const char * path_name,
        parameter_node_t * param_node,
        unsigned int next_subset,
        unsigned int next_level)
{
    cwmp_buffer_t buffer;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;
    xmlnode_t * parameterInfoStructNode;
    xmlnode_t * parameterWritableNode;
    xmlnode_t * parameterNameNode;

    int	count;

    parameter_node_t * child;


    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (!param_node)
    {
        return NULL;
    }

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERNAMESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));

    count = 0;
    if (next_subset == CWMP_NO)
    {
        if(path_name != NULL && *path_name != 0)
        {
            ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterInfoStruct", NULL));
            ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", path_name));
            ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", param_node->rw==0? "0" : "1"));
            count++;
        }
    }
    else
    {
        if (next_level == CWMP_YES)
        {
            for (child = param_node->child; child; child = child->next_sibling)
            {
		if(TRstrcmp(child->name, "{i}") == 0)
	            continue;

                cwmp_buffer_init(&buffer);
                if (child->type == TYPE_OBJECT)
                {
                    cwmp_buffer_write_format_string(&buffer,"%s%s.", path_name, child->name);
                }
                else
                {
                    cwmp_buffer_write_format_string(&buffer,"%s%s", path_name, child->name);
                }
                ESA(parameterInfoStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterInfoStruct", NULL));
                ESA(parameterNameNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Name", cwmp_buffer_string(&buffer)));
                ESA(parameterWritableNode, cwmp_xml_create_child_node(env ,  parameterInfoStructNode, NULL, "Writable", child->rw==0? "0" : "1"));
                count++;

            }
        }
        else
        {
            //all parameters
            cwmp_create_getparameternames_response_all_parameter_names(env, parameterListNode, path_name, param_node, &count);

        }
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterInfoStruct[%d]", count) ));

    return doc;
}


//cwmp_create_getparametervalues_response_message
xmldoc_t* cwmp_create_getparametervalues_response_message(env_t * env ,  header_t * header, parameter_list_t * pl)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;

    xmlnode_t * parameterValueStructNode;
    xmlnode_t * nameStructNode;
    xmlnode_t * valueStructNode;
    int	 count;
    parameter_t ** pv;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERVALUESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));
    pv = pl->parameters;
    count = 0;
    while (count < pl->count)
    {
        ESA(parameterValueStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterValueStruct", NULL));
        ESA(nameStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Name", (*(pv+count))->name));
        ESA(valueStructNode, cwmp_xml_create_child_node(env ,  parameterValueStructNode, NULL, "Value", (*(pv+count))->value));

        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  nameStructNode, SOAP_XSI_TYPE, SOAP_XSD_STRING ));
        ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  valueStructNode, SOAP_XSI_TYPE, CWMP_TYPE( (*(pv+count))->type ) ));
        count++;
    }

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  parameterListNode, SOAP_ENC_ARRAYTYPE, cwmp_get_format_string("cwmp:ParameterValueStruct[%d]", pl->count) ));

    return doc;
}


void walk_tree(xmlnode_t *node)
{
	xmlnode_t *tmpNode;
	//xmlnode_t *prevNode;
	xmlnode_t *childNode;
	xmlnode_t *siblingNode;
	
	if(node == NULL)
		return ;

	cwmp_log_info("%s", (XmlNodeGetNodeValue(node->firstAttr)==NULL)?node->nodeName:XmlNodeGetNodeValue(node->firstAttr));

	extz_log_info("	child:");
	childNode = XmlNodeGetFirstChild(node);
	tmpNode = childNode;
	while(tmpNode!=NULL){
		extz_log_info("%s ", (XmlNodeGetNodeValue(tmpNode->firstAttr)==NULL)?tmpNode->nodeName:XmlNodeGetNodeValue(tmpNode->firstAttr));
		tmpNode = XmlNodeGetNextSibling(tmpNode);
		
	}
	extz_log_info("\n");

	extz_log_info("	sibling:");	
	siblingNode = XmlNodeGetNextSibling(node);
	tmpNode = siblingNode;

	if(tmpNode == NULL)
		tmpNode = node->prevSibling;
		
	if(tmpNode != NULL)
		while(tmpNode->prevSibling!=NULL)
			tmpNode = tmpNode->prevSibling;
	
	while(tmpNode!=NULL){
		if(tmpNode==node)	
			extz_log_info(". ");
		else	
			extz_log_info("%s ", (XmlNodeGetNodeValue(tmpNode->firstAttr)==NULL)?tmpNode->nodeName:XmlNodeGetNodeValue(tmpNode->firstAttr));
			
		tmpNode = XmlNodeGetNextSibling(tmpNode);
	}
	extz_log_info("\n\n"); 

	walk_tree(childNode);
	walk_tree(siblingNode);
}


void walk_node(xmlnode_t *node)
{
	xmlnode_t *tmpNode;
	
	if(node == NULL)
		return ;

	cwmp_log_info("%s\n", (XmlNodeGetNodeValue(node->firstAttr)==NULL)?node->nodeName:XmlNodeGetNodeValue(node->firstAttr));
	tmpNode = XmlNodeGetFirstChild(node);
	while(tmpNode!=NULL){
		walk_node(tmpNode);
		tmpNode = XmlNodeGetNextSibling(tmpNode);
	}
}

void walk_parameter(parameter_node_t *node)
{
	//parameter_node_t *tmp;
	parameter_node_t *child;
	parameter_node_t *childSibling;
	
	if(node == NULL)
		return ;

	cwmp_log_info("%s %x\n", node->name, (unsigned int)node);

	child = node->child;
	if(child != NULL){
		childSibling = child->next_sibling;

		cwmp_log_info("	child=%x child_sibling=%x\n", 
					(unsigned int)child,
					(unsigned int)childSibling);
					
		walk_parameter(child);
		
		while(childSibling != NULL){
			walk_parameter(childSibling);
			childSibling = childSibling->next_sibling;
		}
	}
}

void walk_parameter_tree(parameter_node_t *node)
{
	parameter_node_t *parent;
	parameter_node_t *child;
	//parameter_node_t *sibling;
	//parameter_node_t *tmp;
	
	if(node == NULL)
		return ;

	parent = node;
	child = node->child;

	if(child == NULL){
		cwmp_log_info("%s", node->name);
		return ;
	}

	while(child != NULL){
		parent = child;
		child = child->child;

	}

	return ;
}

char *pop_last_name(char *names_str)
{
	char *p = names_str;

	if(p==NULL)
		return NULL;

	while(*p!='\0')
		p++;

	while(p!=names_str && *p!='.')
		p--;

	p[0]='\0';
	return names_str;
}

// not saft pop and push
char *push_last_name(char *names_str, char *dot)
{
	char *p = names_str;

	if(p==NULL || dot==NULL)
		return NULL;

	if(strlen(names_str)>0){
		strcat(names_str, ".");
		strcat(names_str, dot);
	}
	else
		strcpy(names_str, dot);
	
	return names_str;
}

#define PN_NODE_NAME_STACK_SIZE 1024

static int pack_parameter_attr_node_loop_cnt=0;
static char pack_parameter_attr_node_full_name[PN_NODE_NAME_STACK_SIZE];


void init_pack_parameter_attr_node_loop_num(void)
{
	pack_parameter_attr_node_loop_cnt=0;
	pack_parameter_attr_node_full_name[0]='\0';
}

#define get_pack_parameter_attr_node_loop_num()		pack_parameter_attr_node_loop_cnt
#define set_pack_parameter_attr_node_loop_num(x)	pack_parameter_attr_node_loop_cnt=x

void pack_parameter_attr_node(env_t * env ,  xmlnode_t * parentNode,  parameter_node_t *node)
{
	//parameter_node_t *tmp;
	parameter_node_t *child;
	parameter_node_t *childSibling;

	xmlnode_t * parameterAttrStructNode;
	
	xmlnode_t * nameNode;
    xmlnode_t * notificationNode;

    xmlnode_t * accessListNode;
    xmlnode_t * stringNode;

    static char temp[PN_NODE_NAME_STACK_SIZE];
	
	if(node == NULL){
		pop_last_name(pack_parameter_attr_node_full_name);
		return ;
	}
	
	if(strlen(pack_parameter_attr_node_full_name)>0){
		strcpy(temp, pack_parameter_attr_node_full_name);
		strcat(temp, ".");
		strcat(temp, node->name);
	}
	else{
		strcpy(temp, node->name);
	}

    if(strstr(temp, "{i}") == NULL)
    {
        pack_parameter_attr_node_loop_cnt++;
        ESAVOID(parameterAttrStructNode, cwmp_xml_create_child_node(env ,  parentNode, NULL, "ParameterAttributeStruct", NULL));
        ESAVOID(nameNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "Name", temp));

        sprintf(temp, "%d", node->attr.nc);
        ESAVOID(notificationNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "Notification", temp));

        ESAVOID(accessListNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "AccessList", NULL));
        ESNVOID(XML_OK, cwmp_xml_set_node_attribute(env ,  accessListNode, "SOAP-ENC:arrayType", "xsd:string[1]"));
        ESNVOID(XML_OK, cwmp_xml_set_node_attribute(env ,  accessListNode, "xsi:type", "SOAP-ENC:Array"));
        ESAVOID(stringNode, cwmp_xml_create_child_node(env ,  accessListNode, NULL, "string", "Subscriber"));
    }
	
	child = node->child;
	if(child != NULL){

		push_last_name(pack_parameter_attr_node_full_name, node->name);
		
		childSibling = child->next_sibling;
		pack_parameter_attr_node(env, parentNode, child);
		
		while(childSibling != NULL){
			pack_parameter_attr_node(env, parentNode, childSibling);
			childSibling = childSibling->next_sibling;
		}

		pop_last_name(pack_parameter_attr_node_full_name);
	}
	
}



xmldoc_t* cwmp_create_getparameterattr_response_message(env_t * env ,  header_t * header, parameter_node_t * root, xmlnode_t *obj_node)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * parameterListNode;

    xmlnode_t * parameterAttrStructNode;
    xmlnode_t * nameNode;
    xmlnode_t * notificationNode;

    xmlnode_t * accessListNode;
    xmlnode_t * stringNode;

    xmldoc_t * doc;
    fault_code_t fault;
    
    //int	 count=0;

    char *obj_name;
    char temp[128];
    char obj_parent_name[256];
    char name_for_empty_string[256];

    parameter_node_t *obj_param;
    int is_dot_path = 0;
    //static char full_path[PN_NODE_NAME_STACK_SIZE];

    init_pack_parameter_attr_node_loop_num();
    obj_node = cwmp_xml_get_child_with_name(obj_node, "string");
    if(obj_node == NULL){
        cwmp_log_error("no string node \n");
        cwmp_set_faultcode(&fault, FAULT_CODE_9005);
        return cwmp_create_faultcode_response_message(env, header, &fault);
    }

    doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_GETPARAMETERATTRIBUTESRESPONSE, NULL));
    ESA(parameterListNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "ParameterList", NULL));

    do
    {
        obj_name = cwmp_xml_get_node_value(obj_node);
      
        if(obj_name == NULL)
        {
            sprintf(name_for_empty_string, "%s.", root->name);
            obj_name = name_for_empty_string;
        }

    	if(obj_name != NULL){
    		int len;
    		strcpy(obj_parent_name, obj_name);
    		
    		len = strlen(obj_parent_name);
    		if(obj_parent_name[len-1]=='.'){
    			char *p = obj_parent_name;
    			
    			is_dot_path = 1;

    			obj_parent_name[len-1]='\0';
    			p = obj_parent_name+len-1;
    		}
    		else
    			is_dot_path = 0;
    	}
    	else
    		is_dot_path = 0;

    	obj_param = cwmp_get_parameter_node(root, obj_name);
    	if(obj_param == NULL){
    		cwmp_log_info("get param is null");
    		cwmp_set_faultcode(&fault, FAULT_CODE_9005);
    		return cwmp_create_faultcode_response_message(env, header, &fault);
    	}
    	
    	parameter_node_t * pn = cwmp_get_parameter_node(root, obj_name);
    	if(pn==NULL){
    		cwmp_set_faultcode(&fault, FAULT_CODE_9005);
    		return cwmp_create_faultcode_response_message(env, header, &fault);
    	}

    	if(is_dot_path!=0){
    		strcpy(pack_parameter_attr_node_full_name, pop_last_name(obj_parent_name));
    		pack_parameter_attr_node(env, parameterListNode, pn);
        }
        else{
            pack_parameter_attr_node_loop_cnt++;
        	
    	    ESA(parameterAttrStructNode, cwmp_xml_create_child_node(env ,  parameterListNode, NULL, "ParameterAttributeStruct", NULL));

    	    ESA(nameNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "Name", obj_name));

    		sprintf(temp, "%d", pn->attr.nc);
    	    ESA(notificationNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "Notification", temp));
    	    
    	    ESA(accessListNode, cwmp_xml_create_child_node(env ,  parameterAttrStructNode, NULL, "AccessList", NULL));
    	    //ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  accessListNode, "SOAP-ENC:arrayType", cwmp_get_format_string("xsd:string[%d]", pn->attr.acl)));
    		ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  accessListNode, "SOAP-ENC:arrayType", "xsd:string[1]"));
    		ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  accessListNode, "xsi:type", "SOAP-ENC:Array"));

    		ESA(stringNode, cwmp_xml_create_child_node(env ,  accessListNode, NULL, "string", "Subscriber"));
    	}
        obj_node = XmlNodeGetNextSibling(obj_node);
    } while(obj_node);

    ESN(XML_OK, cwmp_xml_set_node_attribute(env ,  
    										parameterListNode, 
    										SOAP_ENC_ARRAYTYPE, 
    										cwmp_get_format_string("cwmp:ParameterAttributeStruct[%d]", get_pack_parameter_attr_node_loop_num())));

    return doc;
}

xmldoc_t * cwmp_create_setparametervalues_response_message(env_t * env ,  header_t * header, unsigned int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);



    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_SETPARAMETERVALUESRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));

    return doc;
}


xmldoc_t * cwmp_create_setparameterattributes_response_message(env_t * env ,  header_t * header, unsigned int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);



    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_SETPARAMETERATTRIBUTESRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));

    return doc;
}

xmldoc_t * cwmp_create_download_response_message(env_t * env , header_t * header, int status, time_t *begin, time_t *end)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;
    xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }
    

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_DOWNLOADRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));
    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "StartTime", parse_time(begin)));
    ESA(completeTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "CompleteTime", parse_time(end)));



    return doc;
}

xmldoc_t * cwmp_create_upload_response_message(env_t * env , header_t * header, int status, time_t *begin, time_t *end)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;
    xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_UPLOADRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));
    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "StartTime", parse_time(begin)));
    ESA(completeTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "CompleteTime", parse_time(end)));

    return doc;
}





xmldoc_t * cwmp_create_addobject_response_message(env_t * env , header_t * header, int instances, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    xmlnode_t * startTimeNode;
    //xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_ADDOBJECTRESPONSE, NULL));

    ESA(startTimeNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "InstanceNumber", TRitoa(instances)));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));




    return doc;
}



xmldoc_t * cwmp_create_deleteobject_response_message(env_t * env , header_t * header, int status)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * statusNode;
    //xmlnode_t * startTimeNode;
    //xmlnode_t * completeTimeNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_DELETEOBJECTRESPONSE, NULL));
    ESA(statusNode, cwmp_xml_create_child_node(env ,  responseNode, NULL, "Status", status == 0 ? "0" : "1"));



    return doc;
}


xmldoc_t * cwmp_create_reboot_response_message(env_t * env ,  header_t * header)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_REBOOTRESPONSE, NULL));

    return doc;
}


xmldoc_t * cwmp_create_factoryreset_response_message(env_t * env ,  header_t * header)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * responseNode;
    xmlnode_t * headerNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(responseNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_FACTORYRESETRESPONSE, NULL));

    return doc;
}


xmldoc_t* cwmp_create_faultcode_response_message(env_t * env , header_t * header, fault_code_t * fault)
{


    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    //xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env , envelopeNode, header);
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    return doc;
}



xmldoc_t* cwmp_create_faultcode_setparametervalues_response_message(env_t * env , header_t * header, parameter_list_t * param_list, fault_code_t * fault)
{

    int i, count;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    //xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;
    parameter_t ** param = param_list->parameters;

    FUNCTION_TRACE();

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env , envelopeNode, header);
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    cwmp_log_debug("cwmp_create_faultcode_setparametervalues_response_message count %d, %p", param_list->count, *param);
    for(i=0, count = param_list->count; (*param != NULL) && (i<count); i++, param++)
    {
        ESA(faultStructNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "SetParameterValuesFault", NULL));
		ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "ParameterName", (*param)->name));
		ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa((*param)->fault_code)));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING((*param)->fault_code)));
    }


    return doc;
}

xmldoc_t* cwmp_create_faultcode_setparameterattributes_response_message(env_t * env , header_t * header, parameter_list_t * param_list, fault_code_t * fault)
{

    int i, count;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    //xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;
    parameter_t ** param = param_list->parameters;

    FUNCTION_TRACE();

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env , envelopeNode, header);
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    cwmp_log_debug("cwmp_create_faultcode_setparameterattributes_response_message count %d, %p", param_list->count, *param);
    for(i=0, count = param_list->count; (*param != NULL) && (i<count); i++, param++)
    {
        ESA(faultStructNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "SetParameterAttributesFault", NULL));
	ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "ParameterName", (*param)->name));
	ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa((*param)->fault_code)));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING((*param)->fault_code)));
    }


    return doc;
}

xmldoc_t* cwmp_create_faultcode_getparameterattributes_response_message(env_t * env , header_t * header, parameter_list_t * param_list, fault_code_t * fault)
{

    int i, count;
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
   // xmlnode_t * responseNode;
    xmlnode_t * headerNode;
    xmlnode_t * faultStructNode;
    xmlnode_t * newNode, *detailNode, *faultNode;
    parameter_t ** param = param_list->parameters;

    FUNCTION_TRACE();

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );

    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env , envelopeNode, header);
    }
    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);

    cwmp_log_debug("create fault response , code is %d", fault->fault_code);
    ESA(faultStructNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, SOAP_ENV_FAULT, NULL));

    switch(fault->fault_code)
    {
        case FAULT_CODE_9000:
        case FAULT_CODE_9001:
        case FAULT_CODE_9002:
        case FAULT_CODE_9004:
        case FAULT_CODE_9009:
        case FAULT_CODE_9010:
        case FAULT_CODE_9011:
        case FAULT_CODE_9012:
        case FAULT_CODE_9013:
        case FAULT_CODE_9014:
        case FAULT_CODE_9015:
        case FAULT_CODE_9016:
        case FAULT_CODE_9017:
        case FAULT_CODE_9018:
        case FAULT_CODE_9019:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Server"));
            break;
        }
        default:
        {
            ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultcode", "Client"));
            break;
        }
    }

    ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "faultstring", "Client fault"));

    ESA(detailNode, cwmp_xml_create_child_node(env ,  newNode, NULL, "detail", NULL));
    ESA(faultNode, cwmp_xml_create_child_node(env ,  detailNode, NULL, "cwmp:Fault", NULL));


    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultCode", TRitoa(fault->fault_code)));
    ESA(newNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "FaultString", FAULT_STRING(fault->fault_code)));

    cwmp_log_debug("cwmp_create_faultcode_getparameterattributes_response_message count %d, %p", param_list->count, *param);
    for(i=0, count = param_list->count; (*param != NULL) && (i<count); i++, param++)
    {
        ESA(faultStructNode, cwmp_xml_create_child_node(env ,  faultNode, NULL, "GetParameterAttributesFault", NULL));
	ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "ParameterName", (*param)->name));
	ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa((*param)->fault_code)));
        ESA(newNode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING((*param)->fault_code)));
    }


    return doc;
}



xmldoc_t * cwmp_create_transfercomplete_message(env_t * env ,  header_t * header, event_code_t * evcode)
{
    xmlnode_t * envelopeNode;
    xmlnode_t * bodyNode;
    xmlnode_t * rpcNode;
    xmlnode_t * headerNode;
    xmlnode_t * node;

    xmlnode_t * faultStructNode;
    xmlnode_t * faultCode;
    xmlnode_t * faultString;

    xmldoc_t * doc = XmlDocCreateDocument(env->pool );
    envelopeNode    = cwmp_create_envelope_node(env ,  & doc->node);

    if (header)
    {
        headerNode  = cwmp_create_header_node(env ,  envelopeNode, header);
    }

    bodyNode        = cwmp_create_body_node(env ,  envelopeNode);
    ESA(rpcNode, cwmp_xml_create_child_node(env ,  bodyNode, NULL, CWMP_RPC_TRANSFERCOMPLETE, NULL));
    ESA(node, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "CommandKey", evcode->command_key));
	// lizd
    //if(evcode->fault_code)
    {
		static char ok_str[]="OK";

        ESA(faultStructNode, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "FaultStruct", NULL));
        ESA(faultCode, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultCode", TRitoa(evcode->fault_code)));
        if(evcode->fault_code == 0)
        	ESA(faultString, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", ok_str));
        else
        	ESA(faultString, cwmp_xml_create_child_node(env ,  faultStructNode, NULL, "FaultString", FAULT_STRING(evcode->fault_code)));

    }

	// lizd
    //ESA(node, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "StartTime", begintime));
    //ESA(node, cwmp_xml_create_child_node(env ,  rpcNode, NULL, "CompleteTime", endtime));
        
	ESA(node, cwmp_xml_create_child_node(env ,	rpcNode, NULL, "StartTime", parse_time(&evcode->start)));
	ESA(node, cwmp_xml_create_child_node(env ,	rpcNode, NULL, "CompleteTime", parse_time(&evcode->end)));

    return doc;
}



int cwmp_write_doc_to_chunk(xmldoc_t *  doc, cwmp_chunk_t * chunk, pool_t * pool)
{
    //	return cwmp_xml_print_doc_to_chunk(doc, chunk, pool);
    char * xml;
    xml = XmlPrintDocument(pool, doc);
    cwmp_chunk_write_string(chunk, xml, TRstrlen(xml), pool);

    return CWMP_OK;
}


//WAN端口的IP地址
#ifndef __i386__
//路由器上的WAN端口
#define REAL_WAN_IF "usb0"
#else
//PC网卡的名称
#define REAL_WAN_IF "eth0"
#endif

#define CONFIG_FILE_NAME "cfg -e"
#define CALCULATE_START_YEAR 2012
#define PROC_TOZED_WATCHDOG "/proc/tozed/watchdog"
#define VERSION_UPDATE_FILE_NAME "/tmp/.update.version"
#define LTE_VERSION_FILE_NAME "/tmp/.moduleinfo"
#define LTE_STATIC_FILE_NAME "/tmp/.system_info_static"
#define LTE_DYNAMIC_FILE_NAME "/tmp/.system_info_dynamic"

//version info
typedef struct
{
	unsigned int kernel_version;
	unsigned int sdk_version;
	unsigned int sub_sdk_version;
	unsigned int compile_date;
}VersionInfo;

typedef struct
{
	char *month_str;
	int month_value;
} MonthInfo;

const MonthInfo month_array[]=
{
	{"Jan",1},
	{"Feb",2},
	{"Mar",3},
	{"Apr",4},
	{"May",5},
	{"Jun",6},
	{"Jul",7},
	{"Aug",8},
	{"Sep",9},
	{"Oct",10},
	{"Nov",11},
	{"Dec",12},
};

enum RETURN_CODE {
	SUCCESS = 0,
	FAIL = 1,
	ERROR = 2
};

typedef struct
{
	char Enable[4];
	char Name[64];
	char Uptime[16];
	char LastConnectionError[32];
	char Username[32];
	char Password[32];
	char ExternalIPAddress[32];
	char DNSEnabled[4];
	char DNSServers[64];
	char MACAddress[32];
	char X_CMCC_TunnelDial[32];
	char X_CMCC_LNS[32];
	char X_CMCC_LnsIp[32];
	char X_CMCC_LnsName[32];
	char X_CMCC_LnsPassword[32];
	char X_CMCC_L2TPMode[32];
	char X_CMCC_UpstreamTotalByte[16];
	char X_CMCC_DownstreamTotalByte[16];
}PPPUserInfo;


typedef struct
{
	//模块的IMEI号
	char imei[16];
	//设备型号
	char model[32];
	//软件版本
	char software_version[8];
	//硬件版本
	char hardware_version[32];
	//模块版本
	char module_version[64];
	//模块物理层版本
	char module_phy_version[128];
	//模块高层协议版本
	char module_hlp_version[128];
	//模块编译的时间日期
	char module_build_date[128];
	//区域或者客户
	char customer[32];
	//设备所在的大区信息
	char lac[8];
	//设备所在的小区信息
	char cell_id[8];
	//上行流量(KB)
	char tx_bytes[16];
	//下行流量(KB)
	char rx_bytes[16];
	//MAC地址
	char mac[32];
	//升级结果报告状态
	int update_result;
}HttpParams;

static char BANDS[][20] = { "LTE-FDD band 1", "", "LTE-FDD band 3", "", "", "",
			"LTE-FDD band 7", "LTE-FDD band 8", "", "", "", "", "LTE-FDD band 13", "", "", "",
			"LTE-FDD band 17", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
			"LTE band 33", "LTE band 34", "LTE band 35", "LTE band 36", "LTE band 37",
			"LTE band 38", "LTE band 39", "LTE band 40", "LTE band 41" };
			
//所有的http参数
static HttpParams all_http_params;
InfoStruct 
system_version_info;
WanInfo wan_info;
static PPPUserInfo ppp_users[4];

static int reg_status;
static int greg_status;
static int ereg_status;

static int env_change_flag = 1;
#define NORMAL_FIELD_LEN 256
#define BIG_MAX_FIELD_LEN 1024



char *substring(char *p, int begin, int length){
    p = p + begin;
    *(p + length) = '\0';

    return p;
}

int cmd_append_echo(const char *fileName, const char* content){
	FILE* f = fopen(fileName, "a");
	
	if(!f){
		return 1;
	}
	
	fwrite(content, sizeof(char), strlen(content), f);
	
	fclose(f);
	
	fflush(f);
	
	return 0;
}


int write_sys_log(char *value) {
	
	cwmp_log_debug(value);
	
	int file_size = util_get_file_size_by_file_name( SYS_LOG_FILE_NAME );
	if(file_size > 102400){
		return 2;
	}
	
	struct tm *current_time;
	time_t current_secs;
	char shellcmd[256];

	time(&current_secs);
	current_time = localtime(&current_secs);

	sprintf(shellcmd, "%04d-%02d-%02d %02d:%02d:%02d - %s\n",
			current_time->tm_year + 1900, current_time->tm_mon + 1,  current_time->tm_mday, 
			current_time->tm_hour, current_time->tm_min, current_time->tm_sec, value);
	//sprintf(shellcmd, "%s-%02d-%02d %s - %s\n", strYear, month, day, strTime, value);

	return cmd_append_echo(SYS_LOG_FILE_NAME, shellcmd);
}

int read_bin(char *fileName, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = fopen(fileName, "rb");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer), stream);
        fclose(stream);

		memcpy(out, buffer, sizeof(buffer));

		return SUCCESS;
	}

	return FAIL;
}

int read_memory(char *shellcmd, char *out, int size) {

	FILE *stream;
	char buffer[size];
    memset(buffer, 0, sizeof(buffer));

	stream = popen(shellcmd, "r");
	if(stream != NULL){
        fread(buffer, sizeof(char), sizeof(buffer)-1, stream);
        pclose(stream);

		memcpy(out, buffer, strlen(buffer) + 1);

		return SUCCESS;
	} else {
		out[0] = '\0';
	}

	return FAIL;
}

char *get_attr_with_end(char *data, char *dataEnd, char *name, char *value, char *endString) {

	char *pIndex = NULL;
	char *pTail = NULL;

	// 初始化为空字符串
	value[0] = '\0';
	if (data == NULL) {
		return NULL;
	}

	do {

		pIndex = strstr(data, name);
		// write_log_file("pIndex\n");

		if (pIndex == NULL || (dataEnd != NULL && pIndex > dataEnd)) {
			return NULL;
		}

		pIndex += strlen(name);

		if (*pIndex == '=' || *pIndex == ':' || *pIndex == ' ') {
			pIndex++;
			// 首位为空格时去掉
			while (*pIndex == ' ') {
				pIndex++;
			}
			break;
		} else {
			data = pIndex;
		}
	} while (1);

    pTail = strstr(pIndex, endString);
	if (pTail == NULL) {
		return NULL;
	}
	// write_log_file("pTail\n");
	if (*pIndex == '"') {
		pIndex++;
	}
	if (*(pTail - 1) == '"') {
		pTail--;
	}

    memcpy(value, pIndex, pTail - pIndex);
	value[pTail - pIndex] = '\0';

	// write_log_file(name);
	// write_log_file(value);

	return pTail;
}

char *get_attr_by_line(char *data, char *name, char *value) {
	return get_attr_with_end(data, NULL, name, value, "\n");
}

char *get_attr_by_space(char *data, char *name, char *value) {
	return get_attr_with_end(data, NULL, name, value, " ");
}

int get_single_config_attr(char *name, char *value) {

	char shellcmd[256], buffer[256];
	sprintf(shellcmd, "%s | grep %s=", CONFIG_FILE_NAME, name);
	if (read_memory(shellcmd, buffer, sizeof(buffer)) == SUCCESS) {
		if (get_attr_by_line(buffer, name, value) == NULL) {
			value[0] = '\0';
		}
	} else {
		value[0] = '\0';
	}

	return SUCCESS;
}

int uci_get_single_config_attr(char *name, char *value) {

	char shellcmd[256], buffer[256];
	//printf("jiangyibo  111111 \n");
	sprintf(shellcmd, "%s", name);
	if (read_memory(shellcmd, buffer, sizeof(buffer)) == SUCCESS) {
		//printf("jiangyibo mmmd%s\n",buffer);
		strcpy(value,buffer);
/*		if (get_attr_by_line(buffer, name, value) == NULL) {
			value[0] = '\0';
		}
*/		
	} else {
		value[0] = '\0';
	}

	return SUCCESS;
}

int uci_set_single_config_attr(char *name, char *value) {

	char shellcmd[256], buffer[256];
	//printf("jiangyibo  111111 \n");
	sprintf(shellcmd, "%s", name);
	if (read_memory(shellcmd, buffer, sizeof(buffer)) == SUCCESS) {
		//printf("jiangyibo mmmd%s\n",buffer);
		strcpy(value,buffer);
/*		if (get_attr_by_line(buffer, name, value) == NULL) {
			value[0] = '\0';
		}
*/		
	} else {
		value[0] = '\0';
	}

	return SUCCESS;
}




int set_config_attr(char *name,const char *value) {

	char cmd[NORMAL_FIELD_LEN * 2], replaceValue[NORMAL_FIELD_LEN];
	const char *pointer;
	char *pointerReplace;
	unsigned char hasSpace = FALSE;

	memset(cmd, 0, sizeof(cmd));
	memset(replaceValue, 0, sizeof(replaceValue));
	
	pointer = value;
	pointerReplace = replaceValue;
	while(*pointer != '\0') {
		if (*pointer == '`' || *pointer == '$' || *pointer == '\\') {
			// encode special char
			*pointerReplace = '\\';
			pointerReplace++;
			
			hasSpace = TRUE;
		} else {
			if ((*pointer >= '0' && *pointer <= '9') ||
				(*pointer >= 'A' && *pointer <= 'Z') ||
				(*pointer >= 'a' && *pointer <= 'z')) {
				;
			} else {
				hasSpace = TRUE;
			}
		}
		*pointerReplace = *pointer;
	
		pointer++;
		pointerReplace++;
	}
	
	if (hasSpace) {
		sprintf(cmd,"cfg -a %s=\"%s\"", name, replaceValue);
	} else {
		sprintf(cmd,"cfg -a %s=%s", name, replaceValue);
	}
	system(cmd);

	return SUCCESS;
}
/*
int set_single_config_attr(char *name, const char *value) {

	char cmd[512];
	char valuebuffer[512] = {0};
	char *pointer;
	int hasSpace = 0;

	if (value == NULL) {
		sprintf(cmd,"cfg -a %s=", name);
		system(cmd);
		system("cfg -c > /tmp/cfg");
		
		return SUCCESS;
	}
	strncpy(valuebuffer, value, sizeof(valuebuffer));
	memset(cmd, 0, sizeof(cmd));
	pointer = valuebuffer;
	while(*pointer != '\0') {
		if (*pointer == ' ') {
			hasSpace = TRUE;
		} else if (*pointer == '`') {
			// replace special char to space
			*pointer = ' ';
			hasSpace = 1;
		}
		pointer++;
	}
	
	if (hasSpace) {
		sprintf(cmd,"cfg -a %s=\"%s\"", name, value);
	} else {
		sprintf(cmd,"cfg -a %s=%s", name, value);
	}
	system(cmd);
	system("cfg -c > /tmp/cfg");
	
	return SUCCESS;
}
*/

int set_single_config_attr(char *name, const char *value) {

	char cmd[512];
	char valuebuffer[512] = {0};
	char *pointer;
	int hasSpace = 0;

	if (value == NULL) {
		sprintf(cmd,"cfg -a %s=", name);
		system(cmd);
		system("cfg -c > /tmp/cfg");
		
		return SUCCESS;
	}
	strncpy(valuebuffer, value, sizeof(valuebuffer));
	memset(cmd, 0, sizeof(cmd));
	pointer = valuebuffer;
	while(*pointer != '\0') {
		if (*pointer == ' ') {
			hasSpace = TRUE;
		} else if (*pointer == '`') {
			// replace special char to space
			*pointer = ' ';
			hasSpace = 1;
		}
		pointer++;
	}
	
	if (hasSpace) {
		sprintf(cmd,"cfg -a %s=\"%s\"", name, value);
	} else {
		sprintf(cmd,"cfg -a %s=%s", name, value);
	}
	system(cmd);
	system("cfg -c > /tmp/cfg");
	
	return SUCCESS;
}


void replace_special_char(char *value) {
	do {
		if (*value < 32 || *value > 126) {
			*value = ' ';
		}
		value++;
	} while (*value != '\0');
}

int is_true_value(const char *value) {
	if (strncmp(value, STR_TRUE_LC, strlen(STR_TRUE_LC)) == 0 || 
		strncmp(value, STR_TRUE, strlen(STR_TRUE)) == 0 || 
		strncmp(value, "1", 1) == 0) {
		return 1;
	}
	return 0;
}

int is_false_value(char *value) {
	if (strncmp(value, STR_FALSE_UC, strlen(STR_FALSE_UC)) == 0 || 
		strncmp(value, STR_FALSE, strlen(STR_FALSE)) == 0 || 
		strncmp(value, "0", 1) == 0) {
		return 1;
	}
	return 0;
}


void parse_bands(char *hex, char *out) {
	int i;
	char bits[68];
	char bands[48];
	strcpy(bands, "0000000000000000");
	strcat(bands, hex);

	char *p = bands + (strlen(bands) - 16);
	p = p;
	memset(bits, 0, sizeof(bits));
	for (i = 0; i < 16; i++) {
		
	}
}

//去掉双引号
char* strip_double_quote( char* one_string )
{
	char* tmp=strstr(one_string,"\"");
	//首先查找是否存在双引号
	if( tmp == one_string )
	{
		one_string+=1;
	}

	tmp=strstr(one_string,"\"");
	if( tmp != NULL )
	{
		*tmp=0;
	}

	return one_string;
}

//装载客户相关的信息
int load_customer_info(void)
{
	//all_http_params
	const char* customer_file="/etc/rc.d/rc.other.conf";
	FILE* file_handle;
	char buffer[256];
	char*p1;
	char*tmp;
	/*
	#!/bin/sh
	export CPE_CUSTOMER="HEADELE"
	*/
	file_handle=fopen(customer_file,"r");
	//读取文件每一行,并进行分析
	if( file_handle == NULL )
	{
		return FALSE;
	}

	//读取文件的每一行,并进行处理
	while( fgets( buffer,sizeof(buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(buffer," ");
		if( p1 != NULL )
		{
			//要导出信息
			if( !strncmp(buffer,"export",p1-buffer) )
			{
				//跳过所有的空白
				p1=util_skip_all_spaces(p1);
				//查找字符串
				tmp=strstr(p1,"=");
				//获取到了客户名称信息
				if( !strncmp(p1,"CPE_CUSTOMER",tmp-p1) )
				{
					p1=tmp+1;
					//跳过所有的空白
					p1=util_skip_all_spaces(p1);
					//去除最后的空白
					util_strip_traling_spaces(p1);
					//跳过
					p1=strip_double_quote(p1);
					//返回客户信息
					strcpy(all_http_params.customer,p1);
					break;
				}
			}
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return TRUE;
}


//获取MAC地址
void load_mac_addr( void )
{
	MacAddr mac;
	//成功获取MAC地址
	if( !cmd_netdev_get_mac_addr( "br-lan",&mac ) )
	{
		sprintf(all_http_params.mac
				,"%02X:%02X:%02X:%02X:%02X:%02X"
				,mac.value[0]
				,mac.value[1]
				,mac.value[2]
				,mac.value[3]
				,mac.value[4]
				,mac.value[5]
				);
	}
}

int read_imei_info( const char* config_file,InfoStruct* server_wifi_info )
{
	char* p1;
	char* field_name;
	char* field_value;
	char temp_buffer[256];
	FILE* file_handle=fopen( config_file,"r" );

	if( file_handle == NULL )
	{
		return -1;
	}

	memset( temp_buffer,0,sizeof(temp_buffer) );
	/*
	~ / # cat /tmp/.system_info_static 
		device_name:INDUSTRY ROUTER
		hardware_version:TZ7.823.306A
		software_version:6.13.28
		sha:8baadea06e575ed2830cf974d98f8fcb97b2616a
		module_manufacturer:BroadMobi
		module_hardware:BM806U-T1
		module_softver:M1.3.1_E1.0.6_A1.2.2
		module_imei:867070020091490
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(temp_buffer,":");
		if( p1 == NULL )
		{
			continue;
		}

		if( p1 == temp_buffer )
		{
			continue;
		}

		field_name=temp_buffer;
		while( ( *field_name == ' ' ) || ( *field_name == '\t' ) )
		{
			field_name++;
		}

		*p1=0;
		field_value=p1+1;
		while( ( *field_value == ' ' ) || ( *field_value == '\t' ) )
		{
			field_value++;
		}

		for(	p1=field_value+strlen( field_value )-1;
				( ( *p1 == '\r' ) || ( *p1 == '\n' ) || ( *p1 == ' ' ) || ( *p1 == '\t' ) )&&( p1 >= field_value );

			)
		{
			*p1=0;
			p1--;
		}

		if( !strcmp( field_name,"module_imei" ) )
		{
			STRCPY_S( server_wifi_info->imei,field_value );
		}
	}

	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

int read_lte_param( const char* config_file,InfoStruct* server_wifi_info )
{
	char* p1;
	char* field_name;
	char* field_value;
	char temp_buffer[256];
	FILE* file_handle=fopen( config_file,"r" );

	if( file_handle == NULL )
	{
		return -1;
	}

	memset( temp_buffer,0,sizeof(temp_buffer) );
	/*
	~ / # cat /tmp/.system_info_dymatic 
		CELL_ID:176346898
		PCI:247
		RSRP:-105
		RSRQ:-10
		SINR:13.4
		LAC_ID:10173
	*/
	while( fgets( temp_buffer,sizeof(temp_buffer)-1,file_handle ) != NULL )
	{
		p1=strstr(temp_buffer,":");
		if( p1 == NULL )
		{
			continue;
		}

		if( p1 == temp_buffer )
		{
			continue;
		}

		field_name=temp_buffer;
		while( ( *field_name == ' ' ) || ( *field_name == '\t' ) )
		{
			field_name++;
		}

		*p1=0;
		field_value=p1+1;
		while( ( *field_value == ' ' ) || ( *field_value == '\t' ) )
		{
			field_value++;
		}

		for(	p1=field_value+strlen( field_value )-1;
				( ( *p1 == '\r' ) || ( *p1 == '\n' ) || ( *p1 == ' ' ) || ( *p1 == '\t' ) )&&( p1 >= field_value );

			)
		{
			*p1=0;
			p1--;
		}

		if( !strcmp( field_name,"rssi" ) )
		{
			STRCPY_S( server_wifi_info->RSSI,field_value );
		}
		else if( !strcmp( field_name,"CELL_ID" ) )
		{
			STRCPY_S( server_wifi_info->TZGLBCELLID,field_value );
		}
		else if( !strcmp( field_name,"PCI" ) )
		{
			STRCPY_S( server_wifi_info->TZPHYCELLID,field_value );
		}
		else if( !strcmp( field_name,"RSRP" ) )
		{
			STRCPY_S( server_wifi_info->TZRSRP,field_value );
		}
		else if( !strcmp( field_name,"RSRQ" ) )
		{
			STRCPY_S( server_wifi_info->TZRSRQ,field_value );
		}
		else if( !strcmp( field_name,"LAC_ID" ) )
		{
			STRCPY_S( server_wifi_info->LAC,field_value );
		}
		else if( !strcmp( field_name,"iccid" ) )
		{
			STRCPY_S( system_version_info.iccid,field_value );
		}
		else if( !strcmp( field_name,"imsi" ) )
		{
			STRCPY_S( system_version_info.imsi,field_value );
			
		}				
	}

	//关掉文件句柄
	fclose( file_handle );

	return 0;
}

//重新读取模块的状态信息
void read_lte_status( void )
{
    char *p, *q;
    char buffer[1024] = "";

    cmd_cat(TMP_LTE_INFO_FILE, buffer, sizeof(buffer));
    p = strtok(buffer, "$");
    while(p)
    {
        q = strsep(&p, "@");
        if(!strcmp(q, "EARFCN/ARFCN"))
            strcpy(system_version_info.TZEARFCN, p);
        else if(!strcmp(q, "Frequency Band"))
            strcpy(system_version_info.TZBAND, p);
        else if(!strcmp(q, "RSRP"))
            strcpy(system_version_info.TZRSRP, p);
        else if(!strcmp(q, "RSRQ"))
            strcpy(system_version_info.TZRSRQ, p);
        else if(!strcmp(q, "RSSI"))
            strcpy(system_version_info.RSSI, p);
        else if(!strcmp(q, "SINR"))
            strcpy(system_version_info.TZSINR, p);
        else if(!strcmp(q, "Serving CellID"))
            strcpy(system_version_info.TZGLBCELLID, p);
        else if(!strcmp(q, "Physical CellID"))
        {
            strcpy(system_version_info.TZPHYCELLID, p);
            util_strip_traling_spaces(system_version_info.TZPHYCELLID);
        }
        p = strtok(NULL, "$");
    }
}

//上报creg,cgreg,cereg的上报结果
void read_reg_status( void ) {
	int is_sim_exist;
	int need_pin;
	int need_puk;
	int signal_quality;
	int sms_status;
	int act;
	int network_data_service;

	util_read_status_ind(
		&is_sim_exist,
		&need_pin,
		&need_puk,
		&signal_quality,
		&sms_status,
		&reg_status,
		&greg_status,
		&ereg_status,
		&act,
		&network_data_service);
}

void read_ppp_users( void )
{
	strcpy(ppp_users[0].Enable, "1");
	strcpy(ppp_users[1].Enable, "1");
	strcpy(ppp_users[2].Enable, "1");
	strcpy(ppp_users[3].Enable, "1");
	
	strcpy(ppp_users[0].Name, "test01");
	strcpy(ppp_users[1].Name, "test02");
	strcpy(ppp_users[2].Name, "test03");
	strcpy(ppp_users[3].Name, "test04");
	
	strcpy(ppp_users[0].Uptime, "1000");
	strcpy(ppp_users[1].Uptime, "2000");
	strcpy(ppp_users[2].Uptime, "3000");
	strcpy(ppp_users[3].Uptime, "4000");
	
	strcpy(ppp_users[0].Username, "ppp001");
	strcpy(ppp_users[1].Username, "ppp002");
	strcpy(ppp_users[2].Username, "ppp003");
	strcpy(ppp_users[3].Username, "ppp004");
}

int getIfInfoByDevName(const char *devName, WanInfo *wanIf){

	int s = socket(AF_INET, SOCK_DGRAM, 0);
	if(s < 0){
		return 1;
	}
	
	struct ifreq ifr;	
	unsigned char mac[6];
	unsigned long ip, netmask;
	int mtu;
	
	strcpy(ifr.ifr_name, devName);	
	if(ioctl(s, SIOCGIFHWADDR, &ifr) < 0){
		close(s);
		return 1;
	}
	
	memcpy(mac, ifr.ifr_hwaddr.sa_data, sizeof(mac));
	sprintf(wanIf->MACAddress, "%02X:%02X:%02X:%02X:%02X:%02X",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	//printf("jiangyibo 222 %s\n",devName);		
	strcpy(ifr.ifr_name, devName);
	if(ioctl(s, SIOCGIFADDR, &ifr) < 0){
		//printf("jiangyibo 4444 %s\n",devName);	
		ip = 0;
	} else {
			//printf("jiangyibo 3333 %s\n",devName);	
		struct in_addr ip = ( ( ( struct sockaddr_in* )&( ifr.ifr_addr ) )->sin_addr );
		strcpy(wanIf->ExternalIPAddress, inet_ntoa(*(struct in_addr*)&ip));
	}
	
	strcpy(ifr.ifr_name, devName);
	if(ioctl(s, SIOCGIFNETMASK, &ifr) < 0){
		netmask = 0;
	} else {
		struct in_addr netmask = ( ( ( struct sockaddr_in* )&( ifr.ifr_addr ) )->sin_addr );
		strcpy(wanIf->SubnetMask, inet_ntoa(*(struct in_addr*)&netmask));
	}
	strcpy(ifr.ifr_name, devName);
	if(ioctl(s, SIOCGIFMTU, &ifr) < 0){
		mtu = 0;
	} else {
		mtu = *(int*)&ifr.ifr_mtu;
		wanIf->Mtu = mtu;
	}
	close(s);
	
	return 0;

}

int getGatewayByDevName(const char *devName, char *outText){
	
	outText[0] = '\0';
	
	char buffer[BIG_MAX_FIELD_LEN];
	char destination[NORMAL_FIELD_LEN];
	unsigned long gateway;
	
	FILE* f = fopen(ROUTE_FILE_NAME, "r");
	
	if(!f){
		return 1;
	}
	int devNamelen = strlen(devName);
	while(fgets(buffer, sizeof(buffer)-1, f)){
		if(!strncmp(buffer, devName, devNamelen)){
			sscanf(buffer, "%*s%s%lX", destination, &gateway);
			//if(!strncmp(destination, "00000000", 8)){
			if (gateway != 0) {
				sprintf(outText, "%s", inet_ntoa(*(struct in_addr*)&gateway));
				break;
			}
		}
	}
	
	fclose(f);
	
	return 0;
}

int getReservedAddress(char *outText,int sizelen)
{
	char buffer[64];
    memset(outText,0,sizelen);
    char *p1=outText;
    int len = 0;
	FILE *tp = fopen("/etc/rc.d/rc.dhcp.static","r");
	if(!tp){
		return -1;
	}
    while (fgets(buffer,sizeof(buffer)-1,tp)) {
            if (0 == strncmp(buffer,"#",1) || 0 ==strncmp(buffer,"\n",1)) {
                continue;
            } else {
                sscanf(buffer,"%*s%*s%s",p1);
                len += strlen(p1);
                outText[len] = ',';
                p1 = &outText[len+1];
                len++;
            }
    }
    outText[len-1] = '\0';
    fclose(tp);
	
	return 0;
}

int read_dhcptatic_info(DHCPStatic *dhcpstaticinfo,int sizelen)
{
	char buffer[64];
	char shellcmd[128] = {0};
	int i = 0;
	sprintf(shellcmd, "cat /tmp/dhcp.leases | wc -l");
	read_memory(shellcmd, buffer, sizeof(buffer));
	igd_entries.dhcpstatic_entry = atoi(buffer);
	if (igd_entries.dhcpstatic_entry > 0) {
		memset(dhcpstaticinfo,0,sizelen);
		memset(buffer,0,sizeof(buffer));
		FILE *tp = fopen("/tmp/dhcp.leases","r");
		if(!tp){
			return -1;
		}
		for (i = 0; i < igd_entries.dhcpstatic_entry; i++) {
			if (fgets(buffer,sizeof(buffer)-1,tp) != NULL) {
				if (0 == strncmp(buffer, "# Static leases map", 19)) {
					i--;
					continue;
				}
				if ( 0 == strncmp(buffer, "\n", 1) || 0 == strncmp(buffer, "\0", 1) ||  0 == strlen(buffer)) {
					i--;
	                continue;
	            } else {
	                sscanf(buffer,"%*s%*s%s",dhcpstaticinfo[i].IPAddress);
					sscanf(buffer,"%*s%s%*s",dhcpstaticinfo[i].MacAddress);
					strcpy(dhcpstaticinfo[i].Enable, "true");
	            }
			}
		}
	    fclose(tp);
	}else {
		igd_entries.dhcpstatic_entry = 0;
	}
	return 0;
}


int isipv4Str(char *buffer){
	char *p;
	int count = 0;
	p = strstr(buffer, ".");
	
	while(p != NULL){
		count++;
		p = strstr(p+1, ".");
	}
	if(count == 3){
		return 1;
	} else {
		return 0;
	}
}

int getDNS(char *outText, char *type){
	
	outText[0] = '\0';
	
	char buffer[512];
	
	FILE* f = fopen(DNS_FILE_NAME, "r");
	
	if(!f){
		return 1;
	}
	
	
	int len;
	while(fgets(buffer, sizeof(buffer)-1, f)){
		len = 0 ;
		while(isspace(buffer[len]) && buffer[len] != '\n'){
			len++;
		}
		if(buffer[len] == 'n' || buffer[len] == 'N'){
			if(!strncmp(type, "ipv4", 4) && isipv4Str(buffer)){
				strcat(outText, buffer);
			}
			if(!strncmp(type, "ipv6", 4) && !isipv4Str(buffer)){
				strcat(outText, buffer);
			}
		}
	}
	
	fclose(f);
	
	return 0;
}
int getNetStat(char *ethname)
{
    char    buffer[64];
    FILE    *read_fp;
    int        chars_read;
    int        ret;

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer, "ifconfig %s | grep UP", ethname);
    read_fp = popen(buffer, "r");
    if ( read_fp != NULL )
    {
        memset(buffer, 0, sizeof(buffer));
        chars_read = fread(buffer, sizeof(char), sizeof(buffer) - 1, read_fp);
        if (chars_read > 0)
        {
            ret = 1;
        }
        else
        {
            ret = 0;
        }
        if(strstr(buffer, "RUNNING"))
            ret = 2;
        pclose(read_fp);
    }
    else
    {
        ret = 0;
    }

    return ret;
}

void read_wan_info(){
    char buffer[64];
    strcpy(wan_info.Enable, "true");
    
    read_memory("getinfacestatus -i wan -p l3_device",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.Name,buffer);

    read_memory("getinfacestatus -i wan -p proto",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    if(0 == strcmp(buffer,"dhcp")){
        strcpy(wan_info.AddressingType,"DHCP");
    }
    else if(0 == strcmp(buffer,"static")){
        strcpy(wan_info.AddressingType,"Static");
    }
    else if(0 == strcmp(buffer,"pppoe")){
        strcpy(wan_info.AddressingType,"PPPoE");
    }
    else{
        wan_info.AddressingType[0] = 0;
    }

    read_memory("getinfacestatus -i wan -p uptime",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.Uptime,buffer);

    read_memory("getinfacestatus -i wan -p ipv4-address",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.ExternalIPAddress,buffer);

    read_memory("getinfacestatus -i wan -p netmask",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.SubnetMask,buffer);

    read_memory("getinfacestatus -i wan -p nexthop",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.DefaultGateway,buffer);

    strcpy(wan_info.DNSEnabled, "true");
   
    read_memory("getinfacestatus -i wan -p dns-server",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.DNSServers,buffer);
     
    read_memory("getinfacestatus -i wan -p mac",buffer,sizeof(buffer));
    util_strip_traling_spaces(buffer);
    strcpy(wan_info.MACAddress,buffer);
  
    //get mtu 
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        wan_info.Mtu = 1500;
    }
    else{
        struct ifreq ifr;
        strcpy(ifr.ifr_name, wan_info.Name);
        if(ioctl(sock, SIOCGIFMTU, &ifr) < 0){
            wan_info.Mtu = 1500;
	} else {
	    wan_info.Mtu = *(int*)&ifr.ifr_mtu;
	}
	close(sock);
    }
}
/*
void read_wan_info() {

	strcpy(wan_info.Enable, "1");
	strcpy(wan_info.AddressingType, "DHCP");

	char buffer[1024], ifname[16], dns[64];
	memset(buffer, 0, sizeof(buffer));
	
	strcpy(ifname, util_get_wan_interface_name());
	//printf("jiangyibo bbb1 %s\n",ifname);
	strcpy(wan_info.Name, ifname);
	getIfInfoByDevName(ifname, &wan_info);
	
	getGatewayByDevName(ifname, wan_info.DefaultGateway);
	//getIPV6GatewayByDevName(ifname, values[pointer++]);

	char *pdns = wan_info.DNSServers;
    pdns[0] = '\0';
	memset(buffer, 0, sizeof(buffer));
	if (getDNS(buffer, "ipv4") == SUCCESS) {
		get_attr_by_line(buffer, "nameserver", dns);
        strcat(pdns, dns);
		
		get_attr_by_line(buffer + 10, "nameserver", dns);
		if (strlen(pdns) > 0 && strlen(dns) > 0) {
			strcat(pdns, ";");
		}
		strcat(pdns, dns);
	}
	
	memset(buffer, 0, sizeof(buffer));
	if (getDNS(buffer, "ipv6") == SUCCESS) {
		get_attr_by_line(buffer, "nameserver", dns);
		if (strlen(pdns) > 0 && strlen(dns) > 0) {
			strcat(pdns, ";");
		}
		strcat(pdns, dns);
		
		get_attr_by_line(buffer + 10, "nameserver", dns);
		if (strlen(pdns) > 0 && strlen(dns) > 0) {
			strcat(pdns, ";");
		}
		strcat(pdns, dns);
	}
}
*/
int get_parameter_index(char *name, char *str, int max) {

	char *p = strstr(name, str);
	if (p == NULL) {
		return 1;
	}

    int index = atoi(p + strlen(str));
	if (index <= 0) {
		return 1;
	} else if (index > max) {
		return max;
	} else {
		return index;
	}
}

int parse_wan_index(const char *name)
{
	const char * wan = "WANConnectionDevice.";

	char *p = strstr(name, wan);
	if (p == NULL) return 1;

	int index = atoi(p + strlen(wan));
	//cwmp_log_debug("index = %d", index);
	if (index == 0) return 1;

	return index;
}

unsigned int hex2int(char c)
{
	if (c >= '0' && c <= '9')
		return (unsigned int)(c-'0');
	if (c >= 'a' && c <= 'f')
		return (unsigned int)(c-'a'+10);
	if (c >= 'A' && c <= 'F')
		return (unsigned int)(c-'A'+10);

	return 0;
}

char string2int(char *str, unsigned int *pOut)
{
	int i,n;
	unsigned int val;

	*pOut = 0;
	n = strlen(str);

	for(i = 0; i < n && i < 8; i++)
	{
		val = hex2int(*str);
		*pOut = *pOut<<4;
		*pOut |= val;

		str++;
	}

	return i;
}

char *strrstr(char const *s1, char const *s2){

	  register char *last;
	  register char *current;

	  last = NULL;
	  if (*s2 != '\0'){
		    current = strstr(s1, s2);
		    while (current != NULL){
			      last = current;
			      current = strstr(last + 1, s2);
		    }
	  }

	  return last;
}

void parse_domain_name(char *host) {
	int is_ipv4_addr_exist=FALSE;
	int is_ipv6_addr_exist=FALSE;
	char network_dev_name[64];
	char ipv4_addr[64];
	char ipv6_addr[128];
	int result_ind;
	
	if( util_resolv_domain_name(host,ipv4_addr,ipv6_addr,&result_ind ) )
	{
		
		strcpy( network_dev_name, util_get_wan_interface_name() );
		//check if ipv4 addr exist
		is_ipv4_addr_exist=cmd_ip_exist( network_dev_name );
		//check if ipv6 addr exist
		is_ipv6_addr_exist=cmd_ipv6_exist( network_dev_name );
			
		if( is_ipv4_addr_exist && ( result_ind&IPv4_EXIST_FLAG ) )
		{
			strcpy(host, ipv4_addr);
		}

		if( is_ipv6_addr_exist && ( result_ind&IPv6_EXIST_FLAG ) )
		{
			strcpy(host, ipv6_addr);
		}
	}
}

void parse_cell_info(char *value) {
	unsigned int id, act;
	char cellList[512], tmp[64];
	char *head, *head1, *token;

	strcpy(cellList, value);
	strcpy(value, "");

	token = strtok(cellList, ";");
	while(token != NULL && strlen(token) > 0) {
		head = strstr(token, ",");
		if (head != NULL) {
			memset(tmp, 0, sizeof(tmp));
			strncpy(tmp, token, head - token);
			string2int(tmp, &id);
			sprintf(tmp, "%d", id);
			
			strcat(value, tmp);
			strcat(value, ",");

			head++;
			head1 = strstr(head, ",");

			if (head1 != NULL) {
				memset(tmp, 0, sizeof(tmp));
				strncpy(tmp, head, head1 - head);
				string2int(tmp, &id);
				sprintf(tmp, "%d/%d", id / 256, id & 0xFF);
				
				strcat(value, tmp);
				strcat(value, ",");

				head1++;
				act = atoi(head1);
				switch(act) {
					case 7: strcat(value, "4G"); break;
					case 2: strcat(value, "3G"); break;
					case 0: strcat(value, "2G"); break;
					default: break;
				}
			} else {
				strcat(value, ",");
			}
		} else {
			strcat(value, ",");
			strcat(value, ",");
		}

		strcat(value, ";");
		
		token = strtok(NULL, ";");
	}
}

void parse_lock_cell(const char *value, unsigned int *cellId, unsigned int *cellIdCount) {
	char *head, *tail;
	char count[16];
	int tmp;
	
	if (value == NULL) return;

	head = strstr(value, "4G,");
	if (head == NULL) return;
	
	head += 3;
	tail = strstr(head, ";");
	if (tail == NULL) return;
	
	strncpy(count, head, tail - head);
	tmp = atoi(count);
	if (tmp > 0) {
		*cellId = 1 << 16;
		*cellIdCount = tmp << 16;
	}
}

int process_lock_cell(const char *value) {
	char tmp[16];
	unsigned int cellId = 0;
	unsigned int cellIdCount = 0;
	parse_lock_cell(value, &cellId, &cellIdCount);

	sprintf(tmp, "%d", cellId);
	set_single_config_attr("TZ_LOCK_CELL_ID", tmp);
	
	sprintf(tmp, "%d", cellIdCount);
	set_single_config_attr("TZ_LOCK_CELL_ID_COUNT", tmp);
	
	cmd_touch(DELETE_CELL_LOCK_CACHE_FILE);
	sleep(2);
	cmd_rm(ALL_CELL_ID_INFO);
	
	return CWMP_OK;
}


void kill_httpd() {

	char pid[20], userName[20], shellcmd[256], buffer[256];
	strcpy(shellcmd, "ps | grep httpd | grep \"tzwww\" | grep -v grep");

	int count = 0;
	while(count++ < 10) {
		read_memory(shellcmd, buffer, sizeof(buffer));
		
		cwmp_log_debug("kill_httpd: %s", buffer);

		if (strlen(buffer) > 0) {
			sscanf(buffer, "%s%s", pid, userName);

			sprintf(shellcmd, "kill %s >/dev/null 2>&1", pid);
			cwmp_log_debug("kill_httpd: %s", shellcmd);

			system(shellcmd);
		} else {
			break;
		}
	}
}


static char login_status[16];
int get_remote_login_status(char **value) {

	char buffer[256];
	uci_get_single_config_attr("uci get dhcp.lan.https 2>/dev/null", buffer);
	if (strncmp(buffer, STR_YES, sizeof(STR_YES)) == 0) {
		strcpy(login_status, "1");
	} else {
		strcpy(login_status, "0");
	}
	*value = login_status;
	
    return FAULT_CODE_OK;
}

int process_remote_login(int enabled) {

	char shellcmd[256], ip[32];
	
	if (enabled) {
		kill_httpd();
		
		strcpy(shellcmd, "/usr/sbin/httpd -h /tzwww -c /etc/httpd.conf");
		system(shellcmd);

		set_single_config_attr(TZ_ALLOW_LOGIN_FROM_WAN, STR_YES);
		cwmp_log_debug("open_remote_login: %s", shellcmd);
	} else {
		kill_httpd();

		get_single_config_attr(AP_IP_ADDRESS, ip);
		sprintf(shellcmd, "/usr/sbin/httpd -h /tzwww -p %s -c /etc/httpd.conf", ip);
		system(shellcmd);
		
		set_single_config_attr(TZ_ALLOW_LOGIN_FROM_WAN, STR_NO);
		cwmp_log_debug("close_remote_login: %s", shellcmd);
	}
	
    return FAULT_CODE_OK;
}

/********************** Update *************************/

void get_kernel_file_path(int isClient, char *filePath) {
	if (isClient) {
		strcpy(filePath, "/tmp/vmlinux_routing.lzma.uImage");
	} else {
		strcpy(filePath, "/tmp/vmlinux.lzma.uImage");
	}
}

static int enable_log_memory = 0;
void set_log_memory_enable() {
	enable_log_memory = 1;
}
void cwmp_log_memory_info(char *info) {
	if (!enable_log_memory) return;
	
	char memory[16];
	read_memory("ps | grep cwmpd | grep -v grep | awk '{print $3}' | awk 'NR==1'", memory, sizeof(memory));
	
	cwmp_log_debug("%s=============== Memory Used: %s", info, memory);
}

void cwmp_log_memory() {
	cwmp_log_memory_info("");
}

char* GetBasename( char* url )
{
	char* tmp_ptr;
	char* tmp_ptr2;

	tmp_ptr=url;
	tmp_ptr2=strstr( tmp_ptr,"/" );
	while( tmp_ptr2 )
	{
		tmp_ptr=tmp_ptr2+1;
		tmp_ptr2=strstr( tmp_ptr,"/" );
	}

	return tmp_ptr;
}

void GetFilePrefix( char* file_name,char* file_prefix )
{
	char* tmp_ptr;
	char* tmp_ptr2;

	tmp_ptr=file_name+strlen( file_name )+1;
	tmp_ptr2=strstr( file_name,"." );
	while( tmp_ptr2 )
	{
		tmp_ptr=tmp_ptr2+1;
		tmp_ptr2=strstr( tmp_ptr,"." );
	}

	strncpy( file_prefix,file_name,tmp_ptr-file_name-1 );
	file_prefix[ tmp_ptr-file_name-1 ]=0;
}

int util_get_month_value_by_str(char *month_str)
{
	int index = 0;
	while(index < sizeof(month_array) / sizeof(month_array[0]))
	{
		if( strncmp(month_str
				,month_array[index].month_str
				,strlen(month_array[index].month_str)) == 0 )
		{
			return month_array[index].month_value;
		}

		index+=1;
	}

	return 0;
}

int get_sys_time(char *theDateTime) {

	int month, day;
	char strMonth[4], strYear[8], strDay[4], strTime[10];
	char *pointer, datetime[64];
	read_memory("date", datetime, sizeof(datetime));

	pointer = datetime;
	pointer += 4;
	memset(strMonth, 0, sizeof(strMonth));
	strncpy(strMonth, pointer, 3);

	month = util_get_month_value_by_str(strMonth);

	pointer += 4;
	memset(strDay, 0, sizeof(strDay));
	if (*pointer == ' ') {
		strncpy(strDay, pointer + 1, 1);
	} else {
		strncpy(strDay, pointer, 2);
	}
	day = atoi(strDay);
	
	pointer += 3;
	memset(strTime, 0, sizeof(strTime));
	strncpy(strTime, pointer, 8);

	pointer += 13;
	memset(strYear, 0, sizeof(strYear));
	strncpy(strYear, pointer, 4);

	sprintf(theDateTime, "%s-%02d-%02d %s", strYear, month, day, strTime);

	return SUCCESS;
}

int load_timeinfor_cmd(char *time_info)
{
	char datetime[128],*pointer;
	read_memory("date -R", datetime, sizeof(datetime));
	pointer = datetime;
	sscanf(pointer,"%*s%*s%*s%*s%*s%s",time_info);
	return 0;
}


/*
 *second:6 bit
 *minute:6 bit
 *hour:5 bit
 *day:5 bit
 *month:4 bit
 *year:6 bit
 * */
int util_get_compile_date_info(char *ptr, VersionInfo *version_info, int omit_str)
{
	char *tmp_ptr=NULL;
	int value;

	tmp_ptr=strchr(ptr,' ');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//month
	ptr=tmp_ptr+1;


	value=util_get_month_value_by_str(ptr);
	//month 4 bit
	version_info->compile_date=value<<22;

	tmp_ptr=strchr(ptr,' ');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//day
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	//day 5 bit
	version_info->compile_date+=value<<17;


	tmp_ptr=strchr(ptr,' ');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//hour
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	//hour 5 bit
	//version_info->compile_date+=value<<12;

	tmp_ptr=strchr(ptr, ':');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//minute
	ptr=tmp_ptr+1;

	value=strtol(ptr, NULL, 10);
	//minute 6 bit
	//version_info->compile_date+=value<<6;

	tmp_ptr = strchr(ptr, ':');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//second
	ptr=tmp_ptr + 1;

	value=strtol(ptr,NULL,10);
	//second 6 bit
	//version_info->compile_date+=value;

	if( !omit_str )
	{
		tmp_ptr=strchr(ptr,' ');
		if(tmp_ptr == NULL)
		{
			return FALSE;
		}

		ptr=tmp_ptr+1;
	}

	tmp_ptr=strchr(ptr,' ');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	//year
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	//year 6 bit
	if(value >= CALCULATE_START_YEAR)
	{
		version_info->compile_date+=(value-CALCULATE_START_YEAR)<<26;
	}

	return TRUE;
}


//--LSDK-9.2.0_U10.1020
int util_get_sdk_version_info(char* ptr,VersionInfo* version_info)
{
	char* tmp_ptr=NULL;
	int value;

	tmp_ptr=strstr(ptr,"LSDK-");
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}
	ptr=tmp_ptr+strlen("LSDK-");

	value=strtol(ptr,NULL,10);
	//day 5 bit
	version_info->sdk_version=value<<24;


	tmp_ptr=strchr(ptr,'.');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	//day 5 bit
	version_info->sdk_version+=value<<16;

	tmp_ptr=strchr(ptr,'.');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	//day 5 bit
	version_info->sdk_version+=value<<8;


	tmp_ptr=strstr(ptr,"_U");
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}
	ptr=tmp_ptr+strlen("_U");

	value=strtol(ptr,NULL,10);
	//day 5 bit
	version_info->sdk_version+=value;

	tmp_ptr=strchr(ptr,'.');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}
	ptr=tmp_ptr+1;

	value=strtol(ptr,NULL,10);
	version_info->sub_sdk_version=value;

	return TRUE;
}

int util_get_kernel_version_info(char* ptr,VersionInfo* version_info)
{
	char* tmp_ptr=NULL;
	int value=0;

	value=strtol(ptr,NULL,10);
	version_info->kernel_version=value<<24;

	tmp_ptr=strchr(ptr,'.');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;
	value=strtol(ptr,NULL,10);
	version_info->kernel_version+=value<<16;

	tmp_ptr=strchr(ptr,'.');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;
	value=strtol(ptr,NULL,10);
	version_info->kernel_version+=value<<8;


	return util_get_sdk_version_info(ptr,version_info);
}

//Linux (none) 2.6.31--LSDK-9.2.0_U10.1020 #52 Fri Dec 21 13:58:23 HKT 2012 mips unknown
int util_get_current_system_version_info(char *version_str, VersionInfo *version_info)
{
	char *ptr = version_str;
	char *tmp_ptr = version_str;
	int tmp_value;

	if(version_info == NULL)
	{
		return FALSE;
	}

	tmp_ptr=strchr(ptr,')');

	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;

	//get kernel information
	tmp_value = util_get_kernel_version_info(ptr,version_info);
	if( !tmp_value )
	{
		return FALSE;
	}

	tmp_ptr=strchr(ptr,'#');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;

	tmp_ptr=strchr(ptr,' ');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;

	//get compile date information
	return util_get_compile_date_info(ptr, version_info, FALSE);
}

int util_get_update_system_version_info(char *version_str,VersionInfo* version_info)
{
	char *ptr=version_str;
	char *tmp_ptr=version_str;
	int tmp_value;

	//get compile date information
	tmp_value=util_get_compile_date_info(ptr,version_info,TRUE);
	if( !tmp_value )
	{
		return FALSE;
	}

	tmp_ptr=strchr(ptr,'\n');
	if(tmp_ptr == NULL)
	{
		return FALSE;
	}

	ptr=tmp_ptr+1;

	//get kernel information
	return util_get_kernel_version_info(ptr,version_info);
}

/*
void get_module_version(char *moduleVersion) {
	char data[1024];
	if (cmd_cat(LTE_VERSION_FILE_NAME, data, sizeof(data)) == 0) {
		get_attr_by_line(data, "modversion", moduleVersion);
	} else {
		moduleVersion[0] = '\0';	
	}
}

int process_module_file_upload(int stime, char *fpath) {

	char shellcmd[512], log_info[512], moduleVersion[256];
	
	get_module_version(moduleVersion);

	sprintf(log_info, "TR069 module upgrade - moduleVersion: %s", moduleVersion);
	write_sys_log(log_info);

	char *version = strrstr(moduleVersion, "_V");
	if (version == NULL) {
		sprintf(log_info, "TR069 module upgrade - module type error.", moduleVersion);
		write_sys_log(log_info);
		return FAIL;
	}
	sprintf(log_info, "TR069 module upgrade - version: %s", version);
	write_sys_log(log_info);

	moduleVersion[strlen(moduleVersion) - strlen(version)] = '\0';
	
	if (strlen(moduleVersion) == 0) {
		write_sys_log("TR069 module upgrade - moduleVersion is empty.");
		return FAIL;
	}
	sprintf(log_info, "TR069 module upgrade - moduleVersion: %s", moduleVersion);
	write_sys_log(log_info);

	if (strstr(fpath, moduleVersion) == NULL) {
		write_sys_log("TR069 module upgrade - module type not match");
		return FAIL;
	}

	system("killall moduleupdate > /dev/null 2>&1");
	sprintf(shellcmd, "moduleupdate -t%d -T3 -u%s > /dev/null 2>&1 &", stime, fpath);
	system(shellcmd);

	return SUCCESS;
}

json_t *get_module_file_update_result_code(char *cmd) {

	char resp_code_str[NORMAL_FIELD_LEN], log_info[NORMAL_FIELD_LEN], 
		 progress_buff[NORMAL_FIELD_LEN], progress[NORMAL_FIELD_LEN];	
	
	json_t *json_response, *label_json, *value_json;

	if(!cmd_cat(FILE_UPLOAD_RESULT_CODE, resp_code_str, NORMAL_FIELD_LEN)) {
		trim_end_line(resp_code_str, strlen(resp_code_str));
		
		sprintf(log_info, "Remote uploadtool response code: %s", resp_code_str);	
		write_sys_log(TRUE, log_info);

		json_response = create_single_json(TRUE, cmd, resp_code_str);	
	} else {
		json_response = create_single_json(TRUE, cmd, "0");
	}
	
	if(!cmd_cat(FILE_UPLOAD_PROGRESS_FILE, progress_buff, NORMAL_FIELD_LEN)){
		sprintf(progress, "%s", progress_buff);
		trim_end_line(progress, strlen(progress));
	} else {
		strcpy(progress, "-1");
	}
	sprintf(log_info, "Remote uploadtool progress: %s", progress);
	write_sys_log(TRUE, log_info);
	
	label_json = json_new_string("progress");
	value_json = json_new_string(progress);
	json_insert_child(label_json, value_json);
	json_insert_child(json_response, label_json);
	
	return json_response;
}

json_t *process_module_upgrade(json_t *root, char *cmd, char *method){

	char url[NORMAL_FIELD_LEN], message[NORMAL_FIELD_LEN], operate[20];

	json_t *result_json, *remote_json, *remote_operate_json, *updateType_json, *timeout_json;
	remote_json = json_find_first_label(root, "remoteServerAddr");
	remote_operate_json = json_find_first_label(root, "remoteOperate");
	updateType_json = json_find_first_label(root, "updateType");
	timeout_json = json_find_first_label(root, "timeout");
	
	int updateType = 3;
	int stime = 180;
	memset(message, 0, sizeof(message));

	if(remote_json == NULL || remote_operate_json == NULL){
		return create_single_json(FALSE, cmd, "\u7f3a\u5c11\u5fc5\u8981\u53c2\u6570");
	}
	if (updateType_json != NULL) {
		updateType = atoi(updateType_json->child->text);
		if (updateType < 0 || updateType > 3) {
			updateType = 3;
		}
	}
	if (timeout_json != NULL) {
		stime = atoi(timeout_json->child->text);
		if (stime < 10) {
			stime = 10;
		}
	}

	strcpy(url, remote_json->child->text);
	strcpy(operate, remote_operate_json->child->text);

	if (!strcmp("MODULAR", operate)) {
		system("rm /tmp/\*.bins");
		if (process_module_file_upload(updateType, stime, url, message) == SUCCESS) {
			return create_single_json(TRUE, cmd, "");
		} else {
			return create_single_json(FALSE, cmd, message);
		}
	} else if (!strcmp("CHECK_CODE", operate)) {
		return get_module_file_update_result_code(cmd);
	} else {
		return create_single_json(FALSE, cmd, "\u64cd\u4f5c\u65e0\u6548");
	}
}*/


int is_required_update() {

	char buffer[512];
	VersionInfo current_system_version_info, update_system_version_info;
	//int required_update = FALSE;

	memset(&current_system_version_info, 0, sizeof(current_system_version_info));
	memset(&update_system_version_info, 0, sizeof(update_system_version_info));

	write_sys_log("TR069: begin checking is_required_update uboot");

	//get os information
	//Linux (none) 2.6.31--LSDK-9.2.0_U10.1020 #52 Fri Dec 21 13:58:23 HKT 2012 mips unknown
	if(read_memory("uname -a", buffer, sizeof(buffer)) == FAIL) {
		write_sys_log("TR069: execute uname -a error");
		return FALSE;
	}

	if(!util_get_current_system_version_info(buffer, &current_system_version_info)) {
		write_sys_log("TR069: util_get_current_system_version_info error");
		return FALSE;
	}

	if(cmd_cat(VERSION_UPDATE_FILE_NAME, buffer, sizeof(buffer)) == -1 || strlen(buffer) == 0) {
		write_sys_log("TR069: read /tmp/.update.version error");
		return FALSE;
	}

	if(!util_get_update_system_version_info(buffer, &update_system_version_info)) {
		write_sys_log("TR069: util_get_update_system_version_info error");
		return FALSE;
	}

	if (update_system_version_info.compile_date != current_system_version_info.compile_date) {
		write_sys_log("TR069: required update uboot");
		return TRUE;
	} else {
		write_sys_log("TR069: not required update uboot");
		return FALSE;
	}
}

/*
 * ADD 2013-07-18
 *
 */
void watchdog(){

	//system("cat /proc/tozed/watchdog && touch /tmp/watchdogflag && echo \"V\"> /proc/tozed/watchdog");
	if(cmd_cat(PROC_TOZED_WATCHDOG, NULL, 0) == 0){
		if(cmd_touch("/tmp/watchdogflag") == 0){
			cmd_echo("V", PROC_TOZED_WATCHDOG);
		}
	}
	
	sleep(1);
	//system("echo \"V\"> /proc/tozed/watchdog");
	cmd_echo("V", PROC_TOZED_WATCHDOG);
}

int update_config(char *filePath)
{
	char shellcmd[256], log_info[256];

	const char* out_txt = "/tmp/config_out_txt";
	const char* out_exe = "/tmp/config_out_exe";
	const char* dest_file_name = "/usr/manage_requirement_config_file.conf";

	sprintf(shellcmd, "configloader -c %s -o %s -e %s -n > /dev/null 2>&1", 
			filePath, out_txt, out_exe);

	sprintf(log_info, "TR069: %s", shellcmd);
	write_sys_log(log_info);

	int ret = -1;
	ret = system(shellcmd);
	if (ret == 0 && cmd_file_exist(out_txt)) {
		strcpy(log_info, "TR069: Remote configloader success.");
		write_sys_log(log_info);
		
		sprintf(shellcmd, "mv %s %s", filePath, dest_file_name);
		system(shellcmd);

		system("/etc/rc.d/rc.backup-before-fota-upgrade  > /dev/null 2>&1");
		strcpy(log_info, "TR069: backup before-config-upgrade.");
		write_sys_log(log_info);

		system("/etc/rc.d/rc.execute-config-fota-upgrade  > /dev/null 2>&1");
		strcpy(log_info, "TR069: execute-config-fota-upgrade.");
		write_sys_log(log_info);

//		sprintf(shellcmd, "configloader -c %s -o %s -e %s > /dev/null 2>&1",
//			dest_file_name, out_txt, out_exe);
//		ret = system(shellcmd);

		ret = system("/etc/rc.d/rc.restore-after-fota-upgrade  > /dev/null 2>&1");
		strcpy(log_info, "TR069: restore-after-fota-upgrade.");
		write_sys_log(log_info);

		if (ret == 0) {
			strcpy(log_info, "TR069: Remote configloader again success.");
		} else {
			strcpy(log_info, "TR069: Remote configloader again fail.");
		}
		write_sys_log(log_info);
		
		cmd_rm(out_txt);
		cmd_rm(out_exe);

		// create flag file
		cmd_touch(MANAGE_REQUIREMENT_CONFIG_FILE_CHANGED);

		return SUCCESS;
	}
	strcpy(log_info, "TR069: Remote configloader fail.");
	write_sys_log(log_info);

	return FAIL;
}

int update_uboot(int isDirectUpdate) {

	int isClient = TRUE;
	char filePath[256], shellcmd[256], buffer[512];

	if (!isDirectUpdate) {
		if (!is_required_update()) {
			write_sys_log("TR069: Not required update uboot.");
			return SUCCESS;
		}
	}
	get_kernel_file_path(isClient, filePath);

	if (cmd_file_exist(filePath)) {
		
		struct stat statBuffer;
		unsigned long fileSize;

		if(stat(filePath, &statBuffer) < 0) {
            fileSize = -1;
        } else {
            fileSize = statBuffer.st_size;
        }

		if (read_bin(filePath, buffer, sizeof(buffer)) == FAIL) {
			write_sys_log("TR069: read uboot bin fail.");
			return FAIL;
		}

        char *tmp = substring(buffer, 32, 12);
		
        if (fileSize >= 700 * 1024 && !memcmp(tmp, "Linux Kernel", 12)) {
            memset(shellcmd, 0, sizeof(shellcmd));
			if (isClient) {
                sprintf(shellcmd, "update %s /dev/mtd2 > /tmp/updateboot", filePath);
			} else {
                sprintf(shellcmd, "update %s /dev/mtd3 > /tmp/updateboot", filePath);
			}
            
            system(shellcmd);

			write_sys_log("TR069: update kernel success.");

			return SUCCESS;
        } else {
			return FAIL;
		}
	} else {
		if (isDirectUpdate) {
			write_sys_log("TR069: uboot file not exists.");
			return FAIL;
		} else {
			return SUCCESS;
		}
	}
}

static int read_file_data(const char *fileName, char *buffer, int buf_size)
{
	int n;
	FILE *fp;

	if(buffer==NULL || buf_size<=0)
		return 0;

	buffer[0]='\0';

	fp = fopen((char *)fileName, "rb");
	if(fp == NULL)
		return 0;
	else{
		n = fread(buffer, 1, buf_size-1, fp);
		fclose(fp);
	}

	buffer[n]='\0';

	return n;
}

int update_partial(char *url) {

	//int isClient = TRUE;
	//int isForceUpdateUboot = FALSE;
	int ret;

	write_sys_log("TR069: Updating begin");

	char shellcmd[256], buffer[256], flagfile[256];
	char fileNameWithoutExtension[256], filePath[256], tempFileName[256];
	
	char* fileName = GetBasename(url);
	sprintf(flagfile, "/usr/upgrade_flag_%s", fileName);
	cwmp_log_debug("filename: %s", fileName);
	GetFilePrefix(fileName, fileNameWithoutExtension);
	
	sprintf(filePath, "/tmp/%s.zip", fileNameWithoutExtension);

	if (cmd_file_exist(filePath))
	{
		//system("mkdir -p /tmp/tr069_update");
		system("rm -f /tmp/*.tar.lzma /tmp/*.tar.lzma.md5.txt");
		sprintf(shellcmd,"cd /tmp && unzip -o %s > /dev/null 2>&1", filePath);
		if(system(shellcmd)) {
			write_sys_log("TR069: Unzip failed.");
			return FAIL;
		}
		cmd_rm(filePath);
	}

	const char *tmp_upcfg_filepath="/tmp/tmp.upcfg.info";
	sprintf(shellcmd, "cd /tmp && ls | grep P5311_*.conf > %s", tmp_upcfg_filepath);	// don't run cmd: ls /tmp/ | grep xxx
	system(shellcmd);
	if(cmd_file_exist(tmp_upcfg_filepath))
	{
		memset(buffer, 0, sizeof(buffer));
		if(read_file_data(tmp_upcfg_filepath, buffer, sizeof(buffer))>0)
		{
			char *p;
			
			p = strchr(buffer, '\r');
			if(p!=NULL)
				p[0]='\0';

			p = strchr(buffer, '\n');
			if(p!=NULL)
				p[0]='\0';
				
			sprintf(filePath, "/tmp/%s", buffer);
			cwmp_log_debug("checking conf_path: %s\n", filePath);
			if (access(filePath, F_OK)>=0)
			{
				cwmp_log_debug("start update config\n");
				update_config(filePath);
			}
			else
			{
				cwmp_log_debug("config %s no exist\n", filePath);
			}
		}
	}
	
	strcpy(shellcmd, "find /tmp -name \"*.tar.lzma\"");
	memset(buffer, 0, sizeof(buffer));
	if (read_memory(shellcmd, buffer, sizeof(buffer)) == SUCCESS) {
		char *temp = strrstr(buffer, "/");
		if (temp != NULL) {
			strcpy(tempFileName, temp + 1);
			trim_end_line(tempFileName, strlen(tempFileName));
			sprintf(filePath, "/tmp/%s", tempFileName);
			
			cwmp_log_debug("TR069: UPDATE SYSTEM %s.", filePath);
		} else {
			cwmp_log_debug("TR069: find *.tar.lzma failed.");
		}
	} else {
		cwmp_log_debug("TR069: read *.tar.lzma failed.");
	}

	// compare md5
	sprintf(buffer, "%s.md5.txt", filePath);
	if(!cmd_file_exist(buffer)) {
		write_sys_log("TR069: Md5 file not exists.");
		return FAIL;
	}

	memset(shellcmd, 0, sizeof(shellcmd));
	sprintf(shellcmd, "cd /tmp && md5sum -c %s > /dev/null 2>&1", buffer);
	if(system(shellcmd)) {
		write_sys_log("TR069: Md5 file checking fail.");
		return FAIL;
	}
	
	system("chmod +x /tmp/web_update");
	memset(shellcmd, 0, sizeof(shellcmd));

	char updateUbootFlag[NORMAL_FIELD_LEN];
	get_single_config_attr("TZ_ENABLE_UPDATE_UBOOT", updateUbootFlag);
	if (!strcmp(system_version_info.device, "INDUSTRY ROUTER")) {
		sprintf(shellcmd, "/tmp/web_update %s %d >/tmp/.web_update.log 2>&1", filePath, atoi(updateUbootFlag));
	} else {
		char *temp = strstr(filePath, "tar.lzma");
		if(temp == NULL){
			sprintf(shellcmd, "cd / && tar -xzf /tmp/%s", filePath);
		} else {
			sprintf(shellcmd, "cd / && tar -a -xvf /tmp/%s", filePath);
		} 
	}
	watchdog();

	if (system(shellcmd)) {
		write_sys_log("TR069: Updating fail.");
		return FAIL;
	}

	return ret;
}

void load_params(void)
{
    static int first_init = 1;
    if(first_init)
    {
        first_init = 0;
        memset(&all_http_params, 0, sizeof(all_http_params));
        memset(&system_version_info, 0, sizeof(system_version_info));
        memset(&wan_info, 0, sizeof(wan_info));
        // memset(ppp_users, 0, sizeof(ppp_users));

        //装载版本相关的信息
        util_read_version_info( VERSION_INFO_FILE,&system_version_info );
        //获取MAC地址
        load_mac_addr();
        //获取客户信息
        //load_customer_info();
        //获取模块相关的信息
        read_imei_info( SYSTEM_INFO_STATIC,&system_version_info );
    }
	
	read_wan_info();
	// read_ppp_users();

    // if( cmd_file_exist( RC_SYSTEM_CONFIG_FILE ) )
    // {
    //     if( util_server_read_wifi_info_ex( RC_SYSTEM_CONFIG_FILE,&system_version_info ) )
    //     {
    //         util_write_exception_log("sysmanagetool","fail to read customer config file!");
    //     }
    // }

    //read_reg_status();
    // cmd_netdev_flowstat(REAL_WAN_IF, rx_bytes_counter, tx_bytes_counter);
    cmd_netdev_flowstat_ex( REAL_WAN_IF,rx_bytes_counter,tx_bytes_counter,rx_packages_counter,tx_packages_counter);
    //读取LTE的状态
    read_lte_param( SYSTEM_INFO_DYNAMIC,&system_version_info );
}

int get_wan_parameter(enum ParameterName name, char **value)
{
	char *param = NULL;
	static char buffer[64];
	switch(name)
	{
		case X_CMCC_WAN_Enable:
			param = wan_info.Enable;
			break;

		case X_CMCC_WAN_AddressingType:
			param = wan_info.AddressingType;
			break;

		case X_CMCC_WAN_ExternalIPAddress:
			param = wan_info.ExternalIPAddress;
			break;

		case X_CMCC_WAN_SubnetMask:
			param = wan_info.SubnetMask;
			break;
			
		case X_CMCC_WAN_DefaultGateway:
			param = wan_info.DefaultGateway;
			break;

		case X_CMCC_WAN_DNSServers:
			param = wan_info.DNSServers;
			break;

		case X_CMCC_WAN_MACAddress:
			param = wan_info.MACAddress;
			break;
			
		case X_CMCC_WAN_ConnectionStatus:
			param = wan_info.ConnectionStatus;
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"Connected");
			param = buffer;
			break;
			
		case X_CMCC_WAN_ConnectionTrigger:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"AlwaysOn");
			param = buffer;
			break;
			
		case X_CMCC_WAN_ConnectionType:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"IP_Routed");
			param = buffer;
			break;
			
		case X_CMCC_WAN_DNSEnabled:
			param = wan_info.DNSEnabled;
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"true");
			param = buffer;
			break;
			
		case X_CMCC_WAN_DNSOverrideAllowed:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"true");
			param = buffer;
			break;
			
		case X_CMCC_WAN_LastConnectionError:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"ERROR_NONE");
			param = buffer;
			break;
			
		case X_CMCC_WAN_MACAddressOverride:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"false");
			param = buffer;
			break;
			
		case X_CMCC_WAN_MaxMTUSize:
			memset(buffer,0,sizeof(buffer));
			sprintf(buffer,"%d",wan_info.Mtu);
			param = buffer;
			break;
			
		case X_CMCC_WAN_Name:
			param = wan_info.Name;
			break;
			
		case X_CMCC_WAN_PossibleConnectionTypes:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"Unconfigured,IP_Routed");
			param = buffer;
			break;
			
		case X_CMCC_WAN_RSIPAvailable:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"off");
			param = buffer;
			break;
			
		case X_CMCC_WAN_RouteProtocolRx:
			memset(buffer,0,sizeof(buffer));
			strcpy(buffer,"off");
			param = buffer;
			break;
			
		case X_CMCC_WAN_Uptime:
			param = wan_info.Uptime;
			break;
			
		default:
			break;
	}
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	
    return FAULT_CODE_OK;
}

int get_ppp_index(const char *name)
{
	int index = parse_wan_index(name) - 2;
	if (index < 0 || index > 3) return 0;

	return index;
}

void get_device_memory(char *param) {
	char meminfo[128];
	char mem_total[128];
	char mem_free[128];

	//清空字符串
	memset( meminfo,0,sizeof( meminfo ) );
	memset( mem_total,0,sizeof( mem_total ) );
	memset( mem_free,0,sizeof( mem_free ) );
	//读取内存使用信息
	util_read_meminfo( mem_total,mem_free );

	snprintf(meminfo, sizeof( meminfo ), "%s,%s", mem_total, mem_free);
	strcpy(param, meminfo);
}

int get_ppp_parameter(enum ParameterName paramName,const char *name, char **value)
{
	//cwmp_log_debug(name);
	char *param = NULL;
	PPPUserInfo *user = &(ppp_users[get_ppp_index(name)]);
	switch(paramName)
	{
		case X_CMCC_PPP_Enable:
			param = user->Enable;
			break;

		case X_CMCC_PPP_Name:
			param = user->Name;
			break;

		case X_CMCC_PPP_Uptime:
			param = user->Uptime;
			break;

		case X_CMCC_PPP_LastConnectionError:
			break;

		case X_CMCC_PPP_Username:
			param = user->Username;
			break;

		case X_CMCC_PPP_Password:
			break;
			
		case X_CMCC_PPP_ExternalIPAddress:
			{
				param = user->ExternalIPAddress;
				struct in_addr address;
				if( !cmd_get_ip( REAL_WAN_IF, &address ) )
				{
					char tmp_buffer[64];
					STRCPY_S( tmp_buffer, inet_ntoa( address ) );
					STRCPY_S( param, tmp_buffer );
				} else {
					strcpy(param, "");
				}
			}
			break;

		case X_CMCC_PPP_DNSEnabled:
			break;

		case X_CMCC_PPP_DNSServers:
			break;

		case X_CMCC_PPP_MACAddress:
			break;

		case X_CMCC_PPP_TunnelDial:
			break;
			
		case X_CMCC_PPP_LNS:
			break;
			
		case X_CMCC_PPP_LnsIp:
			break;
			
		case X_CMCC_PPP_LnsName:
			break;
			
		case X_CMCC_PPP_LnsPassword:
			break;
			
		case X_CMCC_PPP_L2TPMode:
			break;
			
		case X_CMCC_PPP_UpstreamTotalByte:
			break;
			
		case X_CMCC_PPP_DownstreamTotalByte:
			break;
			
		default:
			break;
	}
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		*value = param;
	}
	
    return FAULT_CODE_OK;
}

int set_ppp_parameter(enum ParameterName paramName, const char *name, const char *value)
{
	PPPUserInfo *user = &(ppp_users[get_ppp_index(name)]);
	switch(paramName)
	{
		case X_CMCC_PPP_Enable:
			strcpy(user->Enable, value);
			break;

		case X_CMCC_PPP_Name:
			strcpy(user->Name, value);
			
		default:
			break;
	}
	
    return FAULT_CODE_OK;
}
extern int is_anhui_version();

int get_sys_parameter(enum SystemParameterName name, char **value)
{
	static char param[256];
	memset(param, 0, sizeof(param));
	
	char buffer[256];
	switch (name)
	{
		case X_SYS_Manufacturer:
			strcpy(buffer, system_version_info.device);
			if (is_anhui_version()) {
				if (strncmp(buffer, "FL", 2) == 0) {
					strcpy(param, "HD");
				} else {
					strcpy(param, "TZ");
				}
			} else {
				strcpy(param, "Shenzhen Tozed Technologies Co., Ltd");
			}
			break;

		case X_SYS_ManufacturerOUI:
        {
            MacAddr mac;
            if (!cmd_netdev_get_mac_addr("br-lan", &mac))
            {
                sprintf(param, "%02X%02X%02X", mac.value[0], mac.value[1], mac.value[2]);
            }
            else
            {
                strcpy(param, "D8D866");
            }
            break;
        }
			
		case X_SYS_ProductClass:
            get_single_config_attr("TZ_DEVICE_TYPE", param);
            if(strlen(param) == 0)
                strcpy(param, system_version_info.device);
			break;
			
		case X_SYS_SerialNumber:
            /*strcpy(param, "TZ0406");
            strcpy(buffer, system_version_info.imei );
            util_strip_traling_spaces(buffer);
            int len = strlen(buffer);
            if(len <= 6)
                strcat(param, buffer);
            else
                strcat(param, buffer + len - 6);*/
			strcpy(param, system_version_info.imei );
			break;

		case X_SYS_SpecVersion:
			strcpy(param, "1.0");
			break;
			
		case X_SYS_HardwareVersion:
            get_single_config_attr("TZ_HARDWARE_VERSION", param);
            if(strlen(param) == 0)
                strcpy(param, system_version_info.hwversion);
			break;
			
		case X_SYS_SoftwareVersion:
            get_single_config_attr("TZ_DISPLAYED_VERSION", param);
            if(strlen(param) == 0)
                strcpy(param, system_version_info.version);
			break;
			
		case X_SYS_ProvisioningCode:
			break;

		case X_SYS_UpTime:
			{
				char *p;
				memset(param, 0, sizeof(param));
				cmd_cat("/proc/uptime", param, sizeof(param));

				p = param;
				while (*p != '\0' && *p != '.') {
					p++;
				}
				*p = '\0';
			}
			break;

		case X_SYS_IMEI:
			strcpy(param, system_version_info.imei);
			break;

		case X_SYS_IMSI:
			strcpy(param, system_version_info.imsi);
			break;

		case X_SYS_ICCID:
			strcpy(param, system_version_info.iccid);
			break;

		case X_SYS_Memory:
			get_device_memory(param);
			break;
			
		case X_SYS_ACS_URL:
			get_single_config_attr("TZ_TR069_URL", param);
			break;
			
		case X_SYS_ACS_Username:
			get_single_config_attr("TZ_TR069_USERNAME", param);
			break;
			
		case X_SYS_ACS_Password:
			get_single_config_attr("TZ_TR069_PASSWD", param);
			break;
			
		case X_SYS_URL:
			break;
			
		case X_SYS_Username:
			get_single_config_attr("TZ_TR069_LINK_USERNAME", param);
			break;
			
		case X_SYS_Password:
			get_single_config_attr("TZ_TR069_LINK_PASSWD", param);
			break;

		default:
			break;
	}
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		replace_special_char(param);
		*value = param;
	}
		
	return FAULT_CODE_OK;

}


int get_parameter(enum ParameterName name, char **value)
{
	switch(name)
	{
		case X_CMCC_WANDeviceNumberOfEntries:
			if (is_anhui_version()) {
				get_single_config_attr("TZ_SPECIAL_DEVICE", param);
				if (strncmp(param, "P11+FourPorts", 13) == 0) {
					strcpy(param, "5");
				}
				else {
					strcpy(param, "2");
				}
			} else {
				strcpy(param, "1");
			}
			break;

		case TRF_WANDeviceNumberOfEntries:
			strcpy(param, "1");
			break;

		case TRF_LANDeviceNumberOfEntries:
			get_single_config_attr("TZ_SPECIAL_DEVICE", param);
			if (strncmp(param, "P11+FourPorts", 13) == 0)
			{
				strcpy(param, "4");
			}
			else
			{
				strcpy(param, "1");
			}
			break;
        case WANIPConnectionNumberOfEntries:
            if(0 != strcmp(wan_info.AddressingType,"PPPoE")){
                strcpy(param, "1");
            }
            else{
                strcpy(param, "0");
            }
            break;

        case DeviceSummary:
            strcpy(param, "1");
            break;
			
		case X_CMCC_NetRegStatus:
			sprintf(param, "%d", reg_status);
			break;

		case X_CMCC_GprsRegStatus:
			sprintf(param, "%d", greg_status);
			break;

		case X_CMCC_EpsRegStatus:
			sprintf(param, "%d", ereg_status);
			break;

		case X_CMCC_CurrentNetwork:
			strcpy(param, system_version_info.PLMN);
			replace_special_char(param);
			break;
			
		case X_CMCC_CurrentNetmode:
			strcpy(param, system_version_info.AcT);
			break;
			
		case X_CMCC_NetworkPriority:
			strcpy(param, system_version_info.TZ_LTE_MODULE_PRIORITY);
			break;

		case X_CMCC_SingalLevel:
			strcpy(param, system_version_info.RSSI);
			break;
			
		case X_CMCC_Txpower:
			strcpy(param, system_version_info.TZTXPOWER);
			break;
			
		case X_CMCC_FrequencyPoint:
			strcpy(param, system_version_info.TZEARFCN);
			break;
			
		case X_CMCC_Band:
			strcpy(param, system_version_info.TZBAND);
			break;
			
		case X_CMCC_BandWidth:
			strcpy(param, system_version_info.TZBANDWIDTH);
			break;
			
		case X_CMCC_EnodeBId:
			{
				int enodeBId = TRatoi(system_version_info.TZGLBCELLID);
				sprintf(param, "%d", enodeBId >> 8);
			}
			break;
			
		case X_CMCC_LAC:
			strcpy(param, system_version_info.LAC);
			break;
			
		case X_CMCC_CellId:
			{
				int cellid = TRatoi(system_version_info.TZGLBCELLID);
                sprintf(param, "%07d-%03d", cellid >> 8, cellid & 0xFF);
			}
			break;
			
		case X_CMCC_GlobeCellId:
			strcpy(param, system_version_info.TZGLBCELLID);
			break;
			
		case X_CMCC_PhysicsCellId:
			strcpy(param, system_version_info.TZPHYCELLID);
			break;
			
		case X_CMCC_ICCID:
			strcpy(param, system_version_info.iccid);
			break;
			
		case X_CMCC_APN:
			get_single_config_attr("TZ_CONFIG_APN_NAME", param);
			break;
			
		case X_CMCC_IsFrequencyLock:
			strcpy(param, "1");
			break;
			
		case X_CMCC_FrequencyLocking:
			// TODO:
			strcpy(param, system_version_info.band_pref);
			break;
			
		case X_CMCC_FrequencyLockInfo:
			// TODO:
			strcpy(param, system_version_info.band_pref);
			break;
			
		case X_CMCC_IsCellLock:
			{
				int lockCellId = TRatoi(system_version_info.TZ_LOCK_CELL_ID);
				strcpy(param, (lockCellId >> 16) == 0 ? "0" : "1");
			}
			break;
			
		case X_CMCC_CellLockType:
			strcpy(param, "1");
			break;
			
		case X_CMCC_CellLocking:
			{
				int lockCellIdCount = TRatoi(system_version_info.TZ_LOCK_CELL_ID_COUNT);
				sprintf(param, "4G,%d;3G,%d;2G,%d", (lockCellIdCount >> 16), (lockCellIdCount >> 8) & 0xFF, lockCellIdCount & 0xFF);
			}
			break;
			
		case X_CMCC_CellLockInfo:
			cmd_cat(ALL_REGISTERED_CELL_ID_INFO, param, sizeof(param) - 1);
			parse_cell_info(param);
			break;
			
		case X_CMCC_LockPin:
			get_single_config_attr("TZ_LOCK_CARD", param);
			break;
			
		case X_CMCC_LockPinType:
			get_single_config_attr("TZ_LOCK_CARD_TYPE", param);
			break;
			
		case X_CMCC_FirstPin:
			get_single_config_attr("TZ_INITIAL_PIN_CODE", param);
			break;
			
		case X_CMCC_FixedPin:
			get_single_config_attr("TZ_MODIFIED_PIN_CODE", param);
			//cwmp_log_debug("TZ_MODIFIED_PIN_CODE: %s, len: %d", param, strlen(param));
			break;
			
		case X_CMCC_EncryptCard:
			get_single_config_attr("TZ_SUPPORT_SIM_ENCRYPT", param);
			if (atoi(param) == 0) {
				strcpy(param, STR_FALSE);
			}
			else {
				strcpy(param, STR_TRUE);
			}
			break;
			
		case X_CMCC_EncryptCardKey:
			strcpy(param, N_A);
			break;

		case X_CMCC_PLMN:
			strcpy(param, system_version_info.PLMN);
			break;

		case X_CMCC_SignalQuality:
			{
				char tozedinds[64];
				cmd_cat("/proc/tozed/ind", tozedinds, sizeof(tozedinds));
				sscanf(tozedinds, "%*s%*s%*s%s%*s", param);
			}
			break;
			
		case X_CMCC_RSRP:
			strcpy(param, system_version_info.TZRSRP);
			break;
			
		case X_CMCC_RSRQ:
			strcpy(param, system_version_info.TZRSRQ);
			break;

		case X_CMCC_RSSI:
            strcpy(param, system_version_info.RSSI);
			break;
			
		case X_CMCC_SINR:
			strcpy(param, system_version_info.TZSINR);
			break;
			
		case X_CMCC_UpstreamTotalByte:
			strcpy(param, tx_bytes_counter);
			break;
			
		case X_CMCC_DownstreamTotalByte:
			strcpy(param, rx_bytes_counter);
			break;
        case X_CMCC_UpstreamPackages:
            strcpy(param, tx_packages_counter);
            break;
            
        case X_CMCC_DownstreamPackges:
            strcpy(param, rx_packages_counter);
            break;
			
		case X_CMCC_StartTime:
			{
				char uptime_string[128];
				char* args[18];
				int args_count;

				//清空字符串
				memset( uptime_string,0,sizeof( uptime_string ) );
				//读取当前运行的时间
				cmd_cat( "/proc/uptime",uptime_string,sizeof( uptime_string ) );
				//对获取的字段进行分割
				args_count=util_get_all_args( uptime_string,' ','\t',args,18 );
				if( args_count >= 1 )
				{
					strcpy(param, args[0]);
					time_t now = (time_t)(time(NULL) - TRatoi(param));
					strcpy(param, parse_time(&now));
				}
			}
			break;
			
		case X_CMCC_LoadAverage:
			{
				char loadavg_string[64];
				char* args[18];
				int args_count;
				char loadavg[64];

				/*
				[root@localhost ~]# cat /proc/loadavg
				4.61 4.36 4.15 9/84 5662

				每个值的含义为：
				参数 解释
				lavg_1 (4.61) 1-分钟平均负载
				lavg_5 (4.36) 5-分钟平均负载
				lavg_15(4.15) 15-分钟平均负载
				*/
				//清空字符串
				memset( loadavg_string,0,sizeof( loadavg_string ) );
				//读取当前运行的时间
				cmd_cat( "/proc/loadavg",loadavg_string,sizeof( loadavg_string ) );
				//对获取的字段进行分割
				args_count=util_get_all_args( loadavg_string,' ','\t',args,18 );
				if( args_count >= 3 )
				{
					snprintf(loadavg, sizeof( loadavg ), "%s,%s,%s", args[0], args[1], args[2]);
					strcpy(param, loadavg);
				}
			}
			break;
			
		case X_CMCC_DeviceMemory:
			get_device_memory(param);
			break;
			
		case X_CMCC_MTU:
			get_single_config_attr("TZ_MTU_USB0", param);
			break;
			
		case X_CMCC_IMEI:
			strcpy(param, system_version_info.imei);
			break;

		case X_CMCC_IMSI:
			strcpy(param, system_version_info.imsi);
			break;
			
		case X_CMCC_ConfigVersion:
			strcpy(param, system_version_info.TZ_CONFIG_FILE_VERSION);
			break;
			
		case X_CMCC_ModuleVersion:
			strcpy(param, system_version_info.module_version);
			break;
			
		case X_CMCC_ModuleType:
			strcpy(param, system_version_info.module_type);
			break;

		default:
			break;
	}
	if (strlen(param) == 0) {
		*value = NULL;
	} else {
		replace_special_char(param);
		*value = param;
	}
	
    return FAULT_CODE_OK;
}


static int connection_request = 0;

int get_connection_request(void) {
	return connection_request;
}

void set_connection_request_true(void) {
	connection_request = 1;
}

void set_connection_request_false(void) {
	connection_request = 0;
}

int is_anhui_version() {
	return 0;
}

//*********************LOCK BAND************************//
#define BAND_LENGTH 128
#define UPDATE_MODULE_FLAG_FILE "/tmp/.update_module_flag"
#define SEPARATOR "$"
#define FIELD_SEPARATOR "@"

int execute_cmd_with_no_return(char *shellcmd) {
    char cmd[NORMAL_FIELD_LEN];
    sprintf(cmd, "%s%s", shellcmd, " >/dev/null 2>/tmp/error.log");

    return system(cmd);
}

void send_at(char *at, char *response, int size) {

    memset(response, 0, size);
    if (at == NULL) return;

    if( !strlen( at ) || cmd_file_exist(UPDATE_MODULE_FLAG_FILE))
    {
        return;
    }
    
    char shellcmd[256];
    cmd_echo(at, "/tmp/tr069_at_send");
    
    system( "rm -f /tmp/tr069_at_recv" );
    system( "killall sendat" );
    sprintf(shellcmd, "sendat -d%s -f/tmp/tr069_at_send -o/tmp/tr069_at_recv", util_get_at_port());
    execute_cmd_with_no_return(shellcmd);

    int count = 60;
    while(count > 0) {
        if (cmd_file_exist("/tmp/tr069_at_recv")) {
            cmd_cat("/tmp/tr069_at_recv", response, size);
            break;
        } else {
            sleep(1);
        }
        count--;
    }

    //replace_special_char(response);
}

static const char hex_map[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
int convertBitsToHex(char *hex, char *bits, int len)
{
    int i = 0;
    int dec = 0;
    int inc = 0;

    for(i = 0; i < len; i++)
    {
        if(bits[i] != '1' && bits[i] != '0')
            return -1;
        dec += ((int)bits[i] - (int)'0') * (0x01 << (3 - i % 4));
        if((i + 1) % 4 == 0)
        {
            hex[i / 4] = hex_map[dec];
            dec = 0;
        }
        else if(i == len - 1)
        {
            inc = 1;
            hex[i / 4] = hex_map[dec];
        }
    }
    i--;
    hex[i / 4 + 1] = '\0';

    return (i / 4 + inc);
}

int setSupportedBands(char *value)
{
    char *at;
    char *p, *bit, *band;
    int ret, offset, len;
    char displayedBand[NORMAL_FIELD_LEN];
    char at_cmd[NORMAL_FIELD_LEN], response[BIG_MAX_FIELD_LEN], allBands[BIG_MAX_FIELD_LEN], allBands_bit[BAND_LENGTH];
    char bands_inhex[BAND_LENGTH / 4 + 2], bands_inhex_set[BAND_LENGTH / 4 + 2];
     
    if(value == NULL)
        return -1;

    len = strlen(value);
    len = len > BAND_LENGTH ? BAND_LENGTH : len;
    ret = convertBitsToHex(bands_inhex_set, value, len);
    if(ret < 0)
        return -1;

    get_single_config_attr("TZ_DISPLAYED_BAND_PREF", displayedBand);
    if ( util_all_char_be_zero_or_zero_length( displayedBand ) ) {
        at = util_get_at_to_query_all_supported_band();
        send_at(at, response, sizeof(response));
        
        ret = util_parse_all_supported_band(response, allBands);
    } else {
        ret = util_get_all_supported_band_names(displayedBand, allBands);
    }

    if (ret != 0) {
        return -1;
    }

    len = strlen(allBands);
    memset(allBands_bit, '0', sizeof(allBands_bit));
    p = allBands;
    while(strsep(&p, SEPARATOR));
    for(p = allBands; p < allBands + len;)
    {
        bit = band = p;
        for(p += strlen(p); p < (allBands +len) && !*p; p++);
        bit = strsep(&band, FIELD_SEPARATOR);

        offset = atoi(bit);
        if (offset >= 0 && offset < BAND_LENGTH) 
            allBands_bit[BAND_LENGTH - offset - 1] = '1';
    }
    memset(bands_inhex, '0', sizeof(bands_inhex));

    convertBitsToHex(bands_inhex, allBands_bit, sizeof(allBands_bit));

    memset(response, 0, sizeof(response));
    ret = util_get_at_to_lock_specified_band(bands_inhex,
        bands_inhex_set, at_cmd);
    if (ret != 0) {
        return -1;
    }
    send_at(at_cmd, response, sizeof(response));

    set_single_config_attr("TZ_BAND_PREF", bands_inhex_set);
    system("cfg -c > /tmp/cfg");

    return 0;
}

char supportBands_bit[BAND_LENGTH];
char *getSupportedBands()
{
    char *at, *p, *band, *bit, *bands_inbits = supportBands_bit;
    int ret, offset;
    char response[BIG_MAX_FIELD_LEN];
    char supportBands[BIG_MAX_FIELD_LEN];
    
    at = util_get_at_to_query_current_supported_band();
    send_at(at, response, sizeof(response));

    ret = util_parse_current_supported_band(response, supportBands);
    if (ret != 0) {
        strcpy(bands_inbits, "N/A");
        return bands_inbits;
    }

    int len = strlen(supportBands);
    memset(supportBands_bit, '0', sizeof(supportBands_bit));
    p = supportBands;
    while(strsep(&p, SEPARATOR));
    for(p = supportBands; p < supportBands + len;)
    {
        bit = band = p;
        for(p += strlen(p); p < (supportBands +len) && !*p; p++);
        bit = strsep(&band, FIELD_SEPARATOR);

        offset = atoi(bit);
        if (offset >= 0 && offset < BAND_LENGTH) 
            supportBands_bit[BAND_LENGTH - offset - 1] = '1';
    }

    return bands_inbits;
}

//*************************Hosts****************************//

char *get_value_by_end(char *data, char *value, char *endString){
    char *pIndex = NULL;
    char *pTail = NULL;
    int i = 0;

    // 初始化为空字符串
    value[0] = '\0';
    if (data == NULL) {
        return NULL;
    }
    pIndex = data;
    while (*pIndex == ' ') {
        pIndex++;
        i++;
        if(i > 19){
            return pIndex;
        }
            
    }

    pTail = strstr(pIndex, endString);
    if (pTail == NULL) {

        return pIndex;
    }
    
    memcpy(value, pIndex, pTail - pIndex);
    value[pTail - pIndex] = '\0';

    return pTail;
}

int execute_cmd_with_length(char *shellcmd, char *out, int size, int cmdLength) {
    char cmd[cmdLength];
    sprintf(cmd, "%s%s", shellcmd, " 2>/tmp/error.log");

    return read_memory(cmd, out, size);
}

int execute_cmd(char *shellcmd, char *out, int size) {
    return execute_cmd_with_length(shellcmd, out, size, NORMAL_FIELD_LEN);
}

int find_client_list_wlan(ARPTable *client_list) {
    int i;
    char buffer[1024 * 16], shellcmd[1024 * 2];
    char tmp_values[1024 * 2];
    char *pHead = NULL, *pTail = NULL;
    
    // 2014-10-11 liyao 先从函数获取，如果获取失败，再按照以前方式获取
    if (!util_get_arp_table_info(client_list, "br-lan")) {
        return client_list->count;
    } else {
        sprintf(shellcmd, "arp -i br-lan | grep -v incomplete");
    
        if (execute_cmd(shellcmd, buffer, sizeof(buffer)) != SUCCESS) {
            return -1;
        }
    
        pHead = strstr(buffer, "(");
        if (pHead == NULL) {
            return -1;
        }
        char *pnextstart;
        pHead = buffer;
        pnextstart = NULL;
        i = 0;
        do{
            pTail = strstr(pHead, "\n");  //一定是"\n"为结束标识
            if(pTail != NULL) {
                memset(tmp_values, 0, sizeof(tmp_values));
                memcpy(tmp_values, pHead, pTail-pHead + 1);
                //sscanf(tmp_values, "%s%s%s%s", values[0][i], values[1][i], values[2][i], values[3][i]);
                pnextstart =strstr(pHead, "("); 
                pnextstart = get_value_by_end(pnextstart + 1, client_list->table[i].ip, ")");
                pnextstart = strstr(pnextstart, "at ");
                pnextstart += strlen("at ");
                pnextstart = get_value_by_end(pnextstart, client_list->table[i].mac, " ");
            
                pHead = pTail + 1;     
                i++;                       
                if(i >= 40)
                    break;
            }
                        
        }while(pTail != NULL);
        client_list->count = i;
        return i;
    }
}



