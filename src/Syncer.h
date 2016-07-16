#ifndef __Syncer_h__
#define __Syncer_h__

#include "Downloader.h"
#include "DataList.h"

#define STATE_START					1
#define STATE_LOAD_SYNC_FILE		1
#define STATE_LOAD_CHAR				2
#define STATE_LOAD_DATASET			3
#define STATE_LOAD_CURR_DATA		4

class Character;

class Syncer : public QObject
{
	Q_OBJECT

public:
	explicit Syncer(QObject *parent = 0);
	virtual ~Syncer();

	QString sync();
	bool currentChanged();

private:
	QString syncNow(bool just_write);
	QString addChecksum(const QString& args);
	void createList(DataList& list, const QString& prefix, QString data);
	QString syncDirectory(uint last_sync_time, const QString& local_path, const QString& prefix,
		DataList& local_list, DataList& server_list, const QString& current_file, bool upload_only);
	QString uploadFile(const QString& file, const QString& prefix, uint modt);
	QString downloadFile(const QString& file, const QString& prefix, uint modt, DataList& local_list);

	Downloader	iDownloader;
	bool		iCurrentChanged;
	QString		iSyncEmail;
	QString		iSyncPassword;
};

#endif // __Syncer_h__
