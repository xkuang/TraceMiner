#include <ctype.h>

// Using the free Borland C++ v10.1 compiler?
// This needs to go before utils.h.
#include "borlandc.h"

#include "utils.h"

// Required for getLine() etc.
extern size_t bufferSize;
extern char *myBuffer;

// Required by getFirstToken and getNextToken.
// We do not want '/n' in the following or it blows up
// getNextToken in extractValue() for oacdty=96. (NVARCHAR/NCHAR)
char *stopChars = " :,\r";


//======================================================================= USAGE
// Usage, aka how to use this utility.
//======================================================================= USAGE
void usage(char *message)
{
    puts(message);
    puts("USAGE:");
    puts("\ttraceminer [options] <trace_file [>output_file][2>debug_file]");
    puts("\n\t'trace_file' is the input trace file from Oracle. It needs to have binds turned on. (stdin)");
    puts("\t'output_file' is where the information you want will be written. (stdout)");
    puts("\t'debug_file' is where errors and debugging information will be written. (stderr)\n");
    puts("\nOPTIONS:");
    puts("\t'-v' or '--verbose' Turn on debugging mode. Debugging text is written to stderr.");
    puts("\t'-m' or '--html' Turn on HTML mode. Anything written to stdout will be in HTML format.");
    puts("\t'-?'. '-h' or '--help' Displays this help, and exits.");
}


//====================================================================== LOGOUT
// Log a message to stdout. This is mainly used to print out the
// various SQL statements.
//====================================================================== LOGOUT
void logOut(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fflush(stdout);         // Makes sure messages appear instantly.
}


//====================================================================== LOGERR
// Log an error message to stderr. This will always happen whether
// debugging or not.
//====================================================================== LOGERR
void logErr(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);         // Makes sure errors appear instantly.
}


//==================================================================== DEBUGERR
// Log debugging messages to stderr if debugging != 0.
//==================================================================== DEBUGERR
void debugErr(char *format, ...)
{
    if (!debugging) return;

    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);         // Makes sure debugging text appears instantly.
}


//===================================================================== GETLINE
// getLine reads from stdin. At end of file, it returns -1.
// If the line read from the file is longer than the buffer,
// GNU getline will safely realloc() the buffer, so no buffer
// over runs are likely.
//===================================================================== GETLINE
int getLine()
{
    debugErr("getLine(): Entry\n");

    int bytesRead;

    // We have to watch out for those "interesting" lines in the trace
    // file where Oracle inserts a blank line and a timestamp line.

    while (1) {
        bytesRead = getline (&myBuffer, &bufferSize, stdin);

        // EOF?
        if (bytesRead == -1)
            break;

        // We must count this line then, if not EOF.
        lineNumber++;

        // Ignore '*** ??? Timestamp' lines.
        if (strncmp(myBuffer, "***", 3) == 0) {
            // I'm not interested in all of the line, only the first 20 odd charactares.
            debugErr("getLine(): At Line: %ld, TIMESTAMP entry '%23.23s ...' ignored.\n", lineNumber, myBuffer);
            continue;
        }

        // This MUST be the final check.
        // If this is a Blank line? (Just LF or CRLF) read again, otherwise we are done.
        if (bytesRead > 2)
            break;
        else
            debugErr("getLine(): At Line: %ld, blank line ignored.\n", lineNumber);
    }

    // Further checking required here.
    if (bytesRead != -1) {
        debugErr("getLine(): Line: %ld, size: %d bytes, content: %s", lineNumber, bytesRead, myBuffer);
    } else
        debugErr("getLine(): After Line: %ld, END OF FILE detected.\n", lineNumber);

    debugErr("getLine(): Exit\n");
    return bytesRead;
}


//=============================================================== GETFIRSTTOKEN
// Extract the first word from the string. Space delimited.
// NULL means no words.
//=============================================================== GETFIRSTTOKEN
char *getFirstToken(char *buffer)
{
    debugErr("getFirstToken(): Entry\n");

    char *token;

    token = strtok(buffer, stopChars);
    debugErr("getFirstToken(): Returning %d characters, '%s'.\n", strlen(token), token);
    debugErr("getFirstToken(): Exit\n");
    return token;
}


