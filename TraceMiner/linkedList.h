#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include "config.h"

typedef struct cursorNode {
    char cursorId[MAXCURSORSIZE+1];     // Extracted cursor# from trace file.
    char *sqlText;                      // SQL text, stripped of binds.
    int bindsPerExec;                   // How many binds required for each EXEC?
    long lineNumber;                    // Which trace file line was the SQL found?
    struct cursorNode *next;            // Next cursorNode in this list.
} cursorNode;

void listAppend(cursorNode *headNode, cursorNode *me);
int listCount(cursorNode *headNode);
void listClear(cursorNode *headNode);
void listDisplay(cursorNode *headNode);
cursorNode *listFind(cursorNode *headNode, char *cursorID);
void listDelete(cursorNode *headNode, cursorNode *me);

#endif

