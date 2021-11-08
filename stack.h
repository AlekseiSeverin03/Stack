#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file_operations.h"


enum OUTPUT_FUNCTION
{
	NOT_OKEY              = 20,
	NULL_STACK_PTR        = 21,
	NULL_DATA_PTR         = 22,
	DATA_PTR_IS_ERR_PTR   = 23,
	WRONG_HASH            = 24,

	OKEY                  = 0,
	EMP_STACK             = 1,
	ERR_ALLOCATION_MEMORY = 2,

	
};

enum ERR_VALUE
{
	ERR_PTR = 15,
};


enum OUTPUT_fMAIN
{
	OK = 0,
	ERR_RESIZE = 21,
};


typedef  double    elem_t;                 // determining the type of array elements
#define  ELEM_FMT  "%lg"

typedef int    hash_t;

typedef struct Stack
{
	int canary1;

	hash_t hash_stack;
	hash_t hash_data;

	int capacity;
	int size;
	elem_t *data;

	int canary2;
}                       Stack;


       void  StackCtor             (      Stack *); 
       void  StackDtor             (      Stack *);
       int   StackPush             (      Stack *, elem_t);
       int   StackPop              (      Stack *, elem_t *);
       int   StackOk               (const Stack *);
       void  StackDump             (const Stack *, const char *, const int, FILE *);
static int   StackResize           (      Stack *, int);
static void  PutCanaryInEndMemory  (      elem_t *, int);
static void  PourPoison            (      elem_t *, int, int);
static int   HashRegionMemory      (const void *, size_t);
static int   HashVar               (const void *, size_t, int);
       void  CheckHash             (      Stack *, FILE *);
       void  CalcHashes            (      Stack *);



const hash_t HASH_COEF = 1;
const int INITIAL_CAPACITY = 16;
const int SPACE_FACTOR = 2;
const int SIZE_CANARY  = sizeof (unsigned long long);            // canary for Stack.data
const int MAGIC_NUM    = 15;
const int POISON       = -666;
const unsigned long long VALUE_DATA_CANARY = 0xFEE1FE11;
const unsigned long long VALUE_STACK_CANARY = 0xC0FFEE;

#define NAME_LOG_FILE "log_file.txt"



#define $kuku printf ("KUKU from %s (%d) %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define STACK_DUMP(wtf, stream) StackDump((wtf), __PRETTY_FUNCTION__, __LINE__, stream);

#define STACK_CHECK(stack_ptr, STREAM_LOG_FILE)                                                                        \
{                                       	                                                                           \
	int code_error = 0;                     	                                                                       \
	int flag = 0;                                                                                                      \
		            	                                                                                               \
	if ((code_error = StackOk (stack_ptr)) == 0) {                                                                     \
        	                                                                                                           \
		;                                                                                                              \
            	                                                                                                       \
	} else {           	                                                                                               \
		            	                                                                                               \
		if (code_error == NULL_STACK_PTR) {                           		                                           \  
		                                                                                                               \
			fprintf (STREAM_LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     \
			fprintf (STREAM_LOG_FILE, "stack_ptr = NULL (in funtion %s, line %d)\n",  __PRETTY_FUNCTION__, __LINE__);  \
			fprintf (STREAM_LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     \
		                                                                                                               \
			flag += 1;                                                                                                 \
	                                                                                                                   \
		}             	                                                                                               \
		            	                                                                                               \
		if (code_error == NULL_DATA_PTR) {                                                                             \  
		                                                                                                               \
			fprintf (STREAM_LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     \
			fprintf (STREAM_LOG_FILE, "data_ptr = NULL (in funtion %s, line %d)\n",  __PRETTY_FUNCTION__, __LINE__);   \
			fprintf (STREAM_LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     \
                                                                                                                       \
			flag += 1;                                                                                                 \
	                                                                                                                   \
		}  			   	                                                                                               \
		            	                                                                                               \
		if (code_error == DATA_PTR_IS_ERR_PTR) {               		                                                   \
 		        	                                                                                                   \
			fprintf (STREAM_LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     \
			fprintf (STREAM_LOG_FILE, "data_ptr = ERR_PTR (in funtion %s, line %d)\n", __PRETTY_FUNCTION__, __LINE__); \
			fprintf (STREAM_LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     \
            	                                                                                                       \
			flag += 1;                                                                                                 \
                	                                                                                                   \
		} 		                                                                                                       \
		            	                                                                                               \
		fprintf (STREAM_LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     	   \
		fprintf (STREAM_LOG_FILE, "ERROR in stack\n");                                                                 \
		fprintf (STREAM_LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");         \
		STACK_DUMP(stack_ptr, STREAM_LOG_FILE);                                                                        \
                                                                                                                       \
		flag += 1;                                                                                            		   \
	}                                                                                                                  \
		            	                                                                                               \
	if (flag > 0)                                                                                                      \
	{                                                                                                                  \
		CLOSE_LOG_FILE(STREAM_LOG_FILE);                                                                               \
		exit (1);                                          															   \
	}                                   																			   \
}

#define STACK_CTOR(stack_ptr)      \
{                                  \
	StackCtor (stack_ptr);         \
}

#define STACK_PUSH(stack_ptr, value, stream)  \
{                                             \
	STACK_CHECK(stack_ptr, stream);           \
	CheckHash (stack_ptr, stream);            \
 											  \
	StackPush (stack_ptr, value);             \
}

#define STACK_POP(stack_ptr, ptr_place, stream)                                     \
{                                                                                   \
	STACK_CHECK(stack_ptr, stream);                     						    \
	CheckHash (stack_ptr, stream);        											\
																					\
	if (StackPop (stack_ptr, ptr_place) != OKEY)                                    \
	{                                                                               \
		fprintf (stream, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");   \
		fprintf (stream, "ERROR in line %d.\n", __LINE__);                          \
		fprintf (stream, "Empty stack!\n");                           			    \
		fprintf (stream, "You can't take anythink from STACK.\n");     				\
		fprintf (stream, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");   \
																					\
		CLOSE_LOG_FILE(stream);														\
		return EMP_STACK;                                                           \
	}                                                                               \
}

#define STACK_DTOR(stack_ptr, stream)   \
{   									\
	STACK_CHECK(stack_ptr, stream); 	\
	CheckHash (stack_ptr, stream);  	\
										\
	StackDtor (stack_ptr);          	\
}
