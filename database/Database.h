#pragma once
/*!
\file
*/

#include <QString>
#include <QSqlDatabase>
#include <QDir>

/*! 
 *  \brief     Класс для работы с БД
 *  \author    Ильина А.
 *  \date     март 2016
 */
 
class Database {
private:
	static const QString connectionName;	// название подключения
	static const QString pathToDb;			// путь к файлу с базой
	static int CurrentSessionId;		// id текущей пользовательской сессии
	static int SmSessionId;				// id текущей сессии модуля поиска
	static int SystemSessionId;			// id текущей сессии Системы

	/*!	Создает в базе таблицы "Sites", "Formats",
	"State", "Scale", "User", "Status", "Session", "Usertype" */
	static void configure();

	/*! Создает в базе вспомогательные таблицы site_regions, site_langs,
	site_types*/
	static void createBridgeTables();

	static void addTriggers();

	static void onDeleteTrigger(const QString& table, 
		const QString& dependentTable);
	
	static void regionsTrigger();

public:
	/*!	Возвращает открытое подключение к базе.
	\return QSqlDatabase db - подключение к базе */
	static QSqlDatabase database();
	
	/*! Создает подключение connectionName к базе. 
	Если необходимо, заново создает таблицы базы pathToDb */
	static void restore(int session_id = Database::SystemSessionId);

	/*!
	Устнонавливает id текущей пользовательской сессии
	\param int _session_id - id сессии */
	static void setCurrentSessionId(int _session_id);

	/*!
	Устнонавливает id текущей сессии поиска
	\param int smSessionId - id сессии */
	static void setSmSessionId(int smSessionId);

	/*!
	Устнонавливает id текущей сессии Системы
	\param int systemSessionId - id сессии */
	static void setSystemSessionId(int systemSessionId);
	
	/*!
	Возвращает id текущей пользовательскоой сессии
	\return int sessionId*/
	static int currentSessionId();
	
	/*!
	Возвращает id текущей сессии модуля поиска
	\return int sessionId*/
	static int smSessionId();

	/*!
	Возвращает id текущей сессии Системы
	\return int sessionId*/
	static int systemSessionId();
};