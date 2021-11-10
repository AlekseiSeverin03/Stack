#include "stack.h"

void StackCheck (Stack *stack_ptr)                                                                        
{                                      	                                                                           
	int code_error = 0;                     	                                                                       
	int flag = 0;                                                                                                      
		            	                                                                                               
	if ((code_error = StackOk (stack_ptr)) == 0) {                                                                     
        	                                                                                                           
		;                                                                                                              
            	                                                                                                       
	} else {           	                                                                                               
		            	                                                                                               
		if (code_error == NULL_STACK_PTR) {                           		                                             
		                                                                                                               
			fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     
			fprintf (LOG_FILE, "stack_ptr = NULL\n");  
			fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     
		                                                                                                               
			flag += 1;                                                                                                 
	                                                                                                                  
		}             	                                                                                              
		            	                                                                                               
		if (code_error == NULL_DATA_PTR) {                                                                               
		                                                                                                               
			fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     
			fprintf (LOG_FILE, "data_ptr = NULL\n");   
			fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     
                                                                                                                       
			flag += 1;                                                                                                 
	                                                                                                                   
		}  			   	                                                                                              
		            	                                                                                               
		if (code_error == DATA_PTR_IS_ERR_PTR) {               		                                                   
 		        	                                                                                                   
			fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     
			fprintf (LOG_FILE, "data_ptr = ERR_PTR\n"); 
			fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     
            	                                                                                                       
			flag += 1;                                                                                                 
                	                                                                                                   
		}

		if (code_error == BAD_HASH) {

			fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     
			fprintf (LOG_FILE, "stack has BAD_HASH\n"); 
			fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");     
            	                                                                                                       
			flag += 1;                                                                                                 
                	                                                                                                   
		}
		                                                                                                       
		            	                                                                                               
		fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");     	   
		fprintf (LOG_FILE, "ERROR in stack\n");                                                                 
		fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");         
		STACK_DUMP(stack_ptr);                                                                        
                                                                                                                       
		flag += 1;                                                                                            		   
	}                                                                                                                  
		            	                                                                                              
	if (flag > 0)                                                                                                     
	{                                                                                                                  
		CLOSE_LOG_FILE();                                                                               
		exit (1);                                          															   
	}                                   																			   
}

//-------------------------------------------------------------------------------------------------------------------------
//! Calc Hashes
//!
//! @param  [in]  stack_ptr  pointer on struct Stack
//!
//! @return void
//-------------------------------------------------------------------------------------------------------------------------

void CalcHashes (Stack *stack_ptr)
{
	assert (stack_ptr != NULL);

	stack_ptr->hash_stack = 0;
	stack_ptr->hash_data  = 0;

	stack_ptr->hash_stack = HashRegionMemory (stack_ptr, sizeof (Stack));	
	stack_ptr->hash_data  = HashRegionMemory (stack_ptr->data, stack_ptr->capacity);
}

//-------------------------------------------------------------------------------------------------------------------------
//! Checking hashes in struck Stack
//!
//! @param  [in]  stack_ptr  pointer on struck Stack
//! @param  [in]  stream     stream in which output errors and warnings
//!
//! @return void
//!
//! @note this function can complete main program if hash will be bad
//-------------------------------------------------------------------------------------------------------------------------

