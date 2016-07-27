#ifndef __DataAccess_h__
#define __DataAccess_h__

#include <QObject>
#include <QString>
#include <QByteArray>

#include "DataList.h"

class DataAccess
{
public:
	static void init();

	static void getDatapadSettings(QString& host, QString& email, QString& current_email,
		QString& password, QString& current_password, int& locked, int& hide_coded_talents);
	static void setServerAccessInfo(const QString& host, const QString& email, const QString& current_email,
		const QString& password, const QString& current_password);
	static void setLocked(int val);
	static void setHideCodedTalents(int val);

	static QString selectedDataSet(QString data_set = QString());
	static void setSelectedDataSet(const QString& val);
	static QString selectedCharacter();
	static void setSelectedCharacter(const QString& val);

	// High level functions:
	static QByteArray getData(const QString& dset, const QString& dir, const QString& key, const QString& value);
	static QByteArray getData(const QString& dset, const QString& file);
	static QByteArray getData(const QString& file);
	static QByteArray getCharacter(const QString& file);
	static QByteArray getCurrentData(const QString& file);
	static void addCharacter(const QString& file, const QByteArray& data);
	static void setCharacterRow(int row, const QString& file, const QByteArray& data);
	static void addBlankCharacter();
	static void addDataSet(const QString& file, const QString& name);
	static void setDataSetRow(int row, const QString& file, const QString& name);
	static void setCurrentDataRow(int row, const QString& file);
	static QString getSystemDataLastUpdate();

	// Read and write files:
	static QByteArray readFile(const QString& file, QString* time = 0);
	static void writeFile(const QString& file, const QByteArray& data);
	static void copyFile(const QString& src, const QString& dest);
	static void deleteFile(const QString& file);
	static bool removeDir(const QString & dirName);
	static int setFileModificationTime(const QString& file, uint modt);
	static bool makeDir(const QString& dir_name, bool is_file);

	// Return value of tag in file data
	static QString getTagValue(const QByteArray& data, const QString& tag);

	// Directory listings:
	static QByteArray findData(const QString& dir, const QString& tag, const QString& value);
	static QStringList listDirectory(const QString& dir);

	static DataList	characters;
	static DataList	dataSets;
	static DataList	currentData;
};

#endif // __DataAccess_h__
