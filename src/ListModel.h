#ifndef __ListModel__
#define __ListModel__

#include <QObject>
#include <QAbstractListModel>

class AbstractDataList;

class ListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	ListModel(AbstractDataList* list, const QString& name, QObject *parent = 0);
	virtual ~ListModel();

	//Q_INVOKABLE QStringList stringList(const QString& col);
	//Q_INVOKABLE QString valueAt(int i, const QString& col);
	//Q_INVOKABLE int rowOf(const QString& col, const QString& val);

	void setDataChanged();
	void setRowCountChanged();
	void setClean();

	virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual int mapRow(int row, const QString& col = QString()) const;
	virtual QString mapCol(const QString& col) const;

protected:
	virtual QHash<int, QByteArray> roleNames() const;

public:
	QString				iName;

protected:
	AbstractDataList*	iDataList;
	bool				iDataChanged;
	bool				iRowCountChanged;
};

class Col2ListModel : public ListModel
{
public:
	Col2ListModel(AbstractDataList* list, const QString& name, QObject *parent = 0);

	virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	virtual int mapRow(int row, const QString& col = QString()) const;
	virtual QString mapCol(const QString& col) const;

protected:
	virtual QHash<int, QByteArray> roleNames() const;
};

class DatGeneralSkillsList : public ListModel
{
public:
	DatGeneralSkillsList(QObject *parent = 0);
};

class DatCombatSkillsList : public ListModel
{
public:
	DatCombatSkillsList(QObject *parent = 0);
};

class DatKnowledgeSkillsList : public ListModel
{
public:
	DatKnowledgeSkillsList(QObject *parent = 0);
};

class DatSpecialSkillsList : public ListModel
{
public:
	DatSpecialSkillsList(QObject *parent = 0);
};

class DatCustomSkillsList : public ListModel
{
public:
	DatCustomSkillsList(QObject *parent = 0);
};

class DatTalentList : public ListModel
{
public:
	DatTalentList(QObject *parent = 0);
};

class DatSpeciesFeaturesList : public ListModel
{
public:
	DatSpeciesFeaturesList(QObject *parent = 0);
};

class DatObligation2List : public Col2ListModel
{
public:
	DatObligation2List(QObject *parent = 0);
};

class DatObligationList : public ListModel
{
public:
	DatObligationList(QObject *parent = 0);
};

class DatDutyList : public ListModel
{
public:
	DatDutyList(QObject *parent = 0);
};

class DatWeaponsList : public ListModel
{
public:
	DatWeaponsList(QObject *parent = 0);
};

class DatArmorList : public ListModel
{
public:
	DatArmorList(QObject *parent = 0);
};

class DatGearList : public ListModel
{
public:
	DatGearList(QObject *parent = 0);
};

class DatInjuryList : public ListModel
{
public:
	DatInjuryList(QObject *parent = 0);
};

class DatXPList : public ListModel
{
public:
	DatXPList(QObject *parent = 0);
};

class DatInventory : public ListModel
{
public:
	DatInventory(QObject *parent = 0);
};

class DatCheckList : public ListModel
{
public:
	DatCheckList(QObject *parent = 0);
};

class DatMotivationList : public ListModel
{
public:
	DatMotivationList(QObject *parent = 0);
};

class DatMoralityList : public ListModel
{
public:
	DatMoralityList(QObject *parent = 0);
};

class DatCharacterList : public Col2ListModel
{
public:
	DatCharacterList(QObject *parent = 0);
};

class DatDataSetList : public ListModel
{
public:
	DatDataSetList(QObject *parent = 0);
};

class DatCurrentDataList : public ListModel
{
public:
	DatCurrentDataList(QObject *parent = 0);
};

class DatShopList : public ListModel
{
public:
	DatShopList(QObject *parent = 0);
};

#endif // __ListModel__
