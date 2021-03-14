/*! \file    check-PDA.c
 *  \brief   Тесты для НКА
 */
#include <check.h>

#include "../cyk.h"


START_TEST(pdatest_0)
{
  g_filename = "tests/test.grammar";
  g_terminal_alphabet = "[()]";

  const char* string = "()(())";


  InitNonterminals();
  InitProductions();

  int len = strlen(string);
  bool occur_table[g_count_nonterminal][len][len];
  memset (occur_table, false, g_count_nonterminal*len*len);


  InitTable((bool*)occur_table, len, string);
  bool res = CYK((bool*)occur_table, len);

  fail_unless(res, "should be equals true");
}
END_TEST


int main(void)
{

  Suite *s1 = suite_create("Core");
  TCase *tc1_1 = tcase_create("CYK");
  SRunner *sr = srunner_create(s1);
  int nf;

  suite_add_tcase(s1, tc1_1);
  tcase_add_test(tc1_1, pdatest_0);
  srunner_run_all(sr, CK_NORMAL);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
