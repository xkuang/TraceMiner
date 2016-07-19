#ifndef __BINDVALUES_H__
#define __BINDVALUES_H__

// Results buffer for our (default 50) bind values.
typedef struct bindValues {
    char bv[MAXBINDS][MAXBINDSIZE+1];
} bindValues;

// This is where the results go, hopefully.
bindValues sqlBinds;


#endif
