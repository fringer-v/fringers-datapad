/* Copyright (c) 2011, Paul McCullagh (Fringer)
 *
 * Datapad
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, please go to
 * http://www.gnu.org/licenses/gpl-2.0.html for further details.
 *
 * 2011-12-07 Paul McCullagh
 *
 * H&G2JCtL
 *
 */

#include <string.h>
#include <sys/types.h>
#include <utime.h>

#include "CurrentData.h"
#include "DataAccess.h"
#include "DatUtil.h"
#include "Character.h"
#include "Gear.h"
#include "Armor.h"
#include "Weapons.h"
#include "CharacterList.h"

// CheckListData -------------------------

void CheckListData::clear()
{
	iRefCount = 0;
	items.clear();
}

CheckListItem* CheckListData::findItem(int reference)
{
	QMutableListIterator<CheckListItem> i(items);
	CheckListItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference == reference) {
			return item;
		}
	}
	return NULL;
}

void CheckListData::checkAll(bool checked)
{
	QMutableListIterator<CheckListItem> i(items);
	CheckListItem* item;

	while (i.hasNext()) {
		item = &i.next();
		item->checked = checked;
	}
}

void CheckListData::deleteItem(int reference)
{
	QMutableListIterator<CheckListItem> i(items);
	CheckListItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference == reference) {
			i.remove();
		}
	}
}

QString CheckListData::getCurrentPool()
{
	QListIterator<CheckListItem> i(items);
	const CheckListItem* item;
	QString curr_pool;

	while (i.hasNext()) {
		item = &i.next();
		if (item->checked) {
			if (item->dice.startsWith("@"))
				curr_pool += item->dice + "@";
			else
				curr_pool += item->dice;
		}
	}
	return curr_pool;
}

bool CheckListData::hasCustomItem() const
{
	QListIterator<CheckListItem> i(items);
	const CheckListItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference > 0)
			return true;
	}
	return false;
}

int CheckListData::strainUsed(QString& consumable) const
{
	QListIterator<CheckListItem> i(items);
	const CheckListItem* item;
	int move = 0;
	int strain = 0;

	while (i.hasNext()) {
		item = &i.next();
		if (item->checked) {
			strain += item->reduceStrain;
			move += item->moveRequired;
			if (!item->consumable.isEmpty())
				consumable = item->consumable;
		}
	}

	if (move > 1)
		strain += (move-1) * 2;
	return strain;
}

int CheckListData::movesUsed() const
{
	QListIterator<CheckListItem> i(items);
	const CheckListItem* item;

	int move = 0;

	while (i.hasNext()) {
		item = &i.next();
		if (item->checked)
			move += item->moveRequired;
	}

	return move;
}

int CheckListData::addItem(const QString& pool, const QString& desc, int move, int strain, int dpoint,
	const QString& cons, const QString& commit_key, int force_cost)
{
	CheckListItem item;
	QString cost;

	if (move) {
		if (move == 1)
			cost = "Maneuver";
		else
			cost = QString("%1 Maneuvers").arg(move);
	}
	if (strain) {
		if (!cost.isEmpty())
			cost += ", ";
		cost += QString("%1 Strain").arg(strain);
	}
	if (dpoint) {
		if (!cost.isEmpty())
			cost += ", ";
		if (dpoint == 1)
			cost += "Destiny Point";
		else
			cost += QString("%1 Destiny Points").arg(dpoint);
	}

	iRefCount--;
	item.reference = iRefCount;
	item.dice = pool;
	if (cost.isEmpty())
		item.description = desc;
	else
		item.description = QString("%1, [B]Cost: %2[b]").arg(desc).arg(cost);
	item.moveRequired = move;
	item.reduceStrain = strain;
	item.consumable = cons;
	item.commitKey = commit_key;
	item.forceCost = force_cost;
	items.append(item);
	return iRefCount;
}

void CheckListData::addCustomItem(int ref, const QString& pool, const QString& desc)
{
	CheckListItem item;

	item.reference = ref;
	item.dice = pool;
	item.description = desc;
	items.append(item);
}

// CurrentData -------------------------

void CurrentData::clear()
{
	wounds = 0;
	strain = 0;
	temporaryStrain = 0;
	conflict = 0;
	woundHistory.clear();
	strainHistory.clear();
	conflictHistory.clear();
	woundDelta = 0;
	strainDelta = 0;
	locked = 0;
	hideAutoTalents = 0;
	invMod.clear();
	inventoryLog.clear();
	stimPacksUsed = 0;
	erpsUsed = 0;
	customSkills.clear();
	checkLists.clear();
	forceCommitted.clear();
	dutyRank = 0;
	experienceLog.clear();
	experienceTotal.clear();

	iFileName.clear();

	iNextInjuryID = 0;
	iInjuryPercent = 0;

	iExpNextRef = 0;
	iExpItem.clear();

	iNextItem = 0;
	iItemCount = 0;
	iItemKey.clear();
	iItemDesc.clear();
	iItemAmount = 0;

	iNextCheckItem = 0;
	iItemPool.clear();

	iNegativeCheck = 0;
	iNegMelee = NEG_CHECK_2_PURPLE;
	iNegRanged = NEG_CHECK_2_PURPLE;
}

QString CurrentData::getFile()
{
	return iFileName;
}

void CurrentData::loadCurrentData(const QString& name)
{
	clear();

	DataList::injuries.clear();
	for (int i=0; i<4; i++)
		appendEmptyWound();

	if (name.isEmpty())
		iFileName.clear();
	else {
		iFileName = name.toLower();
		iFileName.replace(" ", "-");
		iFileName.replace("\"", "-");
		while (iFileName.contains("--"))
			iFileName.replace("--", "-");

		if (QFile().exists(DatUtil::getCurrentFolder() + iFileName + ".tri"))
			iFileName += ".tri";
		else
			iFileName += ".xpad";
		QByteArray data = DataAccess::getCurrentData(iFileName);
		readFromBuffer(data.constData(), data.length());
	}

	QMap<QString, InvModItem>::iterator i;
	for (i = invMod.begin(); i != invMod.end(); i++) {
		changeInventory(i.key(), false);
	}
	Character::instance->inventoryChanged();
	Character::instance->experienceChanged();
	Talents::instance.setRowCountChanged();
}

bool CurrentData::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/Wounds/"))
		wounds = toInt(value);
	else if (path.endsWith("/Strain/"))
		strain = toInt(value);
	else if (path.endsWith("/Conflict/"))
		conflict = toInt(value);
	else if (path.endsWith("/StimPacksUsed/"))
		stimPacksUsed = toInt(value);
	else if (path.endsWith("/ErpsUsed/"))
		erpsUsed = toInt(value);
	else if (path.endsWith("/WoundHistory/"))
		woundHistory = value;
	else if (path.endsWith("/StrainHistory/"))
		strainHistory = value;
	else if (path.endsWith("/ConflictHistory/"))
		conflictHistory = value;
	else if (path.endsWith("/WoundDelta/"))
		woundDelta = toInt(value);
	else if (path.endsWith("/StrainDelta/"))
		strainDelta = toInt(value);
	else if (path.endsWith("/Locked/"))
		locked = toInt(value);
	else if (path.endsWith("/HideAutoTalents/"))
		hideAutoTalents = toInt(value);
	else if (path.endsWith("/Injury/Percent/"))
		iInjuryPercent = toInt(value);
	else if (path.endsWith("/Injury/Type/"))
		setCriticalWound(iInjuryPercent, toInt(value));
	else if (path.endsWith("/XP/#open")) {
		iExpItem.clear();
		iExpItem.type = EXP_XP;
	}
	else if (path.endsWith("/XP/Type/"))
		iExpItem.type = expTypeToInt(value);
	else if (path.endsWith("/XP/Key/"))
		iExpItem.key = value;
	else if (path.endsWith("/XP/Name/"))
		iExpItem.name = value;
	else if (path.endsWith("/XP/Description/"))
		iExpItem.desc = value;
	else if (path.endsWith("/XP/Amount/"))
		iExpItem.amount = toInt(value);
	else if (path.endsWith("/XP/#end"))
		setExpLogItem(iExpItem.type, iExpItem.key, iExpItem.name, iExpItem.desc, iExpItem.amount, true);
	else if (path.endsWith("/Inventory/Item/#open")) {
		iItemCount = 0;
		iItemKey.clear();
		iItemDesc.clear();
		iItemAmount = 0;
	}
	else if (path.endsWith("/Item/Count/"))
		iItemCount = toInt(value);
	else if (path.endsWith("/Item/Key/"))
		iItemKey = value;
	else if (path.endsWith("/Item/Description/"))
		iItemDesc = value;
	else if (path.endsWith("/Item/Amount/"))
		iItemAmount = toInt(value);
	else if (path.endsWith("/Inventory/Item/#end"))
		setInvLogItem(iItemCount, iItemKey, iItemDesc, iItemAmount, true);

	else if (path.endsWith("/StoreItem/Key/"))
		iStoreItemKey = value;
	else if (path.endsWith("/StoreItem/Amount/"))
		iStoreItemAmount = toInt(value);
	else if (path.endsWith("/StoreItem/State/"))
		iStoreItemState = toInt(value);
	else if (path.endsWith("/Storage/StoreItem/#end"))
		storeItem(iStoreItemKey, iStoreItemAmount, iStoreItemState, true);

	else if (path.endsWith("/CurrentData/CheckList/#open"))
		iItemSkill.clear();
	else if (path.endsWith("/CheckList/Skill/"))
		iItemSkill = value;
	else if (path.endsWith("/CheckList/CheckItem/#open")) {
		iItemPool.clear();
		iItemDesc.clear();
	}
	else if (path.endsWith("/CheckItem/DicePool/"))
		iItemPool = value;
	else if (path.endsWith("/CheckItem/Description/"))
		iItemDesc = value;
	else if (path.endsWith("/CheckList/CheckItem/#end"))
		setCheckItem(iItemSkill, iItemPool, iItemDesc);

	else if (path.endsWith("/CustomSkills/Skill/#open")) {
		iSkillName.clear();
		iSkillChar.clear();
		iSkillRank = 0;
	}
	else if (path.endsWith("/Skill/Name/"))
		iSkillName = value;
	else if (path.endsWith("/Skill/Char/"))
		iSkillChar = value;
	else if (path.endsWith("/Skill/Rank/"))
		iSkillRank = toInt(value);
	else if (path.endsWith("/CustomSkills/Skill/#end"))
		addCustomSkill(iSkillName, iSkillChar, iSkillRank, true);

	return true;
}

