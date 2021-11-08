#include <stdio.h>
#include <assert.h>

enum OUT_FUN_WORK_FILE
{
OK_OPEN        =  50,
OK_CLOSE       =  51,
ERR_OPEN_FILE  = -50,
ERR_CLOSE_FILE = -51,
};

const char PREAMBLE[] = "This is log file of the project \"Stack\"\n"
						"Here will be write errors and warnings\n\n";


int Open_File  (FILE **stream, const char *name_file);
int Close_File (FILE *stream,  const char *name_file);


#define OPEN_LOG_FILE(stream)                                   \
{                                                               \
	if (Open_File (&stream, "log_file.txt") == ERR_OPEN_FILE)   \
	{                                                           \
		printf ("Fail to open the file \"log_file.txt\"\n\n");  \
		exit (10);                                              \
	}                                                           \
}

#define CLOSE_LOG_FILE(stream)                                  \
{                                                               \
	if (Close_File (stream, "log_file.txt") == ERR_CLOSE_FILE)  \
	{                                                           \
		printf ("Fail to close the file \"log_file.txt\"\n\n"); \
		exit (11);                                              \
	}                                                           \
}


