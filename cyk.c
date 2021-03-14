/*! \file    cyk.c
 *  \brief   Алгоритм Кока-Янгера-Касами
 *           Для корректной работы необходим файл грамматики нужного формата
 *           и строка, принадлежность которой языку и определяется в 
 *           результате работы алгоритма
 */
#include "cyk.h"


product**       g_productions = NULL; // массив продукций
FILE*           fp  = NULL; // Файловый дескриптор
const char*     g_filename; // имя файла с продукциями (задается пользователем)
size_t          g_count_lines = 0; // кол-во продукций (высчитывается)
bool            isopened = false; // флаг открытия файла
const char*     g_terminal_alphabet; // терминальный алфавит (задается пользователем)
size_t          g_default_nonterminal = 0; // терминал по умолчанию (g_productions[0]->lhs)
size_t          g_count_nonterminal = 0; // кол-во нетерминалов (высчитывается)


/*!
 * \brief   Инициализация продукций
 * \return  true при успешном завершении
 */
bool InitProductions()
{

  g_count_lines = CountLines(g_filename);
  if (!(g_productions = (product**) calloc(g_count_lines, sizeof (product*)))) 
  {
    error("memory allocation error", __FILE__, __LINE__, __func__);
  }

  LoadProducts();

  return true;
}

/*!
 * \brief   Загрузка продукций из файла
 * \return  
 */
void LoadProducts()
{

  size_t row_number = 0;

  char format[FORMAT_LENGTH];
  memset(format, '\0', BUFFER_LENGTH);

  if ((fp = fopen(g_filename, "r")) == NULL) 
  {
    error("read file error", __FILE__, __LINE__, __func__);
  }
  
  isopened = true;
  //rewind(fp);

  // определяем наш формат терминальных данных для чтения из файла
  sprintf(format, "%s%s%s" ,"%c->%", g_terminal_alphabet, "%c");

  while (!feof(fp) && row_number < g_count_lines)
  {
    CNF(row_number, format);
    ++row_number;
  }
  
  fclose(fp);
  isopened = false;
}

/*!
 * \brief   Определение соответствия продукция нормальной форме Хомского
 * \param   row_number Номер продукции в файле
 * \param   format     Необходимый формат продукции в НФХ
 * \return  true при успешном завершении
 */
bool CNF(size_t row_number, char* format)
{
  char buf[BUFFER_LENGTH];

  memset(buf, '\0', BUFFER_LENGTH);

  char last_sym;
  int pos, cur_pos;

  // запоминаем текущее положение каретки в файле
  pos = ftell(fp);
  if (pos == -1)
  {
    error("Ошибка при вызове ftell(fp)", __FILE__, __LINE__, __func__);
  }

  // пробуем считать строку заданного формата и записать данные в перем-ные
  fscanf(fp, "%c->%[A-Z]%c", &buf[0], (buf+1), &last_sym);

  /* если в LHS у нас символ из [A-Z],  
     в RHS ровно 2 нетерминальных символа и после них 
     идет Enter иначе говоря продукция имеет вид X->YZ,
     добавляем нашу продукцию в массив продукций */
  if (isupper(buf[0]) && strlen(buf+1) == TWO_NONTERMINAL && isenter(last_sym))
  {
    AddProduct(TWO_NONTERMINAL, buf, row_number);
  }
  else
  {
    // вновь запоминаем текущее положение каретки
    cur_pos = ftell(fp);
    if (cur_pos == -1)
      error("Ошибка при вызове ftell (maybe invalid production)", __FILE__, __LINE__, __func__);

    /* перемещаемся назад по файлу на (pos - cur_pos) позиций,
       то есть на позицию до первого считывания fscanf */
    if (fseek(fp, pos - cur_pos, SEEK_CUR))
      error("Ошибка при вызове fseek (maybe invalid production)", __FILE__, __LINE__, __func__);

    fscanf(fp, format, &buf[0], (buf+1), &last_sym);
    /* если в LHS у нас символ из [A-Z],  
       в RHS ровно 1 терминальный символ и после него 
       идет Enter иначе говоря продукция имеет вид X->y,
       добавляем нашу продукцию в массив продукций */
    if (isupper(buf[0]) && strlen(buf+1) == ONE_TERMINAL && isenter(last_sym))
    {
      AddProduct(ONE_TERMINAL, buf, row_number);
    }
    else
    {
      error("invalid production", __FILE__, __LINE__, __func__);
    }
  }

  return true;
}

/*!
 * \brief   Добавление продукции в массив g_productions
 * \param   rhs_size    Длина правой части продукции (1 или 2)
 * \param   buf         Буфер для хранения lhs и rhs
 * \param   row_number  Номер продукции в файле (соответственно в массиве g_productions)
 * \return  true при успешном завершении
 */
bool AddProduct(size_t rhs_size, char* buf, size_t row_number)
{
  if (!(g_productions[row_number] = (product*) malloc(sizeof (product) + sizeof (char) * rhs_size))) 
  {
    error("memory allocation error", __FILE__, __LINE__, __func__);
  }

  /* стуктура product в памяти представляет из себя последовательность 
     символов типа char, поэтому мы можем скопировать буфер в нашу структуру */
  strcpy((char*)(g_productions[row_number]), buf);

  /* Пометим нетерминальный символ как встречающийся, чтобы затем посчитать
     общее кол-во нетерминалов */
  SeenNonterminal(g_productions[row_number]->lhs);
  return true;
}

void FreePointers() 
{
  for (size_t i = 0; i < g_count_lines && g_productions; ++i)
  {
    free(g_productions[i]); 
  }
  free(g_productions);
}


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
 * \brief   Печать булевых таблиц при определнном значении константы m
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

  FreePointers();
#if (DEBUG > 0)
  printf( "Error in: file (%s) line(%u) function(%s)\r\n", file, line, func);
#endif
  fprintf(stderr, "Error message : %s;%s\n", error_string, strerror(errno));

  exit(EXIT_FAILURE);
}