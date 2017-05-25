#include <QFileInfo>
#include <QDir>
#include <QDateTime>

#include "md5.h"

#include "Syncer.h"
#include "DatUtil.h"
#include "DataAccess.h"
#include "DatZip.h"
#include "Character.h"
#include "datapad.h"

Syncer::Syncer(QObject *parent) :
	QObject(parent)
{
}

Syncer::~Syncer()
{
}

QString Syncer::sync()
{
	if (Character::instance->host().trimmed().isEmpty())
		return "You have not provided the host name of the server";

	if (Character::instance->email().trimmed().isEmpty())
		return "You have not provided an email for connecting to the server";

	if (Character::instance->password().trimmed().isEmpty())
		return "You have not provided a password for connecting to the server";

	if (!Character::instance->currentEmail().isEmpty() && Character::instance->email() != Character::instance->currentEmail()) {

		// Switching accounts, first upload changes, then
		// then delete local data
		iSyncEmail = Character::instance->currentEmail();
		iSyncPassword = Character::instance->currentPassword();
		syncNow(true);
		// Remove all data:
		DataAccess::removeDir(DatUtil::getCurrentFolder());
		DataAccess::removeDir(DatUtil::getCharacterFolder());
		DataAccess::removeDir(DatUtil::getDataSetFolder());
		QFile::remove(DatUtil::getApplicationData() + "last_sync_time");

		DataAccess::characters.clear();
		Character::instance->emitCharacterCountChanged();
		DataAccess::dataSets.clear();
		DataAccess::currentData.clear();
		iCurrentChanged = true;
	}
	else
		iCurrentChanged = false;

	Character::instance->setCurrentEmailAndPassword("", "");
	iSyncEmail = Character::instance->email();
	iSyncPassword = Character::instance->password();
	QString result = syncNow(false);
	if (result.isEmpty())
		// Record last successful access:
		Character::instance->setCurrentEmailAndPassword(iSyncEmail, iSyncPassword);
	return result;
}

QString Syncer::syncNow(bool upload_only)
{
	QString args;
	QString error;
	uint sync_time = QDateTime::currentDateTimeUtc().toTime_t();
	uint last_sync_time = DataAccess::readFile(DatUtil::getApplicationData() + "last_sync_time").toUInt();
	DataList server_list(QStringList() << "file" << "modt");

	// Get server structure:
	args = QString("contents&user=%1&time=%2").arg(DatUtil::base64(iSyncEmail)).arg(DatUtil::timeNowInSecs());
	QString data = QString::fromUtf8(iDownloader.download("http://" + Character::instance->host() + "/control.php?" + addChecksum(args)));
	if (data.isEmpty()) {
		return "Unable to retrieve synchronisation data from host: "+Character::instance->host();
	}
	if (data.startsWith("ERROR!"))
		return DatUtil::right(data, "ERROR!").trimmed();

	createList(server_list, "chars/", data);
	error = syncDirectory(last_sync_time, DatUtil::getCharacterFolder(), "chars/", DataAccess::characters, server_list, Character::instance->file(), upload_only);
	Character::instance->emitCharacterCountChanged();
	if (!error.isEmpty())
		return error;

	createList(server_list, "dsets/", data);
	error = syncDirectory(last_sync_time, DatUtil::getDataSetFolder(), "dsets/", DataAccess::dataSets, server_list, Character::instance->dataSet(), upload_only);
	if (!error.isEmpty())
		return error;

	createList(server_list, "cdata/", data);
	error = syncDirectory(last_sync_time, DatUtil::getCurrentFolder(), "cdata/", DataAccess::currentData, server_list, CurrentData::instance->fileName, upload_only);
	if (!error.isEmpty())
		return error;

	DataAccess::writeFile(DatUtil::getApplicationData() + "last_sync_time", QString("%1").arg(sync_time).toUtf8());
	return QString();
}

bool Syncer::currentChanged()
{
	return iCurrentChanged;
}

QString Syncer::addChecksum(const QString& args)
{
	QString csum = Md5::hash(QString("%1-%2").arg(args).arg(iSyncPassword).toUtf8());
	return args + "&csum=" + csum;
}

void Syncer::createList(DataList& list, const QString& prefix, QString data)
{
	list.clear();
	while (data.length() > 0) {
		QString line = DatUtil::left(data, "\n");
		data = DatUtil::right(data, "\n");

		QString file = DatUtil::left(line, "\t");
		line = DatUtil::right(line, "\t");

		if (file.startsWith(prefix)) {
			int row = list.appendRow();

			list.setValue(row, "file", DatUtil::right(file, prefix));
			list.setValue(row, "modt", DatUtil::left(line, "\t").toUInt());
		}
	}
}

