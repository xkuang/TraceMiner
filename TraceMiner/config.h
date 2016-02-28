#ifndef __CONFIG_H__
#define __CONFIG_H__

// The maximum number of bind variables in a cursor. The default
// is 50 binds in any one statement. If the program detects any SQL
// with more than 50, it will abort with a helpful message telling
// you what to do.
#define MAXBINDS 50

// The default size for the maximum number of characters in any one bind
// value. The default is 50. This won't stop the program but any bind values
// longer than this setting will be truncated.
#define MAXBINDSIZE 50

// The default size for a cursor id, including the #. I have old and new trace files
// and the newer ones have 15 digits after the #, so 20 is the default. If the program
// detects cursor ids with more than this, it will abort with a helpful message telling
// you what to do.
#define MAXCURSORSIZE 20
#endif
