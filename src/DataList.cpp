#include <string.h>

#include "DataList.h"
#include "Character.h"

DataList DataList::speciesFeatures = DataList(QStringList() << "title" << "subtitle" << "content");
DataList DataList::injuries = DataList(QStringList() << "ref" << "percent" << "type" << "title"
	<< "description" << "dice");
// type: start=0, total=1, amount=2
//DataList ExperienceList::instance = DataList(QStringList() << "ref" << "description" << "amount" << "type");
//DataList DataList::checklist = DataList(QStringList() << "ref" << "checked" << "dice" << "description");
DataList DataList::motivation = DataList(QStringList() << "name1" << "name2" << "notes");
DataList DataList::morality = DataList(QStringList() << "name1" << "name2" << "notes");

// AbstractDataList -----------------------------------

AbstractDataList::AbstractDataList(QStringList columns) :
	iModel(NULL),
	iColumns(columns),
	iColNameToIndex()
{
	for (int i=0; i<columns.size(); i++)
		iColNameToIndex[columns.at(i).toUtf8()] = i;
}

QVariant AbstractDataList::getValue(int row, const char* col)
{
	return getValue(row, colIndex(col));
}

void AbstractDataList::setModel(ListModel* model)
{
	iModel = model;
}

/*
void AbstractDataList::startChanges()
{
	if (iModel)
		iModel->beginReset();
}

void AbstractDataList::endChanges()
{
	if (iModel)
		iModel->endReset();
}
*/

void AbstractDataList::dataChanged()
{
	if (iModel) {
		iModel->setDataChanged();
		iModel->setClean();
	}
}

void AbstractDataList::setDataChanged()
{
	if (iModel)
		iModel->setDataChanged();
}

void AbstractDataList::rowCountChanged()
{
	if (iModel) {
		iModel->setRowCountChanged();
		iModel->setClean();
	}
}

void AbstractDataList::setRowCountChanged()
{
	if (iModel)
		iModel->setRowCountChanged();
}

void AbstractDataList::setClean()
{
	if (iModel)
		iModel->setClean();
}

int AbstractDataList::colCount()
{
	return iColumns.length();
}

QString AbstractDataList::getColName(int col)
{
	if (col < 0 || col >= colCount())
		return QString();
	return iColumns.at(col);
}

QString AbstractDataList::getValueAsString(int row, const char* col)
{
	return getValue(row, col).toString();
}

bool AbstractDataList::getValueAsBool(int row, const char* col)
{
	return getValue(row, col).toBool();
}

int AbstractDataList::getValueAsInt(int row, const char* col)
{
	return getValue(row, col).toInt();
}

uint AbstractDataList::getValueAsUint(int row, const char* col)
{
	return getValue(row, col).toUInt();
}

int AbstractDataList::colIndex(const char* col)
{
	return iColNameToIndex.value(QByteArray(col), -1);
}

// DataRow -----------------------------------

DataRow::DataRow(int count)
{
	for (int i=0; i<count; i++)
		iValues.append(QVariant());
}

QVariant DataRow::getValue(int col)
{
	if (col >= 0 && col < iValues.length())
		return iValues.at(col);
	return QVariant();
}

void DataRow::setValue(int col, QVariant val)
{
	if (col >= 0 && col < iValues.length())
		iValues[col] = val;
}

// DataList -----------------------------------

DataList::DataList(QStringList columns) :
	AbstractDataList(columns),
	iRows()
{
}

int DataList::rowCount()
{
	return iRows.length();
}

int DataList::appendRow()
{
	iRows.append(DataRow(colCount()));
	return iRows.count() - 1;
}

void DataList::insertRow(int row)
{
	iRows.insert(row, DataRow(colCount()));
}

void DataList::deleteRow(int row)
{
	if (row >= 0 && row < rowCount())
		iRows.removeAt(row);
}

void DataList::clear()
{
	iRows.clear();
}

QVariant DataList::getValue(int row, int col)
{
	if (row < 0 || row >= rowCount())
		return QVariant();
	if (col < 0 || col >= colCount())
		return QVariant();
	return iRows[row].getValue(col);
}

int DataList::findRow(const char* col, const QString& value)
{
	int x = colIndex(col);
	int row;

	for (row=0; row<rowCount(); row++) {
		if (getValue(row, x).toString() == value)
			return row;
	}
	return -1;
}

