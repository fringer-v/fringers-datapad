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

CurrentData* CurrentData::instance;
int CurrentData::iReferenceCounter;

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

int CheckListData::plus(const QString& pool, const QString& desc, int move, int strain, int dpoint,
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

void CheckListData::plusCustom(int ref, const QString& pool, const QString& desc)
{
	CheckListItem item;

	item.reference = ref;
	item.dice = pool;
	item.description = desc;
	items.append(item);
}

void CheckListData::commitCount(int ref, int count)
{
	CheckListItem* item;
	if ((item = findItem(ref)))
		item->commitCount = count;
}

void CheckListData::plusRange(int ref, int count)
{
	CheckListItem* item;
	if ((item = findItem(ref)))
		item->plusRange = count;
}

void CheckListData::plusMagnitude(int ref, int count)
{
	CheckListItem* item;
	if ((item = findItem(ref)))
		item->plusMagnitude = count;
}

void CheckListData::plusStrength(int ref, int count)
{
	CheckListItem* item;
	if ((item = findItem(ref)))
		item->plusStrength = count;
}

void CheckListData::plusDuration(int ref, int count)
{
	CheckListItem* item;
	if ((item = findItem(ref)))
		item->plusDuration = count;
}

// CurrentData -------------------------

void CurrentData::clear()
{
	fileName.clear();
	characterFile.clear();

	attributeMods.clear();

	encumbranceValue = 0;
	encumbranceThreshold = 0;
	cumbersomeValue = 0;
	cumbersomeThreshold = 0;
	encumbranceText.clear();

	name.clear();
	npc = false;
	player.clear();
	gender.clear();
	age.clear();
	height.clear();
	build.clear();
	hair.clear();
	eyes.clear();
	features.clear();
	story.clear();
	portrait.clear();
	credits = 0;
	originalCredits = 0;
	career.clear();
	specializations.clear();
	attributes.clear();
	skills.clear();
	obligations.items.clear();
	duties.items.clear();
	motivations.clear();
	moralities.clear();
	talents.clear();
	weapons.clear();
	armor.clear();
	gear.clear();
	species.clear();
	speciesTalents.clear();
	specialFeatures.clear();
	customItems.clear();

	wounds = 0;
	strain = 0;
	temporaryStrain = 0;
	conflict = 0;
	woundHistory.clear();
	strainHistory.clear();
	conflictHistory.clear();
	woundDelta = 0;
	strainDelta = 0;
	invMod.clear();
	inventoryLog.clear();
	stimPacksUsed = 0;
	erpsUsed = 0;
	customSkills.clear();
	autoCheckItems.clear();
	checkLists.clear();
	forceCommitted.clear();
	dutyRank = 0;
	experienceLog.clear();
	experienceTotal.clear();
	injuries.clear();
	for (int i=0; i<4; i++)
		appendEmptyWound();

	iNextInCache = NULL;
	iNegativeCheck = 0;
	iNegMelee = NEG_CHECK_2_PURPLE;
	iNegRanged = NEG_CHECK_2_PURPLE;
}

int CurrentData::getAttribute(const QString& val)
{
	int adjust = 0;

	if (val == MORALITY) {
		if (experienceTotal.contains(EXP_TOT_MORALITY))
			return experienceTotal[EXP_TOT_MORALITY].value;
	}
	if (val == XP) {
		if (experienceTotal.contains(EXP_TOT_XP))
			return experienceTotal[EXP_TOT_XP].value;
	}

	if (!attributes.contains(val)) {
		if (val == MORALITY)
			return 50;
		return 0;
	}

	int att = attributes[val] + attributeMods.get(val);
	if (val == BRAWN) {
		if (isCommitted("ENHANCECONT8") && att < 6)
			att++;
	}
	else if (val == AGILITY) {
		if (isCommitted("ENHANCECONT9") && att < 6)
			att++;
	}
	else if (val == WOUND) {
		if (getAttribute(FORCE) > 0) {
			int mor = getAttribute(MORALITY);

			if (mor < 10)
				adjust = 2;
			else if (mor < 20)
				adjust = 1;
		}
	}
	else if (val == STRAIN) {
		if (getAttribute(FORCE) > 0) {
			int mor = getAttribute(MORALITY);

			if (mor > 90)
				adjust = 2;
			else if (mor > 80)
				adjust = 1;
			else if (mor < 10)
				adjust = -2;
			else if (mor < 20)
				adjust = -1;
		}
	}
	return att + adjust;
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

void CurrentData::useStimPack(Character* charac, int delta)
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
			addItem(charac, -delta, "STIMPACK", QString(), 0);
		}
	}
	else {
		stimPacksUsed += delta;
		if (stimPacksUsed < 0)
			stimPacksUsed = 0;
	}
	writeCurrentData();
}

void CurrentData::useErp(Character* charac, int delta)
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
			addItem(charac, -delta, "ERP", QString(), 0);
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
	int quan = gear.quantity("STIMPACK");
	return QString("%1/%2").arg(gear.carriedQuantity("STIMPACK")).arg(quan);
}

QString CurrentData::erps()
{
	int quan = gear.quantity("ERP");
	return QString("%1/%2").arg(gear.carriedQuantity("ERP")).arg(quan);
}

int CurrentData::stimPackQuantity()
{
	return gear.quantity("STIMPACK");
}

