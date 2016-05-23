#include "Database.h"
#include "Department.h"
#include "Format.h"
#include "State.h"
#include "Scale.h"
#include "SiteLang.h"
#include "User.h"
#include "Status.h"
#include "Session.h"
#include "Usertype.h"
#include "Log.h"
#include "GeodataType.h"
#include "Site.h"
#include "Geodata_record.h"
#include "Language.h"
#include "Region.h"
#include "SiteRegion.h"
#include "SiteType.h"
#include "DepartmentRegion.h"
#include "DepartmentType.h"
#include <QFileInfo>
/*!
\file
*/

const QString Database::pathToDb = QDir::currentPath() + QString("/database/geoDB"); // путь к базе

const QString Database::connectionName = "geoDb_connection";	// название подключения

int Database::CurrentSessionId = 0;

int Database::SmSessionId = 0;

int Database::SystemSessionId = 1;

																/*!
Создает подключение connectionName к базе.
Если необходимо, заново создает таблицы базы pathToDb 
*/
void Database::restore(int session_id)
{
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
	db.setDatabaseName(pathToDb);

	// Если файла pathToDb не существует, создать все таблицы:
	QFileInfo dbFile(pathToDb);
	if (!dbFile.exists()) {
		configure();
		Log::create(session_id, "Database: restore");
	}
}

void Database::setCurrentSessionId(int _session_id)
{
	CurrentSessionId = _session_id;
}

void Database::setSmSessionId(int smSessionId)
{
	SmSessionId = smSessionId;
}

void Database::setSystemSessionId(int systemSessionId)
{
	SystemSessionId = systemSessionId;
}

int Database::currentSessionId()
{
	return CurrentSessionId;
}

int Database::smSessionId()
{
	return SmSessionId;
}

int Database::systemSessionId()
{
	return SystemSessionId;
}


/*!
Возвращает открытое подключение к базе.
\return QSqlDatabase db - подключение к базе
*/
QSqlDatabase Database::database()
{
	if (!QSqlDatabase::contains (connectionName))
		qDebug() << "Database::database() error: connection " << connectionName << " not found";
	
	return QSqlDatabase::database(connectionName, true);
}



/*!
Создает в базе таблицы "Sites", "Formats", 
"State", "Scale", "User", "Status", "Session", "Usertype"
*/
void Database::configure()
{
		Usertype::createTable();
		Usertype::completeTable();
		User::createTable();
		User::completeTable();
		Log::createTable();
		Session::createTable();
		Session::completeTable();
		Language::createTable();
		Language::completeTable();
		Site::createTable();
		Site::completeTable();
		Format::createTable();
		Format::completeTable();
		State::createTable();
		State::completeTable();
		Scale::createTable();
		Scale::completeTable();
		Status::createTable();
		Status::completeTable();
		Geodata_record::createTable();
		// Geodata_record::completeTable();
		GeodataType::createTable();
		GeodataType::completeTable();
		Department::createTable();
		Department::completeTable();
		Region::createTable();
		Region::completeTable();
		createBridgeTables();
		addTriggers();
		
}

void Database::createBridgeTables()
{
	SiteRegion::createTable();
	SiteLang::createTable();
	SiteType::createTable();
	DepartmentRegion::createTable();
	DepartmentType::createTable();
}

void Database::addTriggers()
{
	/*onDeleteTrigger("sites", "site_langs");
	onDeleteTrigger("sites", "site_regions");
	onDeleteTrigger("sites", "site_types");
	onDeleteTrigger("departments", "department_regions");
	onDeleteTrigger("regions", "department_regions");
	onDeleteTrigger("departments", "department_types");*/
	regionsTrigger();	
	QSqlDatabase db = database();
	QSqlQuery query(db);
	QString queryString = "CREATE TRIGGER del_sitelangs\
		 BEFORE DELETE ON sites \
		 FOR EACH ROW BEGIN DELETE from site_langs \
		 WHERE site_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no del_sitelangs trigger:" << query.lastError().text();

	 queryString = "CREATE TRIGGER del_siteregions\
		 BEFORE DELETE ON sites \
		 FOR EACH ROW BEGIN DELETE from site_regions \
		 WHERE site_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no del_siteregions trigger:" << query.lastError().text();

	queryString = "CREATE TRIGGER del_sitetypes\
		 BEFORE DELETE ON sites \
		 FOR EACH ROW BEGIN DELETE from site_types \
		 WHERE site_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no del_sitetypes:" << query.lastError().text();

	queryString = "CREATE TRIGGER del_department_regions\
		 BEFORE DELETE ON departments \
		 FOR EACH ROW BEGIN DELETE from department_regions \
		 WHERE department_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no del_department_regions:" << query.lastError().text();


	queryString = "CREATE TRIGGER del_reg_dep\
		 BEFORE DELETE ON regions \
		 FOR EACH ROW BEGIN DELETE from department_regions \
		 WHERE region_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no ddel_reg_dep:" << query.lastError().text();


	queryString = "CREATE TRIGGER del_dep_types\
		 BEFORE DELETE ON departments \
		 FOR EACH ROW BEGIN DELETE from department_types \
		 WHERE department_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no ddel_dep_types:" << query.lastError().text();

	//qDebug() << queryString;
	db.close();
}

void Database::onDeleteTrigger(const QString& tableName, 
	const QString & dependentTableName)
{
	const QString column_name = tableName.mid(0, tableName.length() - 1) + "_id";
	QSqlDatabase db = database();
	QSqlQuery query(db);
	QString queryString = "CREATE TRIGGER del_" + tableName + "_after_" + dependentTableName
		+ " BEFORE DELETE ON " + tableName
		+ " FOR EACH ROW BEGIN DELETE from " + dependentTableName
		+ " WHERE "+ column_name + " = OLD.id; END;";
	if(!query.exec(queryString))
		qDebug() << "no del_" + tableName + "_after_" + dependentTableName+" trigger:" << query.lastError().text();
//	qDebug() << queryString;
	db.close();
}

void Database::regionsTrigger()
{
	QSqlDatabase db = database();
	QSqlQuery query(db);
	QString queryString = "CREATE TRIGGER del_regions_after_regions\
		 BEFORE DELETE ON regions \
		 FOR EACH ROW BEGIN DELETE from regions \
		 WHERE regions.parent_id = OLD.id; END;";
	if (!query.exec(queryString))
		qDebug() << "no del_regions_after_regions trigger:" << query.lastError().text();
//	qDebug() << queryString;
	db.close();
}
