#include <sys/types.h>
#include <utime.h>
#include <sys/time.h>
#include <errno.h>

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QPointer>
#include <QSettings>
#include <QDebug>

#include "md5.h"

#include "DataAccess.h"
#include "DatUtil.h"
#include "DatXML.h"
#include "CharacterXML.h"

DataList DataAccess::characters(QStringList() << "name" << "file" << "player" << "portrait" << "time" << "modt");
DataList DataAccess::dataSets(QStringList() << "name" << "file" << "time" << "modt");
DataList DataAccess::currentData(QStringList() << "name" << "file" << "time" << "modt");

// FindTag -----------------------------------------

QString FindTag::search(const QByteArray& data, const QString& search_path)
{
	iSearchPath = search_path.toUtf8().constData();
	readFromBuffer(data.constData(), data.length());
	return iResultValue;
}

bool FindTag::xmlElement(const DatStringBuffer& path, const char* value) {
	if (path.endsWith(iSearchPath)) {
		iResultValue = value;
		return false;
	}
	return true;
}

// DataAccess -----------------------------------------

void DataAccess::init()
{
	QString		char_dir = DatUtil::getCharacterFolder();
	QStringList dir;
	QByteArray	data;
	int			row;
	QDateTime	dt;

	dir = listDirectory(char_dir);
	//characters.startChanges();
	foreach (QString file, dir) {
		row = characters.appendRow();
		data = DataAccess::readFile(char_dir + file);
		setCharacterRow(row, file, data);
	}
	// Add one empty:
	//int rows = characters.rowCount() + (characters.rowCount() + 1) % 2 + 1;
	//for (int i=characters.rowCount(); i<rows; i++)
	//	addBlankCharacter();
	characters.rowCountChanged();
	if (Character::instance)
		Character::instance->emitCharacterCountChanged();

	dir = listDirectory(DatUtil::getDataSetFolder());
	//dataSets.startChanges();
	foreach (QString file, dir) {
		QStringList names = listDirectory(DatUtil::getDataSetFolder() + file);
		if (names.size() > 0) {
			QString name = names[0];
			row = dataSets.appendRow();
			setDataSetRow(row, file, name);
		}
	}
	dataSets.rowCountChanged();

	dir = listDirectory(DatUtil::getCurrentFolder());
	//currentData.startChanges();
	foreach (QString file, dir) {
		if (file.endsWith(".tri")) {
			QString new_file = DatUtil::left(file, ".tri") + ".xpad";
			QString to_file = DatUtil::getCurrentFolder() + new_file;
			QFile().rename(DatUtil::getCurrentFolder() + file, to_file);

			utimbuf times;
			times.actime = time(NULL);
			times.modtime = time(NULL);
			if (utime(to_file.toUtf8().constData(), &times) == -1)
				qDebug() << "utime failed" << errno;
			file = new_file;
		}
		row = currentData.appendRow();
		setCurrentDataRow(row, file);
	}
	currentData.rowCountChanged();

	//user = "paul@snap.de";
	//password = "1234";
	//host = "192.168.178.155:8080";

	// Hash the password:
}

void DataAccess::serverAccessInfo(QString& host, QString& email, QString& current_email,
	QString& password, QString& current_password)
{
	QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);

	host = settings.value("host").toString();
	email = settings.value("email").toString();
	current_email = settings.value("current-email").toString();
	password = settings.value("password").toString();
	current_password = settings.value("current-password").toString();
	if (current_email.isEmpty())
		current_password.clear();
}

void DataAccess::setServerAccessInfo(const QString& host, const QString& email, const QString& current_email,
	const QString& password, const QString& current_password)
{
	QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);

	DataAccess::makeDir(DatUtil::getApplicationData() , false);
	settings.setValue("host", host);
	settings.setValue("email", email);
	settings.setValue("current-email", current_email);
	settings.setValue("password", password);
	settings.setValue("current-password", current_password);
	settings.sync();

	QDir dir(DatUtil::getApplicationData());
	QStringList files = dir.entryList();
}