int CurrentData::erpQuantity()
{
	return gear.quantity("ERP");
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

void CurrentData::addExperience(Character* charac, const QString& stype, const QString& key, const QString& name, const QString& desc, int amount)
{
	int type = expTypeToInt(stype.toUtf8().constData());

	setExpLogItem(type, QDateTime::currentDateTime(), key, name, desc, amount, false);
	writeCurrentData();
	charac->emitExperienceChanged();
	ExperienceList::instance.rowCountChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::changeExperience(Character* charac, int ref, const QString& desc, int amount)
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
	charac->emitExperienceChanged();
	ExperienceList::instance.dataChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::removeExperience(Character* charac, int ref)
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
	charac->emitExperienceChanged();
	ExperienceList::instance.rowCountChanged();
	ObligationList::instance.dataChanged();
	DutyList::instance.dataChanged();
}

void CurrentData::addCustomSkill(Character* charac, const QString& name, const QString& characteristic, int ranks)
{
	if (name.isEmpty() || characteristic.isEmpty())
		return;

	CustomSkill skill;

	skill.name = name;
	skill.characteristic = characteristic;
	skill.rank = ranks;
	customSkills.append(skill);
	if (charac) {
		CustomSkills::instance.dataChanged();
		writeCurrentData();
	}
}

void CurrentData::removeCustomSkill(Character* charac, const QString& name)
{
	for (int i=0; i<customSkills.size(); i++) {
		if (customSkills[i].name == name) {
			customSkills.removeAt(i);
			break;
		}
	}
	if (charac) {
		CustomSkills::instance.dataChanged();
		writeCurrentData();
	}
}

void CurrentData::addItem(Character* charac, int count, const QString& itemkey, const QString& desc, int amount)
{
	InventoryLog::instance.setRowCountChanged();
	QDateTime create = QDateTime::currentDateTime();
	QString uuid = setInvLogItem(count, create, create, QString(), itemkey, desc, amount, false);
	writeCurrentData();
	inventoryChanged(charac, uuid, itemkey, true);
	charac->inventoryChanged();
	InventoryLog::instance.makeClean();
	Gear::instance.makeClean();
	Armor::instance.makeClean();
	Weapons::instance.makeClean();
}

void CurrentData::updateItem(Character* charac, int ref, int count, const QString& desc, int amount)
{
	int row = findInvLogItem(ref);
	if (row < 0)
		return;
	InvLogItem item = inventoryLog[row];
	if (item.type != ITEM_AMOUNT)
		return;

	int old_count = item.count;
	int old_amount = item.amount;

	if (!item.uuid.isEmpty()) {
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
		if (invMod.contains(item.uuid) && invMod[item.uuid].quantity != UNKNOWN_QUANTITY)
			invMod[item.uuid].quantity += count - old_count;
	}
	else if (!desc.isEmpty()) {
		// Free text has no count:
		count = 0;
	}
	else
		return;
	InventoryLog::instance.setRowCountChanged();
	inventoryLog[row].count = count;
	inventoryLog[row].update = QDateTime::currentDateTime();
	inventoryLog[row].desc = desc;
	inventoryLog[row].amount = amount;
	if (amount - old_amount) {
		int total = charac->credits();
		charac->setCredits(total + (amount - old_amount));
	}
	InventoryLog::instance.makeClean();
	writeCurrentData();
	if (count - old_count) {
		inventoryChanged(charac, item.uuid, item.itemkey, true);
		charac->inventoryChanged();
	}
}

bool CurrentData::removeItem(Character* charac, int ref)
{
	int row = findInvLogItem(ref);
	if (row < 0)
		return false;
	InvLogItem log_item = inventoryLog[row];
	if (log_item.type != ITEM_AMOUNT && log_item.type != ITEM_ORIG_STOCK)
		return false;

	if (!log_item.uuid.isEmpty() && invMod.contains(log_item.uuid)) {
		ItemList* list;
		int quantity = 0;
		ShopItem shop_item;

		if (log_item.type == ITEM_ORIG_STOCK && invMod[log_item.uuid].rowCount > 1)
			return false;

		if (customItems.contains(log_item.uuid))
			shop_item = customItems[log_item.uuid];
		else
			shop_item = Shop::instance.getItem(log_item.itemkey);

		if (shop_item.type == "GEAR" || shop_item.type.isEmpty())
			list = &gear;
		else if (shop_item.type == "ARMOR")
			list = &armor;
		else
			list = &weapons;
		Item item = list->getItemByUuid(log_item.uuid);

		invMod[item.uuid].rowCount--;
		if (invMod[item.uuid].rowCount == 0) {
			if ((invMod[item.uuid].stored == UNKNOWN_STORED || invMod[item.uuid].stored == item.originalStored) &&
				(invMod[item.uuid].state == UNDEFINED || invMod[item.uuid].state == item.originalState))
				// This record contains no information:
				invMod.remove(item.uuid);
			else
				invMod[item.uuid].quantity = UNKNOWN_QUANTITY;
		}
		else if (invMod[item.uuid].quantity != UNKNOWN_QUANTITY) {
			invMod[item.uuid].quantity -= log_item.count;
			quantity = invMod[item.uuid].quantity;
		}

		if (quantity == 0 && item.originalQuantity == 0) {
			list->removeItemByUuid(item.uuid);
			if (shop_item.type == "GEAR" || shop_item.type.isEmpty())
				Gear::instance.rowCountChanged();
			else if (shop_item.type == "ARMOR")
				Armor::instance.rowCountChanged();
			else
				Weapons::instance.rowCountChanged();
		}
	}

	InventoryLog::instance.setRowCountChanged();
	inventoryLog.removeAt(row);
	if (inventoryLog.size() == 1) {
		inventoryLog.clear();
		charac->setCredits(originalCredits);
	}
	else if (log_item.amount) {
		int total = charac->credits();
		charac->setCredits(total - log_item.amount);
	}
	writeCurrentData();
	InventoryLog::instance.makeClean();
	inventoryChanged(charac, log_item.uuid, log_item.itemkey, true);
	charac->inventoryChanged();
	return true;
}

void CurrentData::storeItem(const QString& in_uuid, const QString& itemkey, int store, int state, Item* item)
{
	QString uuid = in_uuid;
	InvModItem inv_item;

	if (store < 0)
		store = 0;

	if (uuid.isEmpty()) {
		if (itemkey.isEmpty())
			return;
		uuid = itemkey;
	}

	if (!invMod.contains(uuid)) {
		inv_item.uuid = uuid;
		inv_item.itemkey = itemkey;
		invMod[uuid] = inv_item;
	}

	if (item) {
		if (state < NOT_CARRIED || state > IS_EQUIPPED)
			state = NOT_CARRIED;
		if (store >= item->quantity()) {
			store = 0;
			state = NOT_CARRIED;
		}
		if (store == item->originalStored)
			store = UNKNOWN_STORED;
		if (state == item->originalState)
			state = UNDEFINED;
	}

	invMod[uuid].stored = store;
	invMod[uuid].state = state;

	if (item) {
		inv_item = invMod[uuid];
		if (inv_item.quantity == UNKNOWN_QUANTITY &&
			inv_item.stored == UNKNOWN_STORED &&
			inv_item.state == UNDEFINED)
			invMod.remove(uuid);
		writeCurrentData();
	}
}

void CurrentData::clearAutoCheckItems(Character* charac, const QString& skillKey)
{
	uncheckAllItem(charac, skillKey);
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

QString CurrentData::plurize(const QString& thing, int val)
{
	return (val == 1 ? thing : thing+"s").arg(val);
}

// showCheckList() <---- Initialise power
void CurrentData::setupAutoCheckItems(Character* charac, const QString& /*check_list_type*/, const QString& skillKey, const QString& talentKey, const QString& uuid)
{
	Skill* skill = Skill::getSkill(skillKey);
	MethodID skill_id = KeyMethod::instance.getID(skillKey);
	MethodID talent_id = KeyMethod::instance.getID(talentKey);
	Item weapon = weapons.getItemByUuid(uuid);
	int ranks;
	int default_check = 0;

	uncheckAllItem(charac, skillKey);
	autoCheckItems.clear();

	// MANEUVER -------------------------------------------------

	if (!skill || skill->type != SPECIAL)
		autoCheckItems.plus("@+1 " ESC_MOVE, "[B]Maneuver:[b] Perform an action that requires a maneuver", 1, 0);

	// TALENT CHECK LISTS -------------------------------------------------

	if (AllTalents::instance()->isTalent(talentKey)) {
		QString power;
		int ref;
		bool opposed_dicip = false;
		CharSkill char_skill = skills["DISC"];

		int power_id = CharTalentMap::forcePower(talentKey, power);
		int range = talents.get(power + "RANGE").ranks;
		int magnitude = talents.get(power + "MAGNITUDE").ranks;
		int strength = talents.get(power + "STRENGTH").ranks;
		int duration = talents.get(power + "DURATION").ranks;
		int force_dice = charac->force() - charac->forceCommitted();
		if (range > 0) {
			int r1, r2;
			int r = charac->getItemRange(r1, r2);
			do {
				ref = autoCheckItems.plus(".", plurize("[B]Range:[b] Increase range by %1 range band", range), 0, 0);
				autoCheckItems.plusRange(ref, range);
				r += range;
			}
			while (r < 4);
		}

		// showCheckList() <---- Initialise power
		QString pips = ".";
		switch (talent_id) {
			case KM_BINDBASIC:
				pips += ".";
				// No break!
			// itemQualMag()
			case KM_MISDIRBASIC:
			case KM_MISDIRCONTROL1:
				magnitude = magnitude * charac->presence();
				// No break!
			case KM_INFLUENCEBASIC:
			case KM_INFLUENCECONTROL1:
			case KM_MOVEBASIC:
			case KM_MOVECONTROL1:
			case KM_MOVECONTROL2:
			case KM_MOVECONTROL3:
				if (magnitude > 0) {
					for (int i=0; i<(force_dice+1)/2; i++) {
						ref = autoCheckItems.plus(pips, plurize("[B]Magnitude:[b] Effect %1 additional target", magnitude), 0, 0);
						autoCheckItems.plusMagnitude(ref, magnitude);
					}
				}
				break;
			default:
				break;
		}

		switch (talent_id) {
			case KM_BINDBASIC:
				if (strength > 0) {
					for (int i=0; i<(force_dice+1)/2; i++) {
						ref = autoCheckItems.plus(".", plurize("[B]Strength:[b] Disorient targets for %1 round", strength), 0, 0);
						autoCheckItems.plusStrength(ref, strength);
					}
				}

				if (talents.contains("BINDCONTROL1")) {
					autoCheckItems.plus(".", QString("[B]Control:[b] All targets suffer %1 Strain when taking an action").arg(charac->willpower()), 0, 0);
				}

				if (talents.contains("BINDCONTROL2")) {
					autoCheckItems.plus(".", "[B]Control:[b] Move target 1 range band closer or further away", 0, 0);
				}

				if (duration > 0) {
					// itemDuration()
					ref = autoCheckItems.plus("@" ESC_COMMIT " FFF", "[B]Duration:[b] Sustain all ongoing effects", 0, 0);
					autoCheckItems.commitCount(ref, 3);
					autoCheckItems.plusDuration(ref, 1);
				}

				if (talents.contains("BINDMASTERY"))
					autoCheckItems.plus(char_skill.getBasicPool(charac), "[B]Master:[b] If [LI] used, Stagger targets, if [DA] used, cause Crit +10% for each [FP] spent", 0, 0);
				else
					opposed_dicip = true;
				break;
			case KM_INFLUENCECONTROL1:
				if (duration > 0) {
					for (int i=0; i<(force_dice+1)/2; i++) {
						ref = autoCheckItems.plus(".", duration > 1 ?
							QString("[B]Duration:[b] Increase by %1 rounds/minutes").arg(duration) :
							"[B]Duration:[b] Increase by 1 round/minute", 0, 0);
						autoCheckItems.plusDuration(ref, duration);
					}
				}
				break;
			case KM_MISDIRBASIC:
			case KM_MISDIRCONTROL1:
			case KM_MOVEBASIC:
			case KM_MOVECONTROL1:
			case KM_MOVECONTROL2:
			case KM_MOVECONTROL3:
				if (strength > 0) {
					for (int i=0; i<(force_dice+1)/2; i++) {
						ref = autoCheckItems.plus(".", QString("[B]Strength:[b] Increase silhouette by %1").arg(strength), 0, 0);
						autoCheckItems.plusStrength(ref, strength);
						if (i*strength+1 > 9) // Max silhouette!
							break;
					}
				}

				if (duration > 0) {
					// itemDuration()
					ref = autoCheckItems.plus("@" ESC_COMMIT " FF", "[B]Duration:[b] Sustain all ongoing effects", 0, 0);
					autoCheckItems.commitCount(ref, 2);
					autoCheckItems.plusDuration(ref, 1);
				}

				if (talents.contains("MISDIRMASTERY")) {
					int cunn_decep = charac->cunning() + skills["DECEP"].ranks;
					autoCheckItems.plus("..", QString("[B]Mastery:[b] Obscure additional %1 objects or create %1 more illusions").arg(cunn_decep), 0, 0);
				}
				else if (talent_id != KM_MOVECONTROL1)
					opposed_dicip = true;
				break;
			case KM_SENSEBASIC:
			case KM_SENSECONTROL2:
			case KM_INFLUENCEBASIC:
			case KM_WARFORBASIC:
			case KM_FEARSOME:
				opposed_dicip = true;
				break;
			default:
				break;
		}

		if (opposed_dicip) {
			autoCheckItems.plus(char_skill.getBasicPool(charac), "[B]Opposed Check:[b] vs Discipline, if target is a PC or important NPC", 0, 0);
			//if (talent_id == KM_MOVECONTROL1)
			//	default_check = dc;
		}
	}

	// FORCE POWER ITEMS -------------------------------------------------

	int force = nonCommitedForce(charac);

	switch (skill_id) {
		case KM_CHARM:
		case KM_COERC:
		case KM_NEG:
		case KM_DECEP:
		case KM_LEAD:
		//case KM_DONTSHOOT:
			if (force > 0 && talents.contains("INFLUENCECONTROL2"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Influence:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);

			if (talents.contains("JUSTKID"))
				autoCheckItems.plus("-y", "[B]Just Kidding:[b] Once per round, Target: self or ally", 0, 0, 1);
			break;
		case KM_REC:
			if (force > 0 && talents.contains("BAL"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Balance:[b] Gain +1 Strain per [FP]", 0, 0);
			break;
		case KM_ATHL:
			if (talents.contains("NIKTOSP2OC2OP1"))
				autoCheckItems.plus("B", "[B]Climbing Claws:[b] Add when climbing trees and other surfaces their claws can pierce", 0, 0);
			if (force > 0 && talents.contains("ENHANCEBASIC"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_COORD:
			if (force > 0 && talents.contains("ENHANCECONT1"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_RESIL:
			if (force > 0 && talents.contains("ENHANCECONT2"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_PILOTPL:
			if (force > 0 && talents.contains("ENHANCECONT4"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_BRAWL:
			if (force > 0 && talents.contains("ENHANCECONT5"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_BRI:
		case KM_PILOTSP:
			if (force > 0 && talents.contains("ENHANCECONT7"))
				default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]", 0, 0);
			break;
		case KM_ICOOL:
		case KM_IVIG:
			if (force > 0) {
				if (talents.contains("FORSEECONTROL1")) {
					if (talents.contains("FORSEECONTROL3"))
						default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Foresee:[b] Spend [FP] to gain [SU], spend [FP] to grant free move to targets", 0, 0);
					else
						default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Foresee:[b] Spend [FP] to gain [SU] on initiative check", 0, 0);
				}
				if (talents.contains("WARFORCONTROL1")) {
					if (talents.contains("WARFORMAGNITUDE"))
						default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Warde's Foresight:[b] Add [SU] or [AD] per [FP] spent to self or ally's first check this encounter", 0, 0);
					else if (talents.contains("WARFORCONTROL3"))
						default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Warde's Foresight:[b] Add [SU] or [AD] per [FP] spent to first check this encounter", 0, 0);
					else
						default_check = autoCheckItems.plus(QString("F").repeated(force), "[B]Warde's Foresight:[b] Add [SU] per [FP] spent to first check this encounter", 0, 0);
				}

			}
			break;
		case KM_STEAL:
		//case KM_SLEIGHTMIND:
			ranks = talents.ranks("SLEIGHTMIND");
			if (ranks > 0) {
				int dc = autoCheckItems.plus(QString("B").repeated(ranks), "[B]Sleight of Mind:[b] Add if opposition is not immune to Force powers", 0, 0);
				if (talent_id == KM_SLEIGHTMIND)
					default_check = dc;
			}
			break;
		case KM_LTSABER:
			if (force > 0 && talents.ranks("HAWKSWOOP"))
				autoCheckItems.plus(QString("F").repeated(force), "[B]Hawk Bat Swoop:[b] Spend [FP] to engage target, and [FP] to add [AD]", 0, 0);

			if (force > 0 && talents.contains("SABERSW"))
				autoCheckItems.plus(QString("@" ESC_LINKED " %1").arg(force), QString("[B]Saber Swarm:[b] Attack has linked %1 quality").arg(force), 1, 1);
			break;
		case KM_DEFM:
		case KM_DEFR:
		//case KM_REFLECT:
			if (isCommitted("SENSECONTROL1")) {
				QString send_mag = "once";
				if (talents.contains("SENSEDURATION"))
					send_mag = "twice";

				QString send_str = "u";
				if (talents.contains("SENSESTRENGTH"))
					send_str = "uu";
				default_check = autoCheckItems.plus(send_str, QString("[B]Sense Control:[b] Upgrade difficulty of incoming attack %1 per round").arg(send_mag), 0, 0);
			}
			break;
		case KM_FORCE: {
			int ref;
			QString twice = talents.contains("SENSESTRENGTH") ? " (twice)" : "";

			if (talents.contains("SENSECONTROL1")) {
				ref = autoCheckItems.plus("g", "[B]Sense Control:[b] Commit force dice to upgrade difficulty of incoming attacks" + twice, 0, 0, 0, "", "SENSECONTROL1", 1);
				if (isCommitted("SENSECONTROL1"))
					checkItem(charac, ref, skillKey, true);
			}
			if (talents.contains("SENSECONTROL3")) {
				ref = autoCheckItems.plus("g", "[B]Sense Control:[b] Commit force dice to upgrade abilty of combat checks" + twice, 0, 0, 0, "", "SENSECONTROL3", 1);
				if (isCommitted("SENSECONTROL3"))
					checkItem(charac, ref, skillKey, true);
			}
			if (talents.contains("ENHANCECONT8")) {
				ref = autoCheckItems.plus("g", "[B]Enhance:[b] Commit force dice to increase Brawn by one", 0, 0, 0, "", "ENHANCECONT8", 1);
				if (isCommitted("ENHANCECONT8"))
					checkItem(charac, ref, skillKey, true);
			}
			if (talents.contains("ENHANCECONT9")) {
				ref = autoCheckItems.plus("g", "[B]Enhance:[b] Commit force dice to increase Agility by one", 0, 0, 0, "", "ENHANCECONT9", 1);
				if (isCommitted("ENHANCECONT9"))
					checkItem(charac, ref, skillKey, true);
			}
			if (talents.contains("MISDIRCONTROL3")) {
				int count = isCommitted("MISDIRCONTROL3");
				ranks = charac->force() - 1;
				ref = autoCheckItems.plus("g", "[B]Misdirect Control:[b] Add [TH] to all incoming attacks", 0, 0, 0, "", "MISDIRCONTROL3", 1);
				if (count > 0) {
					checkItem(charac, ref, skillKey, true);
					count--;
				}
				if ((ranks & 1) != 0) {
					ref = autoCheckItems.plus("g", "[B]Misdirect Control:[b] Add [TH] to all incoming attacks", 0, 0, 0, "", "MISDIRCONTROL3", 1);
					if (count > 0 && (count & 1) != 0) {
						checkItem(charac, ref, skillKey, true);
						count--;
					}
					ranks--;
				}
				for (int i=0; i<ranks/2; i++) {
					ref = autoCheckItems.plus("gg", "[B]Misdirect Control:[b] Add [TH][TH] to all incoming attacks", 0, 0, 0, "", "MISDIRCONTROL3", 2);
					if (count > 0) {
						checkItem(charac, ref, skillKey, true);
						count -= 2;
					}
				}
			}

			if (talents.contains("MISDIRDURATION")) {
				ref = autoCheckItems.plus("gg", "[B]Misdirect Duration:[b] Sustain misdiration as long as target in range", 0, 0, 0, "", "MISDIRDURATION", 2);
				if (isCommitted("MISDIRDURATION"))
					checkItem(charac, ref, skillKey, true);
			}

			if (talents.contains("BINDDURATION")) {
				ref = autoCheckItems.plus("ggg", "[B]Bind Duration:[b] Sustain all bind effects as long as target in range", 0, 0, 0, "", "BINDDURATION", 3);
				if (isCommitted("BINDDURATION"))
					checkItem(charac, ref, skillKey, true);
			}

			ranks = talents.ranks("INTUITEVA");
			if (ranks > 0) {
				int count = isCommitted("INTUITEVA");
				if (talent_id == KM_INTUITEVA && count < charac->force()) {
					count++;
				}
				for (int i=0; i<ranks; i++) {
					ref = autoCheckItems.plus("g", "[B]Intuitive Evasion:[b] Upgrade combat checks vs vehicle, cost 1 strain per round", 1, 1, 0, "", "INTUITEVA", 1);
					if (count > 0) {
						checkItem(charac, ref, skillKey, true);
						count--;
					}
				}
			}
			break;
		}
		default:
			break;
	}

	// TALENT ITEMS -------------------------------------------------
	// showCheckList() <---- Initialise power

	switch (skill_id) {
		case KM_COERC:
			for (int i=0; i<talents.ranks("INTIM"); i++) {
				int dc = autoCheckItems.plus("d", "[B]Intimidating:[b] Downgrade the difficulty of the check", 0, 1);
				if (talent_id == KM_INTIM)
					default_check = dc;
			}
			break;
		case KM_CHARM:
		case KM_NEG:
			for (int i=0; i<talents.ranks("CONGENIAL"); i++) {
				autoCheckItems.plus("d", "[B]Congenial:[b] Downgrade the difficulty of the check", 0, 1);
			}
			break;
		case KM_DEFM:
		case KM_DEFR:
		//case KM_REFLECT:
			for (int i=0; i<talents.ranks("DODGE"); i++)
				autoCheckItems.plus("u", "[B]Dodge:[b] Out-of-turn incedental", 0, 1);

			ranks = talents.ranks("DURA");
			if (ranks > 0)
				autoCheckItems.plus(QString("@-%1% " ESC_CRITS).arg(ranks*10), "[B]Durable:[b] Reduce critical hits", 0, 0);

			if (skill_id == KM_DEFM) {
				ranks = talents.ranks("PARRY");
				if (ranks > 0) {
					QString par = QString("[B]Parry:[b] Reduce damage by %1").arg(ranks+2);

					if (talents.contains("PARRYIMP"))
						par = par + ", on [DE] or [TH][TH][TH] hit attacker";
					autoCheckItems.plus(QString("@-%1 " ESC_DAMAGE).arg(ranks+2), par, 0, 3);
				}

				for (int i=0; i<talents.ranks("DEFSTA"); i++)
					autoCheckItems.plus("u", "[B]Defensive Stance:[b] Perform during turn, duration: 1 round", i == 0 ? 1 : 0, 1);

				setNegativePool(charac, iNegMelee, skillKey);
			}
			else {
				ranks = talents.ranks("REFLECT");
				if (ranks > 0) {
					QString par = QString("[B]Reflect:[b] vs Lightsaber, reduce damage by %1").arg(ranks+2);

					if (talents.contains("REFLECTIMP"))
						par = "[B]Reflect:[b] vs Lightsaber, on [DE] or [TH][TH][TH] damage apponent (Medium range)";
					autoCheckItems.plus(QString("@-%1 " ESC_DAMAGE).arg(ranks+2), par, 0, 3);
				}

				for (int i=0; i<talents.ranks("SIDESTEP"); i++)
					autoCheckItems.plus("u", "[B]Side Step:[b] Perform during turn, duration: 1 round", i == 0 ? 1 : 0, 1);

				setNegativePool(charac, iNegRanged, skillKey);
			}

			if (talents.contains("SENSEADV")) {
				autoCheckItems.plus("SS", "[B]Sense Advantage:[b] Add to NPC check, once per session", 0, 0);
			}
			break;
		case KM_PILOTSP:
			if (talents.contains("MASPIL"))
				autoCheckItems.plus("@+1 " ESC_ACTION, QString("[B]Master Pilot:[b] Once per round, perform Action as Manuever"), 1, 2);
			break;
		case KM_DISC:
			ranks = talents.ranks("HARDHD");
			if (ranks > 0) {
				int dif = 4-ranks;

				autoCheckItems.plus(QString("D").repeated(dif <= 0 ? 1 : dif), "[B]Hard Headed:[b] Perform check to remove staggered or disoriented effect", 0, 0);
				if (talents.contains("HARDHDIMP")) {
					dif = 6-ranks;
					autoCheckItems.plus(QString("D").repeated(dif <= 0 ? 1 : dif), QString("[B]Hard Headed (Improved):[b] Perfrom check to reduced straing to %1").arg(charac->strain()-1), 0, 0);
				}
			}
			// No break!
		case KM_FDISC:
			ranks = talents.ranks("CONF");
			if (ranks > 0) {
				int dc = autoCheckItems.plus(QString("r").repeated(ranks), "[B]Confidence:[b] May descrease difficulty of fear checks", 0, 0);
				if (skill_id == KM_FDISC)
					default_check = dc;
			}
			break;
		case KM_ICOOL:
		case KM_IVIG:
			for (int i=0; i<talents.ranks("RAPREA"); i++)
				autoCheckItems.plus("s", "[B]Rapid Reaction:[b] Suffer strain to improve initiative", 0, 1);
			break;
		case KM_SKUL:
		case KM_COMP:
		//case KM_BYP:
			ranks = talents.ranks("BYP");
			if (ranks > 0) {
				int dc = autoCheckItems.plus(QString("N").repeated(ranks), QString("[B]Bypass Security:[b] Disable security device or open a locked door"), 0, 0);
				if (talent_id == KM_BYP)
					default_check = dc;
			}
			break;
		case KM_ATHL:
		case KM_COORD:
		//case KM_CONDITIONED:
			ranks = talents.ranks("CONDITIONED");
			if (ranks > 0)
				autoCheckItems.plus(QString("@-%1 " ESC_DAMAGE).arg(ranks), QString("[B]Conditioned:[b] Reduce damage and strain by %1 from falling").arg(ranks), 0, 0);
			break;
		default:
			break;
	}

	switch (skill_id) {
		case KM_CHARM:
			if (talents.contains("DONTSHOOT"))
				autoCheckItems.plus("DDD", "[B]Don't Shoot!:[b] Once per session, cannot be attacked unless attack, Duration: Encounter", 0, 0);
			break;
		case KM_PILOTSP:
		case KM_PILOTPL: {
			int amount = 1;

			if (talents.contains("FULLTHSUP"))
				amount = 2;
			if (talents.contains("FULLTH"))
				autoCheckItems.plus("DDD", QString("[B]Full Throttle:[b] Increase Speed by %1 for %2 rounds, [B]Cost: Action[b]").arg(amount).arg(charac->cunning()), 0, 0);
			if (talents.contains("FULLTHIMP"))
				autoCheckItems.plus("DD", QString("[B]Full Throttle (Improved):[b] Increase Speed by %1 for %2 rounds").arg(amount).arg(charac->cunning()), 1, 1);
			break;
		}
		case KM_COMP:
		case KM_INT:
		//case KM_COD:
			ranks = talents.ranks("COD");
			if (ranks > 0) {
				int dc = autoCheckItems.plus("r"+QString("N").repeated(ranks), QString("[B]Codebreaker:[b] Break code or decrypt communications"), 0, 0);
				if (talent_id == KM_COD)
					default_check = dc;
			}
			break;
		default:
			break;
	}

	// GENERAL COMBAT -------------------------------------------------
	// showCheckList() <---- Initialise power

	bool combat = false;
	bool ranged = false;
	bool gunnery = false;
	if (skill_id == KM_RANGHVY || skill_id == KM_RANGLT) {
		combat = true;
		ranged = true;
	}
	else if (skill_id == KM_GUNN) {
		combat = true;
		ranged = true;
		gunnery = true;
	}
	else if (skill_id == KM_BRAWL || skill_id == KM_MELEE || skill_id == KM_LTSABER) {
		combat = true;
	}

	if (combat) {
		// Aim skills:
		ranks = talents.ranks("TRUEAIM");
		if (ranged && ranks > 0)
			autoCheckItems.plus("B" + QString("U").repeated(ranks), "[B]True Aim:[b] Once per round, use a maneuver to aim", 1, 0);
		else
			autoCheckItems.plus("B", "[B]Aim:[b] Use a maneuver to aim", 1, 0);
		autoCheckItems.plus("B", "[B]Aim Twice:[b] Use a second maneuver to aim longer", 1, 0);
		if (ranged && ranks > 0)
			autoCheckItems.plus("SS" + QString("U").repeated(ranks), "[B]Called Shot (True Aim):[b] Once per round, use a True Aim to call a shot", 1, 0);
		else
			autoCheckItems.plus("SS", "[B]Called Shot:[b] Use a maneuver to call a shot by aiming", 1, 0);
		autoCheckItems.plus("N", "[B]Called Shot (2x Aim):[b] Use a 2nd maneuver to aim longer on a called shot", 1, 0);

		for (int i=0; i<talents.ranks("PRECAIM"); i++) {
			autoCheckItems.plus("N", "[B]Precise Aim:[b] Decrease apponents defense", i ? 0 : 1, 1);
		}
		ranks = talents.ranks("QUICKST");
		if (ranks > 0)
			autoCheckItems.plus(QString("B").repeated(ranks), "[B]Quick Strike:[b] Add if target has not acted in this encounter yet", 0, 0);

		if (!weapon.itemkey.isEmpty() && weapon.itemkey != "UNARMED" && talents.contains("TARGBL"))
			autoCheckItems.plus(QString("@" ESC_DAMAGE " +%1").arg(charac->agility()), "[B]Targeted Blow:[b] Add to damage on hit with non-vehicle weapon", 0, 0, 1);

		ranks = talents.ranks("DISARMSMILE");
		if (ranks > 0)
			autoCheckItems.plus(QString("N").repeated(ranks), "[B]Disarming Smile:[b] Opposed Charm vs Target, Range: Short, Duration: Encounter, [B]Cost: Action[b]", 0, 0);

		if (talent_id != KM_EXHPORT) {
			ranks = talents.ranks("CRIPV");
			if (ranks > 0)
				autoCheckItems.plus("D", "[B]Crippling Blow:[b] If check deals damage, target suffers 1 strain per move for encounter", 0, 0);
		}

		if (gunnery) {
			if (talents.contains("DEAD")) {
				int dam = charac->agility();

				if (!talents.contains("DEADIMP"))
					dam = (dam + 1) / 2;
				autoCheckItems.plus(QString("@+%1 " ESC_DAMAGE).arg(dam), "[B]Dead to Rights:[b] Add to one hit with vehicle mounted weapon", 0, 0, 1);
			}

			if (talents.contains("EXHPORT")) {
				autoCheckItems.plus("@-1 " ESC_DESTINY, "[B]Exhaust Port:[b] Ignore the effects of Massive", 0, 0, 1);
			}

		}
	}
	if (ranged) {
		bool extreme_range = weapon.range() == "Extreme";
		if (!extreme_range) {
			ranks = talents.ranks("SNIPSHOT");
			if (ranks > 0) {
				int ref = autoCheckItems.plus("@" ESC_RANGE " +1", "[B]Sniper Shot:[b] Increase range (and difficulty) of next attack", 1, 0);
				autoCheckItems.plusRange(ref, 1);
			}
		}
		ranks = talents.ranks("POINTBL");
		if (ranks > 0)
			autoCheckItems.plus(QString("@" ESC_DAMAGE " +%1").arg(ranks), "[B]Point Blank:[b] Engaged or Short range", 0, 0);
		if (talents.contains("RAINDEATH"))
			autoCheckItems.plus("@" ESC_AUTOFIRE, "[B]Rain of Death:[b] Ignore increase difficulty due to auto-fire", 1, 0);
		if (skillKey != "RANGLT") {
			if (talents.contains("HEAVYHITTER"))
				autoCheckItems.plus("@" ESC_BREACH " +1", "[B]Heavy Hitter:[b] Once per session use [TR] to add breach property", 1, 0);
			ranks = talents.ranks("BAR");
			if (ranks > 0)
				autoCheckItems.plus(QString("@" ESC_DAMAGE" +%1").arg(ranks), "[B]Barrage:[b] Long or extreme range", 0, 0);
		}
	}

	// GENERALLY APPLICABLE -------------------------------------------------

	for (int i=0; i<talents.ranks("BRA"); i++)
		autoCheckItems.plus("N", "[B]Brace:[b] Remove [SE] if due to any environmental effects, Duration: 1 Round", i == 0 ? 1 : 0, 0);

	if (!skill || skill->type != SPECIAL) {
		if (skillKey != "FORCE" && talents.contains("INTENSFOC"))
			autoCheckItems.plus("U", "[B]Intense Focus:[b] Upgrade the next skill check once", 1, 1);
	}
	if (talents.contains("TOUCH")) {
		if (skillKey != "DEFM" && skillKey != "DEFR")
			autoCheckItems.plus("BB", "[B]Touch of Fate:[b] May be used once per session", 0, 0);
	}
	if (talents.contains("SENSDANG")) {
		if (skillKey != "DEFM" && skillKey != "DEFR")
			autoCheckItems.plus("NN", "[B]Sense Danger:[b] May be used once per session", 0, 0);
	}

	// EQUIPMENT BASED MODS -------------------------------------------------

	bool targeting_comp = false;
	bool integrated_scanner = false;
	bool mount_armor = false;
	bool hunting_goggles = false;
	bool eos = false;

	if (gear.equipped("HNTGOGGLE"))
		hunting_goggles = true;

	if (!weapon.itemkey.isEmpty()) {
		if (weapon.attachList.contains("TOS"))
			autoCheckItems.plus("r", "[B]Telescopic Optical Sight:[b] Reduce difficulty by 1 at Long or Extreme range", 0, 0);
		if (weapon.hasMod("SETTRIGGER_EXTRA_MOD"))
			autoCheckItems.plus("sst", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
		else if (weapon.hasMod("SETTRIGGER_BASE_MOD"))
			autoCheckItems.plus("st", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
	}

	for (int i = 0; i < armor.rowCount(); i++) {
		Item item = armor.itemAt(i);
		if (item.equipped()) {
			if (item.attachList.contains("TARGCOMP"))
				targeting_comp = true;
			if (item.attachList.contains("INTSCAN"))
				integrated_scanner = true;
			if (item.attachList.contains("EOS"))
				eos = true;
			if (item.itemkey == "MOUNTARMOR")
				mount_armor = true;
		}
	}
	if (mount_armor && skill_id == KM_ATHL)
		autoCheckItems.plus("BB", "[B]Mountaineer Armour:[b] Add to all climb or rappel checks", 0, 0);
	if (targeting_comp && ranged)
		autoCheckItems.plus("B", "[B]Targeting Computer:[b] Add Boost dice at Long and Extreme range", 0, 0);
	else if ((integrated_scanner || hunting_goggles) && skill_id == KM_PERC) {
		if (integrated_scanner)
			autoCheckItems.plus("B", "[B]OmniScan 3 Scanner:[b] To detect movement or hidden enemies", 0, 0);
		if (hunting_goggles)
			autoCheckItems.plus("B", "[B]Hunting Goggles:[b] Add when locating prey", 0, 0);
	}
	if (eos) {
		if (combat || skill_id == KM_PERC || skill_id == KM_VIGIL || skill_id == KM_IVIG)
			autoCheckItems.plus("NN", "[B]Enhanced Optics Suite:[b] Darkness, smoke or environmental effects that obscure vision", 0, 0);
	}
	if (hunting_goggles) {
		if (combat && !gunnery)
			autoCheckItems.plus("NN", "[B]Hunting Googles:[b] Concealment, darkness, fog and mist", 0, 0);
	}

	if (skill_id == KM_MED) {
		if (gear.equipped("MEDPAC"))
			default_check = autoCheckItems.plus("B", "[B]Medpac:[b] Add to all Medicine checks when equipped", 0, 0);

		if (gear.carriedQuantity("MEDAIDPATCH") > 0)
			autoCheckItems.plus("sa", "[B]Med-Aid Patch:[b] Maximum usage 1 per check, [B]Cost: 1x Med-Aid Patch[b]", 0, 0, 0, "MEDAIDPATCH");

		if (gear.equipped("BLOODSCAN"))
			autoCheckItems.plus("aa", "[B]Blood Scanner:[b] After use, add to next Medicine check, [B]Cost: Action[b]", 0, 0);

		ranks = talents.ranks("SURG");
		if (ranks > 0)
			autoCheckItems.plus(QString("@" ESC_WOUND " -%1").arg(ranks), "[B]Surgeon:[b] Add to Wounds recovered on successful check", 0, 0);

		if (gear.carriedQuantity("DROIDMINIMED") > 0)
			autoCheckItems.plus(QString("@" ESC_WOUND " -%1").arg(ranks), "[B]Mini-med Droids:[b] Add to Wounds recovered on successful check", 1, 0);
	}

	// CUSTOM CHECKS -------------------------------------------------

	setup_talent_checks();
	if (talentChecks.contains(skillKey)) {
		foreach (CheckItem* item, talentChecks[skillKey].checks) {
			int ref = autoCheckItems.plus(item->pool, item->desc, item->move, item->strain, item->dpoint);
			if (item->default_check)
				checkItem(charac, ref, skillKey, true);
		}
	}

	// VARIOUS ACTIONS -------------------------------------------------

	switch (skill_id) {
		case KM_MED:
			autoCheckItems.plus("D", "[B]Easy Severity:[b] 1-40% Critical injury, 1-50% Wounds, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DD", "[B]Average Severity:[b] 41-90% Critical injury, 51-100% Wounds, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DDD", "[B]Hard Severity:[b] 91-125% Critical injury, 101+% Wounds, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DDDD", "[B]Easy Severity:[b] 126-150% Critical injury, [B]Cost: Action[b]", 0, 0);
			break;
		case KM_MECH:
			autoCheckItems.plus("D", "[B]Damage Control (Easy):[b] System strain or hull trauma less than half threshold, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DD", "[B]Damage Control (Average):[b] System strain or hull trauma less or equal threshold, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DDD", "[B]Damage Control (Hard):[b] System strain or hull trauma exceeds threshold, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DD", "[B]Boost Sheilds:[b] On success add 1 to a shield zone, Duration: [SU] less 1 rounds, [B]Cost: Action[b]", 0, 0);
			break;
		case KM_PILOTSP:
		case KM_PILOTPL:
			autoCheckItems.plus("D", "[B]Gain the Advantage (Easy):[b] Going faster than opponent, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DD", "[B]Gain the Advantage (Average):[b] Vehicle speed identical, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DDD", "[B]Gain the Advantage (Hard):[b] Opponent's speed is one higher, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DDDD", "[B]Gain the Advantage (Daunting):[b] Opponent's speed is 2 or more higher, [B]Cost: Action[b]", 0, 0);
			autoCheckItems.plus("DD", "[B]Co-Pilot:[b] Each [SU] downgrades difficulty of pilot's next check by 1, [B]Cost: Action[b]", 0, 0);
			break;
		default:
			break;
	}

	if (checkLists.contains(skillKey))
		checkLists[skillKey].checkAll(false);

	if (default_check)
		checkItem(charac, default_check, skillKey, true);
}

void CurrentData::exitAutoCheckItems(Character* charac, const QString& skillKey, int commit_count)
{
	int		strain = 0;
	QString	consumable;

	setTemporaryStrain(charac, 0);
	if (checkLists.contains(skillKey))
		strain = checkLists[skillKey].strainUsed(consumable);
	strain += autoCheckItems.strainUsed(consumable);
	if (strain > 0)
		charac->adjustStrain(strain);
	if (!consumable.isEmpty())
		addItem(charac, -1, consumable, QString(), 0);
	if (commit_count) {
		QString power;
		CurrentData::instance->forceCommitted[power + "DURATION"] = commit_count;
		charac->emitForceCommittedChanged();
	}
}

void CurrentData::appendCheckItem(Character* charac, const QString& skillKey, const QString& pool, const QString& desc)
{
	//CheckList::instance.startChanges();
	setCheckItem(skillKey, pool, desc);
	if (charac)
		CheckList::instance.rowCountChanged();
	writeCurrentData();
}

void CurrentData::updateCheckItem(Character* charac, int ref, const QString& skillKey, const QString& pool, const QString& desc)
{
	if (checkLists.contains(skillKey)) {
		CheckListItem* item = checkLists[skillKey].findItem(ref);
		if (item) {
			//CheckList::instance.startChanges();
			item->dice = pool;
			item->description = desc;
			CheckList::instance.rowCountChanged();
			checkChecked(charac, skillKey, false, NULL);
			writeCurrentData();
		}
	}
}

void CurrentData::removeCheckItem(Character* charac, int ref, const QString& skillKey)
{
	if (checkLists.contains(skillKey)) {
		//CheckList::instance.startChanges();
		checkLists[skillKey].deleteItem(ref);
		CheckList::instance.rowCountChanged();
		checkChecked(charac, skillKey, false, NULL);
		writeCurrentData();
	}
}

void CurrentData::uncheckAllItem(Character* charac,const QString& skillKey)
{
	iNegativeCheck = 0;
	//CheckList::instance.startChanges();
	autoCheckItems.checkAll(false);
	if (checkLists.contains(skillKey))
		checkLists[skillKey].checkAll(false);
	CheckList::instance.rowCountChanged();
	charac->setChangeDicePool(QString(), false, NULL);
}

void CurrentData::checkItem(Character* charac, int ref, const QString& skillKey, bool list_setup)
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
				setTemporaryStrain(charac, temporaryStrain + item->reduceStrain);
			else
				setTemporaryStrain(charac, temporaryStrain - item->reduceStrain);
		}
		if (!list_setup) {
			if (!item->commitKey.isEmpty()) {
				forceCommitted[item->commitKey] += item->checked ? item->forceCost : -item->forceCost;
				if (item->commitKey == "ENHANCECONT8")
					charac->emitBrawnChanged();
				else if (item->commitKey == "ENHANCECONT9")
					charac->emitAgilityChanged();
				charac->emitForceCommittedChanged();
				charac->characteristicsChanged();
			}
			CheckList::instance.dataChanged();
			//CheckList::instance.rowCountChanged();
		}
	}
	checkChecked(charac, skillKey, list_setup, item);
}

int CurrentData::negativePool()
{
	return iNegativeCheck;
}

void CurrentData::setNegativePool(Character* charac, int bit, const QString& skillKey)
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
	checkChecked(charac, skillKey, false, NULL);
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

void CurrentData::addCriticalWound(Character* charac, int perc, int type)
{
	setCriticalWound(perc, type);
	if (charac)
		InjuryList::instance.rowCountChanged();
	writeCurrentData();
}

void CurrentData::removeCriticalWound(Character* charac, int ref)
{
	for (int i=0; i<injuries.count(); i++) {
		if (injuries[i].ref == ref) {
			injuries.removeAt(i);
			if (injuries.count() < 4)
				appendEmptyWound();
			break;
		}
	}
	if (charac)
		InjuryList::instance.rowCountChanged();
	writeCurrentData();
}

int CurrentData::nonCommitedForce(Character* charac)
{
	return charac->force() - commitCount();
}

void CurrentData::setCriticalWound(int perc, int type)
{
	InjuryItem item;

	if (!perc)
		return;

	item.ref = ++iReferenceCounter;
	item.percent = perc;
	item.type = type;

	for (int i=0; i<injuries.count(); i++) {
		if (injuries[i].ref == 0) {
			injuries[i] = item;
			if (i >= 3)
				appendEmptyWound();
			return;
		}
	}
}

void CurrentData::appendEmptyWound()
{
	InjuryItem item;

	item.ref = 0;
	item.percent = 0;
	item.type = 0;
	injuries.append(item);
}

int CurrentData::findExpLogItem(int ref)
{
	for (int i=0; i<experienceLog.count(); i++) {
		if (experienceLog[i].ref == ref)
			return i;
	}
	return -1;
}

void CurrentData::setExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount, bool loading)
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
			addExpLogItem(type, when, key, name, desc, amount);
			return;
		}

		// Get the original total:
		int total;
		switch (type) {
			case EXP_XP:
				total = attributes[XP];
				break;
			case EXP_DUTY:
				total = duties.findItem(key).size;
				break;
			case EXP_DUTYRANK:
				total = dutyRank;
				break;
			case EXP_MORALITY:
				total = attributes[MORALITY];
				break;
			case EXP_OBLIGATION:
				total = obligations.findItem(key).size;
				break;
		}

		tot.value = total;
		tot.firstItem = tot.lastItem = experienceLog.count();
		tot.itemCount = 1;
		experienceTotal[exp_key] = tot;
		addExpLogItem(type, when, key, name, "Original Total", total);
	}

	experienceTotal[exp_key].value += amount;
	experienceTotal[exp_key].lastItem = experienceLog.count();
	experienceTotal[exp_key].itemCount++;
	addExpLogItem(type, when, key, name, desc, amount);
}

void CurrentData::addExpLogItem(int type, const QDateTime& when, const QString& key, const QString& name, const QString& desc, int amount)
{
	experienceLog.append(ExpLogItem(++iReferenceCounter, type, when, key, name, desc, amount));
}

int CurrentData::findInvLogItem(int ref)
{
	for (int i=0; i<inventoryLog.count(); i++) {
		if (inventoryLog[i].ref == ref)
			return i;
	}
	return -1;
}

QString CurrentData::setInvLogItem(int count, const QDateTime& create, const QDateTime& update, const QString& in_uuid, const QString& itemkey, const QString& desc, int amount, bool loading)
{
	int total = credits;
	int type = ITEM_AMOUNT;
	ItemList* list = NULL;
	QString uuid;

	if (inventoryLog.size() == 0) {
		// Record, or add orignal credits total:
		if (loading) {
			// The first entry loaded must be the original credit total:
			inventoryLog.append(InvLogItem(++iReferenceCounter, count, create, update, in_uuid, itemkey, desc, amount, ITEM_START));
			credits = amount;
			return in_uuid;
		}

		addInvLogItem(create, update, "[B]Original Total Credits[b]", total, ITEM_START);
	}

	if (!itemkey.isEmpty()) {
		// Add an item to the inventory:
		ShopItem shop_item;

		if (customItems.contains(itemkey))
			shop_item = customItems[itemkey];
		else
			shop_item = Shop::instance.getItem(itemkey);

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

		if (shop_item.type == "GEAR" || shop_item.type.isEmpty())
			list = &gear;
		else if (shop_item.type == "ARMOR")
			list = &armor;
		else
			list = &weapons;

		Item item;
		item.clear();
		item.uuid = in_uuid;
		item.itemkey = itemkey;
		list->aquireItem(item, false);
		uuid = item.uuid;

		// Check the original stock:
		if (!invMod.contains(uuid)) {
			InvModItem inv_item;
			inv_item.uuid = uuid;
			inv_item.itemkey = itemkey;
			invMod[uuid] = inv_item;
		}

		if (invMod[uuid].rowCount == 0) {
			// First row for this item:
			int c = 0;
			if (list->containsByUuid(uuid))
				c = list->getItemByUuid(uuid).originalQuantity;

			if (c) {
				// We have an original stock amount
				if (loading) {
					// This must be the original stock:
					invMod[uuid].quantity = 0;
					type = ITEM_ORIG_STOCK;
				}
				else {
					// Record the original stock:
					invMod[uuid].rowCount++;
					invMod[uuid].quantity = c;
					inventoryLog.append(InvLogItem(++iReferenceCounter, c, create, update, uuid, itemkey, QString(), 0, ITEM_ORIG_STOCK));
				}
			}
			else {
				// No original stock:
				invMod[uuid].quantity = 0;
			}
		}

		invMod[uuid].rowCount++; // Count the number of rows with this key:
		invMod[uuid].quantity += count; // And sum the quantity:

		if (invMod[uuid].quantity == 0) {
			if (list->getItemByUuid(uuid).originalQuantity == 0)
				// This means the item was not added by SWCharGen:
				list->removeItemByUuid(uuid);
		}

		if (shop_item.type == "GEAR" || shop_item.type.isEmpty())
			Gear::instance.setRowCountChanged();
		else if (shop_item.type == "ARMOR")
			Armor::instance.setRowCountChanged();
		else
			Weapons::instance.setRowCountChanged();
	}
	else if (!desc.isEmpty()) {
		// Free text has no count and no item ID:
		count = 0;
	}
	else
		return QString();

	inventoryLog.append(InvLogItem(++iReferenceCounter, count, create, update, uuid, itemkey, desc, amount, type));

	credits = total + amount;

	return uuid;
}

void CurrentData::addInvLogItem(const QDateTime& create, const QDateTime& update, const QString& desc, int amount, int type)
{
	inventoryLog.append(InvLogItem(++iReferenceCounter, 0, create, update, QString(), QString(), desc, amount, type));
}

void CurrentData::inventoryChanged(Character* charac, const QString& uuid, const QString& itemkey, bool signal)
{
	if (!invMod.contains(uuid))
		return;

	if (charac && signal) {
		if (weapons.containsByUuid(uuid)) {
			//Weapons::instance.startChanges();
			// "quantity" is taken dynamically from inventory!
			// Just signal change!
			Weapons::instance.rowCountChanged();
		}
		else if (armor.containsByUuid(uuid)) {
			//Armor::instance.startChanges();
			// "quantity" is taken dynamically from inventory!
			// Just signal change!
			Armor::instance.rowCountChanged();
		}
		else if (gear.containsByUuid(uuid)) {
			//Gear::instance.startChanges();
			// "quantity" is taken dynamically from inventory!
			// Just signal change!
			Gear::instance.rowCountChanged();
		}
	}

	if (itemkey == "STIMPACK")
		charac->emitStimPacksChanged();
	else if (itemkey == "ERP")
		charac->emitErpsChanged();
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

	addCheckItem(skillKey, ++iReferenceCounter, pool, desc);
}

void CurrentData::checkChecked(Character* charac, const QString& skillKey, bool list_setup, CheckListItem* item)
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
	charac->setChangeDicePool(new_curr_pool, list_setup, item);
}

void CurrentData::addCheckItem(const QString& skillKey, int ref, const QString& pool, const QString& desc)
{
	if (!checkLists.contains(skillKey)) {
		CheckListData list;

		list.skillKey = skillKey;
		checkLists[skillKey] = list;
	}
	checkLists[skillKey].plusCustom(ref, pool, desc);
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
	data += QString(" <Injuries>\n");
	for (int i=0; i<injuries.count(); i++) {
		InjuryItem item = injuries[i];
		if (item.ref > 0) {
			data += QString("  <Injury>");
			data += QString("<Percent>%1</Percent>").arg(item.percent);
			data += QString("<Type>%1</Type>").arg(item.type);
			data += QString("</Injury>\n");
		}
	}
	data += QString(" </Injuries>\n");
	data += QString(" <Experience>\n");
	for (int i=0; i<experienceLog.size(); i++) {
		data += QString("  <XP>");
		data += QString("<Type>%1</Type>").arg(expTypeToString(experienceLog[i].type));
		if (experienceLog[i].when.isValid())
			data += QString("<Create>%1</Create>").arg(experienceLog[i].when.toTimeSpec(Qt::UTC).toString(Qt::ISODate));
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
			if (inventoryLog[i].create.isValid())
				data += QString("<Create>%1</Create>").arg(inventoryLog[i].create.toTimeSpec(Qt::UTC).toString(Qt::ISODate));
			if (inventoryLog[i].update.isValid() && inventoryLog[i].update != inventoryLog[i].create)
				data += QString("<Update>%1</Update>").arg(inventoryLog[i].update.toTimeSpec(Qt::UTC).toString(Qt::ISODate));
			if (!inventoryLog[i].uuid.isEmpty() && inventoryLog[i].uuid != inventoryLog[i].itemkey)
				data += QString("<Uuid>%1</Uuid>").arg(inventoryLog[i].uuid);
			if (!inventoryLog[i].itemkey.isEmpty())
				data += QString("<Key>%1</Key>").arg(inventoryLog[i].itemkey);
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
		if (inv_item.stored != UNKNOWN_STORED || inv_item.state != UNDEFINED) {
			data += QString("  <StoreItem>");
			if (!inv_item.uuid.isEmpty() && inv_item.uuid != inv_item.itemkey)
				data += QString("<Uuid>%1</Uuid>").arg(inv_item.uuid);
			if (!inv_item.itemkey.isEmpty())
				data += QString("<Key>%1</Key>").arg(inv_item.itemkey);
			if (inv_item.stored != UNKNOWN_STORED)
				data += QString("<Amount>%1</Amount>").arg(inv_item.stored);
			if (inv_item.state != UNDEFINED)
				data += QString("<State>%1</State>").arg(inv_item.state);
			data += QString("</StoreItem>\n");
		}
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

	if (!fileName.isEmpty()) {
		DataAccess::writeFile(DatUtil::getCurrentFolder() + fileName, data.toUtf8());
		//DataAccess::currentData.startChanges();
		int row = DataAccess::currentData.findRow("file", fileName);
		if (row < 0)
			row = DataAccess::currentData.appendRow();
		DataAccess::setCurrentDataRow(row, fileName);
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

void CurrentData::setTemporaryStrain(Character* charac, int value)
{
	temporaryStrain = value;
	charac->emitCurrentStrainChanged();
}

// CurrentDataXML -------------------------------------------

CurrentDataXML::CurrentDataXML(CurrentData* current_data)
{
	iCurrentData = current_data;
	iInjuryPercent = 0;

	iExpItem.clear();

	iItemCount = 0;
	iItemCreate = QDateTime();
	iItemUpdate = QDateTime();
	iItemUuid.clear();
	iItemKey.clear();
	iItemDesc.clear();
	iItemAmount = 0;

	iItemPool.clear();
}

bool CurrentDataXML::xmlElement(const DatStringBuffer& path, const char* value)
{
	//qDebug() << "XML" << path << value;
	if (path.endsWith("/Wounds/"))
		iCurrentData->wounds = toInt(value);
	else if (path.endsWith("/Strain/"))
		iCurrentData->strain = toInt(value);
	else if (path.endsWith("/Conflict/"))
		iCurrentData->conflict = toInt(value);
	else if (path.endsWith("/StimPacksUsed/"))
		iCurrentData->stimPacksUsed = toInt(value);
	else if (path.endsWith("/ErpsUsed/"))
		iCurrentData->erpsUsed = toInt(value);
	else if (path.endsWith("/WoundHistory/"))
		iCurrentData->woundHistory = value;
	else if (path.endsWith("/StrainHistory/"))
		iCurrentData->strainHistory = value;
	else if (path.endsWith("/ConflictHistory/"))
		iCurrentData->conflictHistory = value;
	else if (path.endsWith("/WoundDelta/"))
		iCurrentData->woundDelta = toInt(value);
	else if (path.endsWith("/StrainDelta/"))
		iCurrentData->strainDelta = toInt(value);
	else if (path.endsWith("/Injury/Percent/"))
		iInjuryPercent = toInt(value);
	else if (path.endsWith("/Injury/Type/"))
		iCurrentData->setCriticalWound(iInjuryPercent, toInt(value));
	else if (path.endsWith("/XP/#open")) {
		iExpItem.clear();
		iExpItem.type = EXP_XP;
	}
	else if (path.endsWith("/XP/Type/"))
		iExpItem.type = CurrentData::expTypeToInt(value);
	else if (path.endsWith("/XP/Create/"))
		iExpItem.when = QDateTime::fromString(value, Qt::ISODate);
	else if (path.endsWith("/XP/Key/"))
		iExpItem.key = value;
	else if (path.endsWith("/XP/Name/"))
		iExpItem.name = value;
	else if (path.endsWith("/XP/Description/"))
		iExpItem.desc = value;
	else if (path.endsWith("/XP/Amount/"))
		iExpItem.amount = toInt(value);
	else if (path.endsWith("/XP/#end"))
		iCurrentData->setExpLogItem(iExpItem.type, iExpItem.when, iExpItem.key, iExpItem.name, iExpItem.desc, iExpItem.amount, true);
	else if (path.endsWith("/Inventory/Item/#open")) {
		iItemCount = 0;
		iItemCreate = QDateTime();
		iItemUpdate = QDateTime();
		iItemUuid.clear();
		iItemKey.clear();
		iItemDesc.clear();
		iItemAmount = 0;
	}
	else if (path.endsWith("/Item/Count/"))
		iItemCount = toInt(value);
	else if (path.endsWith("/Item/Create/"))
		iItemCreate = QDateTime::fromString(value, Qt::ISODate);
	else if (path.endsWith("/Item/Update/"))
		iItemUpdate = QDateTime::fromString(value, Qt::ISODate);
	else if (path.endsWith("/Item/Uuid/"))
		iItemUuid = value;
	else if (path.endsWith("/Item/Key/"))
		iItemKey = value;
	else if (path.endsWith("/Item/Description/"))
		iItemDesc = value;
	else if (path.endsWith("/Item/Amount/"))
		iItemAmount = toInt(value);
	else if (path.endsWith("/Inventory/Item/#end")) {
		if (iItemCreate.isValid() && !iItemUpdate.isValid())
			iItemUpdate = iItemCreate;
		iCurrentData->setInvLogItem(iItemCount, iItemCreate, iItemUpdate, iItemUuid, iItemKey, iItemDesc, iItemAmount, true);
	}
	else if (path.endsWith("/StoreItem/#open")) {
		iStoreItemUuid.clear();
		iStoreItemKey.clear();
		iStoreItemAmount = UNKNOWN_QUANTITY;
		iStoreItemState = UNDEFINED;
	}
	else if (path.endsWith("/StoreItem/Uuid/"))
		iStoreItemUuid = value;
	else if (path.endsWith("/StoreItem/Key/"))
		iStoreItemKey = value;
	else if (path.endsWith("/StoreItem/Amount/"))
		iStoreItemAmount = toInt(value);
	else if (path.endsWith("/StoreItem/State/"))
		iStoreItemState = toInt(value);
	else if (path.endsWith("/Storage/StoreItem/#end"))
		iCurrentData->storeItem(iStoreItemUuid, iStoreItemKey, iStoreItemAmount, iStoreItemState, NULL);

	else if (path.endsWith("/CurrentData/CheckList/#open"))
		iItemCheckSkill.clear();
	else if (path.endsWith("/CheckList/Skill/"))
		iItemCheckSkill = value;
	else if (path.endsWith("/CheckList/CheckItem/#open")) {
		iItemPool.clear();
		iItemDesc.clear();
	}
	else if (path.endsWith("/CheckItem/DicePool/"))
		iItemPool = value;
	else if (path.endsWith("/CheckItem/Description/"))
		iItemDesc = value;
	else if (path.endsWith("/CheckList/CheckItem/#end"))
		iCurrentData->setCheckItem(iItemCheckSkill, iItemPool, iItemDesc);

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
		iCurrentData->addCustomSkill(NULL, iSkillName, iSkillChar, iSkillRank);

	return true;
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
	if (row < CurrentData::instance->customSkills.size())
		skill = CurrentData::instance->customSkills[row];

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

	count = CurrentData::instance->autoCheckItems.items.count();
	if (CurrentData::instance->checkLists.contains(key)) {
		count += CurrentData::instance->checkLists[key].items.count();
	}
	return count+1;
}

QVariant CheckList::getValue(int row, int col)
{
	QString			key = Character::instance->activeSkillKey();
	CheckListItem*	item = NULL;

	if (row >= 0) {
		int c1 = CurrentData::instance->autoCheckItems.items.count();

		if (row < c1) {
			item = &CurrentData::instance->autoCheckItems.items[row];
		}
		else {
			row -= c1;
			if (CurrentData::instance->checkLists.contains(key)) {
				int c2 = CurrentData::instance->checkLists[key].items.count();

				if (row < c2)
					item = &CurrentData::instance->checkLists[key].items[row];
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
	AbstractDataList(QStringList() << "ref" << "itemkey" << "count" << "item"
		<< "description" << "amount" << "type" << "rarity" << "price" << "keycount")

{
}

int InventoryLog::rowCount()
{
	return CurrentData::instance->inventoryLog.size() + 2;
}

QVariant InventoryLog::getValue(int row, int col)
{
	InvLogItem* item = NULL;
	int count = CurrentData::instance->inventoryLog.size();

	if (row >= 0) {
		if (row < count)
			item = &CurrentData::instance->inventoryLog[row];
	}

	if (item) {
		InvModItem mod_item;
		ShopItem shop_item;

		if (!item->uuid.isEmpty())
			mod_item = CurrentData::instance->invMod[item->uuid];
		if (CurrentData::instance->customItems.contains(item->uuid))
			shop_item = CurrentData::instance->customItems[item->uuid];
		else
			shop_item = Shop::instance.getItem(item->itemkey);

		switch (col) {
			case 0:
				return item->ref;
			case 1:
				return item->itemkey;
			case 2:
				return item->count;
			case 3:
				return shop_item.name.isEmpty() ? (item->itemkey.isEmpty() ? item->uuid : item->itemkey) : shop_item.name;
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
					return CurrentData::instance->credits;
				case 6:
					return ITEM_TOTAL;
			}
			return 0;
		}
		else {
			switch (col) {
				case 1:
				case 3:
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