//================================================================ GETNEXTTOKEN
// Extract the next word from the string. Space delimited.
// NULL means no [more] words.
//================================================================ GETNEXTTOKEN
char *getNextToken()
{
    debugErr("getNextToken(): Entry\n");

    char *token;

    token = strtok(NULL, stopChars);
    debugErr("getNextToken(): Returning %d characters, '%s'.\n", strlen(token), token);
    debugErr("getNextToken(): Exit\n");
    return token;
}


//============================================================= GETSQLSTATEMENT
// Extract the full text of the current SQL statement.
//============================================================= GETSQLSTATEMENT
char *getSQLStatement(size_t statementLength)
{
    debugErr("getSQLStatement(): Entry\n");

    int bytesRead = 0;
    char *sqlBuffer;

    // Allocate a buffer to hold the parsed SQL statement as per the length reported
    // in the trace file, plus an extra allowance for those trace files, such as
    // Rich's, where the tables etc have been obfuscated and the length no longer
    // matches that in the PARSING IN CURSOR line. :-)
    // I also add on a little  extra, for luck!
    size_t sqlLength = statementLength + OFFSETFORRICH + MAXBINDS;

    // Allocate a buffer for the whole SQL statement.
    // WARNING: It is the caller's job to free() this buffer
    sqlBuffer = (char *) malloc(sqlLength + 1);
    if (sqlBuffer == NULL) {
        logErr("FATAL: malloc() failed to allocate sqlBuffer (%d bytes)\n", (int)sqlLength);

        // Fatal error, abort.
        return NULL;
    }  else {
        debugErr("getSQLStatement(): sqlBuffer (%d bytes) allocated at %p\n", (int)sqlLength, sqlBuffer);
    }

    // Must, must, must null out the buffer as it may be being reused from
    // a previous read. Don't ask me how I know, jfdi!
    sqlBuffer[0]='\0';


    while (bytesRead != -1) {
        // Read a line of SQL text and append it to the results buffer.
        bytesRead = getLine();
        if (strncmp(myBuffer, "END OF STMT", 11) == 0) break;

        strncat(sqlBuffer, myBuffer, (size_t)bytesRead);
    }

    debugErr("getSQLStatement() returning : %s", sqlBuffer);
    debugErr("getSQLStatement(): Exit\n");
    return sqlBuffer;
}


//============================================================= CONVERTSQLBINDS
// Locate any bind variables and count them. Convert them to '%s'
// in all cases. We will use this later for printing the binds.
//============================================================= CONVERTSQLBINDS
int convertSQLBinds(char *sqlStatement)
{
    debugErr("convertSQLBinds(): Entry\n");

    char *from = sqlStatement;
    char *to = sqlStatement;
    int bindCount = 0;
    int gotBinds = 0;

    // Scan the SQL looking for a colon. This indicates a bind variable.
    // After the first one, we have to adjust the rest of the buffer to
    // strip out the meaningful names and replace them with "%s" instead
    // of ":variable_name". Ignore ":=" though, that's PL/SQL not a bind.
    // Also, to allow the HTML output table to adjust to the browser width,
    // a space will be inserted into the output SQL statement.
    while (*from) {
        if (*from == ':' && *(from + 1) != '=') {
            // First bind variable has been found.
            debugErr("convertSQLBinds(): Found first ':' - followed by '%c'\n", *(from + 1));
            gotBinds = 1;
            break;
        }

        // Keep searching...
        to++;
        from++;
    }

    // If we didn't find any binds, bale out early.
    if (!gotBinds) {
        // Nothing to see here, move along!
        debugErr("convertSQLBinds(): Exit\n");
        return 0;
    }

    // We must have at least one bind variable. From points at the colon.
    while (*from) {
        // Bloody Windows files!
        // Or even, Linux files that have been converted to Windows!
        if (*from == '\r') from++;     // Skip carriage returns.

        // Looking for the [next] colon...
        if (*from != ':') {
            // Copy 'from' character to 'to' character in the buffer.
            *to++ = *from++;
        } else {
            // Found a colon, it's a[nother] bind variable.
            debugErr("convertSQLBinds(): Found next ':' - followed by '%c'\n", *(from + 1));

            if (*(from + 1) != '=') {
                // We have a valid bind variable.
                bindCount++;

                // Skip past the bind in the input buffer.
                // Are there any other characters that could go here I wonder?
                while (*from &&
                       *from != ' ' &&
                       *from != ')' &&
                       *from != ',' &&
                       *from != '\n') from++;

                // Manipulate the output buffer.
                *to++ = '%';
                *to++ = 's';
            } else {
                // We have a PL/SQL ':=' assignment statement. Ignore it.
                *to++ = *from++;
            }
        }
    }

    // Terminate the new SQL statement. It's going to be shorter
    // if the SQL wasn't using single character bind variables or
    // longer if it was.
    *to = '\0';

    debugErr("convertSQLBinds(): Exit\n");
    return bindCount;
}


