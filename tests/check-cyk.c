/*! \file    check-cyk.c
 *  \brief   Тесты для CYK
 */
#include <check.h>

#include "../cyk.h"
#include "../load_product.h"


START_TEST(cyktest_0)
{
  g_filename = "tests/test.grammar";
  g_rhs_alphabet = "[()]";

  const char* string = "()(())";


  InitNonterminals();
  InitProductions(true);

  int len = strlen(string);
  bool occur_table[g_count_nonterminal][len][len];
  memset (occur_table, false, g_count_nonterminal*len*len);


  InitTable((bool*)occur_table, len, string);
  bool res = CYK((bool*)occur_table, len);

  FreeProductions();
  
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
  tcase_add_test(tc1_1, cyktest_0);
  srunner_run_all(sr, CK_NORMAL);
  nf = srunner_ntests_failed(sr);
  srunner_free(sr);

  return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
