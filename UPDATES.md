# Trace Miner Updates

### Version 0.18 - 2nd December 2016.
  - Interesting bug, seemingly related to DBMS_METADATA.GET_CLOB calls where the value for one bind is the bund number of the next one. The text is "value= Bind#" and is weird!


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
