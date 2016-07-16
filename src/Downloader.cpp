#include <QEventLoop>

#include "Downloader.h"
#include "DatUtil.h"

Downloader::Downloader(QObject *parent) :
	QObject(parent)
{
	connect(&iNAM, SIGNAL(finished(QNetworkReply*)), SLOT(downloadComplete(QNetworkReply*)));
}

Downloader::~Downloader()
{
}

void Downloader::setUrl(QString url)
{
	iUrl = url;
	QNetworkRequest request(iUrl);
	iNAM.get(request);
}

void Downloader::setUrl(QString url, const QByteArray& data)
{
	iUrl = url;
	QNetworkRequest request(iUrl);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
	iNAM.post(request, data);
}

QByteArray Downloader::getData() const
{
	return iData;
}

QByteArray Downloader::download(QString url)
{
	iTransferComplete = false;
	setUrl(url);
	while (!iTransferComplete) {
		DatUtil::yield();
	}
	return iData;
}

QString Downloader::upload(QString url, const QByteArray& data)
{
	iTransferComplete = false;
	setUrl(url, data);
	while (!iTransferComplete) {
		DatUtil::yield();
	}
	return iData;
}

QString Downloader::getUrl()
{
	return iUrl;
}

void Downloader::downloadComplete(QNetworkReply* reply)
{
	bool ok = reply->error() == QNetworkReply::NoError;
	iData = reply->readAll();
	reply->deleteLater();
	iTransferComplete = true;
	emit downloaded(ok);
}