void CurrentData::adjustWounds(int delta)
{
	changeWounds(delta);
	writeCurrentData();
}

void CurrentData::adjustStrain(int delta)
{
	changeStrain(delta);
	writeCurrentData();
}

void CurrentData::adjustConflict(int delta)
{
	if (!delta)
		return;
	conflict += delta;
	DatUtil::appendToList(conflictHistory, delta > 0 ? QString("+%1").arg(delta) : QString::number(delta), " ");
	while (conflictHistory.length()  > MAX_HISTORY_LEN)
		conflictHistory = DatUtil::right(conflictHistory, " ");
	writeCurrentData();
}

void CurrentData::useStimPack(int delta)
{
	if (!delta)
		return;
	if (delta > 0) {
		int dst;

		if (stimPacksUsed + erpsUsed >= 5)
			return;
		dst = 5 - (stimPacksUsed + erpsUsed);
		stimPacksUsed++;
		adjustWounds(-dst);
		if (stimPackQuantity() > 0) {
			if (delta > stimPackQuantity())
				delta = stimPackQuantity();
			addItem(-delta, "STIMPACK", QString(), 0);
		}
	}
	else {
		stimPacksUsed += delta;
		if (stimPacksUsed < 0)
			stimPacksUsed = 0;
	}
	writeCurrentData();
}

void CurrentData::useErp(int delta)
{
	if (!delta)
		return;
	if (delta > 0) {
		if (stimPacksUsed + erpsUsed >= 5)
			return;
		erpsUsed++;
		adjustWounds(-3);
		if (erpQuantity() > 0) {
			if (delta > erpQuantity())
				delta = erpQuantity();
			addItem(-delta, "ERP", QString(), 0);
		}
	}
	else {
		erpsUsed += delta;
		if (erpsUsed < 0)
			erpsUsed = 0;
	}
	writeCurrentData();
}

QString CurrentData::stimPacks()
{
	int quan = Gear::instance.getItem("STIMPACK").quantity();
	return QString("%1/%2").arg(Gear::instance.getItem("STIMPACK").carriedQuantity()).arg(quan);
}

QString CurrentData::erps()
{
	int quan = Gear::instance.getItem("ERP").quantity();
	return QString("%1/%2").arg(Gear::instance.getItem("ERP").carriedQuantity()).arg(quan);
}

int CurrentData::stimPackQuantity()
{
	return Gear::instance.getItem("STIMPACK").quantity();
}

int CurrentData::erpQuantity()
{
	return Gear::instance.getItem("ERP").quantity();
}

bool CurrentData::setWoundDelta(int val)
{
	if (woundDelta != val) {
		woundDelta = val;
		writeCurrentData();
		return true;
	}
	return false;
}

bool CurrentData::setStrainDelta(int val)
{
	if (strainDelta != val) {
		strainDelta = val;
		writeCurrentData();
		return true;
	}
	return false;
}

bool CurrentData::setLocked(int val)
{
	if (locked != val) {
		locked = val;
		writeCurrentData();
		return true;
	}
	return false;
}

bool CurrentData::setHideCodedTalents(int val)
{
	if (hideAutoTalents != val) {
		hideAutoTalents = val;
		writeCurrentData();
		return true;
	}
	return false;
}

void CurrentData::addCriticalWound(int perc, int type)
{
	//DataList::injuries.startChanges();
	setCriticalWound(perc, type);
	DataList::injuries.rowCountChanged();
	writeCurrentData();
}

void CurrentData::removeCriticalWound(int ref)
{
	//DataList::injuries.startChanges();
	for (int i=0; i<DataList::injuries.rowCount(); i++) {
		if (DataList::injuries.getValueAsInt(i, "ref") == ref) {
			DataList::injuries.deleteRow(i);
			break;
		}
	}
	DataList::injuries.rowCountChanged();
	writeCurrentData();
}

