//==============================================//
// Copyright (c) Norman Dunbar, 2016.
//==============================================//
// If you wish to use this code, be my guest.
// Just leave my copyright intact, and add a
// wee detail of what it was you changed.
// That's it.
//==============================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "utils.h"
#include "config.h"
#include "linkedList.h"
#include "bindvalues.h"

int processPARSING();
int processBINDS();
int processEXEC();
int processXCTEND();


// Version number.
const float version = 0.1;


//----------------------------------------------//
// This one's for Rich!
//----------------------------------------------//
// Because the test trace file supplied had been
// obfuscated, the lengths of the SQL statements
// being parsed were incorrect. Allocating a new
// buffer meant problems. This is added to all
// statement sizes as a work around.
// In proper production trace files, this should
// be zero.
//----------------------------------------------//
const int offsetForRich = 128;

//===================================================================
// WARNING:
//===================================================================
// This utility caters for up to 50 bind variables (MAXBINDS), each
// pretending to be a string of up to 50 characters (MAXBINDSIZE).
// If you need to change the number or max size of the bind values
// stored and printed, then simply:
//  * Adjust the value(s) in bindvalues.h.
//  * If you made MAXBINDS bigger then add the additional entries to
//    the function printExecDetails() in utils.c.
//  * If you made MAXBINDS smaller then remove the no longer needed
//    entries from the function printExecDetails() in utils.c.
//  * If you changed MAXBINDSIZE then there are no additional changes
//    to be done to cater, the utility copes quite happily.
//  * Recompile.
//===================================================================

// We need the buffer in lots of places, so make it global.
size_t bufferSize = 2048;       // Seems adequate for a buffer. Getline will
                                // *safely* extend it if necessary.
char *myBuffer;                 // Pointer to my buffer.
int debugging = 0;              // Set this for all sorts of stuff in stderr!
int HTMLmode = 0;               // Set this to have output in HTML
long lineNumber = 0;            // Which line are we on? Needs to be global.
cursorNode *headNode;           // Linked list head node. MUST BE A POINTER!
int didItWork = 0;              // Any parse, exec, binds errors while processing?


