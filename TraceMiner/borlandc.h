#ifndef __BORLANDC_H__
#define __BORLANDC_H__

// A header file for the Borland C+= V10.1 aka Embarcadero Compiler.
// This must be #included before utils.h.

#include <stdio.h>

#if defined(__BORLANDC__)

#define strcasecmp(s1,s2)   stricmp(s1,s2)
int getline (char **lineptr, size_t *n, FILE *stream);

#endif

#endif