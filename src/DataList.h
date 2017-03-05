#ifndef __DataList_h__
#define __DataList_h__

#include <QObject>

#include "ListModel.h"
#include "Item.h"

class AbstractDataList
{
public:
	AbstractDataList(QStringList colums);

	virtual int rowCount() = 0;
	virtual QVariant getValue(int row, int col) = 0;
	virtual QVariant getValue(int row, const char* col);

public:
	void setModel(ListModel* model);
	void dataChanged();
	void rowCountChanged();
	void setDataChanged();
	void setRowCountChanged();
	void setClean();
	int colCount();
	QString getColName(int col);
	QString getValueAsString(int row, const char* col);
	bool getValueAsBool(int row, const char* col);
	int getValueAsInt(int row, const char* col);
	uint getValueAsUint(int row, const char* col);

protected:
	int colIndex(const char* col);

	ListModel*				iModel;
	QStringList				iColumns;
	QMap<QByteArray, int>	iColNameToIndex;
};

class DataRow
{
public:
	DataRow(int count);
	QVariant getValue(int col);
	void setValue(int col, QVariant val);

private:
	QList<QVariant> iValues;
};

class DataList : public AbstractDataList
{
public:
	DataList(QStringList colums);

	virtual int rowCount();
	int appendRow();
	void insertRow(int);
	void deleteRow(int row);
	void clear();

	virtual QVariant getValue(int row, int col);

	int findRow(const char* col, const QString& value);
	int findRow(const char* col, int value);
	bool findSortedRow(int& row, const char* col, const QString& value);

	void setValue(int row, int col, QVariant value);
	void setValue(int row, const char* col, const char* value);
	void setValue(int row, const char* col, QString value);
	void setValue(int row, const char* col, int value);
	void setValue(int row, const char* col, uint value);
	void setValue(int row, const char* col, bool value);

private:
	QList<DataRow> iRows;
};

class ItemList : public AbstractDataList
{
public:
	ItemList(QStringList colums);

	virtual int rowCount();
	virtual QVariant getValue(int row, int col);
	virtual QVariant getValue(int row, const char* col);

	void clear();
	void aquireItem(Item& item, bool loading);
	void removeItemByUuid(const QString& uuid);
	Item itemAt(int i);
	bool containsByUuid(const QString& key);
	bool equipped(const QString& key);
	int carriedQuantity(const QString& key);
	int quantity(const QString& key);
	Item getItemByUuid(const QString& uuid);
	QStringList& getItemByKey(const QString& key);
	bool changeEquipment(const QString& uuid, int state, int stored);

private:
	void addSortedItem(const Item& item);

	QMap<QString, Item> iUuidList;
	QMap<QString, QStringList> iItemKeyList;
	QStringList iSortedKeys;
};

#endif // __DataList_h__
