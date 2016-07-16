#ifndef __ImageProvider_h__
#define __ImageProvider_h__

#include <QObject>
#include <QMap>
#include <QQuickImageProvider>

#include "DatUtil.h"

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

	virtual QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
	{
		QPixmap pixmap;

		if (id.startsWith("symbol")) {
			pixmap = drawSymbols(DatUtil::lastRight(id, "/"));
		}
		else {
			pixmap.loadFromData(iImages[id]);
			if (size)
				*size = pixmap.size();
			if (requestedSize.isValid() && requestedSize.width() > 0 && requestedSize.height() > 0)
				pixmap = pixmap.scaled(requestedSize);
		}

		return pixmap;
	}

	static void setImage(const QString &id, const QByteArray& b) {
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
