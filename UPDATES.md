# Trace Miner Updates

### Version 0.21 - 12th January 2017.
  - Default for MAXBINDS changed from 50 to 150 to account for SQL with lots of binds. 50 was being hit too frequently for comfort.
  - Reminder added to the STDOUT file when we hit a limit. It was only in the STDERR file, and only then, when running in verbose mode.
  - Bugger it! When a cursor is CLOSEd version 0.20 deleted the node from the list. However, it seems that Oracle also re-parses the same SQL with a PARSE #CURSOR_ID, rather than a PARSING IN CURSOR action. At least, on AIX anyway. Sigh. This caused subsequent EXECs to barf as the cursor was not found.
  - Data type 96 can, under a few circumstances, emit more than a single line for the hex codes of the bind's value. Code now ignores the subsequent lines and only processes the first, as before, but now resynchronises with the trace file before processing the next bind's value.
  - segfault detected when in verbose mode, at the end of a huge trace file, during the listClear() call. Fixed. Caused by the conversion of a bind from ":a" to "%s" - or so it seems!

### Version 0.20 - 12th January 2017.
  - Intermittent bug fixed when parsing data for type 96 binds.
  - Now processes cursor CLOSE actions.

### Version 0.19 - 2nd December 2016.
  - Busy day today! The utility now notices exec ERROR lines as well as PARSE ERRORs.

### Version 0.18 - 2nd December 2016.
  - Interesting bug, seemingly related to DBMS_METADATA.GET_CLOB calls where the value for one bind is the bind number of the next one. The text is "value= Bind#" and is weird!

### Version 0.17 - 2nd December 2016.
  - Added test.cmd. Test harness for Windows users.
  - Sort of fixed the problem where a bind can be used more than once in a statement. Flagged in the SQL as "__A_:BIND_REUSED__" at the moment.

### Version 0.16 - 31st August 2016.
  - Now compiles on Windows with the latest free Borland C++ Compiler.
  - Minor bug fix. The sqlBinds array was defined in multiple places due to a silly (beginner!) C error in typing. Sigh. 

### Version 0.15 - 18th August 2016.
  - PL/SQL assignemnet statements, ':=', are NOT bind variables. Duh! Fixed this nasty bug.

### Version 0.14 - 21st July 2016.
  - Added checks for PARSE ERROR lines. Prints the error code and the failing SQL at _any_ dep=n.

### Version 0.13 - 19th July 2016.
  - Tidy up makefiles of (more) spurious stuff.
  - 32 bit Makefile added to Release folder.
  - Deleted the Debug folder. It was still QT dependent for some reason.
  - `MAXBINDS` and `MAXBINDSIZE` removed from `bindvalues.h` - duplicated from `config.h`.

### Version 0.12 - 10th March 2016.
  - [Issue #1](https://github.com/NormanDunbar/TraceMiner/issues/1): Spurious date/time stamps in trace file cause utility to barf! Fixed.
  - Output format changed to identify EXEC and PARSE lines.
  - HTML output also amended as above.

### Version 0.11 - 29th February 2016.
  - A couple of (internal) bug fixes.
  - HTML output added.
  - `OFFSETFORRICH` setting moved to `config.h` for ease of adjustment.

### Version 0.1 - 26th February 2016.
This was the first release. 
