/*! \file    util.c
 *  \brief   Вспомогательные функции
 */

#include "cyk.h"


/*
nonterm str2enum(char sym)
{
  int j;
  for (j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
    //if (!strcmp (str, conversion[j].str))
    if (conversion[j].str == sym)
        return conversion[j].val;    
  return UNKNOWN_NONTERMINAL_CHAR_ERR;
}*/

/*!
 * \brief   Получение индекса нетерминала
 * \param   sym        Нетерминал
 * \return  index      Индекс нетерминала в таблице conversion
 */
size_t str2enum(char sym)
{
  int j;
  for (j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
    if (conversion[j].sym == sym)
        return conversion[j].index;    
  return UNKNOWN_NONTERMINAL_CHAR_ERR;
}

/*!
 * \brief   Определение индекса нетерминала
 * \param   sym        Нетерминал
 * \return
 */
void SeenNonterminal(char sym)
{
  for (size_t j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
  {
    if (conversion[j].sym == sym)
    {
      // если нетерминал еще не всречался
      if (conversion[j].index == -1)
      {
        // назначаем нетерминалу индекс
        conversion[j].index = g_count_nonterminal;
        // увеличиваем счетчик кол-ва нетерминалов
        ++g_count_nonterminal;
      }
      break;
    }
  }
}

/*!
 * \brief     Инициализация списка нетерминалов 
 * \details   Добавление в список conversion всех возможных 
              нетерминалов (A-Z) с изначальными индексами -1
 * \return
 */
void InitNonterminals()
{
  for (size_t j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
  {
    conversion[j].sym = (char)(FIRST_LETTER_CHAR + j);
    conversion[j].index = UNKNOWN_NONTERMINAL_CHAR_ERR;
  }
}

/*size_t CountNonterminal()
{
  size_t count = 0;

  for (size_t j = 0;  j < sizeof (conversion) / sizeof (conversion[0]);  ++j)
  {
    if (conversion[j].be_found)
    {
        ++count;
    }
  }
  return count;
}*/

/*!
 * \brief     Получение кол-ва строк в файле
 * \param     filename  Имя файла
 * \return    кол-во строк
 */
size_t CountLines(const char* filename) {

  size_t cnt = 0u;

  if((fp = fopen(filename, "r")) == NULL)
  {
    error("Не удалось прочитать файл", __FILE__, __LINE__, __func__);
    return 0u;
  }
         

  while(! feof(fp)) {
    fscanf(fp, "%*[^\n]\n");
    cnt++;
  }
  fclose(fp);
  return cnt;
}