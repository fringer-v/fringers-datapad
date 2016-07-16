#ifndef __Downloader_h__
#define __Downloader_h__

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Downloader : public QObject
{
	Q_OBJECT

public:
	explicit Downloader(QObject *parent = 0);
	virtual ~Downloader();

	void setUrl(QString url);
	void setUrl(QString url, const QByteArray& data);
	QByteArray getData() const;
	QByteArray download(QString url);
	QString upload(QString url, const QByteArray& data);
	QString getUrl();

signals:
	void downloaded(bool ok);

private slots:
	void downloadComplete(QNetworkReply* pReply);

private:
	QNetworkAccessManager	iNAM;
	QString					iUrl;
	QByteArray				iData;
	bool					iTransferComplete;
};

#endif // __Downloader_h__
