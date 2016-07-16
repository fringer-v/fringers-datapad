#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "DatFile.h"
#include "DataList.h"
#include "Character.h"
#include "DataAccess.h"
#include "ImageProvider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Fringer");
	QCoreApplication::setOrganizationDomain("fringer.space");
	QCoreApplication::setApplicationName("Datapad");

	DataAccess::init();

	// Declaring your C++ class to the QML system
	qmlRegisterType<DatFile>("space.fringer.datapad", 1, 0, "DatFile");

	qmlRegisterType<DatGeneralSkillsList>("space.fringer.datapad", 1, 0, "DatGeneralSkillsList");
	qmlRegisterType<DatCombatSkillsList>("space.fringer.datapad", 1, 0, "DatCombatSkillsList");
	qmlRegisterType<DatKnowledgeSkillsList>("space.fringer.datapad", 1, 0, "DatKnowledgeSkillsList");
	qmlRegisterType<DatSpecialSkillsList>("space.fringer.datapad", 1, 0, "DatSpecialSkillsList");
	qmlRegisterType<DatCustomSkillsList>("space.fringer.datapad", 1, 0, "DatCustomSkillsList");
	qmlRegisterType<DatTalentList>("space.fringer.datapad", 1, 0, "DatTalentList");
	qmlRegisterType<DatSpeciesFeaturesList>("space.fringer.datapad", 1, 0, "DatSpeciesFeaturesList");
	qmlRegisterType<DatObligation2List>("space.fringer.datapad", 1, 0, "DatObligation2List");
	qmlRegisterType<DatObligationList>("space.fringer.datapad", 1, 0, "DatObligationList");
	qmlRegisterType<DatDutyList>("space.fringer.datapad", 1, 0, "DatDutyList");
	qmlRegisterType<DatWeaponsList>("space.fringer.datapad", 1, 0, "DatWeaponsList");
	qmlRegisterType<DatArmorList>("space.fringer.datapad", 1, 0, "DatArmorList");
	qmlRegisterType<DatGearList>("space.fringer.datapad", 1, 0, "DatGearList");
	qmlRegisterType<DatInjuryList>("space.fringer.datapad", 1, 0, "DatInjuryList");
	qmlRegisterType<DatXPList>("space.fringer.datapad", 1, 0, "DatXPList");
	qmlRegisterType<DatInventory>("space.fringer.datapad", 1, 0, "DatInventory");
	qmlRegisterType<DatCheckList>("space.fringer.datapad", 1, 0, "DatCheckList");
	qmlRegisterType<DatMotivationList>("space.fringer.datapad", 1, 0, "DatMotivationList");
	qmlRegisterType<DatMoralityList>("space.fringer.datapad", 1, 0, "DatMoralityList");
	qmlRegisterType<DatShopList>("space.fringer.datapad", 1, 0, "DatShopList");

	qmlRegisterType<DatCharacterList>("space.fringer.datapad", 1, 0, "DatCharacterList");
	qmlRegisterType<DatDataSetList>("space.fringer.datapad", 1, 0, "DatDataSetList");
	qmlRegisterType<DatCurrentDataList>("space.fringer.datapad", 1, 0, "DatCurrentDataList");

	qmlRegisterType<Character>("space.fringer.datapad", 1, 0, "Character");

	QQmlApplicationEngine engine;
	engine.addImageProvider(QLatin1String("datapad-images"), new ImageProvider);
	engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

	return app.exec();
}
