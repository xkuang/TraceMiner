#include <stdlib.h>
#include "linkedList.h"
#include "utils.h"

//==================================================================== LISTFIND
// Give a head node and a cursor id, find it in the list.
// Returns the address of the node if found, else NULL.
//==================================================================== LISTFIND
cursorNode *listFind(cursorNode *headNode, char *cursorID)
{
    debugErr("listFind(): Entry.\n");

    cursorNode *temp;

    debugErr("listFind(): Looking for cursorID = '%s'.\n", cursorID);
    temp = headNode;
    while (temp != NULL) {
        // If you uncomment the following command, your log file will be huge
        // depending on how many SQL statements there are in the input file
        // of course. Try not to give in to the urge to uncomment it!
        // debugErr("listFind(): Checking node %p ('%s') for '%s'\n", temp, temp->cursorId, cursorID);

        if (strncmp(temp->cursorId, cursorID, strlen(cursorID)) == 0) {
            debugErr("listFind(): Found cursorID '%s' in node %p.\n", temp->cursorId, temp);
            debugErr("listFind(): Found node: Exit.\n");
            return temp;        // Found it.
        }

        temp = temp->next;
    }
    debugErr("listFind(): CursorID '%s' not found in list.\n", cursorID);
    debugErr("listFind(): Exit.\n");
    return NULL;                // Not found.
}

//================================================================== LISTAPPEND
// Append a new node to the end of the list. Not very efficient
// I admit, but it works fine.
//================================================================== LISTAPPEND
void listAppend(cursorNode *headNode, cursorNode *me)
{
    debugErr("listAppend(): Entry.\n");

    cursorNode *temp;

    temp = headNode;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    temp->next = me;
    me->next = NULL;
    debugErr("listAppend(): Added node %p ('%s') after node %p.\n\n", me, me->cursorId, temp);
    debugErr("listAppend(): Exit.\n");
}


// Count the nodes in the list. 
int listCount(cursorNode *headNode)
{
    debugErr("listCount(): Entry.\n");

    cursorNode *temp;
    int c = 0;

    temp = headNode;
    while (temp != NULL) {
        temp = temp->next;
        c++;
    }

    debugErr("listCount(): Exit.\n");
    return c;
}


//=================================================================== LISTCLEAR
// Delete all the nodes in the list. And their contents.
//=================================================================== LISTCLEAR
void listClear(cursorNode *headNode)
{
    debugErr("listClear(): Entry.\n");

    cursorNode *temp, *right;

    temp = headNode;
    while (temp != NULL) {
        debugErr("listClear(): Clearing node at %p. Cursor#: %s.\n", temp, temp->cursorId);
        right = temp->next;
        if (temp->sqlText)
            free (temp->sqlText);           // Free memory for the SQL text.
        free (temp);                        // Free memory for the node.
        temp = right;
    }
    debugErr("listClear(): Exit.\n");
}


//================================================================= LISTDISPLAY
// Print out the entire list.
//================================================================= LISTDISPLAY
void listDisplay(cursorNode *headNode)
{
    debugErr("listDisplay(): Entry.\n");

    cursorNode *temp;

    temp = headNode;
    debugErr("listDisplay():Dumping headNode %p.\n", temp);
    while (temp != NULL) {
        debugErr("\nCursorNode %p:\n", temp);
        debugErr("Cursor: %s\n", temp->cursorId);
        debugErr("Binds: %d\n", temp->bindsPerExec);
        debugErr("Line number: %ld\n", temp->lineNumber);
        debugErr("SQL: %s", temp->sqlText);
        debugErr("Next: %p\n", temp->next);
        temp = temp->next;
    }
    debugErr("listDisplay(): Exit.\n");
}