QString DataAccess::selectedDataSet(QString data_set)
{
	if (data_set.isEmpty()) {
		// Load current data set, and character
		QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);
		data_set = settings.value("data-set").toString();
	}
	if (!data_set.isEmpty()) {
		int row = dataSets.findRow("file", DatUtil::left(data_set, "/"));
		if (row < 0)
			data_set.clear();
		else {
			if (dataSets.getValueAsString(row, "name") !=  DatUtil::right(data_set, "/"))
				data_set.clear();
		}
	}
	if (data_set.isEmpty() && dataSets.rowCount() > 0) {
		data_set = dataSets.getValueAsString(0, "file") + "/" + dataSets.getValueAsString(0, "name");
	}

	return data_set;
}

void DataAccess::setSelectedDataSet(const QString& val)
{
	QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);
	settings.setValue("data-set", val);
	settings.sync();
}

QString DataAccess::selectedCharacter()
{
	QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);
	QString char_file = settings.value("char-file").toString();
	if (!char_file.isEmpty()) {
		if (characters.findRow("file", char_file) < 0)
			char_file.clear();
	}
	return char_file;
}

void DataAccess::setSelectedCharacter(const QString& val)
{
	QSettings settings(DatUtil::getApplicationData() + "settings.ini", QSettings::IniFormat);
	settings.setValue("char-file", val);
	settings.sync();
}

QString DataAccess::getSystemDataLastUpdate()
{
	QString sys_data = DatUtil::getSystemDataFolder() + "Data/";

	if (!QFile(sys_data).exists())
		return QString("-- No System Data --");

	QFileInfo info(sys_data);
	return info.lastModified().toString("yyyy-MM-dd hh:mm:ss");
}

QByteArray DataAccess::getData(const QString& dset, const QString& dir, const QString& key, const QString& value)
{
	QByteArray data;

	if (!dset.isEmpty())
		data = findData(DatUtil::getDataSetFolder() + dset + "/" + dir + "/", key, value);
	if (data.isEmpty())
		data = findData(DatUtil::getSystemDataFolder() + "Data/" + dir + "/", key, value);
	return data;
}

QByteArray DataAccess::getData(const QString& dset, const QString& file)
{
	QString path;

	if (!dset.isEmpty())
		path = DatUtil::getDataSetFolder() + dset + "/" + file;
	if (path.isEmpty() || !QFile::exists(path))
		path = DatUtil::getSystemDataFolder() + "Data/" + file;
	return readFile(path);
}

QByteArray DataAccess::getData(const QString& file)
{
	QString path;

	path = DatUtil::getApplicationData() + file;
	return readFile(path);
}

QByteArray DataAccess::getCharacter(const QString& file)
{
	return readFile(DatUtil::getCharacterFolder() + file);
}

QByteArray DataAccess::getCurrentData(const QString& file)
{
	return readFile(DatUtil::getCurrentFolder() + file);
}

// Read and write files:
QByteArray DataAccess::readFile(const QString& file, QString* time)
{
	QFile		f(file);
	QByteArray	data;

	if (f.open(QIODevice::ReadOnly)) {
		if (time) {
			QFileInfo info(f);
			*time = info.lastModified().toString("d MMM yyyy h:m:s");
		}
		data = f.readAll();
		f.close();
	}
	return data;
}

void DataAccess::writeFile(const QString& file, const QByteArray& data)
{
	QFile f(file);

	makeDir(file, true);
	if (f.open(QIODevice::WriteOnly)) {
		f.write(data);
		f.close();
	}
}

void DataAccess::copyFile(const QString& src, const QString& dest)
{
	QByteArray data;

	data = readFile(src);
	writeFile(dest, data);
}

void DataAccess::deleteFile(const QString& file)
{
	QFile::remove(file);
}

bool DataAccess::removeDir(const QString & dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists(dirName)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir())
				result = removeDir(info.absoluteFilePath());
			else
				result = QFile::remove(info.absoluteFilePath());

			if (!result)
				return result;
		}
		result = dir.rmdir(dirName);
	}
	return result;
}

int DataAccess::setFileModificationTime(const QString& file, uint modt)
{
	int		erc;
	utimbuf	tbuf;

	tbuf.actime = modt;
	tbuf.modtime = modt;
	erc = utime(file.toUtf8().constData(), &tbuf);
	if (erc != 0)
		return errno;
	return 0;
}

