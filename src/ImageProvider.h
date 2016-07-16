#ifndef __ImageProvider_h__
#define __ImageProvider_h__

#include <QObject>
#include <QMap>
#include <QQuickImageProvider>
#include <QDebug>

#include "DatUtil.h"
#include "Character.h"

// Dice sequence:
// P=Proficiency (yellow)
// A=Ability (green)
// B=Boost (blue)
// C=Challenge (red)
// D=Difficulty (purple)
// S=Setback (black)
// E=empty
// N=Negative Setback
// U=Upgrade Ability
// u=Upgrade Difficulty
// a=Advantage
// s=Success
// f=Failure
// t=Threat
// y=Despair
// x=Triumph
class ImageProvider : public QQuickImageProvider
{
public:
	ImageProvider();

	virtual ImageType imageType() const
	{
		return QQmlImageProviderBase::Pixmap;
	}

	virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
	{
		QPixmap pixmap;

		if (id.startsWith("symbol")) {
			pixmap = drawSymbols(DatUtil::lastRight(id, "/"));
		}
		else {
qDebug() << "----->> " << id;
			pixmap.loadFromData(iImages[id]);
			if (size)
				*size = pixmap.size();
			if (requestedSize.isValid() && requestedSize.width() > 0 && requestedSize.height() > 0)
				pixmap = pixmap.scaled(requestedSize);
qDebug() << "----->> " << pixmap.size().height() << pixmap.size().width();

			// This crazy "ping" is requried so that the CharacterBox is refreshed
			// after each call to this function. For some reason,
			// The images only display after the LAST call, there are 2 for each
			// image displayed in the character list. The draw of the characterCanvas
			// is done after the first, but it alway displays blank!! No idea why.
			// If a redraw is force AFTER then second call to this function
			// the image is display correctly!?
			// Maybe the image is not really ready until after the second call, but
			// according to the status, it is!
			Character::instance->setImageProviderCount(Character::instance->imageProviderCount()+1);
		}

		return pixmap;
	}

	static void setImage(const QString &id, const QByteArray& b) {
		qDebug() << "SET <<-----" << id << b.length();
		iImages[id] = b;
	}

	static int getSymbolWidth(const QString& list);
	static QString getSymbolLink(const QString& sym_codes);
	static QString getSymbolCode(const QString& token);
	static bool isSymbolToken(const QString& token);

private:
	static QPixmap drawSymbols(const QString &sym_list);
	static void setupSymbolCodes();

	static QMap<QString, QByteArray> iImages;
	static QMap<QChar, QImage> iSymbols;
	static QMap<QString, QChar> iSymbolCodes;
};


#endif // __ImageProvider_h__
