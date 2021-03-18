
#ifndef cyk_h
#define cyk_h

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>

#define EXIT_SUCCESS                   0
#define EXIT_FAILURE                   1

#define all(x) (x[0] && x[1])
#define ln() printf("\n")

bool    CYK(bool* table, int str_len);
bool    InitTable(bool* table, int str_len, const char* string);
void    PrintTables(bool* table, int str_len, int m);

#endif // cyk_h