int CheckHash (Stack *stack_ptr)
{
	assert (stack_ptr != NULL);

	char flag = 0;
	hash_t tmp_hash_data = stack_ptr->hash_data;
	hash_t tmp_hash_stack = stack_ptr->hash_stack;
	stack_ptr->hash_data = 0;
	stack_ptr->hash_stack = 0;

	if (tmp_hash_stack != HashRegionMemory (stack_ptr, sizeof (Stack)))
	{
		flag = 1;
	}

	stack_ptr->hash_data = tmp_hash_data;
	stack_ptr->hash_stack = tmp_hash_stack;

	if (stack_ptr->hash_data != HashRegionMemory (stack_ptr->data, stack_ptr->capacity))
	{
		flag = 2;
	}

	if (flag != 0)
	{
		fprintf (LOG_FILE, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"); 
		fprintf (LOG_FILE, "WRONG_HASH (in funtion %s, line %d)\n", __PRETTY_FUNCTION__, __LINE__);    
		fprintf (LOG_FILE, "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

		return BAD_HASH; 
	}

	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Hashing the region of memory
//!
//! @param  [in]  ptr_reg_mem  pointer on the region of memory
//! @param  [in]  size_mem     size hasing memory (in bytes)
//!
//! @return value hash
//-------------------------------------------------------------------------------------------------------------------------

static hash_t HashRegionMemory (const void *ptr_reg_mem, size_t size_mem)
{
	hash_t hash = 0, hash_coef = HASH_COEF;

	for (int i = 0; i < size_mem; i++)
	{
		hash += (*(hash_t *)((char *) ptr_reg_mem + i) + hash_coef) * hash_coef;
		hash_coef = hash;
	}

	return hash;
}

static hash_t HashVar (const void *ptr_value, size_t nbyte, int *hash_coef)       // This function doesn't used
{
	hash_t hash = 0;
	
	for (int i = 0; i < nbyte; i++)
	{
		hash += *(hash_t *)((char *) ptr_value + i) * (*hash_coef);
		*hash_coef = hash;
	}

	return hash;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Check stack for errors
//!
//! @param  [in]  stack_ptr  pointer on struck Stack
//!
//! @return code error:
//!          _0_ if there are no errors;
//!			_not 0_ if there are errors.
//-------------------------------------------------------------------------------------------------------------------------

int StackOk (Stack *stack_ptr)
{
	if (stack_ptr == NULL) {

		return NULL_STACK_PTR;

	} else if (stack_ptr->data == NULL) {

		return NULL_DATA_PTR;

	} else if (stack_ptr->data == (elem_t *) ERR_PTR) {

		return DATA_PTR_IS_ERR_PTR;

	} else if ((stack_ptr->size < 0) &&
        	   (stack_ptr->capacity < INITIAL_CAPACITY) &&
        	   (stack_ptr->canary1 != VALUE_STACK_CANARY) && 
			   (stack_ptr->canary2 != VALUE_STACK_CANARY) &&
        	   (*(unsigned long long *)((char *)stack_ptr->data - SIZE_CANARY) != VALUE_DATA_CANARY) &&
        	   (*(unsigned long long *)((char *)stack_ptr->data - SIZE_CANARY) != VALUE_DATA_CANARY)) {

   		return NOT_OKEY;

 	}else if (CheckHash (stack_ptr) == BAD_HASH) {
	
		return BAD_HASH;
	}

	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Pour poison in free array cells in struct Stack
//!
//! @param  [in]  data       pointer on array in struck Stack
//! @param  [in]  size_data  number of not free array cells
//! @param  [in]  capacity   number all elements of array 
//!
//! @return void
//-------------------------------------------------------------------------------------------------------------------------

static void PourPoison (elem_t *data, int size_data, int capacity)
{
	assert (data != NULL);

	size_t current_pos	= size_data;
	while (current_pos < capacity)
	{
		data[current_pos] = POISON;

		current_pos++;
	}

	return;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Constructor stack
//!
//! @param  [in]  pointer on struct Stack
//!
//! @return code error
//-------------------------------------------------------------------------------------------------------------------------

int StackCtor (Stack *stack_ptr)
{
	assert (stack_ptr != NULL);

	stack_ptr->canary1 = VALUE_STACK_CANARY;
	stack_ptr->canary2 = VALUE_STACK_CANARY;

	stack_ptr->data = (elem_t *) calloc (INITIAL_CAPACITY * sizeof (elem_t) + 2 * SIZE_CANARY, sizeof (char));
	if (stack_ptr->data == NULL)
	{
		printf ("error allocation memory\n");
		return ERR_ALLOCATION_MEMORY;
	}

	*((unsigned long long *)stack_ptr->data) = VALUE_DATA_CANARY;
	*((unsigned long long *) ((char *)stack_ptr->data + SIZE_CANARY + sizeof (elem_t) * INITIAL_CAPACITY)) = VALUE_DATA_CANARY;
	
	stack_ptr->capacity = INITIAL_CAPACITY;
	stack_ptr->size = 0;
	stack_ptr->data = (elem_t *)((char *) stack_ptr->data + SIZE_CANARY); 

	PourPoison (stack_ptr->data, stack_ptr->size, stack_ptr->capacity);

	CalcHashes (stack_ptr);

	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Destructor stack
//!
//! @param  [in]  stack_ptr  pointer on struct Stack
//!
//! @return void
//-------------------------------------------------------------------------------------------------------------------------

void StackDtor (Stack *stack_ptr)
{
	StackCheck (stack_ptr);

	free ((char *)stack_ptr->data - SIZE_CANARY); 

	stack_ptr->data = (elem_t *) ERR_PTR;

	return;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Pushing value in stack
//!
//! @param  [in]  stack_ptr  pointer on struct Stack
//! @param  [in]  value      pushing value
//!
//! @return code error:
//!          _0_ if there are no errors
//!          _2_ if there are errors
//-------------------------------------------------------------------------------------------------------------------------

int StackPush (Stack *stack_ptr, elem_t value)
{
	StackCheck (stack_ptr);

	if (stack_ptr->size >= stack_ptr->capacity)
		{
			if (StackResize (stack_ptr, stack_ptr->capacity * SPACE_FACTOR) != OKEY)
			{
				return ERR_ALLOCATION_MEMORY;
			}
		}

	stack_ptr->data[stack_ptr->size] = value;

	stack_ptr->size++;

	CalcHashes (stack_ptr);	
	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Poping value from stack
//!
//! @param  [in]  stack_ptr  pointer on struct Stack
//! @param  [out] ptr_place  pointer to the variable to which the value will be written from stack
//!
//! @return code error:
//!          _0_ if there are no errors
//!          _1_ if there are errors
//-------------------------------------------------------------------------------------------------------------------------

int StackPop (Stack *stack_ptr, elem_t *ptr_place)	                //resize down
{
	StackCheck (stack_ptr);

	if (stack_ptr->size == 0)
		return EMP_STACK;

	stack_ptr->size--;

	*ptr_place = stack_ptr->data[stack_ptr->size];

	if ((stack_ptr->size <= stack_ptr->capacity / REDUCTION_FACTOR) &&
		(stack_ptr->capacity / REDUCTION_FACTOR >= INITIAL_CAPACITY))
	{
		StackResize (stack_ptr, stack_ptr->capacity / REDUCTION_FACTOR);
	}
	else
	{
		stack_ptr->data[stack_ptr->size] = POISON;
	}

	CalcHashes (stack_ptr);	
	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Resize stack
//!
//! @param  [in]  stack_ptr     pointer on struct Stack
//! @param  [in]  new_capacity  new capacity stack
//!
//! @return code error:
//!          _0_ if there are no errors
//!          _2_ if there are errors
//-------------------------------------------------------------------------------------------------------------------------

int StackResize (Stack *stack_ptr, int new_capacity)
{
	StackCheck (stack_ptr);
	assert (new_capacity >= 0);

	stack_ptr->data = (elem_t *) realloc ((char *)(stack_ptr->data) - SIZE_CANARY, 
										   new_capacity * sizeof (elem_t) + 2 * SIZE_CANARY);

	if (stack_ptr->data == NULL)
	{
		return ERR_ALLOCATION_MEMORY;
	}

	stack_ptr->data = (elem_t *)((char *)(stack_ptr->data) + SIZE_CANARY);

	PourPoison (stack_ptr->data, stack_ptr->size, new_capacity);

	*((unsigned long long *)((char *)stack_ptr->data +
		 new_capacity * sizeof (elem_t))) = VALUE_DATA_CANARY;            // put canary in end memory for data

	stack_ptr->capacity = new_capacity;

	CalcHashes (stack_ptr);
	return OKEY;
}

//-------------------------------------------------------------------------------------------------------------------------
//! Print all information about stack in stream
//!
//! @param  [in]  stack_ptr      pointer on struct stack
//! @param  [in]  name_function  name somethink function
//! @param  [in]  num_line       number line
//!
//! @return void
//-------------------------------------------------------------------------------------------------------------------------

void StackDump (const Stack *stack_ptr, const char *name_function, 
				const int num_line)
{
	fprintf (LOG_FILE, "\n-----------------------------------------------------------\n");
	fprintf (LOG_FILE, "Dump from \"%s\" on line %d\n", name_function, num_line);
	fprintf (LOG_FILE, "-----------------------------------------------------------\n");
	fprintf (LOG_FILE, "ALL INFORMATION ABOUT STACK\n\n");
	fprintf (LOG_FILE, "Stack [%p] ()\n",                  stack_ptr);
	fprintf (LOG_FILE, "{\n");
	fprintf (LOG_FILE, "\tcanary1 = %0X\n",                stack_ptr->canary1);
	fprintf (LOG_FILE, "\tcanary2 = %0X\n",                stack_ptr->canary2);
	fprintf (LOG_FILE, "\tcapacity = %d\n",                stack_ptr->capacity);
	fprintf (LOG_FILE, "\tsize = %d\n",                    stack_ptr->size);
	fprintf (LOG_FILE, "\tdata [%p]\n",                    stack_ptr->data);
	fprintf (LOG_FILE, "\t{\n");
	fprintf (LOG_FILE, "\t\tcanary1' = %0llX\n",           *((unsigned long long *)((char *)stack_ptr->data - SIZE_CANARY)));

	for (int i = 0; i < stack_ptr->size; i++)
	{
		fprintf (LOG_FILE, "\t\t*[%03d] = " ELEM_FMT "\n", i, stack_ptr->data[i]);
	}

	if (stack_ptr->size + 5 >= stack_ptr->capacity)
	{
		for (int i = stack_ptr->size; i < stack_ptr->capacity; i++)
		{
			fprintf (LOG_FILE, "\t\t [%03d] = " ELEM_FMT "\n", i, stack_ptr->data[i]);
		}
	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			fprintf (LOG_FILE, "\t\t [%03d] = " ELEM_FMT "\n", i, stack_ptr->data[stack_ptr->size + i]);
		}
	
		fprintf (LOG_FILE, "\t\t .....   ....\n");
		fprintf (LOG_FILE, "\t\t [%03d] = " ELEM_FMT "\n", stack_ptr->capacity - 1, stack_ptr->data[stack_ptr->capacity - 1]);
	}

	fprintf (LOG_FILE, "\t\tcanary2' = %0llX\n", *((unsigned long long *)((char *)stack_ptr->data + sizeof (elem_t) * stack_ptr->capacity)));
	fprintf (LOG_FILE, "\t}\n");
	fprintf (LOG_FILE, "}\n");
	fprintf (LOG_FILE, "-----------------------------------------------------------\n\n");

	return;
}