int DataList::findRow(const char* col, int value)
{
	int x = colIndex(col);
	int row;

	for (row=0; row<rowCount(); row++) {
		if (getValue(row, x).toInt() == value)
			return row;
	}
	return -1;
}

bool DataList::findSortedRow(int& row, const char* col, const QString& value)
{
	int x = colIndex(col);

	for (row=0; row<rowCount(); row++) {
		QString rvalue = getValue(row, x).toString();
		if (rvalue == value)
			return true;
		if (rvalue > value)
			break;
	}
	return false;

}

void DataList::setValue(int row, int col, QVariant value)
{
	if (row < 0 || row >= rowCount())
		return;
	if (col < 0 || col >= colCount())
		return;
	iRows[row].setValue(col, value);
}

void DataList::setValue(int row, const char* col, const char* value)
{
	setValue(row, colIndex(col), QVariant(QString(value)));
}

void DataList::setValue(int row, const char* col, QString value)
{
	setValue(row, colIndex(col), QVariant(value));
}

void DataList::setValue(int row, const char* col, int value)
{
	setValue(row, colIndex(col), QVariant(value));
}

void DataList::setValue(int row, const char* col, uint value)
{
	setValue(row, colIndex(col), QVariant(value));
}

void DataList::setValue(int row, const char* col, bool value)
{
	setValue(row, colIndex(col), QVariant(value));
}

// ItemList ------------------------------------

ItemList::ItemList(QStringList columns) :
	AbstractDataList(columns),
	iList()
{
}

int ItemList::rowCount()
{
	return iList.size();
}

QVariant ItemList::getValue(int row, int col)
{
	if (col < 0 || col >= colCount())
		return QVariant();
	return getValue(row, iColumns[col].toUtf8().constData());
}

QVariant ItemList::getValue(int row, const char* col)
{
	Item item = itemAt(row);

	if (strcmp(col, "key") == 0)
		return item.key;
	if (strcmp(col, "name") == 0)
		return item.name();
	if (strcmp(col, "quantity") == 0)
		return item.quantity();
	if (strcmp(col, "stored") == 0)
		return item.stored();
	if (strcmp(col, "encumberance") == 0)
		return item.encumberance();
	if (strcmp(col, "carry_state") == 0)
		return item.state();
	if (strcmp(col, "restricted") == 0)
		return item.restricted();

	if (strcmp(col, "attachments") == 0) {
		return item.attachments;
	}
	if (strcmp(col, "notes") == 0)
		return item.notes;

	return QVariant();
}

void ItemList::clear()
{
	iList.clear();
	iSortedKeys.clear();
}

void ItemList::addItem(const Item& item)
{
	iList[item.key] = item;
	iSortedKeys.clear();
}

void ItemList::removeItem(const QString& key)
{
	iList.remove(key);
	iSortedKeys.clear();
}

Item ItemList::itemAt(int row)
{
	if (row < 0 || row >= rowCount())
		return Item();
	if (iSortedKeys.size() == 0) {
		QMapIterator<QString, Item> i(iList);

		while (i.hasNext()) {
			i.next();
			addSortedItem(i.value());
		}
	}
	return iList[iSortedKeys[row]];
}

bool ItemList::contains(const QString& key)
{
	return iList.contains(key);
}

bool ItemList::equipped(const QString& key)
{
	Item i = getItem(key);
	return i.equipped();
}

int ItemList::carried(const QString& key)
{
	Item i = getItem(key);
	return i.carriedQuantity();
}

Item ItemList::getItem(const QString& key)
{
	if (iList.contains(key))
		return iList[key];
	return Item();
}

bool ItemList::changeEquipment(const QString& key, int state, int stored)
{
	if (iList.contains(key)) {
		Item item = iList[key];

		if (state < NOT_CARRIED || state > IS_EQUIPPED)
			state = NOT_CARRIED;
		if (state == item.originalState())
			state = UNDEFINED;
		Character::instance->currentData()->storeItem(key, stored, state, false);
		setDataChanged();
		return true;
	}
	return false;
}

void ItemList::addSortedItem(const Item& item)
{
	for (int i=0; i<iSortedKeys.size(); i++) {
		Item b = iList[iSortedKeys[i]];
		if (b.name() > item.name()) {
			iSortedKeys.insert(i, item.key);
			return;
		}
	}
	iSortedKeys.append(item.key);
}