bool DataAccess::makeDir(const QString& d, bool is_file)
{
	QDir dir;
	QString dir_name = d;

	if (is_file)
		dir_name = DatUtil::lastLeft(dir_name, "/");
	if (dir.exists(dir_name))
		return true;
	if (!makeDir(DatUtil::lastLeft(dir_name, "/"), false))
		return false;
	return dir.mkdir(dir_name);
}

void DataAccess::addCharacter(const QString& file, const QByteArray& data)
{
	QDir().mkpath(DatUtil::getCharacterFolder());
	writeFile(DatUtil::getCharacterFolder() + file, data);
	int row = characters.findRow("file", file);
	if (row == -1) {
		row = characters.findRow("file", ""); // Find first empty row:
		//characters.startChanges();
		setCharacterRow(row, file, data);
		row = characters.findRow("file", ""); // Find another empty
		if (row == -1) {
			for (int i=0; i<2; i++)
				addBlankCharacter();
		}
		characters.rowCountChanged();
		if (Character::instance)
			Character::instance->emitCharacterCountChanged();
	}
}

void DataAccess::setCharacterRow(int row, const QString& file, const QByteArray& data)
{
	QFileInfo info = QFileInfo(DatUtil::getCharacterFolder() + file);

	ListCharXML charDetails;
	charDetails.load(data, file);
	characters.setValue(row, "file", file);
	characters.setValue(row, "name", charDetails.name);
	characters.setValue(row, "player", charDetails.player);
	characters.setValue(row, "portrait", charDetails.portrait);
	characters.setValue(row, "time", charDetails.lastChanged());
	characters.setValue(row, "modt", info.lastModified().toTime_t());
}

void DataAccess::addBlankCharacter()
{
	int row = characters.appendRow();
	characters.setValue(row, "name", "");
	characters.setValue(row, "file", "");
	characters.setValue(row, "name", "");
	characters.setValue(row, "player", "");
	characters.setValue(row, "portrait", "");
	characters.setValue(row, "time", "");
	characters.setValue(row, "modt", 0);
}

void DataAccess::addDataSet(const QString& file, const QString& name)
{
	int row = dataSets.findRow("file", file);
	if (row == -1)
		row = dataSets.appendRow();
	setDataSetRow(row, file, name);
}

void DataAccess::setDataSetRow(int row, const QString& file, const QString& name)
{
	QFileInfo info = QFileInfo(DatUtil::getDataSetFolder() + file);

	// file.startsWith("DataSet_") ? DatUtil::right(file, "DataSet_") : file
	dataSets.setValue(row, "name", name);
	dataSets.setValue(row, "file", file);
	dataSets.setValue(row, "time", info.lastModified().toString("d MMM yyyy H:m:s"));
	dataSets.setValue(row, "modt", info.lastModified().toTime_t());
}

void DataAccess::setCurrentDataRow(int row, const QString& file)
{
	QFileInfo info = QFileInfo(DatUtil::getCurrentFolder() + file);

	currentData.setValue(row, "name", "");
	currentData.setValue(row, "file", file);
	currentData.setValue(row, "time", info.lastModified().toString("d MMM yyyy H:m:s"));
	currentData.setValue(row, "modt", info.lastModified().toTime_t());
}

// Return value of tag in file data
QString DataAccess::getTagValue(const QByteArray& data, const QString& tag)
{
	QPointer<FindTag> ft(new FindTag);

	return ft->search(data, tag);
}

QByteArray DataAccess::findData(const QString& dir, const QString& tag, const QString& value)
{
	QStringList files = listDirectory(dir);
	QByteArray data;
	QString tval;

	if (value.isEmpty())
		return data;

	foreach (QString file, files) {
		data = readFile(DatUtil::addDirChar(dir) + file);
		tval = getTagValue(data, tag);
		if (tval == value) {
			break;
		}
	}
	return data;
}

QStringList DataAccess::listDirectory(const QString& d)
{
	QDir dir(d);

	return dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files);
}

