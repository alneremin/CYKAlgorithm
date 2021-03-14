/*! \file    main.c
 *  \brief   Алгоритм CYK
 *           для языка {aba(b^n) : n ≥ 0} U {ab(a^n) : n ≥ 0}
 */
#include "cyk.h"

#define assert__(x) for ( ; !(x) ; assert(x) )

/*!
 * \brief   Точка входа в программу
 * \return  EXIT_SUCCESS (0) при успешном завершении
 *          EXIT_FAILURE (1) при наличии ошибок
 */
int main(int argc, const char* argv[])
{

  assert__(argc == 4) {
    printf("Please enter a valid call\n");
    printf("for example: ./cyk-1 cfg-2-cnf.grammar ={a+1231}*b [a-f0-7=+*-{}]\n");
  }
  
  g_filename = argv[1];
  g_terminal_alphabet = argv[3];

  /* Создаем таблицу всех возможных нетерминалов (A-Z) */
  InitNonterminals();

  /* Читаем из файла продукции, параллельно индексируя нетерминалы, которые
     они (продукции) содержат. Считаем кол-во нетерминалов */
  InitProductions();

  //g_nonterminal_charcters = CountNonterminal();

  int len = strlen(argv[2]);
  bool occur_table[g_count_nonterminal][len][len];
  memset (occur_table, false, g_count_nonterminal*len*len);



  InitTable((bool*)occur_table, len, argv[2]);
  bool res = CYK((bool*)occur_table, len);

  if (res)  
  {
    printf("Accepted");
  }
  else
  {
    printf("Rejected");
  }
  printf("\n\n");

  return EXIT_SUCCESS;
}