//================================================================ EXTRACTVALUE
// A quick and dirty function to return the bit after the 'value='
// text for a bind variable. It's not as easy as at first thought
// as NCHAR/NVARCHAR2 stuff need special handling.
//================================================================ EXTRACTVALUE
char *extractValue(int dataType, char *text)
{
    debugErr("extractValue(): Entry\n");
    debugErr("extractValue(): dataType=%d, text='%s'.\n", dataType, text);

    static char buffer[MAXBINDSIZE+1];
    int pseudoDataType = dataType;          // Sometimes I need a different type

    // Is the data string long enough?
    int dataSize = strlen(text);            // Actual data type of the bind
    if (dataSize < 7) {                     // Must be "value=" or something broken!
        debugErr("extractValue(): dataSize too small (%d): Exit\n", dataSize);
        return NULL;
    }

    // Have to test this before the rest. There's no "value=" line.
    // Weird one, used to receive the DBMS_OUTPUT lines in a call to
    // DBMS_OUTPUT.GET_LINES(:LINES, :NUMBER_OF_LINES).
    if (dataType == 123) {
        debugErr("extractValue(): Bind type=123, resynching buffer...\n");
        getLine();
        debugErr("extractValue(): Bind type=123, buffer resync complete....\n");
        debugErr("extractValue(): Possible DBMS_OUTPUT.GET_LINES buffer found: Exit\n");
        return ":A_BUFFER";
    }

    // Find the '='.
    char *from = text + 5;
    if (*from != '=') {
        debugErr("extractValue(): '=' not found: Exit\n");
        return NULL;
    }

    int offset = 0;
    dataSize -= 6;      //Size after the "value=" part. Always > 0.
    from++;

    //--------------------------------------------------------------------
    // DataType 01 is VARCHAR and NVARCHAR2. If the former then the value
    // starts with a double quote. If the latter, the value is in hex.
    // So if it's the latter I need to pretend it's a dataType 96 instead
    // to extract the hex bytes.
    //--------------------------------------------------------------------
    if (dataType == 1) {
        if (*from != '"') {
            pseudoDataType = 96;        // Extract the hex instead, NVARCHAR2.
            debugErr("extractValue(): VARCHAR2(dataType 1) is really NVARCHAR2 (dataType 96).\n");
        }
    }

    // Watch out for illegal numbers. ('value=###')
    if (dataType == 2) {
        if (*from == '#') {
            // We have an illegal number.
            debugErr("extractValue(): illegal number found: Exit\n");
            return "## Illegal number ##";
        }
    }

    // NCHAR/NVARCHAR are a PITA!
    if (pseudoDataType == 96) {
        // NCHAR/NVARCHAR2 types need special treatment. The line is something like
        // value=0 nn 0 nn 0 nn ... and we simply ignore the zeros. We can walk the text using tokens.

        // We MUST getFirstToken() before getNextToken() as we have a new line!
        char *nextToken = getFirstToken(text);

        // Leading quote, and trailing, reduce available space by 2.
        buffer[offset++] = '\'';

        // Loop around all the tokens. Or a max of MAXBINDSIZE -2 offsets.
        // Exit the loop on NULL token, a newline token or out of bind value space.
        while (nextToken && (*nextToken != '\n') && (offset <= MAXBINDSIZE - 2)) {
            // It's HEX, ok?
            char c = (int) strtol(nextToken, NULL, 16);
            if (c)
                buffer[offset++] = c;

            nextToken = getNextToken();
        }

        // Trailing quote and terminator.
        buffer[offset++] = '\'';
        buffer[offset] = '\0';
        goto all_done;              // If anyone mentions "not using goto", I'll ignore you! :-)
    }

    // Anything I haven't trapped above!
    while (*from && (offset < MAXBINDSIZE) && (*from != '\r') && (*from != '\n')) {
        buffer[offset++] = *from++;
    }
    buffer[offset] = '\0';

all_done:
    debugErr("extractvalue(): dataType=%d, length %d, returning: '%s'.\n", dataType, strlen(buffer), buffer);
    debugErr("extractValue(): Exit\n");
    return buffer;
}


