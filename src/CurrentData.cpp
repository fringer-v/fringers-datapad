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

// CharSkill -------------------------------------

int CharSkill::skillRanks()
{
	int ranks = internalRanks;

	if (key == "PERC")
		ranks += CurrentData::instance->getCommitCount("FARSIGHTCONTROL4");

	if (ranks > 5)
		return 5;
	return ranks;
}

int CharSkill::skillAttribute(Character* charac, Skill* skill)
{
	QString ch;

	if (!skill)
		skill = Skill::getSkill(skillID);

	if (!skill)
		return 0;

	if (!charac)
		charac = Character::instance;

	ch = skill->characteristic;
	if (skillID == KM_LTSABER)
		ch = CurrentData::instance->talents.getLightSaberChar();

	return charac->getAttribute(ch);
}

QString CharSkill::getBasicPool(Character* charac)
{
	return DatUtil::getBasicDicePool(skillRanks(), skillAttribute(charac, Skill::getSkill(skillID)));
}

double CharSkill::poolRating(Character* charac)
{
	Skill* skill = Skill::getSkill(key);

	int char_value = charac->getAttribute(skill->characteristic);
	int s_ranks = skillRanks();

	if (s_ranks > char_value) {
		int tmp = char_value;
		char_value = s_ranks;
		s_ranks = tmp;
	}

	// Yellow is worth a bit more than 1.5 of a green:
	return (s_ranks * 1.501) + (char_value-s_ranks);
}

QString CharSkill::getDicePool()
{
	Character* charac;
	CurrentData* current_data;
	Skill* skill;

	if (!(charac = Character::instance))
		return "";
	if (!(current_data = CurrentData::instance))
		return "";

	if (!(skill = Skill::getSkill(skillID)))
		return "EE";

	QString pool;
	int boostCount = 0;
	int setbackCount = 0;
	int addForceDice = 0;
	int optionalRemoveSetback = 0;
	int optionalDowngradeDiff = 0;
	int optionalDowngradeCount = 0;
	int optionalBoost = 0;
	int optionalReduceDiffCount = 0;
	int optionalPlusAbility = 0;
	int optionalUpgrade = 0;
	int threatCount = 0;
	bool defr = false;
	bool defm = false;
	int skill_ranks = skillRanks();
	int skill_attribute = skillAttribute(charac, skill);

	switch (skillID) {
		case KM_DEFM:
			defm = true;
			pool = DatUtil::repeat("S", charac->defenseMelee());
			if (current_data->isCommitted("SENSECONTROL1")) {
				optionalDowngradeCount++;
				if (current_data->talents.contains("SENSESTRENGTH"))
					optionalDowngradeCount++;
			}
			threatCount = current_data->getCommitCount("MISDIRCONTROL3");
			break;
		case KM_DEFR:
			defr = true;
			pool = DatUtil::repeat("S", charac->defenseRanged());
			if (current_data->isCommitted("SENSECONTROL1")) {
				optionalDowngradeCount++;
				if (current_data->talents.contains("SENSESTRENGTH"))
					optionalDowngradeCount++;
			}
			threatCount = current_data->getCommitCount("MISDIRCONTROL3");
			break;
		default:
			pool = DatUtil::getBasicDicePool(skill_ranks, skill_attribute);
			if (skill->type == COMBAT) {
				if (current_data->isCommitted("SENSECONTROL3"))
					pool += "UU";
			}
			break;
	}

	switch (skillID) {
		case KM_REC:
			if (current_data->talents.contains("BAL"))
				addForceDice++;
			break;
		case KM_ICOOL:
		case KM_IVIG:
			if (current_data->talents.contains("FORSEECONTROL1") > 0)
				addForceDice++;
			else if (current_data->talents.contains("WARFORCONTROL1") > 0)
				addForceDice++;
			break;
		case KM_VIGIL:
		case KM_PERC:
			if (current_data->isCommitted("SEEKCONTROL1")) {
				pool += 'U';
			}
			if (current_data->talents.contains("FARSIGHTCONTROL3") > 0)
				addForceDice++;
			break;
		case KM_PILOTPL:
		case KM_PILOTSP:
			if (current_data->gear.equipped("CYAVIONCAAF2")) {
				if (skill_ranks > 0) {
					if (skill_ranks > skill_attribute)
						optionalPlusAbility++;
					else
						optionalUpgrade++;
					skill_ranks++;
				}
			}
			break;
		default:
			break;
	}

	foreach (CharTalent char_talent, current_data->talents.charTalentMap) {
		Talent talent = AllTalents::instance()->getTalent(char_talent.key);

		foreach (DieMod mod, talent.dieModList.modMap) {
			if (mod.skillKey == key || mod.skillType == skill->type) {
				if (talent.key == "SLEIGHTMIND")
					optionalBoost += mod.boostCount * char_talent.ranks;
				else
					boostCount += mod.boostCount * char_talent.ranks;
				if (talent.key == "KEENEYED")
					optionalRemoveSetback += mod.setbackCount * char_talent.ranks;
				else
					setbackCount += mod.setbackCount * char_talent.ranks;
				addForceDice += mod.forceCount;
				pool += DatUtil::repeat("a", mod.advantageCount * char_talent.ranks);
				pool += DatUtil::repeat("t", mod.threatCount * char_talent.ranks);
				pool += DatUtil::repeat("U", mod.upgradeCount * char_talent.ranks);
			}
		}

		if (char_talent.key == "INTIM" && skillID == KM_COERC)
			optionalDowngradeDiff += char_talent.ranks;
		if (char_talent.key == "CONGENIAL" &&
			(skillID == KM_CHARM || skillID == KM_NEG))
			optionalDowngradeDiff += char_talent.ranks;
		if (char_talent.key == "DODGE" &&
			(skillID == KM_DEFM || skillID == KM_DEFR))
			optionalDowngradeCount += char_talent.ranks;
		if (char_talent.key == "CONF" && skillID == KM_FDISC)
			optionalReduceDiffCount += char_talent.ranks;
	}

	for (int i = 0; i<current_data->armor.rowCount(); i++) {
		Item item = current_data->armor.itemAt(i);
		if (item.equipped()) {
			pool += item.getDieMods(key, addForceDice);
			if (item.hasQuality(key)) {
				Quality qual = item.getQuality(key);
				// Adds a rank (not a straight upgrade!
				if (skill_ranks > skill_attribute)
					pool += DatUtil::repeat("A", qual.count);
				else
					pool += DatUtil::repeat("U", qual.count);
			}
		}
	}

	for (int i = 0; i<current_data->gear.rowCount(); i++) {
		Item item = current_data->gear.itemAt(i);
		if (item.equipped() && item.itemkey != "CYAVIONCAAF2") {
			pool += item.getDieMods(key, addForceDice);
			if (item.hasQuality(key)) {
				Quality qual = item.getQuality(key);
				// Adds a rank (not a straight upgrade!
				if (skill_ranks > skill_attribute)
					pool += DatUtil::repeat("A", qual.count);
				else
					pool += DatUtil::repeat("U", qual.count);
			}
		}
	}

	switch (skillID) {
		case KM_CHARM:
		case KM_COERC:
			if (current_data->gear.equipped("EXPJEWELRY"))
				pool += "a";
			break;
		default:
			break;
	}

	pool += DatUtil::repeat("B", boostCount);
	pool += DatUtil::repeat("N", setbackCount);
	pool = DatUtil::normalizeDice(pool);

	if (defr || defm) {
		int r, m;

		current_data->negetiveDefence(r, m);
		r = NEG_PURPLE_COUNT(r);
		m = NEG_PURPLE_COUNT(m);
		if (defr)
			pool = "|"+QString("D").repeated(r)+"|" + pool;
		if (defm)
			pool = "|"+QString("D").repeated(m)+"|" + pool;
	}

	if (addForceDice + optionalRemoveSetback + optionalDowngradeDiff + optionalUpgrade +
		optionalDowngradeCount + optionalBoost + optionalReduceDiffCount + optionalPlusAbility) {
		QString opt;
		// Mark the optional dice
		// Optional dice are just an indicator that something else
		// can be added to the pool
		opt += DatUtil::repeat("A", optionalPlusAbility);
		opt += DatUtil::repeat("U", optionalUpgrade);
		opt += DatUtil::repeat("B", optionalBoost);
		if (addForceDice > 0)
			opt += Character::instance->getForcePool();
		opt += DatUtil::repeat("N", optionalRemoveSetback);
		opt += DatUtil::repeat("d", optionalDowngradeDiff);
		opt += DatUtil::repeat("u", optionalDowngradeCount);
		opt += DatUtil::repeat("r", optionalReduceDiffCount);
		pool += "|"+opt+"|"; // These will be hidden in the display
	}

	pool += DatUtil::repeat("t", threatCount);
	return pool;
}

CharItem CharItemList::findItem(const QString& key)
{
	for (int i=0; i<items.count(); i++) {
		if (items[i].key == key)
			return items[i];
	}
	return CharItem();
}

// ChecklistData -------------------------

void ChecklistData::clear()
{
	iRefCount = 0;
	items.clear();
}

