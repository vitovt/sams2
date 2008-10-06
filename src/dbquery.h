/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef DBQUERY_H
#define DBQUERY_H

using namespace std;

#include <string>
#include <sys/types.h>

class DBConn;

/**
 * @brief Базовый класс для различных способов запросов к БД
 */
class DBQuery
{
  friend class DBConn;

public:

  /*
   * @brief Тип переменной
   */ 
  enum VarType
  {
    T_LONG,        ///< long (tinyint, smallint, integer)
    T_LONGLONG,    ///< long long (bigint)
    T_CHAR,        ///< char (char, varchar)
  };

  /*
   * @brief Преобразование типа переменной в строку
   * @param t Тип переменной
   * @return Тип переменной в виде строке
   */
  static string toString (VarType t);

  /*
   * @brief Деструктор
   */
  virtual ~ DBQuery ();

  /**
   * @brief Выполняет прямой запрос @a query
   *
   * Метод должен быть переопределен наследником.
   * Прямое выполнение представляет собой самый простой способ выполнить инструкцию.
   * Например, следующий код формирует инструкции SQL и выполняет их один раз:
   * @code
   * query->sendQueryDirect("CREATE TABLE my_test(id int, name text)");
   * query->sendQueryDirect("INSERT INTO my_test VALUES(20, 'hello')");
   * @endcode
   *
   * @param query SQL запрос.
   * @retval true Запрос выполнен успешно.
   * @retval false Произошла ошибка при выполнении запроса.
   */
  virtual bool sendQueryDirect (const string & query);

  /**
   * @brief Привязывает буферы данных прикладных программ к столбцам в наборе результатов
   *
   * Например, следующий код используется для печати значений таблицы:
   * @code
   * long num;
   * char desc[50];
   * query->bindCol( 1, DBQuery::T_LONG, &num, 0);
   * query->bindCol( 2, DBQuery::T_CHAR, desc, sizeof(desc));
   * query->sendQueryDirect("SELECT id, name from my_test");
   * while (query->fetch())
   *   printf("id=%ld, name=%s\n", num, desc);
   * @endcode
   *
   * @param colNum Номер столбца, упорядоченный последовательно
   *        в увеличивающемся порядке, начинающемся с 1.
   * @param dstType Тип данных для C параметра.
   * @param buf Указатель на буфер для данных столбца.
   * @param bufLen Длина буфера @a buf в байтах.
   * @retval true При успешном добавлении столбца.
   * @retval false При возникновении ошибки.
   * @sa sendQuery(), fetch()
   */
  virtual bool bindCol (uint colNum, VarType dstType, void *buf, int bufLen);

  /**
   * @brief Привязывает буферы данных прикладных программ к маркерам в SQL запросе
   *
   */
  virtual bool bindParam (uint num, VarType dstType, void *buf, int bufLen);

  /**
   * @brief Готовит команду SQL к выполнению
   *
   * @param query SQL запрос.
   * @retval true Подготовка выполнена успешно.
   * @retval false Произошла ошибка при подготовке запроса.
   * @sa bindParam(), sendQuery()
   */
  virtual bool prepareQuery (const string & query);

  /**
   * @brief Выполняет подготовленную инструкцию
   *
   * Использует текущие (актуальные) значения переменных маркера параметра,
   * если маркеры параметров существуют в инструкции.
   *
   * Этот пример объясняет, как прикладная программа может использовать
   * подготовленное выполнение. Выборка готовит инструкцию INSERT
   * и вставляет 100 строк данных, заменяя буферные значения.
   * @code
   * long id;
   * char name[30];
   * query->prepareQuery("INSERT INTO EMP(ID, NAME) VALUES(?, ?)");
   * query->bindParam(1, DBQuery::T_LONG, &id,  0);
   * query->bindParam(2, DBQuery::T_CHAR, name, sizeof(name));
   * for (id=1; id<=100; id++)
   * {
   *   sprintf(name, "id is %d", id);
   *   query->sendQuery();
   * }
   * @endcode
   *
   * @retval true Запрос выполнен успешно.
   * @retval false Произошла ошибка при выполнении запроса.
   */
  virtual bool sendQuery ();

  /**
   *
   * Выбирает следующую строку данных из набора результатов
   * и возвращает данные для всех связанных столбцов
   *
   * @retval true Если следующая строка из выборки успешно получена
   * @retval false Если в наборе результатов не осталось строк с данными или произошла ошибка
   * @sa sendQuery(), sendQueryDirect()
   */
  virtual bool fetch ();

protected:
  /**
   * @brief Конструктор
   */
  DBQuery ();

  /**
   * @brief Освобождает все ресурсы
   */
  virtual void destroy ();
};

#endif