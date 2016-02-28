#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "linkedList.h"
#include "bindvalues.h"

extern long lineNumber;
extern int debugging;

// Usage, aka how to use this utility.
void usage(char *message);


// Logging routines. One for stderr, one for stdout and a debugger.
void logOut(char *format, ...);


// Log an error message.
void logErr(char *format, ...);


// Log debugging messages to stderr if debugging != 0.
void debugErr(char *format, ...);


// getLine reads from stdin. At end of file, it returns -1.
// If the line read from the file is longer than the buffer,
// GNU getline will safely realloc() the buffer, so no buffer
// over runs are likely.
int getLine();


// Extract the first word from the string. Space delimited.
// NULL means no words.
char *getFirstToken(char *buffer);


// Extract the next word from the string. Space delimited.
// NULL means no [more] words.
char *getNextToken();


// Extract the SQL statement.
char *getSQLStatement(size_t statementLength);


// Locate any bind variables and count them. Convert them to '%s' in
// all cases. We will use this later for printing the binds.
int convertSQLBinds(char *sqlStatement);

// GEt the value part for a bind variable.
char *extractValue(int dataType, char *text);


// Extract the required number of bind variables from the trace file.
int getBindValues(cursorNode *temp, bindValues *bValues);


// Extracts a single bind variables details.
char *getOneBindValue();


// Print out an SQL statement with all the binds replaced
// with '%s' which just happens to be the format specifier for
// a string. We also have an array of strings holding the
// bind values for the specific SQL statement.
void printExecDetails(cursorNode *node);

#endif
