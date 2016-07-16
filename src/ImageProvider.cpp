#include <QPainter>

#include "ImageProvider.h"

QMap<QString, QByteArray> ImageProvider::iImages;
QMap<QChar, QImage> ImageProvider::iSymbols;
QMap<QString, QChar> ImageProvider::iSymbolCodes;

ImageProvider::ImageProvider() :
	QQuickImageProvider(QQuickImageProvider::Pixmap)
{
	iSymbols['P'] = QImage(":/images/yellow18.png");
	iSymbols['A'] = QImage(":/images/green18.png");
	iSymbols['B'] = QImage(":/images/blue18.png");
	iSymbols['C'] = QImage(":/images/red18.png");
	iSymbols['D'] = QImage(":/images/purple18.png");
	iSymbols['S'] = QImage(":/images/black18.png");
	iSymbols['s'] = QImage(":/images/success18.png");
	iSymbols['f'] = QImage(":/images/failure18.png");
	iSymbols['a'] = QImage(":/images/advantage18.png");
	iSymbols['t'] = QImage(":/images/threat18.png");
	iSymbols['y'] = QImage(":/images/despair18.png");
	iSymbols['x'] = QImage(":/images/triumph18.png");
	setupSymbolCodes();
}

int ImageProvider::getSymbolWidth(const QString& sym_list)
{
	int pos = 0;

	for (int i=0; i<sym_list.length(); i++) {
		pos += iSymbols[sym_list[i]].width();
	}
	return pos + (sym_list.length()-1)*2;
}

QString ImageProvider::getSymbolLink(const QString& sym_codes)
{
	return QString("<img src=\"image://datapad-images/symbols/%1\" height=\"18\" width=\"%2\">")
		.arg(sym_codes).arg(getSymbolWidth(sym_codes));
}

QString ImageProvider::getSymbolCode(const QString& token)
{
	return iSymbolCodes[token];
}

bool ImageProvider::isSymbolToken(const QString& token)
{
	return iSymbolCodes.contains(token);
}

QPixmap ImageProvider::drawSymbols(const QString &sym_list)
{
	QImage image(getSymbolWidth(sym_list), 18, QImage::Format_ARGB32_Premultiplied);
	image.fill(0);

	QPainter painter(&image);
	int pos = 0;
	for (int i=0; i<sym_list.length(); i++) {
		painter.drawImage(pos, 0, iSymbols[sym_list[i]]);
		pos += iSymbols[sym_list[i]].width() + 2;
	}

	return QPixmap::fromImage(image);
}

void ImageProvider::setupSymbolCodes()
{
	iSymbolCodes["[ABILITY]"] = 'A';
	iSymbolCodes["[AB]"] = 'A';
	iSymbolCodes["[ADVANTAGE]"] = 'a';
	iSymbolCodes["[AD]"] = 'a';
	iSymbolCodes["[BOOST]"] = 'B';
	iSymbolCodes["[BO]"] = 'B';
	iSymbolCodes["[CHALLENGE]"] = 'C';
	iSymbolCodes["[CH]"] = 'C';
	iSymbolCodes["[DESPAIR]"] = 'y';
	iSymbolCodes["[DE]"] = 'y';
	iSymbolCodes["[DIFFICULTY]"] = 'D';
	iSymbolCodes["[DI]"] = 'D';
	iSymbolCodes["[FAILURE]"] = 'f';
	iSymbolCodes["[FA]"] = 'f';
	iSymbolCodes["[PROFICIENCY]"] = 'P';
	iSymbolCodes["[PR]"] = 'P';
	iSymbolCodes["[SETBACK]"] = 'S';
	iSymbolCodes["[SE]"] = 'S';
	iSymbolCodes["[SUCCESS]"] = 's';
	iSymbolCodes["[SU]"] = 's';
	iSymbolCodes["[THREAT]"] = 't';
	iSymbolCodes["[TH]"] = 't';
	iSymbolCodes["[TRIUMPH]"] = 'x';
	iSymbolCodes["[TR]"] = 'x';
	iSymbolCodes["[LIGHT]"] = 'Z';
	iSymbolCodes["[LI]"] = 'Z';
	iSymbolCodes["[FP]"] = 'Z';
	iSymbolCodes["[DARK]"] = 'z';
	iSymbolCodes["[DA]"] = 'z';
	iSymbolCodes["[REMSETBACK]"] = 'N';
	iSymbolCodes["[RS]"] = 'N';
	iSymbolCodes["[UPGABILITY]"] = 'U';
	iSymbolCodes["[UA]"] = 'U';
	iSymbolCodes["[UPGDIFFICULTY"] = 'u';
	iSymbolCodes["[UD]"] = 'u';

	//iSymbolCodes["[FORCEPOINT]"] = '.';
	//iSymbolCodes["[FP]"] = '.';
	//iSymbolCodes["[FORCE]"] = 'F';
	//iSymbolCodes["[FO]"] = 'F';
	//iSymbolCodes["[RESTRICTED]"] = '=';
	//iSymbolCodes["[RE]"] = '=';
}