//=============================================================== GETBINDVALUES
// Extract the bind variable values. It seems to be a section
// of the trace file that begins with one space, then we have Bind#n,
// then details of that bind are indented by two spaces.
// We need to process different bind types differently.
// Oh, and you cannot rely on the "mxl(nn)" stuff for max length(used length)
// because they tell lies sometimes, but not always.
//
// Entry is with myBuffer holding the "BINDS #CursorID:" line.
//=============================================================== GETBINDVALUES
int getBindValues(cursorNode *temp, bindValues *bValues)
{
    debugErr("getBindValues(): Entry\n");
    debugErr("getBindValues(): Processing %d binds for cursorID '%s'.\n", temp->bindsPerExec, temp->cursorId);
    debugErr("Current Line: %s.\n", myBuffer);

    int thisBind;
    int bindsFound = 0;
    char *thisValue = NULL;

    for (thisBind = 0; thisBind < temp->bindsPerExec; thisBind++) {
        thisValue = getOneBindValue();
        debugErr("getBindValues(): Value for bind %d is '%s'.\n", thisBind, thisValue);
        if (thisValue == NULL) {
            debugErr("getBindValues(): ERROR getting value for bind %d.\n", thisBind);
            debugErr("getBindValues(): Error Exit\n");
            return bindsFound;
        }

        // Save the extracted value.
        strncpy(bValues->bv[thisBind], thisValue, MAXBINDSIZE);
        bValues->bv[thisBind][strlen(bValues->bv[thisBind])] = '\0';
        bindsFound++;
    }

    debugErr("getBindValues(): Exit\n");
    return bindsFound;
}


