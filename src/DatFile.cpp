#include <QFile>
#include <QTextStream>
#include <QDir>

#include "DatFile.h"

DatFile::DatFile(QObject *parent) :
	QObject(parent)
{
}

QString DatFile::read()
{
	if (mSource.isEmpty()){
		emit error("source is empty");
		return QString();
	}

	QFile file(mSource);
	QString fileContent;

	if (file.open(QIODevice::ReadOnly)) {
		QString line;
		QTextStream t( &file );
		do {
			line = t.readLine();
			fileContent += line;
		 } while (!line.isNull());

		file.close();
	}
	else {
		emit error("Unable to open the file");
		return QString();
	}
	return fileContent;
}

bool DatFile::write(const QString& data)
{
	if (mSource.isEmpty())
		return false;

	QFile file(mSource);
	if (!file.open(QFile::WriteOnly | QFile::Truncate))
		return false;

	QTextStream out(&file);
	out << data;

	file.close();

	return true;
}

QString DatFile::getcwd()
{
	return QDir::currentPath();
}