QString Syncer::syncDirectory(uint last_sync_time, const QString& local_path, const QString& prefix,
	DataList& local_list, DataList& server_list, const QString& current_file, bool upload_only)
{
	int local_row;
	int remote_row;
	uint local_modt;
	uint remote_modt;
	QString error;

	//local_list.startChanges();

	for (remote_row=0; remote_row<server_list.rowCount() && error.isEmpty(); remote_row++) {
		QString file = server_list.getValueAsString(remote_row, "file");
		remote_modt = server_list.getValueAsUint(remote_row, "modt");

		if (file.endsWith(".tri"))
			continue;
		local_row = local_list.findRow("file", file);
		if (local_row < 0) {
			if (!upload_only) {
				error = downloadFile(local_path + file, prefix, remote_modt, local_list);
				if (error.isEmpty() && file == current_file)
					iCurrentChanged = true;
			}
		}
		else {
			// Compare date:
			local_modt = local_list.getValueAsUint(local_row, "modt");
			if (local_modt > remote_modt)
				error = uploadFile(local_path + file, prefix, local_modt);
			else if (remote_modt > local_modt) {
				if (!upload_only) {
					error = downloadFile(local_path + file, prefix, remote_modt, local_list);
					if (error.isEmpty() && file == current_file)
						iCurrentChanged = true;
				}
			}
		}
	}

	for (local_row=local_list.rowCount()-1; local_row>=0 && error.isEmpty(); local_row--) {
		QString file = local_list.getValueAsString(local_row, "file");
		local_modt = local_list.getValueAsUint(local_row, "modt");

		if (file.endsWith(".tri"))
			continue;
		remote_row = server_list.findRow("file", file);
		if (remote_row < 0) {
			if (local_modt > last_sync_time)
				error = uploadFile(local_path + file, prefix, local_modt);
			else {
				if (file.endsWith(".zip"))
					DataAccess::removeDir(local_path + file);
				else
					DataAccess::deleteFile(local_path + file);
				local_list.deleteRow(local_row);
				if (file == current_file)
					iCurrentChanged = true;
			}
		}
	}

	if (upload_only)
		local_list.clear();

	local_list.rowCountChanged();
	return error;
}

QString Syncer::uploadFile(const QString& file, const QString& prefix, uint modt)
{
	QByteArray data = DataAccess::readFile(file);
	QString args;

	args = QString("upload=%1&user=%2&modt=%3&time=%4tver=%45")
		.arg(DatUtil::base64(prefix + DatUtil::lastNameOfPath(file)))
		.arg(DatUtil::base64(iSyncEmail))
		.arg(modt)
		.arg(DatUtil::timeNowInSecs())
		.arg(DATAPAD_VERSION_NO);
	QString url = "http://" + Character::instance->host() + "/control.php?" + addChecksum(args);
	QString result = iDownloader.upload(url, data);
	if (result.startsWith("ERROR!"))
		return DatUtil::right(result, "ERROR!").trimmed();
	if (!result.startsWith("OK!")) {
		return "Unable to upload file: "+DatUtil::lastNameOfPath(file);
	}
	return QString();
}

QString Syncer::downloadFile(const QString& file_path, const QString& prefix, uint modt, DataList& local_list)
{
	QString args;
	QString dest_obj;

	QString file_name = DatUtil::lastNameOfPath(file_path);
	args = QString("download=%1&user=%2&time=%3&tver=%4")
		.arg(DatUtil::base64(prefix + file_name))
		.arg(DatUtil::base64(iSyncEmail))
		.arg(DatUtil::timeNowInSecs())
		.arg(DATAPAD_VERSION_NO);
	QByteArray data = iDownloader.download("http://" + Character::instance->host() + "/control.php?" + addChecksum(args));
	if (data.isEmpty()) {
		return "Failed to download file: "+file_name;
	}

	if (file_name.endsWith(".zip")) {
		QString tmp_file = DatUtil::getTempFolder() + file_name;
		DataAccess::writeFile(tmp_file, data);

		QDir dir;
		if (dir.exists(file_path))
			DataAccess::removeDir(file_path);
		if (!DataAccess::makeDir(file_path, false))
			return "Unable to create directory: "+file_name;

		QString err_msg;
		dest_obj = DatZip::uncompress(tmp_file, file_path, err_msg);
		DataAccess::deleteFile(tmp_file);
		if (!err_msg.isEmpty())
			return err_msg;
	}
	else {
		DataAccess::writeFile(file_path, data);
	}

	int err = DataAccess::setFileModificationTime(file_path, modt);
	if (err != 0)
		return QString("Unable to set modification time on file: %1 (%2)").arg(file_name).arg(err);

	int local_row = local_list.findRow("file", file_name);
	if (local_row < 0)
		local_row = local_list.appendRow();
	if (prefix == "chars/")
		DataAccess::setCharacterRow(local_row, file_name, data);
	else if (prefix == "dsets/")
		DataAccess::setDataSetRow(local_row, file_name, dest_obj);
	else
		DataAccess::setCurrentDataRow(local_row, file_name);
	return QString();
}