//============================================================= GETONEBINDVALUE
// This function reads the file to extract the lines that define a single
// bind value's details, and gets the value for that particular bind.
//============================================================= GETONEBINDVALUE
char *getOneBindValue()
{
    debugErr("getOneBindValue(): Entry.\n");
    debugErr("getOneBindValue(): Buffer='%s'\n", myBuffer);

    char *dtyToken;
    char *valueToken;
    int bytesRead;
    int bindType;
    int x;

    // Make sure we have the correct line in the file.
    // If the previous bind was type 25 or 29 then we are already at the
    // correct buffer line and there's no need to read it again. We only
    // need a single additional line from the trace file otherwise.
    //
    // Version 0.21.
    // If the previous bind was a type 96, and a long width too, then it's
    // possible that Oracle emitted more than one line of values in hex. We need to 
    // just read until we find a " Bind#" line. (AIX only so far ...)
    while (strncmp(myBuffer, " Bind#", 6) != 0) {
        debugErr("getOneBindValue(): Attempting to resynchronise...\n");
        bytesRead = getLine();
        if (bytesRead == -1) {
            debugErr("getOneBindValue(): END OF FILE detected early.");
            return NULL;
        }
    }

    debugErr("getOneBindValue(): myBuffer'%s'\n", myBuffer);

    if (strncmp(myBuffer, " Bind#", 6) != 0) {
        logErr("getOneBindValue() did not read the \"Bind#n\" line when expected.\n");
        debugErr("getOneBindValue(): 'Bind#'' not found: Exit\n");
        return NULL;
    }

    bytesRead = getLine();
    dtyToken = getFirstToken(myBuffer);
    
    // Need to see if a bind variable has been defined already and is used
    // more than once in the same statement. This is determined by reading
    // "   No oacdef for this bind." as the first line after the BIND# line.
    if (!strncmp(dtyToken, "No", 2)) {
        // Looks like a bind has been used more than once. Hmmm.
        debugErr("getOneBindValue(): Bind variable reused");
        return "\"__A_:BIND_REUSED__\"";
    }

    // Bind is not a reused one, carry on.
    if (strncmp(dtyToken, "oacdty=", 7) == 0) {
        bindType = atoi(dtyToken+7);
        debugErr("getOneBindValue(): Bind data type is %d.\n", bindType);
    } else {
        // Something is wrong.
        logErr("getOneBindValue(): Bind data type is UNKNOWN.\n");
        debugErr("getOneBindValue(): Unknown bind data type: Exit\n");
        return NULL;
    }

    // The rest (that I know of) are standard format.
    bytesRead = getLine();      // Ignored.
    bytesRead = getLine();      // We find NULLs here in token 3. 'avl=00'
    valueToken = getFirstToken(myBuffer);
    for (x = 0; x < 2; x++)
        valueToken = getNextToken();

    // Check for a NULL value or a RETURNING INTO bind.
    if (strncmp(valueToken, "avl=00", 6) == 0) {
        debugErr("getOneBindValue(): NULL or \"Returning into\" bind: Exit.\n");
        return "NULL";
    }

    // Get the "value=" line.
    bytesRead = getLine();
    
    //This doesn't work for strings with spaces in. 
    //valueToken = getFirstToken(myBuffer);
    
    // This is a fix for the above.
    valueToken=myBuffer;
    debugErr("getOneBindValue(): valueToken='%s'\n", valueToken);
    
    while (isspace(*valueToken))
        valueToken++;

    // This is a weird on, seen on Windows once so far.
    // The value= line has no value but the following BIND# line 
    // is included. This is exactly as extracted from a trace file:
    // "value= Bind#1". Spooky. And it messes things up!
    // Better we check, and barf.
    if (!strncmp(valueToken, "value= Bind#", 12)) {
        debugErr("getOneBindValue(): We have a corrupted trace file.\n");
        debugErr("\t\tDetected \"value= Bind#\" at line %d\n", lineNumber);
        debugErr("\t\tPlease fix the line by inserting a carriage Return after the '=' and before the space.\n");
        logOut("getOneBindValue(): We have a corrupted trace file.\n");
        logOut("\t\tDetected \"value= Bind#\" at line %d\n", lineNumber);
        logOut("\t\tPlease fix the line by inserting a carriage Return after the '=' and before the space.\n");
        
        return NULL;
    }
    
    debugErr("getOneBindValue(): About to extractValue() from '%s'\n", valueToken);
    char *thisValue = extractValue(bindType, valueToken);
    debugErr("getOneBindValue(): got value '%s' from '%s'\n", thisValue, valueToken);

    // Data types 25 and 29 are unhandled in the Trace File (it says so!)
    // We need to resync the buffer to the next bind which means reading
    // a lot of hex dump. The joy of hex eh?
    if (bindType == 25 || bindType == 29) {
        debugErr("getOneBindValue(): Type %d, unhandled bind type: Resync required...\n", bindType);

        // Resync up to the first line with a '[' in it.
        while (bytesRead != -1) {
            bytesRead = getLine();
            if (strchr(myBuffer, '[') )
                break;
        }

        bytesRead = getLine();  // Should be the "Dump" line before the hex dump line(s).
        while (bytesRead > 0 && strchr(myBuffer, '[' )) {
            bytesRead = getLine();
        }

        // Did we hit EOF during the resync?
        if (bytesRead == -1) {
            logErr("getOneBindValue(): FATAL: Data Type %d resync hit an ERROR or EOF.\n", bindType);
            return NULL;
        }
        debugErr("getOneBindValue(): Type %d, unhandled bind type: Resync complete.\n", bindType);
        debugErr("getOneBindValue(): Type %d, unhandled bind type: Exit.\n", bindType);
        return "':UNHANDLED Bind Type'";
    }

    debugErr("getOneBindValue(): Exit\n");
    return thisValue;
}


