/*! \file    util.c
 *  \brief   Вспомогательные функции
 */

#include "load_product.h"

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


/*!
 * \brief   Функция ошибки
 * \param   error_string  Разъяснения ошибки
 * \param   file          Файл, в котором произошла ошибка
 * \param   line          Строка, из которой была вызвана ошибка
 * \param   func          Функция, в которой была вызвана ошибка
 * \return
 */
void error(const char* error_string, const char* file, unsigned int line, const char* func)
{
  if (isopened)
    fclose(fp);

  FreeProductions();
#if (DEBUG > 0)
  printf( "Error in: file (%s) line(%u) function(%s)\r\n", file, line, func);
#endif
  fprintf(stderr, "Error message : %s;%s\n", error_string, strerror(errno));

  exit(EXIT_FAILURE);
}