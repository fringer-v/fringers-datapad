#include <string.h>

#include "DataList.h"
#include "Character.h"

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

ItemList::ItemList() :
	iUuidList(),
	iItemKeyList()
{
}

int ItemList::rowCount()
{
	return iUuidList.size();
}

/*
QVariant ItemList::getValue(int row, int col)
{
	if (col < 0 || col >= colCount())
		return QVariant();
	return getValue(row, iColumns[col].toUtf8().constData());
}

QVariant ItemList::getValue(int row, const char* col)
{
	Item item = itemAt(row);

	if (strcmp(col, "uuid") == 0)
		return item.uuid;
	if (strcmp(col, "itemkey") == 0)
		return item.itemkey;
	if (strcmp(col, "name") == 0)
		return item.name();
	if (strcmp(col, "quantity") == 0)
		return item.quantity();
	if (strcmp(col, "stored") == 0)
		return item.stored();
	if (strcmp(col, "encumbrance") == 0)
		return item.encumbrance();
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
*/

void ItemList::clear()
{
	iUuidList.clear();
	iItemKeyList.clear();
	iSortedKeys.clear();
}

void ItemList::aquireItem(Item& item, bool loading)
{
	if (item.itemkey.isEmpty()) {
		if (item.uuid.isEmpty()) {
			qDebug() << "NO!";
			return;
		}

		// For example UNARMED!
		item.itemkey = item.uuid;
	}

	if (item.unmodified())
		// If unmodified, store as generic:
		item.uuid = item.itemkey;
	else if (item.uuid.isEmpty())
		item.uuid = DatUtil::genUuid();

	if (iUuidList.contains(item.uuid)) {
		if (loading) {
			int current_state = iUuidList[item.uuid].originalState;
			if (item.originalState > current_state)
				iUuidList[item.uuid].originalState = item.originalState;

			if (current_state == NOT_CARRIED) {
				if (item.originalState != NOT_CARRIED)
					iUuidList[item.uuid].originalStored = iUuidList[item.uuid].originalQuantity;
			}
			else {
				// Some items are being carried
				if (item.originalState == NOT_CARRIED)
					// These items are not carried:
					iUuidList[item.uuid].originalStored += item.originalQuantity;
			}

			iUuidList[item.uuid].originalQuantity += item.originalQuantity;

			if (item.notes.length() > iUuidList[item.uuid].notes.length())
				iUuidList[item.uuid].notes = item.notes;
		}
	}
	else {
		iUuidList[item.uuid] = item;
		iItemKeyList[item.itemkey].append(item.uuid);
		iSortedKeys.clear();
	}
}

void ItemList::removeItemByUuid(const QString& uuid)
{
	if (iUuidList.contains(uuid)) {
		QString itemkey = iUuidList[uuid].itemkey;
		iUuidList.remove(uuid);
		iItemKeyList[itemkey].removeOne(uuid);
		iSortedKeys.clear();
	}
}

Item ItemList::itemAt(int row)
{
	if (row < 0 || row >= rowCount())
		return Item();
	if (iSortedKeys.size() == 0) {
		QMapIterator<QString, Item> i(iUuidList);

		while (i.hasNext()) {
			i.next();
			addSortedItem(i.value());
		}
	}
	return iUuidList[iSortedKeys[row]];
}

bool ItemList::containsByUuid(const QString& uuid)
{
	return iUuidList.contains(uuid);
}

bool ItemList::equipped(const QString& key)
{
	QListIterator<QString> i(getItemByKey(key));

	while (i.hasNext()) {
		if (iUuidList[i.next()].equipped())
			return true;
	}
	return false;
}

int ItemList::carriedQuantity(const QString& key)
{
	int count = 0;
	QListIterator<QString> i(getItemByKey(key));

	while (i.hasNext()) {
		count += iUuidList[i.next()].carriedQuantity();
	}
	return count;
}

int ItemList::quantity(const QString& key)
{
	int count = 0;
	QListIterator<QString> i(getItemByKey(key));

	while (i.hasNext()) {
		count += iUuidList[i.next()].quantity();
	}
	return count;
}

Item ItemList::getItemByUuid(const QString& uuid)
{
	if (iUuidList.contains(uuid))
		return iUuidList[uuid];
	return Item();
}

QStringList& ItemList::getItemByKey(const QString& key)
{
	static QStringList empty;
	if (iItemKeyList.contains(key))
		return iItemKeyList[key];
	return empty;
}

bool ItemList::changeEquipment(const QString& uuid, int state, int stored)
{
	if (iUuidList.contains(uuid)) {
		Item item = iUuidList[uuid];

		CurrentData::instance->storeItem(uuid, item.itemkey, stored, state, &item);
		return true;
	}
	return false;
}

void ItemList::addSortedItem(const Item& item)
{
	for (int i=0; i<iSortedKeys.size(); i++) {
		Item b = iUuidList[iSortedKeys[i]];
		if (b.name() > item.name()) {
			iSortedKeys.insert(i, item.uuid);
			return;
		}
	}
	iSortedKeys.append(item.uuid);
}
