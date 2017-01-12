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


//=================================================================== LISTCOUNT
// Count the nodes in the list. 
//=================================================================== LISTCOUNT
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


//================================================================== LISTDELETE
// Remove a node from the list. Not very efficient I admit, but it 
// works fine. Don't forget to reclaim the space for the SQL code!
//================================================================== LISTDELETE
void listDelete(cursorNode *headNode, cursorNode *me)
{
    debugErr("listDelete(): Entry.\n");

    cursorNode *temp, *prev;

    debugErr("listDelete(): Looking for node = '%p'.\n", me);
    temp = prev = headNode;
    
    // Don't delete the head node!
    if (me == headNode) {
        debugErr("listDelete(): Cannot delete the headNode!!!\n");
        debugErr("listDelete(): headNode Exit.\n");
        return;
    }
    
    // Start scanning.
    while (temp != NULL) {
        debugErr("listDelete(): Looking for cursor id '%s' at address %p.\n", me->cursorId, temp);
        if (temp == me) {
            // Found it!
            debugErr("listDelete(): Found node %p.\n", temp);
            
            // Free the SQL text's memory.
            if (temp->sqlText) {
                debugErr("listDelete(): Freeing SQL memory at %p for cursor '%s' .\n", temp->sqlText, me->cursorId);
                free (temp->sqlText);
            }
            
            // And the node itself.
            prev->next = temp->next;
            debugErr("listDelete(): Deleting node at %p.\n", temp);
            free (temp);
            
            debugErr("listDelete(): node deleted: Exit.\n");
            return;
        }

        // Keep looking...
        prev = temp;
        temp = temp->next;
    }
    
    debugErr("listDelete(): Node '%p' not found in list.\n", me);
    debugErr("listDelete(): Not found: Exit.\n");
}