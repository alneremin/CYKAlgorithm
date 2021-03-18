
#ifndef load_product_h
#define load_product_h

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

#define BUFFER_LENGTH                  15
#define FORMAT_LENGTH                  30
#define LETTER_COUNT                   26

#define FIRST_LETTER_CHAR              'A'


#define UNKNOWN_NONTERMINAL_CHAR_ERR  -1

#define ONE_TERMINAL                   1
#define TWO_NONTERMINAL                2

#define EXIT_SUCCESS                   0
#define EXIT_FAILURE                   1

#define isenter(x) x == '\n'

struct 
{
    char  sym;
    int   index;
} conversion[LETTER_COUNT];

struct production 
{
    char lhs;
    char rhs[];
};

typedef struct production product; 

bool    InitProductions(bool use_cnf);
void    LoadProducts(bool use_cnf);
bool    CNF(size_t row_number, char* format);
bool    AnyForm(size_t row_number, char* format);
bool    AddProduct(size_t rhs_size, char* buf, size_t row_number);
void    FreeProductions();

size_t  str2enum (char str);
void    SeenNonterminal(char sym);
void    InitNonterminals();

size_t  CountLines(const char* filename);
void    error(const char* error_string, const char* file, unsigned int line, const char* func);

extern product**       g_productions;
extern const char*     g_filename;
extern FILE*           fp;
extern size_t          g_count_lines;
extern bool            isopened;
extern const char*     g_rhs_alphabet;
extern size_t          g_default_nonterminal;
extern size_t          g_count_nonterminal;

#endif // load_product_h