void CurrentData::addExperience(const QString& stype, const QString& key, const QString& name, const QString& desc, int amount)
{
	int type = expTypeToInt(stype.toUtf8().constData());

	setExpLogItem(type, key, name, desc, amount, false);
	writeCurrentData();
	Character::instance->experienceChanged();
	ExperienceList::instance.rowCountChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::changeExperience(int ref, const QString& desc, int amount)
{
	int idx = findExpLogItem(ref);
	if (idx < 0)
		return;

	ExpLogItem item = experienceLog[idx];
	QString type = expTypeToString(item.type, item.key);

	experienceLog[idx].desc = desc;
	experienceLog[idx].amount = amount;
	experienceTotal[type].value += amount - item.amount;

	writeCurrentData();
	Character::instance->experienceChanged();
	ExperienceList::instance.dataChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::removeExperience(int ref)
{
	int idx = findExpLogItem(ref);
	if (idx < 0)
		return;

	ExpLogItem item = experienceLog[idx];
	QString type = expTypeToString(item.type, item.key);
	ExpLogTotal tot = experienceTotal[type];

	// May only delete first item, if it is the last:
	if (idx == tot.firstItem && tot.itemCount > 1)
		return;

	// Find the next last item:
	if (idx == tot.lastItem) {
		int i;

		for (i=idx-1; i>tot.firstItem; i--) {
			ExpLogItem item2 = experienceLog[i];
			QString type2 = expTypeToString(item2.type, item2.key);
			if (type == type2)
				break;
		}
		tot.lastItem = i;
	}

	experienceLog.removeAt(idx);
	tot.value -= item.amount;
	tot.itemCount--;
	if (tot.itemCount == 0)
		experienceTotal.remove(type);
	else
		experienceTotal[type] = tot;

	// Adjust all:
	QMutableMapIterator<QString, ExpLogTotal> i(experienceTotal);
	while (i.hasNext()) {
		i.next();
		if (idx < i.value().firstItem)
			i.value().firstItem--;
		if (idx < i.value().lastItem)
			i.value().lastItem--;
	}

	writeCurrentData();
	Character::instance->experienceChanged();
	ExperienceList::instance.rowCountChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::addCustomSkill(const QString& name, const QString& charac, int ranks, bool loading)
{
	if (name.isEmpty() || charac.isEmpty())
		return;

	CustomSkill skill;

	skill.name = name;
	skill.characteristic = charac;
	skill.rank = ranks;
	customSkills.append(skill);
	if (!loading) {
		CustomSkills::instance.dataChanged();
		writeCurrentData();
	}
}

void CurrentData::removeCustomSkill(const QString& name)
{
	for (int i=0; i<customSkills.size(); i++) {
		if (customSkills[i].name == name) {
			customSkills.removeAt(i);
			break;
		}
	}
	CustomSkills::instance.dataChanged();
	writeCurrentData();
}

void CurrentData::addItem(int count, const QString& key, const QString& desc, int amount)
{
	InventoryLog::instance.setRowCountChanged();
	setInvLogItem(count, key, desc, amount, false);
	InventoryLog::instance.setClean();
	writeCurrentData();
	changeInventory(key, true);
	Character::instance->inventoryChanged();
}

void CurrentData::updateItem(int ref, int count, const QString& desc, int amount)
{
	int row = findInvLogItem(ref);
	if (row < 0)
		return;
	InvLogItem item = inventoryLog[row];
	if (item.type != ITEM_AMOUNT)
		return;

	int old_count = item.count;
	int old_amount = item.amount;

	if (!item.key.isEmpty()) {
		if (!count)
			count = 1;
		if (count > 0) {
			if (amount > 0)
				amount = -amount;
		}
		else {
			if (amount < 0)
				amount = -amount;
		}
		if (invMod.contains(item.key) && invMod[item.key].quantity != UNKNOWN_QUANTITY)
			invMod[item.key].quantity += count - old_count;
	}
	else if (!desc.isEmpty()) {
		// Free text has no count:
		count = 0;
	}
	else
		return;
	InventoryLog::instance.setRowCountChanged();
	inventoryLog[row].count = count;
	inventoryLog[row].desc = desc;
	inventoryLog[row].amount = amount;
	if (amount - old_amount) {
		int total = Character::instance->credits();
		Character::instance->setCredits(total + (amount - old_amount));
	}
	InventoryLog::instance.setClean();
	writeCurrentData();
	if (count - old_count) {
		changeInventory(item.key, true);
		Character::instance->inventoryChanged();
	}
}

bool CurrentData::removeItem(int ref)
{
	int row = findInvLogItem(ref);
	if (row < 0)
		return false;
	InvLogItem item = inventoryLog[row];
	if (item.type != ITEM_AMOUNT && item.type != ITEM_ORIG_STOCK)
		return false;

	if (!item.key.isEmpty() && invMod.contains(item.key)) {
		ShopItem	shop_item;
		ItemList*	list;
		int			quantity = 0;

		if (item.type == ITEM_ORIG_STOCK && invMod[item.key].rowCount > 1)
			return false;

		shop_item = Shop::instance.getItem(item.key);
		if (shop_item.type == "GEAR")
			list = &Gear::instance;
		else if (shop_item.type == "ARMOR")
			list = &Armor::instance;
		else
			list = &Weapons::instance;

		invMod[item.key].rowCount--;
		if (invMod[item.key].rowCount == 0) {
			if (invMod[item.key].stored > 0 || invMod[item.key].state != UNDEFINED)
				invMod[item.key].quantity = UNKNOWN_QUANTITY;
			else
				invMod.remove(item.key);
		}
		else if (invMod[item.key].quantity != UNKNOWN_QUANTITY) {
			invMod[item.key].quantity -= item.count;
			quantity = invMod[item.key].quantity;
		}

		if (quantity == 0) {
			if (list->contains(item.key)) {
				if (list->getItem(item.key).originalQuantity() == 0) {
					list->removeItem(item.key);
					list->setRowCountChanged();
					list->setClean();
				}
			}
		}
		else {
			if (!list->contains(item.key)) {
				Item new_item;
				new_item.clear(item.key);
				list->addItem(new_item);
				list->setRowCountChanged();
				list->setClean();
			}
		}
	}

	InventoryLog::instance.setRowCountChanged();
	inventoryLog.removeAt(row);
	if (inventoryLog.size() == 1) {
		inventoryLog.clear();
		Character::instance->setCredits(Character::instance->originalCredits);
	}
	else if (item.amount) {
		int total = Character::instance->credits();
		Character::instance->setCredits(total - item.amount);
	}
	writeCurrentData();
	InventoryLog::instance.setClean();
	changeInventory(item.key, true);
	Character::instance->inventoryChanged();
	return true;
}

void CurrentData::storeItem(const QString& key, int count, int state, bool loading)
{
	if (!invMod.contains(key)) {
		invMod[key].key = key;
		invMod[key].quantity = UNKNOWN_QUANTITY;
	}
	invMod[key].stored = count;
	invMod[key].state = state;
	if (!loading)
		writeCurrentData();
	if (key == "STIMPACK")
		Character::instance->emitStimPacksChanged();
	else if (key == "ERP")
		Character::instance->emitErpsChanged();
}

void CurrentData::clearAutoCheckItems(const QString& skillKey)
{
	uncheckAllItem(skillKey);
	autoCheckItems.clear();
}

struct CheckItem {
	const char* talentKey;
	const char* skillKey;
	const char* skillKey2;
	const char* pool;
	const char* desc;
	int			move;
	int			strain;
	int			dpoint;
	int			default_check;
};

struct TalentCheck {
	QList<CheckItem*> checks;
};

static QMap<QString, TalentCheck> talentChecks;

extern CheckItem check_items[];

static void setup_talent_checks()
{
	if (!talentChecks.isEmpty())
		return;

	for (int i=0;; i++) {
		if (!check_items[i].talentKey)
			break;
		if (check_items[i].skillKey && *check_items[i].skillKey)
			talentChecks[check_items[i].skillKey].checks.append(&check_items[i]);
		if (check_items[i].skillKey2 && *check_items[i].skillKey2)
			talentChecks[check_items[i].skillKey2].checks.append(&check_items[i]);
	}
}

void CurrentData::setupAutoCheckItems(const QString& skillKey, const QString& weaponKey)
{
	bool combat = false;
	bool ranged = false;
	bool targeting_comp = false;
	bool integrated_scanner = false;
	bool mount_armor = false;
	bool hunting_goggles = false;
	bool eos = false;
	bool piloting = false;
	bool charm = false;
	Skill* skill = Skill::getSkill(skillKey);
	QString send_mag = "once";
	int ranks;
	int force;
	int default_check = 0;

	uncheckAllItem(skillKey);
	autoCheckItems.clear();

	if (Gear::instance.equipped("HNTGOGGLE"))
		hunting_goggles = true;

	if (Character::instance->talents.contains("SENSEDURATION"))
		send_mag = "twice";

	if (skillKey == "RANGHVY" || skillKey == "RANGLT" || skillKey == "GUNN") {
		combat = true;
		ranged = true;
	}
	else if (skillKey == "BRAWL" || skillKey == "MELEE" || skillKey == "LTSABER") {
		combat = true;
	}
	else if (skillKey == "CHARM")
		charm = true;

	if (!skill || skill->type != SPECIAL)
		autoCheckItems.addItem("@+1 Move", "[B]Maneuver:[b] Perform an action that requires a maneuver", 1, 0);

	for (int i=0; i<Character::instance->talents.ranks("BRA"); i++)
		autoCheckItems.addItem("N", "[B]Brace:[b] Remove [SE] if due to any environmental effects, Duration: 1 Round", i == 0 ? 1 : 0, 0);

	if (combat) {
		if (isCommitted("SENSECONTROL3")) {
			QString send_str = "U";
			if (Character::instance->talents.contains("SENSESTRENGTH"))
				send_str = "UU";
			default_check = autoCheckItems.addItem(send_str, QString("[B]Sense Control:[b] Upgrade ability of combat check %1 per round").arg(send_mag), 0, 0);
		}
	}

	if (charm || skillKey == "COERC" || skillKey == "NEG" ||
		skillKey == "DECEP" || skillKey == "LEAD") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("INFLUENCECONTROL2"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Influence:[b] Spend [LI] to gain [SU] or [AD] vs 1 engaged target, "+Character::instance->talents.forceUpgrades("INFLUENCECONTROL2", RAN | MAG), 0, 0);
		if (Character::instance->talents.contains("JUSTKID"))
			autoCheckItems.addItem("-y", "[B]Just Kidding:[b] Once per round, Target: self or ally", 0, 0, 1);
	}
	else if (skillKey == "REC") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("BAL"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Balance:[b] Gain +1 Strain per [LI]", 0, 0);
	}
	else if (skillKey == "ATHL") {
		if (Character::instance->talents.contains("NIKTOSP2OC2OP1"))
			autoCheckItems.addItem("B", "[B]Climbing Claws:[b] Add when climbing trees and other surfaces their claws can pierce", 0, 0);
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCEBASIC"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "COORD") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCECONT1"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "RESIL") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCECONT2"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "PILOTPL") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCECONT4"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "BRAWL") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCECONT5"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "PILOTSP") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("ENHANCECONT7"))
			default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Enhance:[b] Spend [LI] to gain [SU] or [AD]", 0, 0);
	}
	else if (skillKey == "ICOOL" || skillKey == "IVIG") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.contains("FORSEECONTROL1")) {
			if (Character::instance->talents.contains("FORSEECONTROL3"))
				default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Foresee:[b] Spend [LI] to gain [SU], spend [LI] to grant free move to targets", 0, 0);
			else
				default_check = autoCheckItems.addItem(QString("F").repeated(force), "[B]Foresee:[b] Spend [LI] to gain [SU] on initiative check", 0, 0);
		}
		for (int i=0; i<Character::instance->talents.ranks("RAPREA"); i++)
			autoCheckItems.addItem("s", "[B]Rapid Reaction:[b] Suffer straing to improve initiative", 0, 1);
	}
	else if (skillKey == "STEAL") {
		ranks = Character::instance->talents.ranks("SLEIGHTMIND");
		if (ranks > 0)
			autoCheckItems.addItem(QString("B").repeated(ranks), "[B]Sleight of Mind:[b] Add if opposition is not immune to Force powers", 0, 0);
	}
	else if (skillKey == "LTSABER") {
		force = Character::instance->nonCommitedForce();
		if (force > 0 && Character::instance->talents.ranks("HAWKSWOOP"))
			autoCheckItems.addItem(QString("F").repeated(force), "[B]Hawk Bat Swoop:[b] Spend [LI] to engage target, and [LI] to add [AD]", 0, 0);

		if (force > 0 && Character::instance->talents.contains("SABERSW"))
			autoCheckItems.addItem(QString("@Linked %1").arg(force), QString("[B]Saber Swarm:[b] Attack has linked %1 quality").arg(force), 1, 1);
	}
	else if (skillKey == "DISC") {
		force = Character::instance->nonCommitedForce();
		if (force > 0) {
			if (Character::instance->talents.contains("INFLUENCECONTROL1"))
				autoCheckItems.addItem(QString("F").repeated(force), "[B]Influence:[b] vs Discipline, "+Character::instance->talents.forceUpgrades("INFLUENCECONTROL1", MAG | RAN | DUR), 0, 0);
			if (Character::instance->talents.contains("MOVECONTROL1")) {
				QString type = "Hurl";

				if (Character::instance->talents.contains("MOVECONTROL2"))
					DatUtil::appendToList(type, "Pull", "/");

				if (Character::instance->talents.contains("MOVECONTROL3"))
					DatUtil::appendToList(type, "Manipulate", "/");

				autoCheckItems.addItem(QString("F").repeated(force), "[B]Move:[b] "+type+" objects, "+Character::instance->talents.forceUpgrades("INFLUENCECONTROL1", MAG | RAN | DUR), 0, 0);
			}
		}
	}

	if (!skill || skill->type != SPECIAL) {
		if (skillKey != "FORCE" && Character::instance->talents.contains("INTENSFOC"))
			autoCheckItems.addItem("U", "[B]Intense Focus:[b] Upgrade the next skill check once", 1, 1);
	}

	if (skillKey == "COERC") {
		for (int i=0; i<Character::instance->talents.ranks("INTIM"); i++) {
			autoCheckItems.addItem("d", "[B]Intimidating:[b] Downgrade the difficulty of the check", 0, 1);
		}
	}
	else if (charm || skillKey == "NEG") {
		for (int i=0; i<Character::instance->talents.ranks("CONGENIAL"); i++) {
			autoCheckItems.addItem("d", "[B]Congenial:[b] Downgrade the difficulty of the check", 0, 1);
		}
	}
	else if (skillKey == "DEFM" || skillKey == "DEFR") {
		if (isCommitted("SENSECONTROL1")) {
			QString send_str = "u";
			if (Character::instance->talents.contains("SENSESTRENGTH"))
				send_str = "uu";
			default_check = autoCheckItems.addItem(send_str, QString("[B]Sense Control:[b] Upgrade difficulty of incoming attack %1 per round").arg(send_mag), 0, 0);
		}

		for (int i=0; i<Character::instance->talents.ranks("DODGE"); i++)
			autoCheckItems.addItem("u", "[B]Dodge:[b] Out-of-turn incedental", 0, 1);

		ranks = Character::instance->talents.ranks("DURA");
		if (ranks > 0)
			autoCheckItems.addItem(QString("@-%1% Crits").arg(ranks*10), "[B]Durable:[b] Reduce critical hits", 0, 0);

		if (skillKey == "DEFM") {
			ranks = Character::instance->talents.ranks("PARRY");
			if (ranks > 0) {
				QString par = QString("[B]Parry:[b] Reduce damage by %1").arg(ranks+2);

				if (Character::instance->talents.contains("PARRYIMP"))
					par = par + ", on [DE] or [TH][TH][TH] hit attacker";
				autoCheckItems.addItem(QString("@-%1 Damage").arg(ranks+2), par, 0, 3);
			}

			for (int i=0; i<Character::instance->talents.ranks("DEFSTA"); i++)
				autoCheckItems.addItem("u", "[B]Defensive Stance:[b] Perform during turn, duration: 1 round", i == 0 ? 1 : 0, 1);

			setNegativePool(iNegMelee, skillKey);
		}
		else {
			ranks = Character::instance->talents.ranks("REFLECT");
			if (ranks > 0) {
				QString par = QString("[B]Reflect:[b] vs Lightsaber, reduce damage by %1").arg(ranks+2);

				if (Character::instance->talents.contains("REFLECTIMP"))
					par = "[B]Reflect:[b] vs Lightsaber, on [DE] or [TH][TH][TH] damage apponent (Medium range)";
				autoCheckItems.addItem(QString("@-%1 Damage").arg(ranks+2), par, 0, 3);
			}

			for (int i=0; i<Character::instance->talents.ranks("SIDESTEP"); i++)
				autoCheckItems.addItem("u", "[B]Side Step:[b] Perform during turn, duration: 1 round", i == 0 ? 1 : 0, 1);

			setNegativePool(iNegRanged, skillKey);
		}

		if (Character::instance->talents.contains("SENSEADV")) {
			autoCheckItems.addItem("SS", "[B]Sense Advantage:[b] Add to NPC check, once per session", 0, 0);
		}
	}
	else if (skillKey == "PILOTSP") {
		piloting = true;
		if (Character::instance->talents.contains("MASPIL"))
			autoCheckItems.addItem("@+1 Action", QString("[B]Master Pilot:[b] Once per round, perform Action as Manuever"), 1, 2);
	}
	else if (skillKey == "FORCE") {
		int ref;

		if (Character::instance->talents.contains("SENSECONTROL1")) {
			ref = autoCheckItems.addItem("g", "[B]Sense Control:[b] Commit force dice to upgrade difficulty of incoming attacks", 0, 0, 0, "", "SENSECONTROL1", 1);
			if (isCommitted("SENSECONTROL1"))
				checkItem(ref, skillKey, true);
		}
		if (Character::instance->talents.contains("SENSECONTROL3")) {
			ref = autoCheckItems.addItem("g", "[B]Sense Control:[b] Commit force dice to upgrade abilty of combat checks", 0, 0, 0, "", "SENSECONTROL3", 1);
			if (isCommitted("SENSECONTROL3"))
				checkItem(ref, skillKey, true);
		}
		if (Character::instance->talents.contains("ENHANCECONT8")) {
			ref = autoCheckItems.addItem("g", "[B]Enhance:[b] Commit force dice to increase Brawn by one", 0, 0, 0, "", "ENHANCECONT8", 1);
			if (isCommitted("ENHANCECONT8"))
				checkItem(ref, skillKey, true);
		}
		if (Character::instance->talents.contains("ENHANCECONT9")) {
			ref = autoCheckItems.addItem("g", "[B]Enhance:[b] Commit force dice to increase Agility by one", 0, 0, 0, "", "ENHANCECONT9", 1);
			if (isCommitted("ENHANCECONT9"))
				checkItem(ref, skillKey, true);
		}
		if (Character::instance->talents.contains("MISDIRCONTROL3")) {
			int count = isCommitted("MISDIRCONTROL3");
			ranks = Character::instance->force();
			for (int i=0; i<ranks; i++) {
				ref = autoCheckItems.addItem("g", "[B]Misdirect Control:[b] Add [TH] to all incoming attacks", 0, 0, 0, "", "MISDIRCONTROL3", 1);
				if (i < count)
					checkItem(ref, skillKey, true);
			}
		}

		if (Character::instance->talents.contains("MISDIRDURATION")) {
			ref = autoCheckItems.addItem("gg", "[B]Misdirect Duration:[b] Sustain misdiration as long as target in range", 0, 0, 0, "", "MISDIRDURATION", 2);
			if (isCommitted("MISDIRDURATION"))
				checkItem(ref, skillKey, true);
		}
	}
	else if (skillKey == "PILOTPL") {
		piloting = true;
	}

	if (charm) {
		if (Character::instance->talents.contains("DONTSHOOT"))
			autoCheckItems.addItem("DDD", "[B]Don't Shoot!:[b] Once per session, cannot be attacked unless attack, Duration: Encounter", 0, 0);
	}

	if (piloting) {
		int amount = 1;

		if (Character::instance->talents.contains("FULLTHSUP"))
			amount = 2;
		if (Character::instance->talents.contains("FULLTH"))
			autoCheckItems.addItem("DDD", QString("[B]Full Throttle:[b] Increase Speed by %1 for %2 rounds, [B]Cost: Action[b]").arg(amount).arg(Character::instance->cunning()), 0, 0);
		if (Character::instance->talents.contains("FULLTHIMP"))
			autoCheckItems.addItem("DD", QString("[B]Full Throttle (Improved):[b] Increase Speed by %1 for %2 rounds").arg(amount).arg(Character::instance->cunning()), 1, 1);
	}

	if (combat) {
		// Aim skills:
		ranks = Character::instance->talents.ranks("TRUEAIM");
		if (ranged && ranks > 0)
			autoCheckItems.addItem("B" + QString("U").repeated(ranks), "[B]True Aim:[b] Once per round, use a maneuver to aim", 1, 0);
		else
			autoCheckItems.addItem("B", "[B]Aim:[b] Use a maneuver to aim", 1, 0);
		autoCheckItems.addItem("B", "[B]Aim Twice:[b] Use a second maneuver to aim longer", 1, 0);
		if (ranged && ranks > 0)
			autoCheckItems.addItem("SS" + QString("U").repeated(ranks), "[B]Called Shot (True Aim):[b] Once per round, use a True Aim to call a shot", 1, 0);
		else
			autoCheckItems.addItem("SS", "[B]Called Shot:[b] Use a maneuver to call a shot by aiming", 1, 0);
		autoCheckItems.addItem("N", "[B]Called Shot (2x Aim):[b] Use a 2nd maneuver to aim longer on a called shot", 1, 0);

		for (int i=0; i<Character::instance->talents.ranks("PRECAIM"); i++) {
			autoCheckItems.addItem("N", "[B]Precise Aim:[b] Decrease apponents defense", i ? 0 : 1, 1);
		}
		ranks = Character::instance->talents.ranks("QUICKST");
		if (ranks > 0)
			autoCheckItems.addItem(QString("B").repeated(ranks), "[B]Quick Strike:[b] Add if target has not acted in this encounter yet", 0, 0);

		if (!weaponKey.isEmpty() && weaponKey != "UNARMED" && Character::instance->talents.contains("TARGBL"))
			autoCheckItems.addItem(QString("@Damage +%1").arg(Character::instance->agility()), "[B]Targeted Blow:[b] Add to damage on hit with non-vehicle weapon", 0, 0, 1);

		ranks = Character::instance->talents.ranks("DISARMSMILE");
		if (ranks > 0)
			autoCheckItems.addItem(QString("N").repeated(ranks), "[B]Disarming Smile:[b] Opposed Charm vs Target, Range: Short, Duration: Encounter, [B]Cost: Action[b]", 0, 0);

	}
	if (ranged) {
		bool extreme_range = Shop::instance.getItem(weaponKey).range == "Extreme";
		if (!extreme_range) {
			ranks = Character::instance->talents.ranks("SNIPSHOT");
			if (ranks > 0)
				autoCheckItems.addItem("@Range +1", "[B]Sniper Shot:[b] Increase range (and difficulty) of next attack", 1, 0);
		}
		ranks = Character::instance->talents.ranks("POINTBL");
		if (ranks > 0)
			autoCheckItems.addItem(QString("@Damage +%1").arg(ranks), "[B]Point Blank:[b] Engaged or Short range", 0, 0);
		if (Character::instance->talents.contains("RAINDEATH"))
			autoCheckItems.addItem("@Auto-fire", "[B]Rain of Death:[b] Ignore increase difficulty due to auto-fire", 1, 0);
		if (skillKey != "RANGLT") {
			if (Character::instance->talents.contains("HEAVYHITTER"))
				autoCheckItems.addItem("@Breach +1", "[B]Heavy Hitter:[b] Once per session use [TR] to add breach property", 1, 0);
			ranks = Character::instance->talents.ranks("BAR");
			if (ranks > 0)
				autoCheckItems.addItem(QString("@Damage +%1").arg(ranks), "[B]Barrage:[b] Long or extreme range", 0, 0);
		}
	}

	Item weapon = Weapons::instance.getItem(weaponKey);
	if (!weapon.key.isEmpty()) {
		if (weapon.attachList.contains("TOS"))
			autoCheckItems.addItem("-D", "[B]Telescopic Optical Sight:[b] Reduce difficulty by 1 at Long or Extreme range", 0, 0);
		if (weapon.hasMod("SETTRIGGER_EXTRA_MOD"))
			autoCheckItems.addItem("sst", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
		else if (weapon.hasMod("SETTRIGGER_BASE_MOD"))
			autoCheckItems.addItem("st", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
	}

	for (int i = 0; i < Armor::instance.rowCount(); i++) {
		Item item = Armor::instance.itemAt(i);
		if (item.equipped()) {
			if (item.attachList.contains("TARGCOMP"))
				targeting_comp = true;
			if (item.attachList.contains("INTSCAN"))
				integrated_scanner = true;
			if (item.attachList.contains("EOS"))
				eos = true;
			if (item.key == "MOUNTARMOR")
				mount_armor = true;
		}
	}
	if (mount_armor && skillKey == "ATHL")
		autoCheckItems.addItem("BB", "[B]Mountaineer Armour:[b] Add to all climb or rappel checks", 0, 0);
	if (targeting_comp && ranged)
		autoCheckItems.addItem("B", "[B]Targeting Computer:[b] Add Boost dice at Long and Extreme range", 0, 0);
	else if ((integrated_scanner || hunting_goggles) && skillKey == "PERC") {
		if (integrated_scanner)
			autoCheckItems.addItem("B", "[B]OmniScan 3 Scanner:[b] To detect movement or hidden enemies", 0, 0);
		if (hunting_goggles)
			autoCheckItems.addItem("B", "[B]Hunting Goggles:[b] Add when locating prey", 0, 0);
	}
	if (eos) {
		if (combat || skillKey == "PERC" || skillKey == "VIGIL" || skillKey == "IVIG")
			autoCheckItems.addItem("NN", "[B]Enhanced Optics Suite:[b] Darkness, smoke or environmental effects that obscure vision", 0, 0);
	}
	if (hunting_goggles) {
		if (combat && skillKey != "GUNN")
			autoCheckItems.addItem("NN", "[B]Hunting Googles:[b] Concealment, darkness, fog and mist", 0, 0);
	}

	if (Character::instance->talents.contains("TOUCH")) {
		if (skillKey != "DEFM" && skillKey != "DEFR")
			autoCheckItems.addItem("BB", "[B]Touch of Fate:[b] May be used once per session", 0, 0);
	}
	if (Character::instance->talents.contains("SENSDANG")) {
		if (skillKey != "DEFM" && skillKey != "DEFR")
			autoCheckItems.addItem("NN", "[B]Sense Danger:[b] May be used once per session", 0, 0);
	}

	setup_talent_checks();
	if (talentChecks.contains(skillKey)) {
		foreach (CheckItem* item, talentChecks[skillKey].checks) {
			int ref = autoCheckItems.addItem(item->pool, item->desc, item->move, item->strain, item->dpoint);
			if (item->default_check)
				checkItem(ref, skillKey, true);
		}
	}

	// MEDPAC
	if (skillKey == "MED") {
		if (Gear::instance.equipped("MEDPAC"))
			default_check = autoCheckItems.addItem("B", "[B]Medpac:[b] Add to all Medicine checks when equipped", 0, 0);

		ranks = Gear::instance.carried("MEDAIDPATCH");
		if (ranks > 0)
			autoCheckItems.addItem("sa", "[B]Med-Aid PAtch:[b] Maximum usage 1 per check, [B]Cost: 1x Med-Aid Patch[b]", 0, 0, 0, "MEDAIDPATCH");

		if (Gear::instance.equipped("BLOODSCAN"))
			autoCheckItems.addItem("aa", "[B]Blood Scanner:[b] After use, add to next Medicine check, [B]Cost: Action[b]", 0, 0);

		ranks = Character::instance->talents.ranks("SURG");
		if (ranks > 0)
			autoCheckItems.addItem(QString("@Wound -%1").arg(ranks), "[B]Surgeon:[b] Add to Wounds recovered on successful check", 0, 0);

		ranks = Gear::instance.carried("DROIDMINIMED");
		if (ranks > 0)
			autoCheckItems.addItem(QString("@Wound -%1").arg(ranks), "[B]Mini-med Droids:[b] Add to Wounds recovered on successful check", 1, 0);

		autoCheckItems.addItem("D", "[B]Easy Severity:[b] 1-40% Critical injury, 1-50% Wounds, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DD", "[B]Average Severity:[b] 41-90% Critical injury, 51-100% Wounds, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DDD", "[B]Hard Severity:[b] 91-125% Critical injury, 101+% Wounds, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DDDD", "[B]Easy Severity:[b] 126-150% Critical injury, [B]Cost: Action[b]", 0, 0);
	}
	else if (skillKey == "MECH") { // 2264
		autoCheckItems.addItem("D", "[B]Damage Control (Easy):[b] System strain or hull trauma less than half threshold, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DD", "[B]Damage Control (Average):[b] System strain or hull trauma less or equal threshold, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DDD", "[B]Damage Control (Hard):[b] System strain or hull trauma exceeds threshold, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DD", "[B]Boost Sheilds:[b] On success add 1 to a shield zone, Duration: [SU] less 1 rounds, [B]Cost: Action[b]", 0, 0);
	}
	else if (piloting) {
		autoCheckItems.addItem("D", "[B]Gain the Advantage (Easy):[b] Going faster than opponent, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DD", "[B]Gain the Advantage (Average):[b] Vehicle speed identical, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DDD", "[B]Gain the Advantage (Hard):[b] Opponent's speed is one higher, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DDDD", "[B]Gain the Advantage (Daunting):[b] Opponent's speed is 2 or more higher, [B]Cost: Action[b]", 0, 0);
		autoCheckItems.addItem("DD", "[B]Co-Pilot:[b] Each [SU] downgrades difficulty of pilot's next check by 1, [B]Cost: Action[b]", 0, 0);
	}

	if (checkLists.contains(skillKey))
		checkLists[skillKey].checkAll(false);

	if (default_check)
		checkItem(default_check, skillKey, true);
}

void CurrentData::exitAutoCheckItems(const QString& skillKey)
{
	int		strain = 0;
	QString	consumable;

	Character::instance->setTemporaryStrain(0);
	if (checkLists.contains(skillKey))
		strain = checkLists[skillKey].strainUsed(consumable);
	strain += autoCheckItems.strainUsed(consumable);
	if (strain > 0)
		Character::instance->adjustStrain(strain);
	if (!consumable.isEmpty())
		addItem(-1, consumable, QString(), 0);
}

void CurrentData::addCheckItem(const QString& skillKey, const QString& pool, const QString& desc)
{
	//CheckList::instance.startChanges();
	setCheckItem(skillKey, pool, desc);
	CheckList::instance.rowCountChanged();
	writeCurrentData();
}

void CurrentData::updateCheckItem(int ref, const QString& skillKey, const QString& pool, const QString& desc)
{
	if (checkLists.contains(skillKey)) {
		CheckListItem* item = checkLists[skillKey].findItem(ref);
		if (item) {
			//CheckList::instance.startChanges();
			item->dice = pool;
			item->description = desc;
			CheckList::instance.rowCountChanged();
			checkChecked(skillKey);
			writeCurrentData();
		}
	}
}

void CurrentData::removeCheckItem(int ref, const QString& skillKey)
{
	if (checkLists.contains(skillKey)) {
		//CheckList::instance.startChanges();
		checkLists[skillKey].deleteItem(ref);
		CheckList::instance.rowCountChanged();
		checkChecked(skillKey);
		writeCurrentData();
	}
}

void CurrentData::uncheckAllItem(const QString& skillKey)
{
	iNegativeCheck = 0;
	//CheckList::instance.startChanges();
	autoCheckItems.checkAll(false);
	if (checkLists.contains(skillKey))
		checkLists[skillKey].checkAll(false);
	CheckList::instance.rowCountChanged();
	Character::instance->setChangeDicePool(QString());
}

void CurrentData::checkItem(int ref, const QString& skillKey, bool list_setup)
{
	CheckListItem* item;

	if (!(item = autoCheckItems.findItem(ref))) {
		if (checkLists.contains(skillKey))
			item = checkLists[skillKey].findItem(ref);
	}
	if (item) {
		//CheckList::instance.startChanges();
		item->checked = !item->checked;
		if (item->reduceStrain) {
			if (item->checked)
				Character::instance->setTemporaryStrain(temporaryStrain + item->reduceStrain);
			else
				Character::instance->setTemporaryStrain(temporaryStrain - item->reduceStrain);
		}
		if (!list_setup) {
			if (!item->commitKey.isEmpty()) {
				forceCommitted[item->commitKey] += item->checked ? item->forceCost : -item->forceCost;
				if (item->commitKey == "ENHANCECONT8")
					Character::instance->emitBrawnChanged();
				else if (item->commitKey == "ENHANCECONT9")
					Character::instance->emitAgilityChanged();
				Character::instance->characteristicsChanged();
			}
			CheckList::instance.rowCountChanged();
		}
	}
	checkChecked(skillKey);
}

int CurrentData::negativePool()
{
	return iNegativeCheck;
}

void CurrentData::setNegativePool(int bit, const QString& skillKey)
{
	if (bit == 0)
		iNegativeCheck = 0;
	else if ((iNegativeCheck & bit) == 0)
		iNegativeCheck |= bit;
	else
		iNegativeCheck &= ~bit;
	if (skillKey == "DEFM") {
		if (bit != 0) {
			iNegMelee = NEG_PURPLE(iNegativeCheck);
			SpecialSkills::instance.setDataChanged();
		}
	}
	if (skillKey == "DEFR") {
		if (bit != 0) {
			iNegRanged = NEG_PURPLE(iNegativeCheck);
			SpecialSkills::instance.setDataChanged();
		}
	}
	checkChecked(skillKey);
}

void CurrentData::negetiveDefence(int& r, int &m)
{
	r = iNegRanged;
	m = iNegMelee;
}

int CurrentData::commitCount()
{
	QMapIterator<QString, int> i(forceCommitted);
	int count = 0;

	while (i.hasNext()) {
		i.next();
		count += i.value();
	}
	return count;
}

int CurrentData::isCommitted(const QString& key)
{
	if (forceCommitted.contains(key))
		return forceCommitted[key];
	return 0;
}

void CurrentData::checkChecked(const QString& skillKey)
{
	QString new_curr_pool;

	new_curr_pool = autoCheckItems.getCurrentPool();
	if (checkLists.contains(skillKey))
		new_curr_pool += checkLists[skillKey].getCurrentPool();

	if ((iNegativeCheck & NEG_CHECK_1_BLACK) != 0)
		new_curr_pool += "S";
	if ((iNegativeCheck & NEG_CHECK_2_BLACK) != 0)
		new_curr_pool += "SS";
	if ((iNegativeCheck & NEG_CHECK_3_BLACK) != 0)
		new_curr_pool += "SSS";
	if ((iNegativeCheck & NEG_CHECK_1_PURPLE) != 0)
		new_curr_pool += "D";
	if ((iNegativeCheck & NEG_CHECK_2_PURPLE) != 0)
		new_curr_pool += "DD";
	if ((iNegativeCheck & NEG_CHECK_3_PURPLE) != 0)
		new_curr_pool += "DDD";
	if ((iNegativeCheck & NEG_CHECK_4_PURPLE) != 0)
		new_curr_pool += "DDDD";
	if ((iNegativeCheck & NEG_CHECK_1_RED) != 0)
		new_curr_pool += "C";
	if ((iNegativeCheck & NEG_CHECK_2_RED) != 0)
		new_curr_pool += "CC";
	if ((iNegativeCheck & NEG_CHECK_3_RED) != 0)
		new_curr_pool += "CCC";
	if ((iNegativeCheck & NEG_CHECK_1_UPGRADE) != 0)
		new_curr_pool += "u";
	if ((iNegativeCheck & NEG_CHECK_2_UPGRADE) != 0)
		new_curr_pool += "uu";
	Character::instance->setChangeDicePool(new_curr_pool);
}

int CurrentData::expTypeToInt(const char* type)
{
	if (strcmp(type, EXP_TOT_XP) == 0)
		return EXP_XP;
	if (strcmp(type, EXP_TOT_DUTY) == 0)
		return EXP_DUTY;
	if (strcmp(type, EXP_TOT_DUTYRANK) == 0)
		return EXP_DUTYRANK;
	if (strcmp(type, EXP_TOT_MORALITY) == 0)
		return EXP_MORALITY;
	if (strcmp(type, EXP_TOT_OBLIGATION) == 0)
		return EXP_OBLIGATION;
	return EXP_UNKNOWN;
}

QString CurrentData::expTypeToString(int type, const QString& key)
{
	switch (type) {
		case EXP_XP:
			return EXP_TOT_XP;
		case EXP_DUTY:
			if (!key.isEmpty())
				return key;
			return EXP_TOT_DUTY;
		case EXP_DUTYRANK:
			return EXP_TOT_DUTYRANK;
		case EXP_MORALITY:
			return EXP_TOT_MORALITY;
		case EXP_OBLIGATION:
			if (!key.isEmpty())
				return key;
			return EXP_TOT_OBLIGATION;
	}
	return QString();
}

void CurrentData::setCriticalWound(int perc, int type)
{
	Injury* injury;
	int row;

	if (!perc)
		return;

	for (row=0; row<DataList::injuries.rowCount(); row++) {
		if (DataList::injuries.getValueAsInt(row, "ref") == 0)
			break;
	}

	injury = DatUtil::getInjury(perc, type);
	DataList::injuries.setValue(row, "ref", ++iNextInjuryID);
	DataList::injuries.setValue(row, "percent", perc);
	DataList::injuries.setValue(row, "type", type);
	DataList::injuries.setValue(row, "title", injury->title);
	DataList::injuries.setValue(row, "description", injury->description);
	DataList::injuries.setValue(row, "dice", injury->dice);
	if (row == DataList::injuries.rowCount()-1)
		appendEmptyWound();
}

void CurrentData::appendEmptyWound()
{
	int row = DataList::injuries.appendRow();
	DataList::injuries.setValue(row, "ref", 0);
	DataList::injuries.setValue(row, "percent", 0);
	DataList::injuries.setValue(row, "type", 0);
	DataList::injuries.setValue(row, "title", "");
	DataList::injuries.setValue(row, "description", "");
	DataList::injuries.setValue(row, "dice", "EEEE");
}

int CurrentData::findExpLogItem(int ref)
{
	for (int i=0; i<experienceLog.count(); i++) {
		if (experienceLog[i].ref == ref)
			return i;
	}
	return -1;
}

void CurrentData::setExpLogItem(int type, const QString& key, const QString& name, const QString& desc, int amount, bool loading)
{
	ExpLogTotal	tot;
	QString		exp_key = expTypeToString(type, key);

	if (experienceTotal.contains(exp_key))
		tot = experienceTotal[exp_key];

	if (tot.itemCount == 0) {
		// This is the first item of its kind!
		if (loading) {
			tot.value = amount;
			tot.firstItem = tot.lastItem = experienceLog.count();
			tot.itemCount = 1;
			experienceTotal[exp_key] = tot;
			addExpLogItem(type, key, name, desc, amount);
			return;
		}

		// Get the original total:
		int total;
		switch (type) {
			case EXP_XP:
				total = Character::instance->getAttribute(XP);
				break;
			case EXP_DUTY:
				total = Character::instance->duties.findItem(key).size;
				break;
			case EXP_DUTYRANK:
				total = dutyRank;
				break;
			case EXP_MORALITY:
				total = Character::instance->getAttribute(MORALITY);
				break;
			case EXP_OBLIGATION:
				total = Character::instance->obligations.findItem(key).size;
				break;
		}

		tot.value = total;
		tot.firstItem = tot.lastItem = experienceLog.count();
		tot.itemCount = 0;
		experienceTotal[exp_key] = tot;
		addExpLogItem(type, key, name, "Original Total", total);
	}

	experienceTotal[exp_key].value += amount;
	experienceTotal[exp_key].lastItem = experienceLog.count();
	experienceTotal[exp_key].itemCount++;
	addExpLogItem(type, key, name, desc, amount);
}

void CurrentData::addExpLogItem(int type, const QString& key, const QString& name, const QString& desc, int amount)
{
	experienceLog.append(ExpLogItem(++iExpNextRef, type, key, name, desc, amount));
// 	Character::instance->setAttribute("NEWXP", total + amount - Character::instance->totalXP());
}

int CurrentData::findInvLogItem(int ref)
{
	for (int i=0; i<inventoryLog.count(); i++) {
		if (inventoryLog[i].ref == ref)
			return i;
	}
	return -1;
}

void CurrentData::setInvLogItem(int count, const QString& in_key, const QString& desc, int amount, bool loading)
{
	int total = Character::instance->credits();
	int type = ITEM_AMOUNT;
	QString key = in_key.toUpper();

	if (!key.isEmpty()) {
		if (!count)
			count = 1;
		if (count > 0) {
			if (amount > 0)
				amount = -amount;
		}
		else {
			if (amount < 0)
				amount = -amount;
		}
	}
	else if (!desc.isEmpty()) {
		// Free text has no count:
		count = 0;
	}
	else
		return;

	if (inventoryLog.size() == 0) {
		if (loading) {
			// The first entry loaded myst be the original credit total:
			inventoryLog.append(InvLogItem(++iNextItem, count, key, desc, amount, ITEM_START));
			Character::instance->setCredits(amount);
			return;
		}

		addInvLogItem("[B]Original Total Credits[b]", total, ITEM_START);
	}

	if (!key.isEmpty()) {
		// Check the original stock:
		ShopItem item = Shop::instance.getItem(key);
		ItemList* list;

		if (item.type == "GEAR")
			list = &Gear::instance;
		else if (item.type == "ARMOR")
			list = &Armor::instance;
		else
			list = &Weapons::instance;

		if (invMod[key].rowCount == 0) {
			int c = 0;
			if (list->contains(key))
				c = list->getItem(key).originalQuantity();

			if (c) {
				// We have an original stock amount
				if (loading) {
					// This must be the original stock:
					invMod[key].quantity = 0;
					type = ITEM_ORIG_STOCK;
				}
				else {
					// Record the original stock:
					invMod[key].rowCount++;
					invMod[key].quantity = c;
					inventoryLog.append(InvLogItem(++iNextItem, c, key, QString(), 0, ITEM_ORIG_STOCK));
				}
			}
			else {
				// No original stock:
				invMod[key].quantity = 0;
			}
		}

		invMod[key].rowCount++; // Count the number of rows with this key:
		invMod[key].quantity += count; // And sum the quantity:

		if (invMod[key].quantity == 0) {
			if (list->contains(key)) {
				if (list->getItem(key).originalQuantity() == 0) {
					list->removeItem(key);
					list->setRowCountChanged();
					list->setClean();
				}
			}
		}
		else {
			if (!list->contains(key)) {
				Item new_item;
				new_item.clear(key);
				list->addItem(new_item);
				list->setRowCountChanged();
				list->setClean();
			}
		}
	}

	inventoryLog.append(InvLogItem(++iNextItem, count, key, desc, amount, type));
	//Character::instance->setLastInvLine(text + (amount != 0 ? QString(", %1 CR").arg(amount) : ""));

	Character::instance->setCredits(total + amount);
}

void CurrentData::addInvLogItem(const QString& desc, int amount, int type)
{
	inventoryLog.append(InvLogItem(++iNextItem, 0, QString(), desc, amount, type));
}

void CurrentData::changeInventory(const QString& key, bool signal)
{
	if (!invMod.contains(key))
		return;

	if (Weapons::instance.contains(key) && signal) {
		//Weapons::instance.startChanges();
		// "quantity" is taken dynamically from inventory!
		// Just signal change!
		Weapons::instance.rowCountChanged();
	}

	if (Armor::instance.contains(key) && signal) {
		//Armor::instance.startChanges();
		// "quantity" is taken dynamically from inventory!
		// Just signal change!
		Armor::instance.rowCountChanged();
	}

	if (Gear::instance.contains(key) && signal) {
		//Gear::instance.startChanges();
		// "quantity" is taken dynamically from inventory!
		// Just signal change!
		Gear::instance.rowCountChanged();
	}

	if (key == "STIMPACK")
		Character::instance->emitStimPacksChanged();
	else if (key == "ERP")
		Character::instance->emitErpsChanged();
}

void CurrentData::setCheckItem(const QString& skillKey, const QString& pool, const QString& desc)
{

	if (skillKey.isEmpty()) {
		// Add to all combat skills:
		setCheckItem("BRAWL", pool, desc);
		setCheckItem("GUNN", pool, desc);
		setCheckItem("MELEE", pool, desc);
		setCheckItem("RANGHVY", pool, desc);
		setCheckItem("RANGLT", pool, desc);
		setCheckItem("LTSABER", pool, desc);
		return;
	}

	addCheckItem(skillKey, ++iNextCheckItem, pool, desc);
}

void CurrentData::addCheckItem(const QString& skillKey, int ref, const QString& pool, const QString& desc)
{
	if (!checkLists.contains(skillKey)) {
		CheckListData list;

		list.skillKey = skillKey;
		checkLists[skillKey] = list;
	}
	checkLists[skillKey].addCustomItem(ref, pool, desc);
}

void CurrentData::writeCurrentData()
{
	QString data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>\n";

	data += "<CurrentData>\n";
	data += QString(" <Wounds>%1</Wounds>\n").arg(wounds);
	data += QString(" <Strain>%1</Strain>\n").arg(strain);
	data += QString(" <Conflict>%1</Conflict>\n").arg(conflict);
	data += QString(" <StimPacksUsed>%1</StimPacksUsed>\n").arg(stimPacksUsed);
	data += QString(" <ErpsUsed>%1</ErpsUsed>\n").arg(erpsUsed);
	data += QString(" <WoundHistory>%1</WoundHistory>\n").arg(woundHistory);
	data += QString(" <StrainHistory>%1</StrainHistory>\n").arg(strainHistory);
	data += QString(" <ConflictHistory>%1</ConflictHistory>\n").arg(conflictHistory);
	data += QString(" <WoundDelta>%1</WoundDelta>\n").arg(woundDelta);
	data += QString(" <StrainDelta>%1</StrainDelta>\n").arg(strainDelta);
	if (locked != 0)
		data += QString(" <Locked>%1</Locked>\n").arg(locked);
	if (hideAutoTalents != 0)
		data += QString(" <HideAutoTalents>%1</HideAutoTalents>\n").arg(hideAutoTalents);
	data += QString(" <Injuries>\n");
	for (int i=0; i<DataList::injuries.rowCount(); i++) {
		if (DataList::injuries.getValueAsInt(i, "ref") > 0) {
			data += QString("  <Injury>");
			data += QString("<Percent>%1</Percent>").arg(DataList::injuries.getValueAsInt(i, "percent"));
			data += QString("<Type>%1</Type>").arg(DataList::injuries.getValueAsInt(i, "type"));
			data += QString("</Injury>\n");
		}
	}
	data += QString(" </Injuries>\n");
	data += QString(" <Experience>\n");
	for (int i=0; i<experienceLog.size(); i++) {
		data += QString("  <XP>");
		data += QString("<Type>%1</Type>").arg(expTypeToString(experienceLog[i].type));
		if (!experienceLog[i].key.isEmpty())
			data += QString("<Key>%1</Key>").arg(experienceLog[i].key);
		if (!experienceLog[i].name.isEmpty())
			data += QString("<Name>%1</Name>").arg(experienceLog[i].name);
		data += QString("<Description>%1</Description>").arg(experienceLog[i].desc);
		data += QString("<Amount>%1</Amount>").arg(experienceLog[i].amount);
		data += QString("</XP>\n");
	}
	data += QString(" </Experience>\n");
	data += QString(" <Inventory>\n");
	for (int i=0; i<inventoryLog.size(); i++) {
		if (inventoryLog[i].type >= ITEM_START) {
			data += QString("  <Item>");
			if (inventoryLog[i].count != 0)
				data += QString("<Count>%1</Count>").arg(inventoryLog[i].count);
			if (!inventoryLog[i].key.isEmpty())
				data += QString("<Key>%1</Key>").arg(inventoryLog[i].key);
			if (!inventoryLog[i].desc.isEmpty())
				data += QString("<Description>%1</Description>").arg(inventoryLog[i].desc);
			if (inventoryLog[i].amount != 0)
				data += QString("<Amount>%1</Amount>").arg(inventoryLog[i].amount);
			data += QString("</Item>\n");
		}
	}
	data += QString(" </Inventory>\n");
	data += QString(" <Storage>\n");
	QMapIterator<QString, InvModItem> s(invMod);
	while (s.hasNext()) {
		s.next();
		InvModItem inv_item = s.value();
		if (inv_item.stored > 0 || inv_item.state != UNDEFINED)
			data += QString("  <StoreItem><Key>%1</Key><Amount>%2</Amount><State>%3</State></StoreItem>\n").arg(s.key()).arg(inv_item.stored).arg(inv_item.state);
	}
	data += QString(" </Storage>\n");

	QMapIterator<QString, CheckListData> i(checkLists);
	while (i.hasNext()) {
		i.next();
		if (i.value().hasCustomItem()) {
			QListIterator<CheckListItem> j(i.value().items);
			const CheckListItem* item;

			data += QString(" <CheckList>\n");
			data += QString("  <Skill>%1</Skill>\n").arg(i.key());

			while (j.hasNext()) {
				item = &j.next();
				if (item->reference > 0) {
					data += QString("  <CheckItem>");
					data += QString("<DicePool>%1</DicePool>").arg(item->dice);
					data += QString("<Description>%1</Description>").arg(item->description);
					data += QString("</CheckItem>\n");
				}
			}

			data += QString(" </CheckList>\n");
		}
	}

	data += QString(" <CustomSkills>\n");
	for (int i=0; i<customSkills.size(); i++) {
		data += QString("  <Skill>");
		data += QString("<Name>%1</Name>").arg(customSkills[i].name);
		data += QString("<Char>%1</Char>").arg(customSkills[i].characteristic);
		data += QString("<Rank>%1</Rank>").arg(customSkills[i].rank);
		data += QString("</Skill>\n");
	}
	data += QString(" </CustomSkills>\n");
	data += "</CurrentData>\n";

	if (!iFileName.isEmpty()) {
		DataAccess::writeFile(DatUtil::getCurrentFolder() + iFileName, data.toUtf8());
		//DataAccess::currentData.startChanges();
		int row = DataAccess::currentData.findRow("file", iFileName);
		if (row < 0)
			row = DataAccess::currentData.appendRow();
		DataAccess::setCurrentDataRow(row, iFileName);
		DataAccess::currentData.rowCountChanged();
	}
}

void CurrentData::changeWounds(int delta)
{
	if (!delta)
		return;
	wounds += delta;
	if (wounds < 0)
		wounds = 0;
	//DatUtil::appendToList(woundHistory, delta > 0 ? QString("+%1").arg(delta) : QString::number(delta), " ");
	DatUtil::appendToList(woundHistory, QString("%1").arg(wounds), " ");
	while (woundHistory.length() > MAX_HISTORY_LEN)
		woundHistory = DatUtil::right(woundHistory, " ");
}

void CurrentData::changeStrain(int delta)
{
	if (!delta)
		return;
	strain += delta;
	if (strain < 0)
		strain = 0;
	//DatUtil::appendToList(strainHistory, delta > 0 ? QString("+%1").arg(delta) : QString::number(delta), " ");
	DatUtil::appendToList(strainHistory, QString("%1").arg(strain), " ");
	while (strainHistory.length() > MAX_HISTORY_LEN)
		strainHistory = DatUtil::right(strainHistory, " ");
}

// CustomSkills -------------------------------------------

CustomSkills CustomSkills::instance = CustomSkills();

CustomSkills::CustomSkills() :
	AbstractDataList(QStringList() << "key" << "skill" << "career" << "rank" << "dice" << "characteristic")
{
}

int CustomSkills::rowCount()
{
	return 6;
}

QVariant CustomSkills::getValue(int row, int col)
{
	CustomSkill skill;
	if (row < Character::instance->currentData()->customSkills.size())
		skill = Character::instance->currentData()->customSkills[row];

	switch (col) {
		case 0:
			return skill.name;
		case 1:
			if (skill.characteristic.isEmpty())
				return "";
			return QString("%1 (%2)").arg(skill.name).arg(DatUtil::capitalize(skill.characteristic));
		case 2:
			return 0;
		case 3:
			return skill.rank;
		case 4:
			if (skill.characteristic.isEmpty())
				return QString("EEEEE");
			return DatUtil::getBasicDicePool(skill.rank, Character::instance->getAttribute(skill.characteristic));
		case 5:
			return skill.characteristic;
	}
	return QVariant();
}

// CheckList -------------------------------------------

CheckList CheckList::instance = CheckList();

CheckList::CheckList() :
	AbstractDataList(QStringList() << "ref" << "checked" << "dice" << "description")
{
}

int CheckList::rowCount()
{
	QString key = Character::instance->activeSkillKey();
	int count = 0;

	count = Character::instance->currentData()->autoCheckItems.items.count();
	if (Character::instance->currentData()->checkLists.contains(key)) {
		count += Character::instance->currentData()->checkLists[key].items.count();
	}
	return count+1;
}

QVariant CheckList::getValue(int row, int col)
{
	QString			key = Character::instance->activeSkillKey();
	CheckListItem*	item = NULL;

	if (row >= 0) {
		int c1 = Character::instance->currentData()->autoCheckItems.items.count();

		if (row < c1) {
			item = &Character::instance->currentData()->autoCheckItems.items[row];
		}
		else {
			row -= c1;
			if (Character::instance->currentData()->checkLists.contains(key)) {
				int c2 = Character::instance->currentData()->checkLists[key].items.count();

				if (row < c2)
					item = &Character::instance->currentData()->checkLists[key].items[row];
			}
		}
	}
	if (item) {
		switch (col) {
			case 0:
				return item->reference;
			case 1:
				return item->checked;
			case 2:
				return item->dice;
			case 3:
				return item->description;
		}
	}
	else {
		switch (col) {
			case 0:
				return 0;
			case 1:
				return false;
			case 2:
				return "";
			case 3:
				return "Add to Checklist...";
		}
	}
	return QVariant();
}

// InventoryLog -------------------------------------------

//DataList InventoryLog::instance = DataList(QStringList());


InventoryLog InventoryLog::instance = InventoryLog();

InventoryLog::InventoryLog() :
	AbstractDataList(QStringList() << "ref" << "key" << "count" << "item"
		<< "description" << "amount" << "type" << "rarity" << "price" << "keycount")

{
}

int InventoryLog::rowCount()
{
	return Character::instance->currentData()->inventoryLog.size() + 2;
}

QVariant InventoryLog::getValue(int row, int col)
{
	InvLogItem* item = NULL;
	int count = Character::instance->currentData()->inventoryLog.size();

	if (row >= 0) {
		if (row < count)
			item = &Character::instance->currentData()->inventoryLog[row];
	}

	if (item) {
		ShopItem shop_item;
		InvModItem	mod_item;

		if (!item->key.isEmpty()) {
			shop_item = Shop::instance.getItem(item->key);
			mod_item = Character::instance->currentData()->invMod[item->key];
		}

		switch (col) {
			case 0:
				return item->ref;
			case 1:
				return item->key;
			case 2:
				return item->count;
			case 3:
				return shop_item.name;
			case 4:
				return item->desc;
			case 5:
				return item->amount;
			case 6:
				return item->type;
			case 7:
				return shop_item.rarity;
			case 8:
				return shop_item.price;
			case 9:
				return mod_item.rowCount;
		}
	}
	else {
		if (row == count) {
			switch (col) {
				case 1:
				case 3:
					return "";
				case 4:
					return "[B]Current Total Credits[b]";
				case 5:
					return Character::instance->credits();
				case 6:
					return ITEM_TOTAL;
			}
			return 0;
		}
		else {
			switch (col) {
				case 3:
				case 1:
					return "";
				case 4:
					return "Add to Inventory...";
				case 6:
					return ITEM_ADD_ITEM;
			}
			return 0;
		}
	}
	return QVariant();
}

/*
struct CheckItem {
	const char* talentKey;
	const char* skillKey;
	const char* skillKey2;
	const char* pool;
	const char* desc;
	int			move;
	int			strain;
	int			dpoint;
	int			default_check;
};
*/

CheckItem check_items[] = {
	{
		"KEENEYED",
		"PERC", "VIGIL",
		"N", "[B]Keen Eyed:[b] Also halve search time",
		0, 0, 0, 1
	},
	{
		NULL,
		"", "",
		"", "",
		0, 0, 0, 0
	}
};