//============================================================ PRINTEXECDETAILS
// Print out an SQL statement with all the binds replaced
// with '%s' which just happens to be the format specifier for
// a string. We also have an array of strings holding the
// bind values for the specific SQL statement.
//
// This is mental! However the C standard says that when
// passing lots of args to a function any unused ones are
// ignored and are correctly tidied off the stack by the
// caller. It does exactly as it says on the tin!
//============================================================ PRINTEXECDETAILS
void printExecDetails(cursorNode *node)
{

    debugErr("printExecDetails(): Entry for node %p.\n", node);
    debugErr("printExecDetails(): Printing node %p, cursorID '%s' parsed at line %ld.\n", node, node->cursorId, node->lineNumber);

    // We need to build a format string for logOut, but we need some extra characters
    // at the front for line numbers etc. FormatAddition defines how many extras.
    int sqlSize = strlen(node->sqlText);
    const int formatAddition = 100;
    char formatText[sqlSize + formatAddition +1];
    extern int HTMLmode;

    if (HTMLmode == 0) {
        strcpy(formatText, "%9ld :%18.18s: %10ld : ");
        strcat(formatText, node->sqlText);
    } else {
        // Don't make the following bigger than "formatAddition", or else!
        strncpy(formatText, "<tr><td align=\"right\">%ld</td><td>%s</td><td align=\"right\">%ld</td><td>", formatAddition);
        formatText[formatAddition] = '\0';      // Strncat doesn't add the trailing '\0'
        strcat(formatText, node->sqlText);
        strcat(formatText, "</td></tr>");
    }

    // If config.h has MAXBINDS set to 'n', then there must be v->bv[0] through
    // v->bv[n-1] in the following. No more, no less.
    // By default this is 150, so we have bv[0] through bv[149] below.
    bindValues *v = &sqlBinds;
    logOut(formatText, lineNumber, node->cursorId, node->lineNumber,
           v->bv[0],  v->bv[1],  v->bv[2],  v->bv[3],  v->bv[4],
           v->bv[5],  v->bv[6],  v->bv[7],  v->bv[8],  v->bv[9],
           v->bv[10], v->bv[11], v->bv[12], v->bv[13], v->bv[14],
           v->bv[15], v->bv[16], v->bv[17], v->bv[18], v->bv[19],
           v->bv[20], v->bv[21], v->bv[22], v->bv[23], v->bv[24],
           v->bv[25], v->bv[26], v->bv[27], v->bv[28], v->bv[29],
           v->bv[30], v->bv[31], v->bv[32], v->bv[33], v->bv[34],
           v->bv[35], v->bv[36], v->bv[37], v->bv[38], v->bv[39],
           v->bv[40], v->bv[41], v->bv[42], v->bv[43], v->bv[44],
           v->bv[45], v->bv[46], v->bv[47], v->bv[48], v->bv[49],

           v->bv[50], v->bv[51], v->bv[52], v->bv[53], v->bv[54],
           v->bv[55], v->bv[56], v->bv[57], v->bv[58], v->bv[59],
           v->bv[60], v->bv[61], v->bv[62], v->bv[63], v->bv[64],
           v->bv[65], v->bv[66], v->bv[67], v->bv[68], v->bv[69],
           v->bv[70], v->bv[71], v->bv[72], v->bv[73], v->bv[74],
           v->bv[75], v->bv[76], v->bv[77], v->bv[78], v->bv[79],
           v->bv[80], v->bv[81], v->bv[82], v->bv[83], v->bv[84],
           v->bv[85], v->bv[86], v->bv[87], v->bv[88], v->bv[89],
           v->bv[90], v->bv[91], v->bv[92], v->bv[93], v->bv[94],
           v->bv[95], v->bv[96], v->bv[97], v->bv[98], v->bv[99],

           v->bv[100], v->bv[101], v->bv[102], v->bv[103], v->bv[104],
           v->bv[105], v->bv[106], v->bv[107], v->bv[108], v->bv[109],
           v->bv[110], v->bv[111], v->bv[112], v->bv[113], v->bv[114],
           v->bv[115], v->bv[116], v->bv[117], v->bv[118], v->bv[119],
           v->bv[120], v->bv[121], v->bv[122], v->bv[123], v->bv[124],
           v->bv[125], v->bv[126], v->bv[127], v->bv[128], v->bv[129],
           v->bv[130], v->bv[131], v->bv[132], v->bv[133], v->bv[134],
           v->bv[135], v->bv[136], v->bv[137], v->bv[138], v->bv[139],
           v->bv[140], v->bv[141], v->bv[142], v->bv[143], v->bv[144],
           v->bv[145], v->bv[146], v->bv[147], v->bv[148], v->bv[149]);
  
    debugErr("printExecDetails(): Exit for node %p.\n", node);
}
