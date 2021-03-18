/*! \file    load_product.c
 *  \brief   Загрузка продукций из файла
 *           Для корректной работы необходим файл грамматики нужного формата
 */
#include "load_product.h"

product**       g_productions = NULL; // массив продукций
FILE*           fp  = NULL; // Файловый дескриптор
const char*     g_filename; // имя файла с продукциями (задается пользователем)
size_t          g_count_lines = 0; // кол-во продукций (высчитывается)
bool            isopened = false; // флаг открытия файла
const char*     g_rhs_alphabet; // терминальный алфавит (задается пользователем)
size_t          g_default_nonterminal = 0; // терминал по умолчанию (g_productions[0]->lhs)
size_t          g_count_nonterminal = 0; // кол-во нетерминалов (высчитывается)

/*!
 * \brief   Инициализация продукций
 * \param   use_cnf Использовать НФХ
 * \return  true при успешном завершении
 */
bool InitProductions(bool use_cnf)
{

  g_count_lines = CountLines(g_filename);
  if (!(g_productions = (product**) calloc(g_count_lines, sizeof (product*)))) 
  {
    error("memory allocation error", __FILE__, __LINE__, __func__);
  }

  LoadProducts(use_cnf);

  return true;
}

/*!
 * \brief   Загрузка продукций из файла
 * \param   use_cnf Использовать НФХ
 * \return  
 */
void LoadProducts(bool use_cnf)
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
  sprintf(format, "%s%s%s" ,"%c->%", g_rhs_alphabet, "%c");

  while (!feof(fp) && row_number < g_count_lines)
  {
    if (use_cnf)
        CNF(row_number, format);
    else
        AnyForm(row_number, format);
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
 * \brief   Определение соответствия продукции формату X -> a
 *          X - нетерминал; a - любое выражение
 *          (кроме A -> %epsilon, где %epsilon - пустая строка)
 * \param   row_number Номер продукции в файле
 * \param   format     Необходимый формат продукции в НФХ
 * \return  true при успешном завершении
 */
bool AnyForm(size_t row_number, char* format)
{
  char buf[BUFFER_LENGTH];

  memset(buf, '\0', BUFFER_LENGTH);

  char last_sym;
  int pos;

  // запоминаем текущее положение каретки в файле
  pos = ftell(fp);
  if (pos == -1)
  {
    error("Ошибка при вызове ftell(fp)", __FILE__, __LINE__, __func__);
  }

  // пробуем считать строку заданного формата и записать данные в перем-ные
  fscanf(fp, format, &buf[0], (buf+1), &last_sym);

  /* если в LHS у нас символ из [A-Z],  
     в RHS ровно 2 нетерминальных символа и после них 
     идет Enter иначе говоря продукция имеет вид X->YZ,
     добавляем нашу продукцию в массив продукций */
  if (isupper(buf[0]) && strlen(buf+1) > 0 && isenter(last_sym))
  {
    AddProduct(strlen(buf+1), buf, row_number);
  }
  else
  {
    error("invalid production", __FILE__, __LINE__, __func__);
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

void FreeProductions() 
{
  for (size_t i = 0; i < g_count_lines && g_productions; ++i)
  {
    free(g_productions[i]); 
  }
  free(g_productions);
}


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