//======================================================================== MAIN
//======================================================================== MAIN
//======================================================================== MAIN
//======================================================================== MAIN
int main(int argc, char *argv[])
{
    int bytesRead;                  // Size of the line read from stdin.
    int startParsing = 0;           // Have we found the first "============" yet?

    logOut("TraceMiner: Version %4.2f\n", version);

    // Check for command line parameters. Yes, I know, it's pretty basic stuff...
    if (argc == 2) {
        if ((strncasecmp(argv[1], "--verbose", 9) == 0)  ||
            (strncasecmp(argv[1], "-v", 2) == 0)) {
            debugging = 1;
        } else {
            usage("ERROR: Invalid parameter: detected.");
            return 1;
        }
    } else {
        if (argc != 1) {
            usage("ERROR: Too many parameters supplied.");
            return 1;
        }
    }

    myBuffer = (char *) malloc (bufferSize + 1);
    if (myBuffer == NULL) {
        logErr("ERROR: malloc() failed to allocate buffer (%d bytes)\n", (int)bufferSize);
        return 1;
    }

    // Let's do a rudimentary check to see if this is really a trace file.
    bytesRead = getLine();
    if (bytesRead == -1) {
        logErr("FATAL: Premature end of trace file.'\n");
        return 1;
    }

    if (strncmp(myBuffer, "Trace", 5) != 0) {
        logErr("FATAL: Your trace file probably isn't a trace file. Missing 'Trace File' in header!'\n");
        return 1;
    }

    // Seems to be ok, print the trace file name.
    logOut("Processing: %s\n\n", myBuffer);

    // Headings next.
    logOut("---------------------------------------------------------------------------------------------------------------------------------------\n");
    logOut("%-11.11s%18.18s: %-55.55s\n", "Trace Line:", "Cursor ID", "SQL Text with binds replaced");
    logOut("---------------------------------------------------------------------------------------------------------------------------------------\n");

    // Set up the head node.
    headNode = (cursorNode *)malloc(sizeof(cursorNode));
    if (headNode == NULL) {
        logErr("FATAL: Cannot allocate headNode for linked list.\n");
        return 1;
    }

    debugErr("\nHeadNode allocated at address %p.\n\n", headNode);

    // strncpy won't terminate!
    // Unless it finds a '\0' in the first 9 chars of course. ;-)
    strncpy(headNode->cursorId, "Head Node", 9);
    headNode->cursorId[9] = '\0';
    headNode->bindsPerExec = -1;
    headNode->lineNumber = -1;
    headNode->sqlText = NULL;
    headNode->next = NULL;

    // Here's the main loop. We read and process lines until error or EOF.
    while (1) {
        // Read a line from stdin until EOF or error.
        bytesRead = getLine ();
        if (bytesRead == -1) {
            break;
        }

        // Locate the first "===========" line, to signal we can go mining.
        if (!startParsing) {
            if (strncmp(myBuffer, "==========", 10) == 0)
                startParsing = 1;

            continue;
        }

        //--------------------------------------------------------
        // What kind of line is this? Each check does its bit and
        // skips further unnecessary checks by "continuing" the
        // while loop, short circuiting it.
        //--------------------------------------------------------
        if (strncmp(myBuffer, "PARSING", 7) == 0) {
            didItWork = processPARSING();
            if (didItWork < 0)
                return 1;
            continue;
        }

        if (strncmp(myBuffer, "BINDS", 5) == 0) {
            didItWork = processBINDS();
            if (didItWork < 0)
                return 1;
            continue;
        }

        if (strncmp(myBuffer, "EXEC", 4) == 0) {
            didItWork = processEXEC();
            if (didItWork < 0)
                return 1;
            continue;
        }

        if (strncmp(myBuffer, "XCTEND", 6) == 0) {
            didItWork = processXCTEND();
            if (didItWork < 0)
                return 1;
            //continue;    // No further tests to skip.
        }

    } // while (1)

    // If we are debugging, dump the entire linked list.
    if (debugging) {
        debugErr("\n\nTree List\n");
        listDisplay(headNode);                         // Dump the entire list.
    }

    // Clear the list and free all allocated memory. It's
    // not strictly necessary, as the OS //should// do it
    // on exit. But I'm old school!
    debugErr("\n\nTree Felling in Progress. HeadNode = %p.\n", headNode);
    listClear(headNode);

    // Job done, no errors, bale out.
    return 0;
}


