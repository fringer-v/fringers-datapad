#include <QDebug>

#include "ListModel.h"
#include "DataList.h"
#include "DataAccess.h"
#include "DatUtil.h"
#include "Gear.h"
#include "Armor.h"
#include "Weapons.h"
#include "Talents.h"
#include "CharacterList.h"

// ListModel -----------------------------------

ListModel::ListModel(AbstractDataList* list, const QString& name, QObject *parent) :
	QAbstractListModel(parent),
	iName(name),
	iDataList(list),
	iDataChanged(false),
	iRowCountChanged(false)
{
	list->setModel(this);
}

ListModel::~ListModel()
{
}

/*
QStringList ListModel::stringList(const QString& col)
{
	QStringList list;

	for (int i=0; i<rowCount(); i++)
		list.append(iDataList->getValueAsString(mapRow(i, col), mapCol(col).toUtf8().constData()));
	return list;
}

QString ListModel::valueAt(int row, const QString& col)
{
	return iDataList->getValueAsString(mapRow(row, col), mapCol(col).toUtf8().constData());
}

int ListModel::rowOf(const QString& col, const QString& val)
{
	for (int i=0; i<rowCount(); i++)
		if (iDataList->getValueAsString(mapRow(i, col), mapCol(col).toUtf8().constData()) == val)
			return i;
	return -1;
}
*/

void ListModel::setDataChanged()
{
	iDataChanged = true;
}

void ListModel::setRowCountChanged()
{
	//iDataChanged = true;
	iRowCountChanged = true;
}

void ListModel::makeClean()
{
	if (iDataChanged) {
		iDataChanged = false;
		emit dataChanged(createIndex(0, 0), createIndex(iDataList->rowCount()-1, iDataList->colCount()-1));
	}
	if (iRowCountChanged) {
		iRowCountChanged = false;
		emit layoutChanged();
	}

}

int ListModel::rowCount(const QModelIndex & parent) const {
	Q_UNUSED(parent);
	return mapRow(iDataList->rowCount());
}

QVariant ListModel::data(const QModelIndex& index, int role) const {
	int col = role - (Qt::UserRole + 1);

	return iDataList->getValue(index.row(), col);
}

int ListModel::mapRow(int row, const QString& col) const
{
	Q_UNUSED(col);
	return row;
}

QString ListModel::mapCol(const QString& col) const
{
	return col;
}

QHash<int, QByteArray> ListModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	for (int i=0; i<iDataList->colCount(); i++) {
		roles[(int) Qt::UserRole + 1 + i] = iDataList->getColName(i).toUtf8();
	}
	return roles;
}

// ListModel -----------------------------------

Col2ListModel::Col2ListModel(AbstractDataList* list, const QString& name, QObject *parent) :
	ListModel(list, name, parent)
{
}

QVariant Col2ListModel::data(const QModelIndex & index, int role) const
{
	int col = role - (Qt::UserRole + 1);

	return iDataList->getValue((index.row() * 2) + (col / iDataList->colCount()), col % iDataList->colCount());
}

int Col2ListModel::mapRow(int row, const QString& col) const
{
	if (col.isEmpty())
		return (row + 1) / 2;
	return (row * 2) + DatUtil::lastRight(col, "_").toInt();
}

QString Col2ListModel::mapCol(const QString& col) const
{
	return DatUtil::lastLeft(col, "_");
}

QHash<int, QByteArray> Col2ListModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	for (int i=0; i<iDataList->colCount(); i++) {
		roles[(int) Qt::UserRole + 1 + i] = (iDataList->getColName(i) + "_0").toUtf8();
		roles[(int) Qt::UserRole + 1 + i + iDataList->colCount()] = (iDataList->getColName(i) + "_1").toUtf8();
	}
	return roles;
}

// DatGeneralSkillsList -----------------------------------

DatGeneralSkillsList::DatGeneralSkillsList(QObject *parent) :
	ListModel(&GeneralSkills::instance, "DatGeneralSkillsList", parent)
{
}

// DatCombatSkillsList -----------------------------------

