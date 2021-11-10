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
    BAD_HASH              = 25,

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
    ERR_RESIZE = 71,
    ERR_MEM = 44,
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


       int   StackCtor             (      Stack *); 
       void  StackDtor             (      Stack *);
       int   StackPush             (      Stack *, elem_t);
       int   StackPop              (      Stack *, elem_t *);
       void  StackCheck            (      Stack *);
       int   StackOk               (      Stack *);
       void  StackDump             (const Stack *, const char *, const int);
static int   StackResize           (      Stack *, int);

static void  PourPoison            (      elem_t *, int, int);
static int   HashRegionMemory      (const void *, size_t);
static hash_t HashVar              (const void *, size_t, int);
       int   CheckHash             (const Stack *);
       void  CalcHashes            (      Stack *);


const int                 LITTLE_N_ELEMENT   = 5;
const hash_t              HASH_COEF          = 1;
const int                 INITIAL_CAPACITY   = 16;
const int                 SPACE_FACTOR       = 2;
const int                 REDUCTION_FACTOR   = 4;
const int                 SIZE_CANARY        = sizeof (unsigned long long);            // canary for Stack.data
const int                 MAGIC_NUM          = 15;
const int                 POISON             = -666;
const unsigned long long  VALUE_DATA_CANARY  = 0xFEE1FE11;
const unsigned long long  VALUE_STACK_CANARY = 0xC0FFEE;


extern FILE *LOG_FILE;

#define NAME_LOG_FILE "log_file.txt"



#define $kuku printf ("KUKU from %s (line: %d, function:  %s)\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

#define STACK_DUMP(wtf) StackDump((wtf), __PRETTY_FUNCTION__, __LINE__);

#define STACK_CTOR(stack_ptr)                                                         \
{                                                                                     \
    if (StackCtor (stack_ptr) == ERR_ALLOCATION_MEMORY)                               \
    {                                                                                 \
        fprintf (LOG_FILE, "\nError allocation memory in function \"StackCtor\"\n");  \
                                                                                      \
        CLOSE_LOG_FILE ();                                                            \
        return ERR_MEM;                                                               \
    }                                                                                 \
}

#define STACK_PUSH(stack_ptr, value)                                                  \
{                                                                                     \
    if (StackPush (stack_ptr, value) == ERR_ALLOCATION_MEMORY)                        \
    {                                                                                 \
        fprintf (LOG_FILE, "\nError allocation memory in function \"StackPush\"\n");  \
                                                                                      \
        CLOSE_LOG_FILE ();                                                            \
        return ERR_MEM;                                                               \
    }                                                                                 \
}

#define STACK_POP(stack_ptr, ptr_place)                                             \
{                                                                                   \
    if (StackPop (stack_ptr, ptr_place) == EMP_STACK)                               \
    {                                                                               \
        fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); \
        fprintf (LOG_FILE, "ERROR in line %d.\n", __LINE__);                        \
        fprintf (LOG_FILE, "Empty stack!\n");                                       \
        fprintf (LOG_FILE, "You can't take anythink from STACK.\n");                \
        fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); \
                                                                                    \
        CLOSE_LOG_FILE();                                                           \
        return EMP_STACK;                                                           \
    }                                                                               \
}

#define STACK_DTOR(stack_ptr)   \
{                               \
    StackDtor (stack_ptr);      \
}

