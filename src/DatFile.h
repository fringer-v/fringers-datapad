#ifndef __DatFile_h__
#define __DatFile_h__

#include <QObject>

class DatFile : public QObject
{
	Q_OBJECT

public:
	explicit DatFile(QObject *parent = 0);

	Q_PROPERTY(QString source
		READ source
		WRITE setSource
		NOTIFY sourceChanged)

	Q_INVOKABLE QString read();
	Q_INVOKABLE bool write(const QString& data);
	Q_INVOKABLE QString getcwd();

	QString source() { return mSource; };

public slots:
	void setSource(const QString& source) { mSource = source; };

signals:
	void sourceChanged(const QString& source);
	void error(const QString& msg);

private:
	QString mSource;
};

#endif // __DatFile_h__
