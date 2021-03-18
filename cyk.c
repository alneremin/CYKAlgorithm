/*! \file    cyk.c
 *  \brief   Алгоритм Кока-Янгера-Касами
 *           Для корректной работы необходим файл грамматики нужного формата
 *           и строка, принадлежность которой языку и определяется в 
 *           результате работы алгоритма
 */
#include "cyk.h"
#include "load_product.h"

/*!
 * \brief   Инициализация таблицы CYK
 * \param   table       Указатель на массив булевых значений
 * \param   str_len     Длина строки (принадлежность которой языку необходимо выяснить)
 * \param   string      Строка для обработки
 * \return  true при успешном завершении
 */
bool InitTable(bool* table, int str_len, const char* string)
{

  int nonterm_enum;
  for (size_t i = 0; i < str_len; ++i) // для каждого символа строки
  {
    for (size_t j = 0; j < g_count_lines; ++j) // для каждой продукции
    {
      // если символ строки равен терминалу в RHS
      if (string[i] == g_productions[j]->rhs[0])
      {
        // узнаем индекс нетерминала
        nonterm_enum = str2enum(g_productions[j]->lhs);
        if (nonterm_enum == UNKNOWN_NONTERMINAL_CHAR_ERR)
          error("Неизвестный нетерминал", __FILE__, __LINE__, __func__);

        *(table + nonterm_enum * str_len * str_len + str_len * i + i) = true;
      }

    }
  }
  return true;
}

/*!
 * \brief   Алгоритм CYK
 * \param   table       Указатель на массив булевых значений
 * \param   str_len     Длина строки (принадлежность которой языку необходимо выяснить)
 * \return  true при успешном завершении
 */
bool CYK(bool* table, int str_len)
{
  int     nonterm_enums[TWO_NONTERMINAL];
  bool    states[TWO_NONTERMINAL];
  size_t  lhs;

  for (size_t m = 1; m < str_len; ++m) // m - размер подстроки
  {
    for (size_t i = 0; i < (str_len - m); ++i) // i - начало интервала
    {
      for (size_t k = 0; k < m; ++k) // k - индекс разбиения
      {
        for (size_t j = 0; j < g_count_lines; ++j) // j - индекс продукции
        {
          if (strlen(g_productions[j]->rhs) == TWO_NONTERMINAL)
          {
            for (size_t l = 0; l < TWO_NONTERMINAL; ++l)
            {
              nonterm_enums[l] = str2enum(g_productions[j]->rhs[l]);
              if (nonterm_enums[l] == UNKNOWN_NONTERMINAL_CHAR_ERR)
                error("Неизвестный нетерминал", __FILE__, __LINE__, __func__);
            }
            lhs = str2enum(g_productions[j]->lhs);
            if (lhs == UNKNOWN_NONTERMINAL_CHAR_ERR)
              error("Неизвестный нетерминал", __FILE__, __LINE__, __func__);
            /* table - это указатель на массив
               nonterm_enums[0] - это номер нетерминального символа, с помощью 
               nonterm_enums[0] * str_len * str_len мы перемещаем указатель на булеву таблицу
               нетерминала nonterm_enums[0]
               i - это номер строки в булевой таблице (начало интервала), с помощью 
               str_len * i мы перемещаем указатель на нужную строку в таблице
               (i + k) - это номер столбца в булевой таблице (часть интервала до индекса разбиения) */
            states[0] = *(table + nonterm_enums[0] * str_len * str_len + str_len * i + (i + k));

            /* (i + k + 1) - это номер строки в булевой таблице nonterm_enums[1] (индекс разбиения интервала)
               с помощью str_len * (i + k + 1) мы перемещаем указатель на нужную строку в таблице  
               (i + m) - это номер столбца в булевой таблице (конец интервала) */
            states[1] = *(table + nonterm_enums[1] * str_len * str_len + str_len * (i + k + 1) + (i + m));

            // если все значения таблиц true
            if (all(states))
            {
              /* Обращаемся к таблице нетерминала lhs и записываем значение true: 
                 номер строки = начало интервала; номер столбца = конец интервала */
              *(table + lhs * str_len * str_len + str_len * i + (i + m)) = true;
            }
            //table[nonterm_enum][i][i] = true;
          }
        }
      }
    }
  }

  // если с помощью начального символа прочитана вся строка 
  if (*(table + g_default_nonterminal * str_len * str_len + str_len * 0 + (str_len - 1)))
  {
    return true;
  }
  else
  {
    return false;
  }
  
}

/*!
 * \brief   Печать булевых таблиц при определенном значении константы m
 * \param   table       Указатель на массив булевых значений
 * \param   str_len     Длина строки (принадлежность которой языку необходимо выяснить)
 * \param   m           Константа (длина рассматриваемых подстрок)
 * \return
 */
void PrintTables(bool* table, int str_len, int m)
{
  bool state;

  printf("Размер подстроки = %d\n",  m);
  for (size_t i = 0; i < str_len; ++i) 
  {
    for (size_t k = 0; k < g_count_nonterminal; ++k) 
    {
      for (size_t j = 0; j < str_len; ++j) 
      {
        state = *(table + k * str_len * str_len + str_len * i + j);
        printf("%d ", state);
      }
      printf("\t");
    }
    ln();
  }
}