//============================================================== PROCESSPARSING
// Is this line a PARSING IN CURSOR line?
//============================================================== PROCESSPARSING
int processPARSING()
{
    debugErr("processPARSING(): Entry for line %d.\n", lineNumber);

    int nodeExists = 0;                 // Has cursorID been used before?
    char *cursorToken;                  // #nnnnnnnnnn cursor number for SQL parsing lines.
    char *lengthToken;                  // len=nnn for SQL parsing lines.
    char *depthToken;                   // dep=nn for SQL parsing lines
    char *sqlStatement;                 // The actual SQL statement in question.
    long sqlLineNumber;                 // Trace file line where this SQL Statement starts.

    // The line after this one is where the SQL actually starts.
    sqlLineNumber = lineNumber +1;

    int x;
    cursorToken = getFirstToken(myBuffer);      // Ignore 'PARSING'.
    for (x = 0; x < 3; x++) {
        cursorToken = getNextToken();           // Get the cursor #.
    }

    lengthToken = getNextToken();               // How big is this SQL?
    depthToken = getNextToken();                // What depth is it?

    // Is this statement recursive SQL?
    if (strncmp(depthToken, "dep=0", 5) != 0) {
        debugErr("processPARSING(): Recursive SQL: Exit.\n");
        return 0;                               // Not interested.
    }

    // Is the cursor id too big?
    if (strlen(cursorToken) > MAXCURSORSIZE) {
        logErr("\n\nprocessPARSING(): FATAL ERROR:\n");
        logErr("processPARSING(): CursorID '%s' has %d characters which has exceeded the current maximum \n", cursorToken, strlen(cursorToken));
        logErr("                  of %d compiled into the program.\n", MAXCURSORSIZE);
        logErr("                  You must edit file 'config.h' and increase MAXCURSORSIZE to at least %d, then recompile.\n", strlen(cursorToken));
        return -1;
    }

    // Create/find a cursorNode to hold the details.
    cursorNode *newNode = listFind(headNode, cursorToken);
    if (newNode == NULL) {
        // Not in the list yet, so allocate a new node.
        nodeExists = 0;
        newNode = (cursorNode *)malloc(sizeof(cursorNode));
        if (newNode == NULL) {
            logErr("FATAL: processPARSING(): Unable to allocate a new cursorNode.");
            return -1;
        } else {
            debugErr("processPARSING(): New list node allocated at %p.\n", newNode);
        }
    } else {
        debugErr("processPARSING(): Reusing list node at %p.\n", newNode);
        nodeExists = 1;
        debugErr("processPARSING(): Freeing old SQL (at %p) of existing node at %p.\n", newNode->sqlText, newNode);
        free(newNode->sqlText);
        debugErr("processPARSING(): Old SQL contents have been freed.\n");
    }

    debugErr("processPARSING(): CursorID is now '%s', %d characters.\n", cursorToken, strlen(cursorToken));

    // Only set newNode->next if we have a new node, don't screw the list
    // if we are reusing one. Yes, I had a bug in which did exactly that!
    // Also, the cursorID is the same, so no need to update it.
    if (!nodeExists) {
        newNode->next = NULL;                   // We are last in the list.
        debugErr("processPARSING(): newNode at %p, for cursorID '%s', is the new last in list.\n", newNode, cursorToken);
        strncpy(newNode->cursorId, cursorToken, strlen(cursorToken));   // Cursor ID.
        newNode->cursorId[strlen(cursorToken)] = '\0';                  // Don't forget to teminate.
                debugErr("processPARSING(): newNode at %p is for cursorID '%s'.\n", newNode, newNode->cursorId);
    }

    newNode->bindsPerExec = 0    ;          // Number of binds per exec.
    debugErr("processPARSING(): newNode at %p assuming no binds for now.\n", newNode);
    newNode->lineNumber = sqlLineNumber;    // Where we found it.
    debugErr("processPARSING(): newNode at %p, cursorID '%s', for SQL at line %ld.\n", newNode, newNode->cursorId, newNode->lineNumber);


    // We have an application SQL statement to extract.
    int sqlSize = atoi(lengthToken + 4);
    sqlStatement = getSQLStatement((size_t) sqlSize);
    if (sqlStatement == NULL) {
        debugErr("processPARSING(): NULL SQL Statement: Exit.\n", lineNumber);
        return 0;
    }

    debugErr("processPARSING(): SQL Statement at line %ld, CursorID '%s':\n%s", sqlLineNumber, newNode->cursorId, sqlStatement);

    // Convert the SQL to extract the binds.
    int binds = convertSQLBinds(sqlStatement);
    debugErr("processPARSING(): CursorID '%s'\n%sAfter removing %d binds.\n", newNode->cursorId, sqlStatement, binds);

    // Don't overflow!
    if (binds > MAXBINDS) {
        logErr("\n\nprocessPARSING(): FATAL ERROR:\n");
        logErr("processPARSING(): CursorID '%s' has %d bind variables. This exceeds the\n", newNode->cursorId);
        logErr("                  current maximum of %d compiled into the program.\n", MAXBINDS);
        logErr("                  You must edit file 'config.h' and increase MAXBINDS to at least %d, then recompile.\n", binds);
        return -1;
    }

    // Update the new node.
    newNode->bindsPerExec = binds;          // Number of binds per exec.
    debugErr("processPARSING(): newNode at %p updated with %d binds.\n", newNode, newNode->bindsPerExec);
    newNode->sqlText = sqlStatement;        // SQL statement text, with no binds.
    debugErr("processPARSING(): newNode at %p has this (modified) SQL at address %p\n%s\n", newNode, &newNode->sqlText, newNode->sqlText);

    if (!nodeExists)
        listAppend(headNode, newNode);

    // If we are debugging, dump the entire linked list.
    if (debugging) {
        debugErr("\n=============================================================================================");
        debugErr("\nprocessPARSING(): Current Tree List.");
        debugErr("\n=============================================================================================\n\n");
        listDisplay(headNode);                         // Dump the entire list.
        debugErr("\n=============================================================================================");
        debugErr("\nprocessPARSING(): End of Tree List.");
        debugErr("\n=============================================================================================\n\n");
    }

    debugErr("processPARSING(): Exit.\n");
    return 0;
}


