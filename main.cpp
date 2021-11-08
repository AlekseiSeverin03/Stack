#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "stack.h"

int main ()
{
	Stack stack;

	FILE *stream_log = NULL;
	OPEN_LOG_FILE(stream_log);

	STACK_CTOR(&stack);

	STACK_PUSH (&stack, 4, stream_log);
	STACK_PUSH (&stack, 1, stream_log);
	STACK_PUSH (&stack, 4, stream_log);
	STACK_PUSH (&stack, 1, stream_log);

	elem_t x = 0;

	STACK_POP (&stack, &x, stream_log);
	STACK_POP (&stack, &x, stream_log);

	STACK_DTOR (&stack, stream_log);

	CLOSE_LOG_FILE(stream_log);
	return 0;
}