DatCombatSkillsList::DatCombatSkillsList(QObject *parent) :
	ListModel(&CombatSkills::instance, "DatCombatSkillsList", parent)
{
}

// DatKnowledgeSkillsList -----------------------------------

DatKnowledgeSkillsList::DatKnowledgeSkillsList(QObject *parent) :
	ListModel(&KnowledgeSkills::instance, "DatKnowledgeSkillsList", parent)
{
}

// DatSpecialSkillsList -----------------------------------

DatSpecialSkillsList::DatSpecialSkillsList(QObject *parent) :
	ListModel(&SpecialSkills::instance, "DatSpecialSkillsList", parent)
{
}

// DatCustomSkillsList -----------------------------------

DatCustomSkillsList::DatCustomSkillsList(QObject *parent) :
	ListModel(&CustomSkills::instance, "DatCustomSkillsList", parent)
{
}

// DatTalentList -----------------------------------

DatTalentList::DatTalentList(QObject *parent) :
	ListModel(&Talents::instance, "DatTalentList", parent)
{
}

// DatSpeciesFeaturesList -----------------------------------

DatSpeciesFeaturesList::DatSpeciesFeaturesList(QObject *parent) :
	ListModel(&SpecialFeaturesList::instance, "DatSpeciesFeaturesList", parent)
{
}

// DatObligation2List -----------------------------------

DatObligation2List::DatObligation2List(QObject *parent) :
	Col2ListModel(&ObligationList::instance, "DatObligation2List", parent)
{
}

// DatObligationList -----------------------------------

DatObligationList::DatObligationList(QObject *parent) :
	ListModel(&ObligationList::instance, "DatObligationList", parent)
{
}

// DatDutyList -----------------------------------

DatDutyList::DatDutyList(QObject *parent) :
	ListModel(&DutyList::instance, "DatDutyList", parent)
{
}

// DatWeaponsList -----------------------------------

DatWeaponsList::DatWeaponsList(QObject *parent) :
	ListModel(&Weapons::instance, "DatWeaponsList", parent)
{
}

// DatArmorList -----------------------------------

DatArmorList::DatArmorList(QObject *parent) :
	ListModel(&Armor::instance, "DatArmorList", parent)
{
}

// DatGearList -----------------------------------

DatGearList::DatGearList(QObject *parent) :
	ListModel(&Gear::instance, "DatGearList", parent)
{
}

// DatInjuryList -----------------------------------

DatInjuryList::DatInjuryList(QObject *parent) :
	ListModel(&InjuryList::instance, "DatInjuryList", parent)
{
}

// DatXPList -----------------------------------

DatXPList::DatXPList(QObject *parent) :
	ListModel(&ExperienceList::instance, "DatXPList", parent)
{
}

// DatInventory -----------------------------------

DatInventory::DatInventory(QObject *parent) :
	ListModel(&InventoryLog::instance, "DatInventory", parent)
{
}

// DatCheckList -----------------------------------

DatCheckList::DatCheckList(QObject *parent) :
	ListModel(&CheckList::instance, "DatCheckList", parent)
{
}

// Motivation -----------------------------------

DatMotivationList::DatMotivationList(QObject *parent) :
	ListModel(&MotivationList::instance, "DatMotivationList", parent)
{
}

// Morality -----------------------------------

DatMoralityList::DatMoralityList(QObject *parent) :
	ListModel(&MoralityList::instance, "DatMoralityList", parent)
{
}

// DatCharacterList -----------------------------------

DatCharacterList::DatCharacterList(QObject *parent) :
	Col2ListModel(&DataAccess::characters, "DatCharacterList", parent)
{
}

// DatDataSetList -----------------------------------

DatDataSetList::DatDataSetList(QObject *parent) :
	ListModel(&DataAccess::dataSets, "DatDataSetList", parent)
{
}

// DatCurrentDataList -----------------------------------

DatCurrentDataList::DatCurrentDataList(QObject *parent) :
	ListModel(&DataAccess::currentData, "DatCurrentDataList", parent)
{
}

// DatShopList -----------------------------------

DatShopList::DatShopList(QObject *parent) :
	ListModel(&Shop::instance, "DatShopList", parent)
{
}