ChecklistItem* ChecklistData::findItem(int reference)
{
	QMutableListIterator<ChecklistItem> i(items);
	ChecklistItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference == reference) {
			return item;
		}
	}
	return NULL;
}

void ChecklistData::checkAll(bool checked)
{
	QMutableListIterator<ChecklistItem> i(items);
	ChecklistItem* item;

	while (i.hasNext()) {
		item = &i.next();
		item->checked = checked;
	}
}

void ChecklistData::deleteItem(int reference)
{
	QMutableListIterator<ChecklistItem> i(items);
	ChecklistItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference == reference) {
			i.remove();
		}
	}
}

QString ChecklistData::getCurrentPool()
{
	QListIterator<ChecklistItem> i(items);
	const ChecklistItem* item;
	QString curr_pool;

	while (i.hasNext()) {
		item = &i.next();
		if (item->checked) {
			if (!item->dicePool.startsWith("@"))
				curr_pool += item->dicePool;
		}
	}
	return curr_pool;
}

bool ChecklistData::hasCustomItem() const
{
	QListIterator<ChecklistItem> i(items);
	const ChecklistItem* item;

	while (i.hasNext()) {
		item = &i.next();
		if (item->reference > 0)
			return true;
	}
	return false;
}

int ChecklistData::strainUsed(QString& consumable) const
{
	QListIterator<ChecklistItem> i(items);
	const ChecklistItem* item;
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

int ChecklistData::movesUsed() const
{
	QListIterator<ChecklistItem> i(items);
	const ChecklistItem* item;

	int move = 0;

	while (i.hasNext()) {
		item = &i.next();
		if (item->checked)
			move += item->moveRequired;
	}

	return move;
}

void ChecklistData::appendCustom(int ref, const QString& pool, const QString& desc)
{
	ChecklistItem item;

	item.reference = ref;
	item.dicePool = pool;
	item.description = desc;
	items.append(item);
}

int ChecklistData::append(ChecklistItem& item)
{
	QString cost;

	if (item.moveRequired) {
		if (item.moveRequired == 1)
			cost = "Maneuver";
		else
			cost = QString("%1 Maneuvers").arg(item.moveRequired);
	}
	if (item.reduceStrain) {
		if (!cost.isEmpty())
			cost += ", ";
		cost += QString("%1 Strain").arg(item.reduceStrain);
	}
	if (item.destinyPointCost) {
		if (!cost.isEmpty())
			cost += ", ";
		if (item.destinyPointCost == 1)
			cost += "Destiny Point";
		else
			cost += QString("%1 Destiny Points").arg(item.destinyPointCost);
	}
	if (!cost.isEmpty())
		item.description += QString(", [B]Cost: %1[b]").arg(cost);

	iRefCount--;
	item.reference = iRefCount;
	items.append(item);
	return iRefCount;
}

void ChecklistData::plusDamage(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusDamage = count;
}

void ChecklistData::plusPierce(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusPierce = count;
}

void ChecklistData::plusCrit(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusCrit = count;
}

void ChecklistData::plusRange(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusRange = count;
}

void ChecklistData::plusMagnitude(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusMagnitude = count;
}

void ChecklistData::plusStrength(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusStrength = count;
}

void ChecklistData::plusDuration(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusDuration = count;
}

void ChecklistData::plusExtra(int ref, int count)
{
	ChecklistItem* item;
	if ((item = findItem(ref)))
		item->plusExtra = count;
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
	checklistItems.clear();
	checkLists.clear();
	dutyRank = 0;
	experienceLog.clear();
	experienceTotal.clear();
	injuries.clear();
	for (int i=0; i<4; i++)
		appendEmptyWound();

	iForceCommitted.clear();
	iCommitCountMax = 0;

	iCharSkills.clear();

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

void CurrentData::clearChecklist(Character* charac)
{
	uncheckAllItem(charac);
	checklistItems.clear();
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

/*
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
*/

QString CurrentData::plurize(const QString& thing, int val)
{
	return (val == 1 ? thing : thing+"s").arg(val);
}

// showChecklist() <---- Initialise power
void CurrentData::setChecklist(Character* charac, QString skillKey, QString talentKey, Item& weapon)
{
	Skill* skill = Skill::getSkill(skillKey);
	MethodID skill_id = KeyMethod::instance.getID(skillKey);
	MethodID talent_id = KeyMethod::instance.getID(talentKey);
	int ranks;
	int ref;

	uncheckAllItem(charac);
	checklistItems.clear();

	// MANEUVER -------------------------------------------------

	if (!skill || skill->type != SPECIAL)
		checklistAdd(DatUtil::poolText("+1 Move"), "[B]Maneuver:[b] Perform an additional action that requires a maneuver", 1, 0);

	// TALENT CHECK LISTS -------------------------------------------------

	if (!talentKey.isEmpty()) {
		QString power;
		QString base;
		bool opposed_dicip = false;
		CharSkill disc_skill = getCharSkill(KM_DISC);
		Talent talent = AllTalents::instance()->getTalent(talentKey);

		CharTalentMap::forcePower(talentKey, power, base);
		int magnitude = talents.get(power + "MAGNITUDE").ranks;
		int strength = talents.get(power + "STRENGTH").ranks;
		int duration = talents.get(power + "DURATION").ranks;
		int force_dice = charac->force() - forceCommitCount();

		checklistRangeAdd(power);

		switch (talent_id) {
			case KM_INFLUENCECONTROL1:
			case KM_FARSIGHTBASIC:
			case KM_FARSIGHTCONTROL1:
			case KM_FARSIGHTCONTROL2:
			case KM_FARSIGHTCONTROL4: // -- ongoing
			case KM_FARSIGHTCONTROL5:
			case KM_FARSIGHTCONTROL6:
			case KM_FARSIGHTMASTERY:
				checklistMultiForce(1, "[B]Duration:[b] Increase by %1 %2", "round/minute", "rounds/minutes", duration, PLUS_DURATION);
				break;
			default:
				break;
		}

		// showChecklist() <---- Initialise power
		int pip_count = 1;
		switch (talent_id) {
			case KM_BINDBASIC:
			case KM_BINDCONTROL1:
			case KM_BINDCONTROL2:
			case KM_BINDMASTERY:
				pip_count++;
				// No break!
			// checkQualMag()
			case KM_MISDIRBASIC:
			case KM_MISDIRCONTROL1:
			case KM_MISDIRCONTROL2:
			case KM_BATMEDBASIC:
			case KM_BATMEDCONTROL1:
			case KM_BATMEDCONTROL2:
			case KM_BATMEDMASTERY:
			case KM_BATMEDDURATION:
			case KM_MISDIRDURATION:
				magnitude = magnitude * charac->presence();
				// No break!
			case KM_SENSEBASIC:
			case KM_SENSECONTROL2:
			case KM_INFLUENCEBASIC:
			case KM_INFLUENCECONTROL1:
			case KM_MOVEBASIC:
			case KM_MOVECONTROL1:
			case KM_MOVECONTROL2:
			case KM_MOVECONTROL3:
				checklistMultiForce(pip_count, "[B]Magnitude:[b] Effect %1 additional %2", "target", "targets", magnitude, PLUS_MAGNITUDE);
				break;
			case KM_SEEKBASIC:
			case KM_SEEKSTRENGTH:
			case KM_SEEKDURATION:
			case KM_SEEKMASTERY:
			{
				CharSkill v_skill = getCharSkill(KM_VIGIL);

				ref = checklistAdd(".", "[B]Locate:[b] Gain insight into general location or direction of person or object", 0, 0);
				if (talent_id == KM_SEEKMASTERY || talent_id == KM_SEEKDURATION)
					checkItem(charac, ref);

				if (talents.contains("SEEKCONTROL2"))
					checklistMultiForce(1, "[B]Control: Targets:[b] Track %1 additional %2", "target", "targets", 1, PLUS_DURATION);

				if (magnitude > 0) {
					ref = checklistAdd(".", plurize("[B]Magnitude:[b] Gain %1 additional helpful detail(s) about the location, direction or path", magnitude), 0, 0);
					checklistItems.plusMagnitude(ref, magnitude);
				}
				if (talents.contains("SEEKDURATION"))
					checklistMayCommit(1, "SEEKDURATION", "[B]Duration:[b] Commit to track target even when it moves", 1);
				checklistAdd(v_skill.getBasicPool(charac)+"DD", "[B]Illusions:[b] (or vs Discipline), to see through illusions", 0, 0);
				checklistMultiForce(1, "[B]Stength:[b] Eliminate %1 Force-based %2", "illusion", "illusions", strength, PLUS_STRENGTH, talent_id == KM_SEEKSTRENGTH);

				if (talents.contains("SEEKMASTERY")) {
					ref = checklistAdd("...", "[B]Seek Mastery:[b] Add [TR] to combat checks against target until end of encounter", 0, 0);
					checklistItems.plusRange(ref, 1);
					if (talent_id == KM_SEEKMASTERY)
						checkItem(charac, ref);
				}

				break;
			}
			case KM_WARFORBASIC:
			case KM_WARFORCONTROL2:
			case KM_WARFORCONTROL4:
				condChecklistAdd("WARFORCONTROL2", DatUtil::poolText("+1 Move"), "[B]Maneuver:[b] Once per encounter use basic power as a maneuver", 1, 0);
				ranks = talents.ranks("WARFORSTRENGTH");
				if (ranks > 0) {
					ref = checklistAdd("..", QString("[B]Strength:[b] Add %1 to next check against target").arg(QString("[BO]").repeated(ranks)), 0, 0);
					checklistItems.plusStrength(ref, ranks);
				}
				ranks = talents.ranks("WARFORDURATION");
				if (ranks > 0) {
					ref = checklistAdd("..", plurize("[B]Duration:[b] Bonuses apply to all checks for %1 round", ranks), 0, 0);
					checklistItems.plusDuration(ref, ranks);
				}
				break;
			case KM_SUPPRESSBASIC:
			case KM_SUPPRESSDURATION:
			case KM_SUPPRESSCONTROL2:
			case KM_SUPPRESSCONTROL3:
			case KM_SUPPRESSMASTERY:
				checklistMultiForce(1, "[B]Strength:[b] Add additional [FA] to hostile Force checks", QString(), QString(), strength, PLUS_STRENGTH);

				if (talents.contains("SUPPRESSDURATION"))
					checklistMayCommit(1, "SUPPRESSDURATION", "[B]Duration:[b] Commit to sustain ongoing effects on each target in range", 1);
				if (talents.contains("SUPPRESSCONTROL2")) {
					ref = checklistAdd(DatUtil::poolText("-1 Destiny"), "[B]Out of Turn:[b] Once per session, use Suppress as an out-of-turn incidental", 0, 0, 1);
					if (talent_id == KM_SUPPRESSCONTROL2)
						checkItem(charac, ref);
				}
				if (talents.contains("SUPPRESSMASTERY")) {
					ref = checklistAdd(disc_skill.getBasicPool(charac) + ".", "[B]Mastery:[b] vs Discipline, target must uncommit all [FO] and end effects", 0, 0);
					checklistItems.plusExtra(ref, 1);
					if (talent_id == KM_SUPPRESSMASTERY)
						checkItem(charac, ref);
				}
				break;
			case KM_FARSIGHTBASIC:
			case KM_FARSIGHTCONTROL1:
			case KM_FARSIGHTCONTROL2:
			case KM_FARSIGHTCONTROL4: // -- ongoing
			case KM_FARSIGHTCONTROL5:
			case KM_FARSIGHTCONTROL6:
			case KM_FARSIGHTMASTERY:
				condChecklistAdd("FARSIGHTCONTROL1", ".", "[B]Microscopic Details:[b] See microscopic details of one object (Engaged range)", 0, 0);
				condChecklistAdd("FARSIGHTCONTROL2", ".", "[B]Transparent:[b] May see through one object within range", 0, 0);
				condChecklistAdd("FARSIGHTCONTROL6", ".", "[B]Fine Details:[b] May make out fine details on a single object within range", 0, 0);
				multiCommit(charac, "FARSIGHTCONTROL4", "[B]Ongoing Effect:[b] Sustain all effects and increase ranks in Perception by %1", QString(), 5 - getCharSkill(KM_PERC).internalRanks, false);
				condChecklistAdd("FARSIGHTCONTROL5", "..", "[B]Panoramic Vision:[b] See in a 360-degree arc simultaneously", 0, 0);
				condChecklistAdd("FARSIGHTMASTERY", ".", "[B]Mastery:[b] See as though the PC is at another position within Short (Planetary) range", 0, 0);
				break;
			case KM_FORSEEBASIC:
				checklistMultiForce(1, "[B]Duration:[b] See %1 more %2 into the future", "day", "days", duration, PLUS_DURATION);
				checklistMultiForce(1, "[B]Strength:[b] Pick out %1 additional specific %2", "detail", "details", strength, PLUS_STRENGTH);
				break;
			default:
				break;
		}

		switch (talent_id) {
			case KM_BINDBASIC:
			case KM_BINDCONTROL1:
			case KM_BINDCONTROL2:
			case KM_BINDMASTERY:
				checklistMultiForce(1, "[B]Strength:[b] Disorient targets for %1 %2", "round", "rounds", strength, PLUS_STRENGTH);

				condChecklistAdd("BINDCONTROL1", ".", QString("[B]Casuse Strain:[b] All targets suffer %1 Strain when taking an action").arg(charac->willpower()), 0, 0);
				condChecklistAdd("BINDCONTROL2", ".", "[B]Move Target:[b] Move target 1 range band closer or further away", 0, 0);

				if (duration > 0)
					checklistMayCommit(3, "BINDDURATION", "[B]Duration:[b] Commit to sustain all ongoing effects", 3);

				if (talents.contains("BINDMASTERY")) {
					ref = checklistAdd(disc_skill.getBasicPool(charac), "[B]Mastery:[b] If [LI] used, Stagger targets, if [DA] used, cause Crit +10% for each [FP] spent", 0, 0);
					if (talent_id == KM_BINDMASTERY)
						checkItem(charac, ref);
				}
				else
					opposed_dicip = true;
				break;
			case KM_BATMEDBASIC:
			case KM_BATMEDCONTROL1:
			case KM_BATMEDCONTROL2:
			case KM_BATMEDMASTERY:
			case KM_BATMEDDURATION:
				if (strength > 0) {
					for (int i=0; i<(force_dice+1)/2+1; i++) {
						ref = checklistAdd("..", "[B]Strength:[b] Add additonal [SU] to affected targets", 0, 0);
						checklistItems.plusStrength(ref, strength);
					}
				}

				if (talents.contains("BATMEDCONTROL1")) {
					CharSkill disc_skill = getCharSkill(KM_LEAD);
					ref = checklistAdd(disc_skill.getBasicPool(charac)+"D", "[B]Orders:[b] On success, send simple orders to the target", 0, 0);
					if (talent_id == KM_BATMEDCONTROL1)
						checkItem(charac, ref);
				}
				if (talents.contains("BATMEDCONTROL2")) {
					ref = checklistAdd(DatUtil::poolText("+Range"), "[B]Planetary Scale:[b] Change range of power to planetary scale", 0, 4);
					checklistItems.plusExtra(ref, 1);
					if (talent_id == KM_BATMEDCONTROL2)
						checkItem(charac, ref);
				}
				if (talents.contains("BATMEDMASTERY")) {
					ref = checklistAdd(DatUtil::poolText("+Skill"), "[B]Mastery:[b] Targets count as having same ranks in one skill as", 0, 0);
					checklistItems.plusExtra(ref, 2);
					if (talent_id == KM_BATMEDMASTERY)
						checkItem(charac, ref);
				}
				if (duration > 0)
					checklistMayCommit(3, "BATMEDDURATION", "[B]Duration:[b] Commit to sustain effects of power on each target while it remains in range", 3);
				break;
			case KM_MISDIRBASIC:
			case KM_MISDIRCONTROL1:
			case KM_MISDIRCONTROL2:
			case KM_MISDIRMASTERY:
			case KM_MISDIRDURATION:
			case KM_MOVEBASIC:
			case KM_MOVECONTROL1:
			case KM_MOVECONTROL2:
			case KM_MOVECONTROL3:
				if (strength > 0) {
					for (int i=0; i<(force_dice+1)/2+2; i++) {
						ref = checklistAdd(".", QString("[B]Strength:[b] Increase silhouette by %1").arg(strength), 0, 0);
						checklistItems.plusStrength(ref, strength);
						if (i*strength+1 > 9) // Max silhouette!
							break;
					}
				}

				if (duration > 0)
					checklistMayCommit(2, "MISDIRDURATION", "[B]Duration:[b] Commit to sustain all ongoing effects", 2);

				if (talents.contains("MISDIRMASTERY")) {
					int cunn_decep = charac->cunning() + getCharSkill(KM_DECEP).skillRanks();
					ref = checklistAdd("..", QString("[B]Mastery:[b] Obscure additional %1 objects or create %1 more illusions").arg(cunn_decep), 0, 0);
					if (talent_id == KM_MISDIRMASTERY)
						checkItem(charac, ref);
				}
				else if (talent_id != KM_MOVECONTROL1)
					opposed_dicip = true;
				break;
			case KM_SENSEBASIC:
			case KM_SENSECONTROL2:
			case KM_INFLUENCEBASIC:
			case KM_WARFORBASIC:
			case KM_WARFORCONTROL2:
			case KM_WARFORCONTROL4:
			case KM_FEARSOME:
				opposed_dicip = true;
				break;
			default:
				break;
		}

		if (opposed_dicip) {
			checklistAdd(disc_skill.getBasicPool(charac), "[B]Opposed Check:[b] vs Discipline, if target is a PC or important NPC", 0, 0);
		}

		if (talent.force && (!skill || skill->type != SPECIAL)) {
			condChecklistAdd("FORCEALLY", DatUtil::poolText("+1 Move"), "[B]The Force Is My Ally:[b] Do action as a maneuver once per session", 1, 2);
			condChecklistAdd("NATMYSTIC", DatUtil::poolText("+Reroll"), "[B]Natural Mystic:[b] Once per session, may reroll 1 Force check", 0, 0);
		}
	}

	// FORCE POWER ITEMS -------------------------------------------------

	bool have_force = charac->force() > 0;
	int force = nonCommitedForce(charac);
	QString force_pool = charac->getForcePool();

	switch (skill_id) {
		case KM_CHARM:
		case KM_COERC:
		case KM_NEG:
		case KM_DECEP:
		case KM_LEAD:
		//case KM_DONTSHOOT:
			if (have_force && talents.contains("INFLUENCECONTROL2"))
				checklistSelected(force_pool, "[B]Influence:[b] Spend [FP] to gain [SU] or [AD]");

			if (talents.contains("JUSTKID"))
				checklistAdd("-y", "[B]Just Kidding:[b] Once per round, Target: self or ally", 0, 0, 1);
			break;
		case KM_REC:
			if (have_force && talents.contains("BAL"))
				checklistSelected(force_pool, "[B]Balance:[b] Gain +1 Strain per [FP]");
			break;
		case KM_ATHL:
			condChecklistAdd("NIKTOSP2OC2OP1", "B", "[B]Climbing Claws:[b] Add when climbing trees and other surfaces their claws can pierce", 0, 0);
			if (have_force && talents.contains("ENHANCEBASIC"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_COORD:
			if (have_force && talents.contains("ENHANCECONT1"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_RESIL:
			if (have_force && talents.contains("ENHANCECONT2"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_PILOTPL:
			if (have_force && talents.contains("ENHANCECONT4"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_BRAWL:
			if (have_force && talents.contains("ENHANCECONT5"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_BRI:
		case KM_PILOTSP:
			if (have_force && talents.contains("ENHANCECONT7"))
				checklistSelected(force_pool, "[B]Enhance:[b] Spend [FP] to gain [SU] or [AD]");
			break;
		case KM_ICOOL:
		case KM_IVIG:
			if (have_force) {
				bool have_foresee = false;
				if (talents.contains("FORSEECONTROL1")) {
					QString text = "[B]Foresee:[b] Spend [FP] to gain [SU]";

					if (talent_id != KM_FORSEECONTROL1)
						checklistRangeAdd("FORSEE");

					int magnitude = talents.get("FORSEEMAGNITUDE").ranks;
					checklistMultiForce(1, "[B]Foreesee Magnitude:[b] Effect %1 additional %2", "target", "targets", magnitude, PLUS_MAGNITUDE);

					if (talents.contains("KM_FORSEECONTROL2"))
						text += ", tagets increase defense by 2 for first round";

					condChecklistAdd("KM_FORSEECONTROL3", ".", "[B]Foresee Manuever:[b] Targets may take free manuever before the first round", 0, 0);

					have_foresee = true;
				}
				if (talents.contains("WARFORCONTROL1")) {
					if (talents.contains("WARFORMAGNITUDE"))
						checklistSelected(force_pool, "[B]Warde's Foresight:[b] Add [SU] or [AD] per [FP] spent to self or ally's first check this encounter");
					else if (talents.contains("WARFORCONTROL3"))
						checklistSelected(force_pool, "[B]Warde's Foresight:[b] Add [SU] or [AD] per [FP] spent to first check this encounter");
					else
						checklistSelected(force_pool, "[B]Warde's Foresight:[b] Add [SU] per [FP] spent to first check this encounter");
				}
				ranks = talents.ranks("WARFORDURATION");
				if (ranks > 0) {
					ref = checklistAdd("..", plurize("[B]Warde's Foresight Duration:[b] Bonuses apply to all checks for %1 round", ranks), 0, 0);
					checklistItems.plusDuration(ref, ranks);
				}
				if (talents.contains("WARFORCONTROL4")) {
					checklistAdd(DatUtil::poolText("+Action/+Init"), plurize("[B]Ally Initiative:[b] If [DA] used may perform action but enemies add [SU], else allies add [SU]", ranks), 0, 0);
				}
			}
			break;
		case KM_VIGIL:
		case KM_PERC:
			if (talents.contains("FARSIGHTCONTROL3"))
				checklistSelected(force_pool, "[B]Farsight:[b] Add [SU] or [AD] per [FP] spent");

			condChecklistAddRanked("KEENEYED", "N", "[B]Keen Eyed:[b] Reduce any search time by 50%", 0, 0);
			break;
		case KM_STEAL:
			condChecklistAddRanked("SLEIGHTMIND", "B", "[B]Sleight of Mind:[b] Add if opposition is not immune to Force powers", 0, 0);
			break;
		case KM_LTSABER:
			if (have_force && talents.ranks("HAWKSWOOP"))
				checklistAdd(force_pool, "[B]Hawk Bat Swoop:[b] Spend [FP] to engage target, and [FP] to add [AD]", 0, 0);

			if (have_force && talents.contains("SABERSW"))
				checklistAdd(DatUtil::poolText("Linked %1", force), QString("[B]Saber Swarm:[b] Attack has linked %1 quality").arg(force), 1, 1);

			condChecklistAdd("SARSWEEP", "Daa", "[B]Sarlacc Sweep:[b] [AD][AD] may only be use to hit additional engaged targets", 0, 1);
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
				checklistSelected(send_str, QString("[B]Sense Control:[b] Upgrade difficulty of incoming attack %1 per round").arg(send_mag));
			}
			break;
		case KM_SURV:
			condChecklistAddRanked("EXTRACK", "N", "[B]Expert Tracker:[b] Reduce tracking time by 50%", 0, 0);
			condChecklistAdd("FORAG", "NN", "[B]Forager:[b] Add when searching for food, shelter and water and reduce time by 50%", 0, 0);
			break;
		case KM_FORCECOMMIT: {
			QString twice = talents.contains("SENSESTRENGTH") ? " (twice)" : "";

			if (talents.contains("SENSECONTROL1"))
				checklistCommit(1, "SENSECONTROL1", "[B]Sense Control:[b] Commit force dice to upgrade difficulty of incoming attacks" + twice);
			if (talents.contains("SENSECONTROL3"))
				checklistCommit(1, "SENSECONTROL3", "[B]Sense Control:[b] Commit force dice to upgrade abilty of combat checks" + twice);
			if (talents.contains("ENHANCECONT8"))
				checklistCommit(1, "ENHANCECONT8", "[B]Enhance:[b] Commit force dice to increase Brawn by one");
			if (talents.contains("ENHANCECONT9"))
				checklistCommit(1, "ENHANCECONT9", "[B]Enhance:[b] Commit force dice to increase Agility by one");
			multiCommit(charac, "MISDIRCONTROL3", "[B]Misdirect Control:[b] Add %1 to all incoming attacks", "[TH]");

			if (talents.contains("MISDIRDURATION"))
				checklistCommit(2, "MISDIRDURATION", "[B]Misdirect Duration:[b] Sustain misdiration as long as target in range");

			if (talents.contains("BINDDURATION"))
				checklistCommit(3, "BINDDURATION", "[B]Bind Duration:[b] Sustain all bind effects as long as target in range");

			if (talents.contains("BATMEDDURATION"))
				checklistCommit(3, "BATMEDDURATION", "[B]Battle Meditation Duration:[b] Sustain effects of meditation as long as target in range");

			if (talents.contains("SEEKDURATION"))
				checklistCommit(1, "SEEKDURATION", "[B]Seek Duration:[b] Track maving target(s)");

			if (talents.contains("SEEKCONTROL1"))
				checklistCommit(1, "SEEKCONTROL1", "[B]Seek Skills:[b] Upgrade the ability of Vigilance and Perception checks once");

			if (talents.contains("SEEKCONTROL3")) {
				int inc = charac->cunning() + getCharSkill(KM_PERC).skillRanks();
				checklistCommit(3, "SEEKCONTROL3", QString("[B]Seek Pierce:[b] Attacks gain or increase pierce by %1").arg(inc));
			}

			if (talents.contains("SUPPRESSDURATION"))
				checklistCommit(1, "SEEKDURATION", "[B]Suppress Duration:[b] Sustain ongoing Suppress effects on each target in range");

			multiCommit(charac, "SUPPRESSCONTROL1", "[B]Suppress Control:[b] Remove %1 from incoming force checks", "[FP]");

			multiCommit(charac, "FARSIGHTCONTROL4", "[B]Farsight: Ongoing Effect:[b] Sustain Farsight effects and increase ranks in Perception by %1", QString(), 5 - getCharSkill(KM_PERC).internalRanks);

			ranks = talents.ranks("INTUITEVA");
			if (ranks > 0) {
				int force = charac->force();
				for (int i=0; i<ranks && i<force; i++)
					checklistCommit(1, QString("INTUITEVA~%1").arg(i), "[B]Intuitive Evasion:[b] Upgrade combat checks vs vehicle, cost 1 strain per round", 1, 1);
			}

			break;
		}
		default:
			break;
	}

	// TALENT ITEMS -------------------------------------------------
	// showChecklist() <---- Initialise power

	switch (skill_id) {
		case KM_COERC:
			ranks = talents.ranks("INTIM");
			for (int i=0; i<ranks; i++) {
				ref = checklistAdd("d", "[B]Intimidating:[b] Downgrade the difficulty of the check", 0, 1);
				if (i == 0 && talent_id == KM_INTIM)
					checkItem(charac, ref);
			}
			break;
		case KM_CHARM:
		case KM_NEG:
			for (int i=0; i<talents.ranks("CONGENIAL"); i++) {
				checklistAdd("d", "[B]Congenial:[b] Downgrade the difficulty of the check", 0, 1);
			}
			break;
		case KM_DEFM:
		case KM_DEFR:
		//case KM_REFLECT:
			for (int i=0; i<talents.ranks("DODGE"); i++)
				checklistAdd("u", "[B]Dodge:[b] Out-of-turn incedental", 0, 1);

			ranks = talents.ranks("DURA");
			if (ranks > 0) {
				ref = checklistAdd(DatUtil::poolText("%1% Crits", -ranks*10), "[B]Durable:[b] Reduce critical hits", 0, 0);
				checklistItems.plusCrit(ref, -ranks*10);
			}

			if (skill_id == KM_DEFM) {
				ranks = talents.ranks("PARRY");
				if (ranks > 0) {
					QString par = QString("Reduce damage by %1").arg(ranks+2);
					if (talents.contains("PARRYIMP"))
						par = par + ", on [DE] or [TH][TH][TH] hit attacker";
					ref = checklistDamage(-(ranks+2), "[B]Parry:[b] "+par, 3, 0);
					if (talent_id == KM_PARRY || talent_id == KM_PARRYIMP)
						checkItem(charac, ref);
					if (talents.contains("UNARMPARRY")) {
						ref = checklistDamage(-(ranks+2), "[B]Unarmed Parry:[b] "+par, 2, 0);
						if (talent_id == KM_UNARMPARRY)
							checkItem(charac, ref);
					}
				}

				setNegativePool(charac, iNegMelee, skillKey);
			}
			else {
				ranks = talents.ranks("REFLECT");
				if (ranks > 0) {
					QString par = QString("[B]Reflect:[b] vs Lightsaber, reduce damage by %1").arg(ranks+2);

					if (talents.contains("REFLECTIMP"))
						par = "[B]Reflect:[b] vs Lightsaber, on [DE] or [TH][TH][TH] damage apponent (Medium range)";
					ref = checklistDamage(-(ranks+2), par, 3, 0);
					if (talent_id == KM_REFLECT || talent_id == KM_REFLECTIMP)
						checkItem(charac, ref);
				}

				setNegativePool(charac, iNegRanged, skillKey);
			}

			if (talents.contains("HERO"))
				checklistAdd(DatUtil::poolText("Ignor Crit"), "[B]Heroic Fortitude:[b] Ignore crits on Brawn/Agility until end of encounter", 0, 0, 1);

			if (talents.contains("HEROICRES")) {
				ranks = getCharSkill(KM_RESIL).skillRanks();
				checklistAdd(DatUtil::poolText("Soak %1", ranks), "[B]Heroic Resilience:[b] After hit but before damage increase soak", 0, 0, 1);
			}

			condChecklistAdd("SENSEADV", "SS", "[B]Sense Advantage:[b] Add to NPC check, once per session", 0, 0);
			break;
		case KM_PILOTSP:
			condChecklistAdd("MASPIL", DatUtil::poolText("+1 Action"), QString("[B]Master Pilot:[b] Once per round, perform Action as Manuever"), 1, 2);
			break;
		case KM_DISC:
			ranks = talents.ranks("HARDHD");
			if (ranks > 0) {
				int dif = 4-ranks;

				ref = checklistAdd(QString("D").repeated(dif <= 0 ? 1 : dif), "[B]Hard Headed:[b] Perform check to remove staggered or disoriented effect", 0, 0);
				if (talent_id == KM_HARDHD)
					checkItem(charac, ref);
				if (talents.contains("HARDHDIMP")) {
					dif = 6-ranks;
					ref = checklistAdd(QString("D").repeated(dif <= 0 ? 1 : dif), QString("[B]Hard Headed (Improved):[b] Perform check to reduce strain to %1").arg(charac->strain()-1), 0, 0);
					if (talent_id == KM_HARDHDIMP)
						checkItem(charac, ref);
				}
			}
			// No break
		case KM_FDISC:
			condChecklistAddRanked("CONF", "r", "[B]Confidence:[b] May decrease difficulty of fear checks", 0, 0);
			break;
		case KM_ICOOL:
		case KM_IVIG:
			for (int i=0; i<talents.ranks("RAPREA"); i++) {
				ref = checklistAdd("s", "[B]Rapid Reaction:[b] Suffer strain to improve initiative", 0, 1);
				if (i == 0 && talent_id == KM_RAPREA)
					checkItem(charac, ref);
			}
			break;
		case KM_SKUL:
		case KM_COMP:
			condChecklistAddRanked("BYP", "N", QString("[B]Bypass Security:[b] Disable security device or open a locked door"), 0, 0);
			break;
		case KM_ATHL:
		case KM_COORD:
			ranks = talents.ranks("CONDITIONED");
			if (ranks > 0)
				checklistDamage(-ranks, QString("[B]Conditioned:[b] Reduce damage and strain by %1 from falling").arg(ranks), 0, 0);
			break;
		case KM_VIGIL:
			condChecklistAdd("INTERJECT", "DD", "[B]Interjection:[b] Add [SU]/[FA] equal to [SU] and [AD]/[TH] equal to [AD] to a social check", 0, 3);
			break;
		case KM_DECEP:
			condChecklistAdd("NOWYOUSEE", "DDD", QString("[B]Now You See Me:[b] Once per session, cause %1 NPC(s) within Medium range to forget PC").arg(charac->cunning()), 0, 0);
			break;
		default:
			break;
	}

	switch (skill_id) {
		case KM_STEAL:
		case KM_SKUL:
			condChecklistAdd("MASSHAD", "r", "[B]Master of Shadows:[b] Once per round decrease difficulty", 0, 2);
			break;
		case KM_CHARM:
			condChecklistAdd("DONTSHOOT", "DDD", "[B]Don't Shoot!:[b] Once per session, cannot be attacked unless attack, Duration: Encounter", 0, 0);
			break;
		case KM_PILOTSP:
		case KM_PILOTPL: {
			int amount = 1;

			if (talents.contains("FULLTHSUP"))
				amount = 2;
			if (talents.contains("FULLTH")) {
				ref = checklistAdd("DDD", QString("[B]Full Throttle:[b] Increase Speed by %1 for %2 rounds, [B]Cost: Action[b]").arg(amount).arg(charac->cunning()), 0, 0);
				if (talent_id == KM_FULLTH || talent_id == KM_FULLTHSUP)
					checkItem(charac, ref);
			}
			condChecklistAdd("FULLTHIMP", "DD", QString("[B]Full Throttle (Improved):[b] Increase Speed by %1 for %2 rounds").arg(amount).arg(charac->cunning()), 1, 1);
			break;
		}
		case KM_COMP:
		case KM_INT:
		//case KM_COD:
			ranks = talents.ranks("COD");
			if (ranks > 0) {
				ref = checklistAdd("r"+QString("N").repeated(ranks), QString("[B]Codebreaker:[b] Break code or decrypt communications"), 0, 0);
				if (talent_id == KM_COD)
					checkItem(charac, ref);
			}
			break;
		default:
			break;
	}

	// GENERAL COMBAT -------------------------------------------------
	// showChecklist() <---- Initialise power

	bool combat = false;
	bool ranged = false;
	bool gunnery = false;
	bool close = false;
	bool using_weapon = !weapon.itemkey.isEmpty() && !weapon.isUnarmed();
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
		close = true;
	}

	if (combat) {
		// Aim skills:
		ranks = talents.ranks("TRUEAIM");
		if (ranged && ranks > 0)
			checklistAdd("B" + QString("U").repeated(ranks), "[B]True Aim:[b] Once per round, use a maneuver to aim", 1, 0);
		else
			checklistAdd("B", "[B]Aim:[b] Use a maneuver to aim", 1, 0);
		checklistAdd("B", "[B]Aim Twice:[b] Use a second maneuver to aim longer", 1, 0);
		if (ranged && ranks > 0)
			checklistAdd("SS" + QString("U").repeated(ranks), "[B]Called Shot (True Aim):[b] Once per round, use a True Aim to call a shot", 1, 0);
		else
			checklistAdd("SS", "[B]Called Shot:[b] Use a maneuver to call a shot by aiming", 1, 0);
		checklistAdd("N", "[B]Called Shot (2x Aim):[b] Use a 2nd maneuver to aim longer on a called shot", 1, 0);

		for (int i=0; i<talents.ranks("PRECAIM"); i++) {
			checklistAdd("N", "[B]Precise Aim:[b] Decrease apponents defense", i ? 0 : 1, 1);
		}

		condChecklistAddRanked("QUICKST", "B", "[B]Quick Strike:[b] Add if target has not acted in this encounter yet", 0, 0);

		if (using_weapon) {
			if (talents.contains("TARGBL"))
				checklistDamage(charac->agility(), "[B]Targeted Blow:[b] Add to damage on hit with non-vehicle weapon", 0, 1);
			if (talents.contains("ANAT"))
				checklistDamage(charac->intellect(), "[B]Anatomy Lessons:[b] Add to damage on hit with non-vehicle weapon", 0, 1);
		}

		condChecklistAddRanked("DISARMSMILE", "N", "[B]Disarming Smile:[b] Opposed Charm vs Target, Range: Short, Duration: Encounter, [B]Cost: Action[b]", 0, 0);

		if (talent_id != KM_EXHPORT) {
			ranks = talents.ranks("CRIPV");
			if (ranks > 0)
				checklistAdd("D", "[B]Crippling Blow:[b] If check deals damage, target suffers 1 strain per move for encounter", 0, 0);
		}

		ranks = talents.ranks("PREYWEAK");
		if (ranks > 0)
			checklistDamage(ranks, "[B]Prey on the Weak:[b] Add to damage if target is disoriented", 0, 0);

		if (ranged) {
			ranks = talents.ranks("SNIPSHOT");
			if (ranks > 0) {
				int r1, r2;
				charac->getItemRange(r1, r2);
				if (r1 >= RANGE_ENGAGED) {
					int i = 0;
					do {
						ref = checklistAdd("D", "[B]Sniper Shot:[b] Increase range (and difficulty) of non-thrown ranged attack", i == 0 ? 1 : 0, 0);
						checklistItems.plusRange(ref, 1);
						r1 += 1;
						i++;
					}
					while (i < ranks && r1 < RANGE_EXTREME);
				}
			}

			ranks = talents.ranks("POINTBL");
			if (ranks > 0)
				checklistDamage(ranks, "[B]Point Blank:[b] Engaged or Short range", 0, 0);
			condChecklistAdd("RAINDEATH", "r", "[B]Rain of Death:[b] Ignore increase difficulty due to auto-fire", 1, 0);
			if (skillKey != "RANGLT") {
				if (talents.contains("HEAVYHITTER")) {
					ref = checklistAdd(DatUtil::poolText("Breach +1"), "[B]Heavy Hitter:[b] Once per session use [TR] to add breach property", 1, 0);
					checklistItems.plusPierce(ref, 10);
				}
				ranks = talents.ranks("BAR");
				if (ranks > 0)
					checklistDamage(ranks, "[B]Barrage:[b] Increase damage at Long or extreme range", 0, 0);
			}
			if (gunnery) {
				if (talents.contains("DEAD")) {
					int dam = charac->agility();

					if (!talents.contains("DEADIMP"))
						dam = (dam + 1) / 2;
					checklistDamage(dam, "[B]Dead to Rights:[b] Add to one hit with vehicle mounted weapon", 0, 1);
				}

				if (talents.contains("EXHPORT")) {
					checklistAdd(DatUtil::poolText("-Crit Increase"), "[B]Exhaust Port:[b] Ignore the effects of Massive", 0, 0, 1);
				}
			}
		}
		else {
			condChecklistAdd("MULTOPP", "B", "[B]Multiple Opponents:[b] When engaged with multiple opponent", 0, 0);
		}
		if (close) {
			condChecklistAdd("PRECSTR", DatUtil::poolText("Inflict ")+"D \\C\\r\\i\\t", "[B]Precision Strike:[b] Inflict an Easy ([DI]) Critical hit on success", 0, 1);

			if (skill_id != KM_LTSABER) {
				ranks = talents.ranks("FRENZ");
				for (int i=0; i<ranks; i++) {
					ref = checklistAdd("U", "[B]Frenzied Attack:[b] Upgrade Melee or Brawl attacks", 0, 1);
					if (i == 0 && talent_id == KM_FRENZ)
						checkItem(charac, ref);
				}

				condChecklistAdd("NATBRAW", DatUtil::poolText("+Reroll"), "[B]Natural Brawler:[b] Once per session, reroll a Brawl or Melee attack", 0, 0);
			}
		}
	}

	// GENERALLY APPLICABLE -------------------------------------------------

	if (!skill || skill->type != SPECIAL) {
		if (skill_id != KM_DEFM && skill_id != KM_DEFR && skill_id != KM_FORCECOMMIT && talents.contains("INTENSFOC"))
			checklistAdd("U", "[B]Intense Focus:[b] Upgrade the next skill check once", 1, 1);
	}
	if (talents.contains("TOUCH")) {
		if (skill_id != KM_DEFM && skill_id != KM_DEFR && skill_id != KM_FORCECOMMIT)
			checklistAdd("BB", "[B]Touch of Fate:[b] May be used once per session", 0, 0);
	}
	if (talents.contains("SENSDANG")) {
		if (skill_id != KM_DEFM && skill_id != KM_DEFR && skill_id != KM_FORCECOMMIT)
			checklistAdd("NN", "[B]Sense Danger:[b] May be used once per session", 0, 0);
	}

	if (skill_id != KM_FORCECOMMIT) {
		if (skill_id != KM_DEFR) {
			for (int i=0; i<talents.ranks("DEFSTA"); i++) {
				if (skill_id == KM_DEFM && talent_id != KM_DEFSTA)
					checklistAdd("u", "[B]Defensive Stance:[b] If performed during your turn, duration: 1 round", 0, 0);
				else {
					ref = checklistAdd(talent_id == KM_DEFSTA ? "u" : DatUtil::poolText("Upg. M. Def"), "[B]Defensive Stance:[b] Upgrade incoming Melee attacks for 1 round", i == 0 ? 1 : 0, 1);
					if (i == 0 && talent_id == KM_DEFSTA)
						checkItem(charac, ref);
				}
			}
		}

		if (skill_id != KM_DEFM) {
			for (int i=0; i<talents.ranks("SIDESTEP"); i++) {
				if (skill_id == KM_DEFR && talent_id != KM_SIDESTEP)
					checklistAdd("u", "[B]Side Step:[b] If performed during your turn, duration: 1 round", 0, 0);
				else {
					ref = checklistAdd(talent_id == KM_SIDESTEP ? "u" : DatUtil::poolText("Upg. R. Def"), "[B]Side Step:[b] Upgrade incoming Ranged attacks for 1 round", i == 0 ? 1 : 0, 1);
					if (i == 0 && talent_id == KM_SIDESTEP)
						checkItem(charac, ref);
				}
			}
		}

		for (int i=0; i<talents.ranks("BRA"); i++) {
			if (skill_id == KM_RESIL && talent_id != KM_BRA)
				checklistAdd("N", "[B]Brace:[b] If performed during your turn, duration: 1 round", 0, 0);
			else {
				ref = checklistAdd(talent_id == KM_BRA ? "N" : DatUtil::poolText("Rem. Setback"), "[B]Brace:[b] Remove [SE] if due to any environmental effects, Duration: 1 Round", i == 0 ? 1 : 0, 0);
				if (i == 0 && talent_id == KM_BRA)
					checkItem(charac, ref);
			}
		}
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
			checklistAdd("r", "[B]Telescopic Optical Sight:[b] Reduce difficulty by 1 at Long or Extreme range", 0, 0);
		if (weapon.hasMod("SETTRIGGER_EXTRA_MOD"))
			checklistAdd("sst", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
		else if (weapon.hasMod("SETTRIGGER_BASE_MOD"))
			checklistAdd("st", "[B]Set Trigger:[b] Add if this is the first combat check of the encounter", 0, 0);
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
		checklistAdd("BB", "[B]Mountaineer Armour:[b] Add to all climb or rappel checks", 0, 0);
	if (targeting_comp && ranged)
		checklistAdd("B", "[B]Targeting Computer:[b] Add Boost dice at Long and Extreme range", 0, 0);
	else if ((integrated_scanner || hunting_goggles) && skill_id == KM_PERC) {
		if (integrated_scanner)
			checklistAdd("B", "[B]OmniScan 3 Scanner:[b] To detect movement or hidden enemies", 0, 0);
		if (hunting_goggles)
			checklistAdd("B", "[B]Hunting Goggles:[b] Add when locating prey", 0, 0);
	}
	if (eos) {
		if (combat || skill_id == KM_PERC || skill_id == KM_VIGIL || skill_id == KM_IVIG)
			checklistAdd("NN", "[B]Enhanced Optics Suite:[b] Darkness, smoke or environmental effects that obscure vision", 0, 0);
	}
	if (hunting_goggles) {
		if (combat && !gunnery)
			checklistAdd("NN", "[B]Hunting Googles:[b] Concealment, darkness, fog and mist", 0, 0);
	}

	switch (skill_id) {
		case KM_MED:
			if (gear.equipped("MEDPAC"))
				checklistSelected("B", "[B]Medpac:[b] Add to all Medicine checks when equipped");

			if (gear.carriedQuantity("MEDAIDPATCH") > 0)
				checklistAdd("sa", "[B]Med-Aid Patch:[b] Maximum usage 1 per check, [B]Cost: 1x Med-Aid Patch[b]", 0, 0, 0, "MEDAIDPATCH");

			if (gear.equipped("BLOODSCAN"))
				checklistAdd("aa", "[B]Blood Scanner:[b] After use, add to next Medicine check, [B]Cost: Action[b]", 0, 0);

			ranks = talents.ranks("SURG");
			if (ranks > 0)
				checklistAdd(DatUtil::poolText("Wounds -%1", ranks), "[B]Surgeon:[b] Add to Wounds recovered on successful check", 0, 0);

			ranks = gear.carriedQuantity("DROIDMINIMED");
			if (ranks > 0)
				checklistAdd(DatUtil::poolText("Wounds -%1", ranks), "[B]Mini-med Droids:[b] Add to Wounds recovered on successful check", 1, 0);
			break;
		case KM_PILOTSP:
		case KM_PILOTPL:
			if (gear.equipped("CYAVIONCAAF2")) {
				CharSkill char_skill = getCharSkill(skill_id);
				int skill_ranks = char_skill.skillRanks();
				int skill_attribute = char_skill.skillAttribute(charac, skill);
				if (skill_ranks > 0) {
					ref = checklistAdd(skill_ranks > skill_attribute ? "A" : "U", "[B]CAAF-2 Avionics Interface:[b] Add if vehicle can be interfaced", 0, 0);
					if (skill_id == KM_PILOTSP)
						checkItem(charac, ref); // Space is autocheck, not Planet because not all vehicles can be interfaced
				}
			}
			break;
	default:
			break;
	}

	// VARIOUS ACTIONS -------------------------------------------------

	switch (skill_id) {
		case KM_MED:
			checklistAdd("D", "[B]Easy Severity:[b] 1-40% Critical injury, 1-50% Wounds, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DD", "[B]Average Severity:[b] 41-90% Critical injury, 51-100% Wounds, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DDD", "[B]Hard Severity:[b] 91-125% Critical injury, 101+% Wounds, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DDDD", "[B]Easy Severity:[b] 126-150% Critical injury, [B]Cost: Action[b]", 0, 0);
			break;
		case KM_MECH:
			checklistAdd("D", "[B]Damage Control (Easy):[b] System strain or hull trauma less than half threshold, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DD", "[B]Damage Control (Average):[b] System strain or hull trauma less or equal threshold, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DDD", "[B]Damage Control (Hard):[b] System strain or hull trauma exceeds threshold, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DD", "[B]Boost Sheilds:[b] On success add 1 to a shield zone, Duration: [SU] less 1 rounds, [B]Cost: Action[b]", 0, 0);
			break;
		case KM_PILOTSP:
		case KM_PILOTPL:
			checklistAdd("D", "[B]Gain the Advantage (Easy):[b] Going faster than opponent, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DD", "[B]Gain the Advantage (Average):[b] Vehicle speed identical, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DDD", "[B]Gain the Advantage (Hard):[b] Opponent's speed is one higher, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DDDD", "[B]Gain the Advantage (Daunting):[b] Opponent's speed is 2 or more higher, [B]Cost: Action[b]", 0, 0);
			checklistAdd("DD", "[B]Co-Pilot:[b] Each [SU] downgrades difficulty of pilot's next check by 1, [B]Cost: Action[b]", 0, 0);
			break;
		default:
			break;
	}

	// CUSTOM CHECKS -------------------------------------------------

	if (checkLists.contains(skillKey))
		checkLists[skillKey].checkAll(false);
}

void CurrentData::exitChecklist(Character* charac)
{
	QString key = getCustomChecklistKey(charac);
	int strain = 0;
	QString	consumable;

	setTemporaryStrain(charac, 0);
	if (checkLists.contains(key))
		strain = checkLists[key].strainUsed(consumable);
	strain += checklistItems.strainUsed(consumable);
	if (strain > 0)
		charac->adjustStrain(strain);
	if (!consumable.isEmpty())
		addItem(charac, -1, consumable, QString(), 0);
	//if (commit_count) {
	//	QString power;
	//	CurrentData::instance->forceCommitted[power + "DURATION"] = commit_count;
	//	charac->emitForceCommittedChanged();
	//}
}

void CurrentData::appendCheckItem(Character* charac, const QString& pool, const QString& desc)
{
	QString key = getCustomChecklistKey(charac);
	addCheckItem(key, pool, desc);
	if (charac)
		CheckList::instance.rowCountChanged();
	writeCurrentData();
}

void CurrentData::updateCheckItem(Character* charac, int ref, const QString& pool, const QString& desc)
{
	QString key = getCustomChecklistKey(charac);
	if (checkLists.contains(key)) {
		ChecklistItem* item = checkLists[key].findItem(ref);
		if (item) {
			//CheckList::instance.startChanges();
			item->dicePool = pool;
			item->description = desc;
			CheckList::instance.rowCountChanged();
			dicePoolChanged(charac);
			writeCurrentData();
		}
	}
}

void CurrentData::removeCheckItem(Character* charac, int ref)
{
	QString key = getCustomChecklistKey(charac);
	if (checkLists.contains(key)) {
		//CheckList::instance.startChanges();
		checkLists[key].deleteItem(ref);
		CheckList::instance.rowCountChanged();
		dicePoolChanged(charac);
		writeCurrentData();
	}
}

void CurrentData::uncheckAllItem(Character* charac)
{
	QString key = getCustomChecklistKey(charac);

	iNegativeCheck = 0;
	//CheckList::instance.startChanges();
	checklistItems.checkAll(false);
	if (checkLists.contains(key))
		checkLists[key].checkAll(false);
	CheckList::instance.rowCountChanged();
	charac->setChangeDicePool(QString());
}

void CurrentData::checklistRangeAdd(QString power)
{
	int range = talents.get(power + "RANGE").ranks;
	if (range > 0) {
		int r1, r2;
		Character::instance->getItemRange(r1, r2);
		if (r1 >= RANGE_ENGAGED) {
			do {
				int ref = checklistAdd(".", plurize("[B]Range:[b] Increase range by %1 range band", range), 0, 0);
				checklistItems.plusRange(ref, range);
				r1 += range;
			}
			while (r1 < RANGE_EXTREME);
		}
	}
}

void CurrentData::checklistMultiForce(int pip_count, QString desc, QString sin, QString plu, int amount, PlusType type, bool select_one)
{
	if (amount > 0) {
		int force_dice = Character::instance->force() - forceCommitCount();
		for (int i=0; i<(force_dice+1)/2+2; i++) {
			int ref = checklistAdd(QString(".").repeated(pip_count), sin.isEmpty() ? desc : QString(desc).arg(amount).arg(amount == 1 ? sin : plu), 0, 0);
			switch (type) {
				case PLUS_DURATION:
					checklistItems.plusDuration(ref, amount);
					break;
				case PLUS_MAGNITUDE:
					checklistItems.plusMagnitude(ref, amount);
					break;
				case PLUS_STRENGTH:
					checklistItems.plusStrength(ref, amount);
					break;
			}
			if (i == 0 && select_one)
				checkItem(Character::instance, ref);
		}
	}
}

int CurrentData::checklistAdd(const QString& pool, const QString& desc, int move, int strain, int dpoint,
	const QString& cons, const QString& commit_key, int commit_count)
{
	ChecklistItem item;

	item.dicePool = pool;
	item.description = desc;
	item.moveRequired = move;
	item.reduceStrain = strain;
	item.destinyPointCost = dpoint;
	item.consumable = cons;
	item.commitKey = commit_key;
	item.forceCommitCount = commit_count;
	return checklistAppend(item);
}

void CurrentData::condChecklistAdd(const QString& talent, const QString& pool, const QString& desc, int move, int strain)
{
	if (talents.contains(talent)) {
		ChecklistItem item;

		item.dicePool = pool;
		item.description = desc;
		item.moveRequired = move;
		item.reduceStrain = strain;
		int ref = checklistAppend(item);
		if (Character::instance->getCurrentTalentKey() == talent)
			checkItem(Character::instance, ref);
	}
}

void CurrentData::condChecklistAddRanked(const QString& talent, const QString& pool, const QString& desc, int move, int strain)
{
	int ranks = talents.ranks(talent);
	if (ranks > 0) {
		ChecklistItem item;

		item.dicePool = pool.repeated(ranks);
		item.description = desc;
		item.moveRequired = move;
		item.reduceStrain = strain;
		int ref = checklistAppend(item);
		if (Character::instance->getCurrentTalentKey() == talent)
			checkItem(Character::instance, ref);
	}
}

void CurrentData::checklistSelected(const QString& pool, const QString& desc)
{
	ChecklistItem item;

	item.dicePool = pool;
	item.description = desc;
	int ref = checklistAppend(item);
	checkItem(Character::instance, ref);
}

void CurrentData::multiCommit(Character* charac, QString key, QString desc, QString arg, int extra_limit,  bool commit_list)
{
	if (talents.contains(key)) {
		int no = 0;
		int force = charac->force() - 1;
		checklistCommit(1, QString("%1~%2").arg(key).arg(no), QString(desc).arg(arg.isEmpty() ? "1" : arg.repeated(1)), 0, 0, commit_list);
		no++;
		extra_limit--;
		if ((force & 1) != 0 && extra_limit > 0) {
			checklistCommit(1, QString("%1~%2").arg(key).arg(no), QString(desc).arg(arg.isEmpty() ? "1" : arg.repeated(1)), 0, 0, commit_list);
			no++;
			force--;
			extra_limit--;
		}
		for (int i=0; i<force/2 && extra_limit > 0; i++) {
			checklistCommit(2, QString("%1~%2").arg(key).arg(no), QString(desc).arg(arg.isEmpty() ? "2" : arg.repeated(2)), 0, 0, commit_list);
			no++;
			extra_limit -= 2;
		}
	}
}

int CurrentData::checklistCommit(int commit_count, const QString& commit_key, const QString& desc, int move, int strain, bool commit_list)
{
	ChecklistItem item;

	item.dicePool = QString(commit_list ? "g" : "n").repeated(commit_count); // Red
	item.forceCommitCount = commit_count;
	item.commitKey = commit_key;
	item.description = desc;
	item.moveRequired = move;
	item.reduceStrain = strain;
	//item.plusDuration = duration;
	return checklistAppend(item);
}

int CurrentData::checklistMayCommit(int commit_count, const QString& commit_key, const QString& desc, int duration)
{
	ChecklistItem item;

	item.dicePool = QString("n").repeated(commit_count); // Black
	item.forceCommitCount = commit_count;
	item.commitKey = commit_key;
	item.description = desc;
	item.plusDuration = duration;
	return checklistAppend(item);
}

int CurrentData::checklistDamage(int damage, const QString& desc, int strain, int dpoint)
{
	ChecklistItem item;

	item.plusDamage = damage;
	item.dicePool = DatUtil::poolText("Damage %1", damage);
	item.description = desc;
	item.reduceStrain = strain;
	item.destinyPointCost = dpoint;
	return checklistAppend(item);
}

int CurrentData::checklistAppend(ChecklistItem& item)
{
	// Check for committed dice:
	if (!item.commitKey.isEmpty()) {
		int cc = CurrentData::instance->isCommitted(item.commitKey);
		if (cc > 0) {
			item.checked = true;
			CurrentData::instance->setupChecklistForce(cc);
		}
	}

	int ref = checklistItems.append(item);

	if (!item.commitKey.isEmpty()) {
		// Select the item, if this is the selected talent:
		Character* charac = Character::instance;
		QString key = item.commitKey;

		// Strip of the suffix, if it is the first!
		if (key.contains("~")) {
			int i = DatUtil::right(key, "~").toInt();
			if (i == 0)
				key = DatUtil::left(key, "~");
		}

		if (charac->getCurrentTalentKey() == key) {
			if (checkItem(charac, ref))
				return ref;
		}
	}

	dicePoolChanged(Character::instance, &item, true);
	return ref;
}

bool CurrentData::checkItem(Character* charac, int ref, bool list_setup)
{
	QString key = getCustomChecklistKey(charac);
	ChecklistItem* item;

	if (!(item = checklistItems.findItem(ref))) {
		if (checkLists.contains(key))
			item = checkLists[key].findItem(ref);
	}
	// If called on list setup, then we want to check this item!
	if (item && (!list_setup || !item->checked)) {
		item->checked = !item->checked;
		if (!item->commitKey.isEmpty()) {
			iForceCommitted[item->commitKey] += item->checked ? item->forceCommitCount : -item->forceCommitCount;
			if (item->commitKey.contains("~")) {
				int max = DatUtil::right(item->commitKey, "~").toInt()+1;
				if (iCommitCountMax < max)
					iCommitCountMax = max;
			}
			if (item->commitKey == "ENHANCECONT8")
				charac->emitBrawnChanged();
			else if (item->commitKey == "ENHANCECONT9")
				charac->emitAgilityChanged();
			charac->emitForceCommittedChanged();
			charac->characteristicsChanged();
		}
		CheckList::instance.dataChanged();
		dicePoolChanged(charac, item, list_setup);
		return true;
	}

	return false;
}

void CurrentData::setupChecklistForce(int commit_count)
{
	Character::instance->adjustPoolForCommittedForce(commit_count);
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
	dicePoolChanged(charac);
}

void CurrentData::negetiveDefence(int& r, int &m)
{
	r = iNegRanged;
	m = iNegMelee;
}

int CurrentData::forceCommitCount()
{
	QMapIterator<QString, int> i(iForceCommitted);
	int count = 0;

	while (i.hasNext()) {
		i.next();
		count += i.value();
	}
	return count;
}

int CurrentData::isCommitted(const QString& key)
{
	if (iForceCommitted.contains(key))
		return iForceCommitted[key];
	return 0;
}

int CurrentData::getCommitCount(const QString& key)
{
	if (iForceCommitted.contains(key))
		return iForceCommitted[key];

	int count = 0;
	QString key_no;
	for (int i=0; i<iCommitCountMax; i++) {
		key_no = QString("%1~%2").arg(key).arg(i);
		if (iForceCommitted.contains(key_no))
			count += iForceCommitted[key_no];
	}
	return count;
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
	return charac->force() - forceCommitCount();
}

void CurrentData::setCharSkill(const CharSkill& char_skill)
{
	MethodID skill_id = KeyMethod::instance.getID(char_skill.key);
	iCharSkills[skill_id] = char_skill;
}

CharSkill CurrentData::getCharSkill(MethodID skill_id)
{
	MethodID int_skill_id = skill_id;
	CharSkill char_skill;

	switch (skill_id) {
		case KM_ICOOL:
			int_skill_id = KM_COOL;
			break;
		case KM_IVIG:
			int_skill_id = KM_VIGIL;
			break;
		case KM_REC:
			int_skill_id = KM_COOL;
			break;
		case KM_FDISC:
			int_skill_id = KM_DISC;
			break;
		case KM_FCOOL:
			int_skill_id = KM_COOL;
			break;
		default:
			break;
	}

	if (iCharSkills.contains(int_skill_id)) {
		char_skill = iCharSkills[int_skill_id];
		char_skill.skillID = skill_id;
		char_skill.intSkillID = int_skill_id;
	}

	return char_skill;
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

void CurrentData::dicePoolChanged(Character* charac, ChecklistItem* item, bool list_setup)
{
	QString new_curr_pool;

	new_curr_pool = checklistItems.getCurrentPool();

	QString custom_checklist = getCustomChecklistKey(charac);
	if (checkLists.contains(custom_checklist))
		new_curr_pool += checkLists[custom_checklist].getCurrentPool();

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

	if (item) {
		if (item->checked)
			setTemporaryStrain(charac, temporaryStrain + item->reduceStrain);
		else if (!list_setup)
			setTemporaryStrain(charac, temporaryStrain - item->reduceStrain);
	}

	charac->setChangeDicePool(new_curr_pool, item, list_setup);
}

QString CurrentData::getCustomChecklistKey(Character* charac)
{
	QString key = charac->getCurrentTalentKey();
	if (key.isEmpty())
		key = charac->getCurrentSkillKey();

	QString power;
	QString base;
	if (CharTalentMap::forcePower(key, power, base) > 0)
		// Use the "base" key for force powers:
		key = power + "BASIC";
	return key;
}

void CurrentData::addCheckItem(const QString& skillKey, const QString& pool, const QString& desc)
{
	int ref = ++iReferenceCounter;
	if (!checkLists.contains(skillKey)) {
		ChecklistData list;

		list.skillKey = skillKey;
		checkLists[skillKey] = list;
	}
	checkLists[skillKey].appendCustom(ref, pool, desc);
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

	QMapIterator<QString, ChecklistData> i(checkLists);
	while (i.hasNext()) {
		i.next();
		if (i.value().hasCustomItem()) {
			QListIterator<ChecklistItem> j(i.value().items);
			const ChecklistItem* item;

			data += QString(" <CheckList>\n");
			data += QString("  <Skill>%1</Skill>\n").arg(i.key());

			while (j.hasNext()) {
				item = &j.next();
				if (item->reference > 0) {
					data += QString("  <CheckItem>");
					data += QString("<DicePool>%1</DicePool>").arg(item->dicePool);
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
		iCurrentData->addCheckItem(iItemCheckSkill, iItemPool, iItemDesc);

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
	QString key = Character::instance->getCurrentSkillKey();
	int count = 0;

	count = CurrentData::instance->checklistItems.items.count();
	if (CurrentData::instance->checkLists.contains(key)) {
		count += CurrentData::instance->checkLists[key].items.count();
	}
	return count+1;
}

QVariant CheckList::getValue(int row, int col)
{
	QString key = Character::instance->getCurrentSkillKey();
	ChecklistItem* item = NULL;

	if (row >= 0) {
		int c1 = CurrentData::instance->checklistItems.items.count();

		if (row < c1) {
			item = &CurrentData::instance->checklistItems.items[row];
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
				return item->dice();
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