//================================================================ PROCESSBINDS
// Is this line an BINDS line then?
//================================================================ PROCESSBINDS
int processBINDS()
{
    debugErr("processBINDS(): Entry for line %d.\n", lineNumber);

    int binds = 0;

    char *cursorToken = getFirstToken(myBuffer);    // Ignore 'BINDS'
    cursorToken = getNextToken();                   // Get the cursor#
    debugErr("processBINDS(): BINDS found for cursorID '%s'\n", cursorToken);

    // Do we have it in the list? If not, we can't use it. Probably not
    // a "dep=0" cursor.
    cursorNode *temp = listFind(headNode, cursorToken);

    // Probably a recursive SQL if not found.
    if (temp == NULL) {
        debugErr("processBINDS(): CursorID '%s', not found in list.\n", cursorToken);
        debugErr("processBINDS(): Cursor not found in list: Exit.\n");
        return 0;
    }

    debugErr("processBINDS(): CursorID '%s', found in list node %p.\n", cursorToken, temp);

    // Extract the bind variables.
    if (temp->bindsPerExec != 0) {
        binds = getBindValues(temp, &sqlBinds);
        debugErr("processBINDS(): getBindValues() returned %d, cursorID '%s' required %d bind values.\n", binds, temp->cursorId, temp->bindsPerExec);
    }

    // Exit on error.
    if (binds == -1) {
        debugErr("processBINDS(): getBindValues error: Exit.\n");
        return -1;
    }

    if (binds != temp->bindsPerExec) {
        logErr("processBINDS(): getBindValues() returned %d, cursorID '%s' required %d bind values.\n", binds, temp->cursorId, temp->bindsPerExec);
        return -1;
    } else {
        debugErr("processBINDS(): getBindValues() returned %d, cursorID '%s' required %d bind values.\n", binds, temp->cursorId, temp->bindsPerExec);
    }

    return 0;
}


//================================================================= PROCESSEXEC
// Is this line an EXEC line then?
//================================================================= PROCESSEXEC
int processEXEC()
{
    debugErr("processEXEC(): Entry for line %d.\n", lineNumber);

    int x;
    char *depthToken;
    char *cursorToken = getFirstToken(myBuffer);    // Ignore 'EXEC'
    debugErr("processEXEC(): EXECuting SQL statement %s.\n", cursorToken);

    cursorToken = getNextToken();                   // Get the cursor #
    for (x=0; x < 8; x++) {
        depthToken = getNextToken();                // Get the depth
    }

    // Recursive SQL? Bale out if so.
    if (strncmp(depthToken, "dep=0", 5) != 0) {
        debugErr("processEXEC(): Reursive SQL: Exit.\n");
        return 0;
    }

    // This is not recursive SQL, so we should have this in the
    // list, error out if not found.
    cursorNode *temp = listFind(headNode, cursorToken);
    if (temp == NULL) {
        debugErr("processEXEC(): CursorID '%s', not found in list.\n", cursorToken);
        debugErr("processEXEC(): Cursor not found in list: Exit.\n");
        return -1;
    }

    debugErr("processEXEC(): CursorID '%s', found in list node %p.\n", cursorToken, temp);

    // We have the correct cursor, and the bindValues stucture (should) be
    // holding the correct number of bind variable values, so let's print it.
    printExecDetails(temp);
    debugErr("processEXEC(): Exit.\n");
    return 0;
}


//=============================================================== PROCESSXCTEND
// Is this line a COMMIT/ROLLBACK line then? If so, print
// a blank line after the statement to separate out the
// various transactions.
//=============================================================== PROCESSXCTEND
int processXCTEND()
{
    debugErr("processXCTEND(): Entry for line %d.\n", lineNumber);

    char *format = "%10ld:%18.18s: %s\n\n";     // Saves typing!

    char *cursorToken = getFirstToken(myBuffer);    // Ignore XCTEND.
    cursorToken = getNextToken();
    if (strncmp(cursorToken, "rlbk=0", 6) == 0) {
        debugErr("EXECuting COMMIT.\n", cursorToken);
        logOut(format, lineNumber, " ", "COMMIT");
    } else {
        debugErr("EXECuting ROLLBACK.\n", cursorToken);
        logOut(format, lineNumber, " ", "ROLLBACK");
    }
    debugErr("processXCTEND(): Exit.\n");
    return 0;
}
