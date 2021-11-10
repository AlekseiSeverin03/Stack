#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "stack.h"


FILE *LOG_FILE = NULL;

int main ()
{
	Stack stack;


	OPEN_LOG_FILE();

	STACK_CTOR (&stack);

	STACK_PUSH (&stack, 4);
	STACK_PUSH (&stack, 1);
	STACK_PUSH (&stack, 4);
	STACK_PUSH (&stack, 1);

	elem_t x = 0;

	STACK_POP (&stack, &x);
	STACK_POP (&stack, &x);
	
	STACK_DUMP (&stack);
	STACK_DTOR (&stack);

	CLOSE_LOG_FILE();
	return 0;
}
