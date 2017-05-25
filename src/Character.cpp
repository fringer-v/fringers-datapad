#include <QEventLoop>
#include <QDir>

#include "Character.h"
#include "DatUtil.h"
#include "DataAccess.h"
#include "CharacterXML.h"
#include "DatZip.h"
#include "ItemAttachments.h"
#include "ItemDescriptors.h"
#include "md5.h"
#include "DataSet.h"
#include "CharacterList.h"
#include "CharCache.h"

int Character::iLoading;

// CharSkill -------------------------------------

QString CharSkill::getDicePool(Skill* skill, QString ch)
{
	Character* character;
	CurrentData* current_data;

	if (!(character = Character::instance))
		return "";
	if (!(current_data = CurrentData::instance))
		return "";

	if (!skill) {
		skill = Skill::getSkill(key);
		if (!skill)
			return "EE";
		ch = skill->characteristic;
		if (strcmp(skill->key, "LTSABER") == 0)
			ch = current_data->talents.getLightSaberChar();
	}

	QString pool;
	int boostCount = 0;
	int setbackCount = 0;
	int addForceDice = 0;
	int optionalRemoveSetback = 0;
	int optionalUpgradeCount = 0;
	int optionalDowngradeCount = 0;
	int optionalBoost = 0;
	int threatCount = 0;
	bool defr = false;
	bool defm = false;

	if (strcmp(skill->key, "DEFM") == 0) {
		defm = true;
		pool = DatUtil::repeat("S", character->defenseMelee());
		if (current_data->isCommitted("SENSECONTROL1")) {
			optionalDowngradeCount++;
			if (current_data->talents.contains("SENSESTRENGTH"))
				optionalDowngradeCount++;
		}
		threatCount = current_data->isCommitted("MISDIRCONTROL3");
	}
	else if (strcmp(skill->key, "DEFR") == 0) {
		defr = true;
		pool = DatUtil::repeat("S", character->defenseRanged());
		if (current_data->isCommitted("SENSECONTROL1")) {
			optionalDowngradeCount++;
			if (current_data->talents.contains("SENSESTRENGTH"))
				optionalDowngradeCount++;
		}
		threatCount = current_data->isCommitted("MISDIRCONTROL3");
	}
	else {
		pool = DatUtil::getBasicDicePool(ranks, character->getAttribute(skill ? ch : 0));
		if (skill->type == COMBAT) {
			if (current_data->isCommitted("SENSECONTROL3"))
				pool += "UU";
		}
	}

	if (strcmp(skill->key, "REC") == 0) {
		if (current_data->talents.contains("BAL"))
			addForceDice++;
	}
	else if (strcmp(skill->key, "ICOOL") == 0 || strcmp(skill->key, "IVIG") == 0) {
		if (current_data->talents.contains("FORSEECONTROL1") > 0)
			addForceDice++;
		else if (current_data->talents.contains("WARFORCONTROL1") > 0)
			addForceDice++;
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

		if (char_talent.key == "INTIM" && key == "COERC")
			optionalUpgradeCount += char_talent.ranks;
		if (char_talent.key == "CONGENIAL" &&
			(key == "CHARM" || key == "NEG"))
			optionalUpgradeCount += char_talent.ranks;
		if (char_talent.key == "DODGE" &&
			(key == "DEFM" || key == "DEFR"))
			optionalDowngradeCount += char_talent.ranks;
	}

	for (int i = 0; i<current_data->armor.rowCount(); i++) {
		Item item = current_data->armor.itemAt(i);
		if (item.equipped()) {
			if (item.dieModList.contains(key)) {
				DieMod mod = item.dieModList.get(key);
				pool += DatUtil::repeat("B", mod.boostCount);
				pool += DatUtil::repeat("N", mod.setbackCount);
				addForceDice += mod.forceCount;
				pool += DatUtil::repeat("a", mod.advantageCount);
				pool += DatUtil::repeat("t", mod.threatCount);
				pool += DatUtil::repeat("U", mod.upgradeCount);
			}
			if (item.hasQuality(key)) {
				Quality qual = item.getQuality(key);
				pool += DatUtil::repeat("U", qual.count);
			}
		}
	}

	if (key == "CHARM" || key == "COERC") {
		if (current_data->gear.equipped("EXPJEWELRY"))
			pool += "a";
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

	if (addForceDice + optionalRemoveSetback + optionalUpgradeCount + optionalDowngradeCount + optionalBoost) {
		QString opt;
		// Mark the optional dice
		// Optional dice are just an indicator that something else
		// can be added to the pool
		opt += DatUtil::repeat("B", optionalBoost);
		if (addForceDice > 0)
			opt += DatUtil::repeat("F", current_data->nonCommitedForce(Character::instance));
		opt += DatUtil::repeat("N", optionalRemoveSetback);
		opt += DatUtil::repeat("d", optionalUpgradeCount);
		opt += DatUtil::repeat("u", optionalDowngradeCount);
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

// Character -------------------------------------

Character* Character::instance;

Character::Character(QObject *parent) :
	QObject(parent)
{
	if (!CurrentData::instance)
		CurrentData::instance = new CurrentData();

	iLocked = 0;
	iHideCodedTalents = 0;

	iActiveSkill.clear();
	iActiveSkillKey.clear();
	iDicePool.clear();
	iItemUuid.clear();
	iItemItemKey.clear();
	iItemName.clear();
	iItemSkill.clear();
	iItemDamage.clear();
	iItemCritical.clear();
	iItemQualities.clear();
	iItemAttachments.clear();
	iItemCritPlus = 0;
	iItemPierce = 0;

	iChangeDicePool.clear();
	iModDicePool.clear();
	iModItemDamage = 0;
	iModItemPierce = 0;
	iModItemCrit = 0;
	iModItemRange = 0;

	iImageProviderCount = 0;

	connect(&iCharacterDownloader, SIGNAL(downloaded(bool)), SLOT(characterDownloaded(bool)));
	connect(&iDataSetDownloader, SIGNAL(downloaded(bool)), SLOT(dataSetDownloaded(bool)));
	connect(&iSystemDataDownloader, SIGNAL(downloaded(bool)), SLOT(systemDataDownloaded(bool)));

	setLastSystemDataUpdate(DataAccess::getSystemDataLastUpdate());

	Character::instance = this;
}

void Character::init(void)
{
	DataAccess::getDatapadSettings(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword, iLocked, iHideCodedTalents);
	emit hostChanged(iHost);
	emit emailChanged(iEmail);
	emit passwordChanged(iPassword);
	emit lockedChanged(iLocked);
	emit hideCodedTalentsChanged(iHideCodedTalents);
	setDataSet(DataAccess::selectedDataSet());
	setFile(DataAccess::selectedCharacter());
}

void Character::setCurrentEmailAndPassword(const QString& v, const QString& p)
{
	if (iCurrentEmail != v || iCurrentPassword != p) {
		iCurrentEmail = v;
		iCurrentPassword = p;
		DataAccess::setServerAccessInfo(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword);
	}
}

void Character::setHost(const QString& v)
{
	if (v.isEmpty() || iHost != v) {
		iHost = v;
		DataAccess::setServerAccessInfo(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword);
		emit hostChanged(v.isEmpty() ? DEFAULT_HOST : v);
	}
}

void Character::setEmail(const QString& v)
{
	if (iEmail != v) {
		iEmail = v;
		DataAccess::setServerAccessInfo(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword);
		emit emailChanged(v);
	}
}

void Character::setPassword(const QString& v)
{
	QString hashed_password = Md5::hash(v.toUtf8() + "1484-026/394-001/4634-92296-729/293--737");
	if (iPassword != hashed_password) {
		iPassword = hashed_password;
		DataAccess::setServerAccessInfo(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword);
		emit passwordChanged(v);
	}
}

void Character::setLocked(int t)
{
	if (iLocked != t) {
		iLocked = t;
		DataAccess::setLocked(t);
		emit lockedChanged(t);
	}
}

void Character::setHideCodedTalents(int t)
{
	if (iHideCodedTalents != t) {
		iHideCodedTalents = t;
		DataAccess::setHideCodedTalents(t);
		emit hideCodedTalentsChanged(t);
		Talents::instance.rowCountChanged();
	}
}

void Character::setLoading(bool v)
{
	if (v) {
		//qDebug() << "INC LOAD" << iLoading << " ---> " << iLoading+1;
		iLoading++;
	}
	else {
		iLoading--;
		//qDebug() << "DEC LOAD" << iLoading+1 << " ---> " << iLoading;
	}
	emit loadingChanged(iLoading != 0);
	if (v)
		DatUtil::yield();
}

void Character::setLastSystemDataUpdate(const QString& la)
{
	if (iLastSystemDataUpdate != la) {
		iLastSystemDataUpdate = la;
		emit lastSystemDataUpdateChanged(la);
	}
}

void Character::setFile(const QString& file)
{
	if (iFile.isNull() || iFile != file) {
		setLoading(true);
		iFile = file;
		if (iFile.isEmpty())
			iFile = "";
		if (!iDataSet.isNull()) {
			if (!iFile.isEmpty() && iDataSet.isEmpty() && DataAccess::dataSets.rowCount() > 0) {
				// Automatically select a data set:
				QString data_set = DataAccess::dataSets.getValueAsString(0, "file") + "/" + DataAccess::dataSets.getValueAsString(0, "name");
				setDataSet(data_set);
				goto selected;
			}
			reload();
		}

		selected:
		DataAccess::setSelectedCharacter(iFile);
		setLoading(false);
		emit fileChanged(iFile);
	}
}

void Character::setDataSet(const QString& data_set)
{
	if (iDataSet.isNull() || iDataSet != data_set) {
		ShopWeapons* shop_weapons;
		ShopArmor* shop_armor;
		ShopGear* shop_gear;

		setLoading(true);
		iDataSet = data_set;
		if (iDataSet.isEmpty())
			iDataSet = "";

		DataSet::instance.clear();
		Shop::instance.clear();

		DataSet::instance.loadCareers(iDataSet);
		DataSet::instance.loadSpecies(iDataSet);

		shop_weapons = new ShopWeapons;
		shop_weapons->loadWeapons();
		delete shop_weapons;

		shop_armor = new ShopArmor;
		shop_armor->loadArmor();
		delete shop_armor;

		shop_gear = new ShopGear;
		shop_gear->loadGear();
		delete shop_gear;

		Motivations::instance.loadMotivations(iDataSet);
		Moralities::instance.loadMoralities(iDataSet);
		AllTalents::instance()->loadTalents(iDataSet);
		ItemAttachments::instance()->loadItemAttachments(iDataSet);
		ItemDescriptors::instance()->loadItemDescriptors(iDataSet);

		if (!iFile.isNull())
			reload();
		setLoading(false);
		DataAccess::setSelectedDataSet(iDataSet);

		Weapons::instance.setDataChanged();
		Armor::instance.setDataChanged();
		Gear::instance.setDataChanged();
		MotivationList::instance.setDataChanged();
		MoralityList::instance.setDataChanged();
		emit dataSetChanged(iDataSet);
	}
}

void Character::downloadCharacter(const QString& url)
{
	if (url.isEmpty())
		return;
	setLoading(true);
	iCharacterDownloader.setUrl(url);
}

void Character::downloadDataSet(const QString& url)
{
	if (url.isEmpty())
		return;
	setLoading(true);
	iDataSetDownloader.setUrl(url);
}

void Character::downloadSystemData(const QString& url)
{
	if (url.isEmpty())
		return;
	setLoading(true);
	iSystemDataDownloader.setUrl(url);
}

void Character::syncWithServer()
{
	setLoading(true);
	QString error = iSyncer.sync();
	CharCache::instance.clearCache();
	if (!error.isEmpty())
		emit alert("Sync with Server Failed", error);

	if (iSyncer.currentChanged()) {
		iFile = "x";
		setDataSet(DataAccess::selectedDataSet(iDataSet));
		setFile(DataAccess::selectedCharacter());
	}
	else {
		setDataSet(DataAccess::selectedDataSet(iDataSet));
	}

	setLoading(false);
}

void Character::characterDownloaded(bool ok)
{
	if (!ok) {
		setLoading(false);
		emit alert("Download Character Data Failed", "Unable to retrieve Character Data from URL: "+iCharacterDownloader.getUrl());
		return;
	}

	QString url = iCharacterDownloader.getUrl();
	QString file = DatUtil::lastNameOfPath(url);
	QByteArray data = iCharacterDownloader.getData();

	DataAccess::addCharacter(file, data);
	setFile(file);
	setLoading(false);
}

void Character::dataSetDownloaded(bool ok)
{
	if (!ok) {
		setLoading(false);
		emit alert("Download DataSet Failed", "Unable to retrieve DataSet from URL: "+iDataSetDownloader.getUrl());
		return;
	}

	QString err_msg;
	QString dest = processDownload(iDataSetDownloader.getUrl(),
		iDataSetDownloader.getData(), DatUtil::getDataSetFolder(), err_msg);

	if (dest.isEmpty()) {
		setLoading(false);
		emit alert("Unpack DataSet Failed", err_msg);
		return;
	}

	DataAccess::addDataSet(DatUtil::left(dest, "/"), DatUtil::right(dest, "/"));
	setDataSet(dest);
	setLoading(false);
}

void Character::systemDataDownloaded(bool ok)
{
	if (!ok) {
		setLoading(false);
		emit alert("Download System Data Failed", "Unable to retrieve System Data from URL: "+iSystemDataDownloader.getUrl());
		return;
	}

	QString err_msg;
	QString dest = processDownload(iSystemDataDownloader.getUrl(),
		iSystemDataDownloader.getData(), DatUtil::getSystemDataFolder(), err_msg);

	if (dest.isEmpty()) {
		setLoading(false);
		emit alert("Unpack DataSet Failed", err_msg);
		return;
	}

	if (dest != "Data") {
		emit alert("Download System Data Failed", QString("Unpack DataSet Failed, the archived folder must be called \"Data\", not \"%1\"").arg(dest));
		DataAccess::removeDir(DatUtil::getSystemDataFolder() + dest);
		return;
	}

	setLastSystemDataUpdate(DataAccess::getSystemDataLastUpdate());
	reload();
	setLoading(false);
}

QString Character::activeSkill()
{
	return iActiveSkill;
}

QString Character::activeSkillKey()
{
	return iActiveSkillKey;
}

QString Character::dicePool()
{
	return DatUtil::normalizeDice(iDicePool + iModDicePool);
}

int Character::negativePool()
{
	return CurrentData::instance->negativePool();
}

QString Character::itemUuid()
{
	return iItemUuid;
}

QString Character::itemItemKey()
{
	return iItemItemKey;
}

QString Character::itemName()
{
	return iItemName;
}

QString Character::itemRange()
{
	if (iModItemRange) {
		int range = 0;

		// Engaged, short, medium, long, extreme.
		switch (iItemRange[0].unicode()) {
			case 'E':
				if (iItemRange[1].unicode() == 'x')
					range = 4;
				else
					range = 0;
				break;
			case 'S':
				range = 1;
				break;
			case 'M':
				range = 2;
				break;
			case 'L':
				range = 3;
				break;
		}
		range += iModItemRange;
		switch (range) {
			case 0:
				return "Engaged";
			case 1:
				return "Short";
			case 2:
				return "Medium";
			case 3:
				return "Long";
			default:
				if (range < 0)
					return "Short";
				return "Extreme";
		}
	}
	return iItemRange;
}

QString Character::itemSkill()
{
	return iItemSkill;
}

QString Character::itemDamage()
{
	QString damage = iItemDamage;


	if (iModItemDamage) {
		QString val = iItemDamage;
		QString rig;

		if (val.contains("[")) {
			val = DatUtil::left(val, "[");
			rig = DatUtil::right(val, "[");
		}

		if (val.contains("+")) {
			val = DatUtil::left(val, "+");
			rig = DatUtil::right(val, "+") + rig;
		}

		damage = QString("%1%2").arg(val.toInt() + iModItemDamage).arg(rig);
	}

	if (iItemPierce + iModItemPierce)
		damage = damage + QString(" ➤ %1").arg(iItemPierce + iModItemPierce);

	return damage;
}

QString Character::itemCritical()
{
	QString crit_dice = DatUtil::repeat("a", iItemCritical.toInt());

	if (!crit_dice.isEmpty() && iItemCritPlus > 0)
		return crit_dice + QString(" +%1%").arg(iItemCritPlus*10+iModItemCrit);
	return crit_dice;
}

QString Character::itemQualities()
{
	return iItemQualities;
}

QString Character::itemAttachments()
{
	return iItemAttachments;
}

QString Character::itemManeuvers()
{
	QString val = QString("  %1|/ 2  ").arg(CurrentData::instance->autoCheckItems.movesUsed());
	return val;
}

QString Character::itemStrain()
{
	QString cons;

	QString val = QString("  %1|(%2 / %3)  ")
		.arg(CurrentData::instance->autoCheckItems.strainUsed(cons))
		.arg(currentStrain())
		.arg(strain());
	return val;
}

int Character::itemCritPlus()
{
	return iItemCritPlus;
}

int Character::itemPierce()
{
	return iItemPierce;
}

int Character::imageProviderCount()
{
	return iImageProviderCount;
}

void Character::adjustWounds(int delta)
{
	CurrentData::instance->adjustWounds(delta);
	emit currentWoundsChanged(CurrentData::instance->wounds);
	emit woundHistoryChanged(CurrentData::instance->woundHistory);
}

void Character::adjustStrain(int delta)
{
	CurrentData::instance->adjustStrain(delta);
	emit currentStrainChanged(CurrentData::instance->strain+CurrentData::instance->temporaryStrain);
	emit strainHistoryChanged(CurrentData::instance->strainHistory);
}

void Character::adjustConflict(int delta)
{
	CurrentData::instance->adjustConflict(delta);
	emit currentConflictChanged(CurrentData::instance->conflict);
	emit conflictHistoryChanged(CurrentData::instance->conflictHistory);
}

void Character::useStimPack(int delta)
{
	CurrentData::instance->useStimPack(this, delta);
	emit stimPacksUsedChanged(CurrentData::instance->stimPacksUsed);
	emit currentWoundsChanged(CurrentData::instance->wounds);
	emit woundHistoryChanged(CurrentData::instance->woundHistory);
}

void Character::useErp(int delta)
{
	CurrentData::instance->useErp(this, delta);
	emit erpsUsedChanged(CurrentData::instance->erpsUsed);
	emit currentWoundsChanged(CurrentData::instance->wounds);
	emit woundHistoryChanged(CurrentData::instance->woundHistory);
}

void Character::setWoundDelta(int val)
{
	if (CurrentData::instance->setWoundDelta(val))
		emit woundDeltaChanged(val);
}

void Character::setStrainDelta(int val)
{
	if (CurrentData::instance->setStrainDelta(val))
		emit strainDeltaChanged(val);
}

void Character::addCriticalWound(int perc, int type)
{
	CurrentData::instance->addCriticalWound(this, perc, type);
}

void Character::removeCriticalWound(int ref)
{
	CurrentData::instance->removeCriticalWound(this, ref);
}

void Character::addExperience(const QString& type, const QString& key, const QString& name, const QString& desc, int amount)
{
	CurrentData::instance->addExperience(this, type, key, name, desc, amount);
}

void Character::changeExperience(int ref, const QString& desc, int amount)
{
	CurrentData::instance->changeExperience(this, ref, desc, amount);
}

void Character::removeExperience(int ref)
{
	CurrentData::instance->removeExperience(this, ref);
}

void Character::addCustomSkill(const QString& name, const QString& characteristic, int ranks)
{
	CurrentData::instance->addCustomSkill(this, name, characteristic, ranks);
}

void Character::removeCustomSkill(const QString& name)
{
	CurrentData::instance->removeCustomSkill(this, name);
}

void Character::addInvItem(int count, const QString& itemkey, const QString& desc, int amount)
{
	if (!itemkey.isEmpty() && !desc.isEmpty())
		return;
	CurrentData::instance->addItem(this, count, itemkey, desc, amount);
	if (amount != 0)
		emit creditsChanged(credits());
}

void Character::updateInvItem(int ref, int count, const QString& desc, int amount)
{
	CurrentData::instance->updateItem(this, ref, count, desc, amount);
}

bool Character::removeInvItem(int ref)
{
	return CurrentData::instance->removeItem(this, ref);
}

int Character::getPrice(const QString& key)
{
	if (Shop::instance.contains(key))
		return Shop::instance.getItem(key).price;
	return 0;
}

int Character::getRarity(const QString& key)
{
	if (Shop::instance.contains(key))
		return Shop::instance.getItem(key).rarity;
	return 0;
}

void Character::searchShop(QString search_string)
{
	Shop::instance.search(search_string);
}

void Character::appendCheckItem(const QString& pool, const QString& desc)
{
	return CurrentData::instance->appendCheckItem(this, iActiveSkillKey, pool, desc);
}

void Character::updateCheckItem(int ref, const QString& pool, const QString& desc)
{
	return CurrentData::instance->updateCheckItem(this, ref, iActiveSkillKey, pool, desc);
}

void Character::removeCheckItem(int ref)
{
	CurrentData::instance->removeCheckItem(this, ref, iActiveSkillKey);
}

void Character::checkItem(int ref)
{
	CurrentData::instance->checkItem(this, ref, iActiveSkillKey, false);
}

void Character::changeEquipment(const QString& uuid, int state, int stored)
{
	bool weapons_changed = CurrentData::instance->weapons.changeEquipment(uuid, state, stored);
	bool armor_changed = CurrentData::instance->armor.changeEquipment(uuid, state, stored);
	bool gear_changed = CurrentData::instance->gear.changeEquipment(uuid, state, stored);

	if (weapons_changed)
		Weapons::instance.setDataChanged();
	if (armor_changed)
		Armor::instance.setDataChanged();
	if (gear_changed)
		Gear::instance.setDataChanged();

	inventoryChanged();

	if (weapons_changed || armor_changed) {
		Weapons::instance.makeClean();
		Armor::instance.makeClean();
	}

	if (gear_changed)
		Gear::instance.makeClean();
}

void Character::showCharacteristics()
{
	InjuryList::instance.makeClean();
}

void Character::showDescription()
{
	ObligationList::instance.makeClean();
	DutyList::instance.makeClean();
	MotivationList::instance.makeClean();
	MoralityList::instance.makeClean();
}

void Character::showSkills()
{
	GeneralSkills::instance.makeClean();
	CombatSkills::instance.makeClean();
	KnowledgeSkills::instance.makeClean();
	SpecialSkills::instance.makeClean();
	CustomSkills::instance.makeClean();
}

void Character::showTalents()
{
	ExperienceList::instance.makeClean();
	SpecialFeaturesList::instance.makeClean();
	Talents::instance.makeClean();
}

void Character::showWeaponsAndArmor()
{
	Weapons::instance.makeClean();
	Armor::instance.makeClean();
}

void Character::showGear()
{
	Gear::instance.makeClean();
}

void Character::showInventory()
{
	InventoryLog::instance.makeClean();
}

void Character::showCheckList()
{
	iModDicePool.clear();
	CurrentData::instance->clearAutoCheckItems(this, iActiveSkillKey);
	CheckList::instance.setRowCountChanged();
	CheckList::instance.makeClean();
}

void Character::hideCheckList()
{
	CurrentData::instance->exitAutoCheckItems(this, iActiveSkillKey);
}

void Character::fillCheckList()
{
	CurrentData::instance->setupAutoCheckItems(this, iActiveSkillKey, iItemUuid);
	CheckList::instance.setRowCountChanged();
	CheckList::instance.makeClean();
}

void Character::setName(const QString& name)
{
	if (CurrentData::instance->name != name) {
		CurrentData::instance->name = name;
		emit nameChanged(name);
	}
}

void Character::setPlayer(const QString& player)
{
	if (CurrentData::instance->player != player) {
		CurrentData::instance->player = player;
		emit playerChanged(player);
	}
}

void Character::setGender(const QString& gender)
{
	if (CurrentData::instance->gender != gender) {
		CurrentData::instance->gender = gender;
		emit genderChanged(gender);
	}
}

void Character::setAge(const QString& age)
{
	if (CurrentData::instance->age != age) {
		CurrentData::instance->age = age;
		emit ageChanged(age);
	}
}

void Character::setHeight(const QString& height)
{
	if (CurrentData::instance->height != height) {
		CurrentData::instance->height = height;
		emit heightChanged(height);
	}
}

void Character::setBuild(const QString& build)
{
	if (CurrentData::instance->build != build) {
		CurrentData::instance->build = build;
		emit buildChanged(build);
	}
}

void Character::setHair(const QString& hair)
{
	if (CurrentData::instance->hair != hair) {
		CurrentData::instance->hair = hair;
		emit hairChanged(hair);
	}
}

void Character::setEyes(const QString& eyes)
{
	if (CurrentData::instance->eyes != eyes) {
		CurrentData::instance->eyes = eyes;
		emit eyesChanged(eyes);
	}
}

void Character::setFeatures(const QString& features)
{
	if (CurrentData::instance->features != features) {
		CurrentData::instance->features = features;
		emit featuresChanged(features);
	}
}

void Character::setStory(const QString& value)
{
	if (CurrentData::instance->story != value) {
		CurrentData::instance->story = value;
		emit storyChanged(value);
	}
}

void Character::setSpecies(const QString& species)
{
	if (CurrentData::instance->species != species) {
		CurrentData::instance->species = species;
		emit speciesChanged(species);
	}
}

void Character::setCareer(const QString& career)
{
	if (CurrentData::instance->career != career) {
		CurrentData::instance->career = career;
		emit careerChanged(career);
	}
}

void Character::setSpecializations(const QString& spec)
{
	if (CurrentData::instance->specializations != spec) {
		CurrentData::instance->specializations = spec;
		emit specializationsChanged(spec);
	}
}

void Character::setPortrait(const QString& portrait)
{
	if (CurrentData::instance->portrait != portrait) {
		CurrentData::instance->portrait = portrait;
		emit portraitChanged(portrait);
	}
}

void Character::setCredits(int c)
{
	if (CurrentData::instance->credits != c) {
		CurrentData::instance->credits = c;
		emit creditsChanged(c);
	}
}

void Character::setEncValue(int c)
{
	if (CurrentData::instance->encumbranceValue != c) {
		CurrentData::instance->encumbranceValue = c;
		emit encValueChanged(c);
	}
}

void Character::setEncThreshold(int c)
{
	if (CurrentData::instance->encumbranceThreshold != c) {
		CurrentData::instance->encumbranceThreshold = c;
		emit encThresholdChanged(c);
	}
}

void Character::setCumbValue(int c)
{
	if (CurrentData::instance->cumbersomeValue != c) {
		CurrentData::instance->cumbersomeValue = c;
		emit cumbValueChanged(c);
	}
}

void Character::setCumbThreshold(int c)
{
	if (CurrentData::instance->cumbersomeThreshold != c) {
		CurrentData::instance->cumbersomeThreshold = c;
		emit cumbThresholdChanged(c);
	}
}

void Character::setEncText(const QString& t)
{
	if (CurrentData::instance->encumbranceText != t) {
		CurrentData::instance->encumbranceText = t;
		emit encTextChanged(t);
	}
}

void Character::setActiveSkill(const QString& t)
{
	if (iActiveSkill != t) {
		Skill* skill;

		iActiveSkill = t;
		if ((skill = Skill::getSkillByName(t)))
			iActiveSkillKey = skill->key;
		else {
			iActiveSkillKey = t;
			if ((skill = Skill::getSkill(t)))
				iActiveSkill = skill->name;
		}
		emit activeSkillChanged(iActiveSkill);
	}
}

void Character::setDicePool(const QString& t)
{
	QString pool =  t;

	// This means set to the active skill!
	if (pool == "?") {
		CharSkill char_skill;

		if (CurrentData::instance->skills.contains(iActiveSkillKey))
			char_skill = CurrentData::instance->skills[iActiveSkillKey];
		else
			char_skill.key = iActiveSkillKey;
		pool = char_skill.getDicePool();
	}

	// ab|cd|12|xx|34 = ab1234...
	while (DatUtil::contains(pool, "|")) {
		QString rpool;

		rpool = DatUtil::right(pool, "|");
		pool = DatUtil::left(pool, "|");
		pool += DatUtil::right(rpool, "|");
	}

	if (iDicePool != pool) {
		iDicePool = pool;
		emit dicePoolChanged(dicePool());
	}
}

void Character::setNegativePool(int t)
{
	CurrentData::instance->setNegativePool(this, t, iActiveSkillKey);
	emit negativePoolChanged(negativePool());
}

void Character::setItemUuid(const QString& t)
{
	if (iItemUuid != t) {
		iItemUuid = t;
		emit itemUuidChanged(t);
	}
}

void Character::setItemItemKey(const QString& t)
{
	if (iItemItemKey != t) {
		iItemItemKey = t;
		emit itemItemKeyChanged(t);
	}
}

void Character::setItemName(const QString& t)
{
	if (iItemName != t) {
		iItemName = t;
		emit itemNameChanged(t);
	}
}

void Character::setItemRange(const QString& t)
{
	if (iItemRange != t) {
		iItemRange = t;
		emit itemRangeChanged(t);
	}
}

void Character::setItemSkill(const QString& t)
{
	if (iItemSkill != t) {
		iItemSkill = t;
		emit itemSkillChanged(t);
	}
}

void Character::setItemDamage(const QString& t)
{
	if (iItemDamage != t) {
		iItemDamage = t;
		emit itemDamageChanged(t);
	}
}

void Character::setItemCritical(const QString& t)
{
	if (iItemCritical != t) {
		iItemCritical = t;
		emit itemCriticalChanged(itemCritical());
	}
}

void Character::setItemQualities(const QString& t)
{
	if (iItemQualities != t) {
		iItemQualities = t;
		emit itemQualitiesChanged(t);
	}
}

void Character::setItemAttachments(const QString& t)
{
	if (iItemAttachments != t) {
		iItemAttachments = t;
		emit itemAttachmentsChanged(t);
	}
}

void Character::setItemManeuvers(const QString& t)
{
}

void Character::setItemStrain(const QString& t)
{
}

void Character::setItemCritPlus(int t)
{
	if (iItemCritPlus != t) {
		iItemCritPlus = t;
		emit itemCriticalChanged(itemCritical());
		emit itemCritPlusChanged(t);
	}
}

void Character::setItemPierce(int t)
{
	if (iItemPierce != t) {
		iItemPierce = t;
		emit itemDamageChanged(itemDamage());
		emit itemPierceChanged(t);
	}
}

void Character::setImageProviderCount(int t)
{
	if (iImageProviderCount != t) {
		iImageProviderCount = t;
		emit imageProviderCountChanged(t);
	}
}

int Character::getAttribute(const QString& val)
{
	return CurrentData::instance->getAttribute(val);
}

int Character::setAttributeMods(const CharMods& mods)
{
	int changed = CurrentData::instance->attributeMods.set(mods);
	if (changed & BR_BIT)
		emit brawnChanged(brawn());
	if (changed & AG_BIT)
		emit agilityChanged(agility());
	if (changed & INT_BIT)
		emit intellectChanged(intellect());
	if (changed & CUN_BIT)
		emit cunningChanged(cunning());
	if (changed & WIL_BIT)
		emit willpowerChanged(willpower());
	if (changed & PR_BIT)
		emit presenceChanged(presence());
	if (changed & SOAK_BIT)
		emit soakChanged(soak());
	if (changed & DRANGED_BIT)
		emit defenseRangedChanged(defenseRanged());
	if (changed & DMELEE_BIT)
		emit defenseMeleeChanged(defenseMelee());
	return changed;
}

void Character::setChangeDicePool(const QString& dice)
{
	if (iChangeDicePool != dice) {
		iChangeDicePool = dice;
		iModDicePool.clear();
		iModItemDamage = 0;
		iModItemPierce = 0;
		iModItemCrit = 0;
		iModItemRange = 0;

		int i=0;
		while (i<dice.length()) {
			if (dice[i].isDigit())
				;
			else if (((dice[i].unicode() == '-' || dice[i].unicode() == '+') &&
				 i+1<dice.length() && dice[i+1].isDigit()) || dice[i].isDigit()) {
				bool neg = false;
				QString num;
				int val;

				if (dice[i].unicode() == '-') {
					neg = true;
					i++;
				}
				else if (dice[i].unicode() == '+')
					i++;
				while (i<dice.length() && dice[i].isDigit()) {
					num.append(dice[i]);
					i++;
				}

				val = num.toInt() * (neg ? -1 : 1);
				if (i<dice.length() && dice[i].unicode() == '%') {
					iModItemCrit += val;
					i++;
				}
				else if (i<dice.length() && dice[i].unicode() == 'R') {
					iModItemRange += val;
					i++;
				}
				else if (i<dice.length() && dice[i].unicode() == 'M')
					i++;
				else
					iModItemDamage += val;
			}
			else if (dice[i].unicode() == '@') {
				// iItemModPierce
				QString str;

				i++;
				while (i<dice.length() && dice[i].unicode() != '@') {
					str.append(dice[i].unicode());
					i++;
				}
				if (i<dice.length())
					i++;
				if (str == "Breach +1")
					iModItemPierce += 10;
				if (str.startsWith("Damage ")) {
					int v = DatUtil::right(str, "Damage ").toInt();
					iModItemDamage += v;
				}
				else if (str.startsWith("Range ")) {
					int v = DatUtil::right(str, "Range ").toInt();
					iModItemRange += v;
				}
			}
			else {
				iModDicePool.append(dice[i]);
				i++;
			}

		}

		emit dicePoolChanged(dicePool());
		emit negativePoolChanged(negativePool());
		emit itemCriticalChanged(itemCritical());
		emit itemDamageChanged(itemDamage());
		emit itemRangeChanged(itemRange());
	}
	emit itemManeuversChanged(itemManeuvers());
	emit itemStrainChanged(itemStrain());
}

void Character::inventoryChanged()
{
	int quantity;
	bool wearingArmor = false;
	int maxArmorSoak = 0;
	int thr = 5;
	int enc = 0;
	int light_items = 0;
	int grenade_count = 0;
	bool grenade_bandolier = false;
	bool mil_belt_pouch = false;
	int cumb = 0;
	CharMods mods;
	int burly = 0;
	int val;
	CurrentData* current_data = CurrentData::instance;

	if (current_data->talents.contains("BURLY")) {
		CharTalent& ch_talent = current_data->talents.get("BURLY");
		Talent talent = AllTalents::instance()->getTalent("BURLY");

		burly = ch_talent.ranks * talent.burly;
	}

	if (current_data->gear.equipped(ShopGear::grenBandKey))
		grenade_bandolier = true;

	mods.clear();
	for (int i = 0; i < current_data->weapons.rowCount(); i++) {
		Item item = current_data->weapons.itemAt(i);
		quantity = item.carriedQuantity();
		if (quantity > 0) {
			val = item.encCarriedVal(burly);
			enc += val * quantity;
			cumb += item.cumbersome(burly) * quantity;

			if (item.equipped())
				item.characteristicDelta(mods);

			if (val > 0) {
				if (grenade_bandolier && grenade_count<5) {
					if (item.isGrenade()) {
						int amount = 5-grenade_count;
						if (amount > quantity)
							amount = quantity;
						enc -= val * amount;
						grenade_count += amount;
					}
				}
			}
			else
				light_items += quantity;
		}
	}

	maxArmorSoak = 0;
	for (int i = 0; i < current_data->armor.rowCount(); i++) {
		Item item = current_data->armor.itemAt(i);
		quantity = item.carriedQuantity();
		if (quantity > 0) {
			cumb += item.cumbersome(burly) * quantity;
			if (item.equipped()) {
				// Only one armor item can be equipped!
				enc += item.encWornValue();
				quantity--;
			}
			if (quantity > 0) {
				val = item.encCarriedVal(0);
				enc += val * quantity;
				if (val == 0)
					light_items += quantity;
			}
			if (item.equipped()) {
				wearingArmor = true;
				item.characteristicDelta(mods);
				if (maxArmorSoak < item.soakVal())
					maxArmorSoak = item.soakVal();
				mods.inc(V_SOAK, item.soakVal());
				mods.inc(V_DRANGED, item.rangeDef());
				mods.inc(V_DMELEE, item.meleeDef());
				if (item.hasQuality("ENCTADD"))
					thr += item.getQuality("ENCTADD").count;
			}
		}
	}

	for (int i = 0; i < current_data->gear.rowCount(); i++) {
		Item item = current_data->gear.itemAt(i);
		quantity = item.carriedQuantity();
		if (quantity > 0) {
			val = item.encCarriedVal(0);
			enc += val * quantity;
			cumb += item.cumbersome(burly) * quantity;
			if (item.equipped()) {
				if (val == 0 && !item.canBeWorn())
					light_items += quantity;

				item.characteristicDelta(mods);
				if (item.hasQuality("ENCTADD"))
					thr += item.getQuality("ENCTADD").count;
				if (item.hasQuality("SOAKADD"))
					mods.inc(V_SOAK, item.getQuality("SOAKADD").count);
				if (item.itemkey == "BELTPOUCHMIL")
					mil_belt_pouch = true;
			}
			else {
				if (val == 0)
					light_items += quantity;
			}
		}
	}

	if (mil_belt_pouch) {
		light_items -= 2;
		if (light_items < 0)
			light_items = 0;
	}
	enc += light_items / 10;

	setEncValue(enc);
	setCumbValue(cumb);

	// Add armor talents:
	foreach (CharTalent char_talent, current_data->talents.charTalentMap) {
		Talent talent = AllTalents::instance()->getTalent(char_talent.key);

		if (talent.soakValue > 0) {
			if (!talent.requirementWearingArmor || wearingArmor) {
				int add_soak = talent.soakValue * (talent.ranked ? char_talent.ranks : 1);
				mods.inc(V_SOAK, add_soak);
				maxArmorSoak += add_soak;
			}
		}
	}

	// Add defenses (which may depend on armor!
	foreach (CharTalent char_talent, current_data->talents.charTalentMap) {
		Talent talent = AllTalents::instance()->getTalent(char_talent.key);

		if ((!talent.requirementWearingArmor || wearingArmor) &&
			(!talent.requirementSoakAtLeast || maxArmorSoak >= talent.requirementSoakAtLeast)) {
			if (talent.defenseRanged)
				mods.inc(V_DRANGED, talent.defenseRanged * (talent.ranked ? char_talent.ranks : 1));
			if (talent.defenseMelee)
				mods.inc(V_DMELEE, talent.defenseMelee * (talent.ranked ? char_talent.ranks : 1));
		}
	}

	setAttributeMods(mods);
	characteristicsChanged();

	thr += brawn();
	setEncThreshold(thr);

	QString enc_string;
	QString light_string;

	if (light_items == 0)
		light_string = "no minor items";
	else if (light_items == 1)
		light_string = "1 minor item";
	else
		light_string = QString("%1 minor items").arg(light_items);

	if (enc <= thr)
		enc_string = QString("You are unencumbered (%1)").arg(light_string);
	else {
		int amount = enc - thr;

		enc_string = QString("You are encumbered (%1), add ").arg(light_string);
		for (int i=0; i<amount; i++)
			enc_string += "[SE]";
		enc_string += " to all Brawn and Agility checks.";
		if (amount > brawn())
			enc_string += "[BR]You also loose your free maneuver.";
	}

	if (cumb > brawn()) {
		int amount = cumb - brawn();

		enc_string += "[BR] Your weapon is too cumbersome, add ";
		for (int i=0; i<amount; i++)
			enc_string += "[SE]";
		enc_string += " to combat checks.";
	}
	setEncText(enc_string);

	setCumbThreshold(brawn());
}

void Character::emitExperienceChanged()
{
	emit totalXPChanged(totalXP());
	emit moralityChanged(morality());
	emit strainChanged(strain());
	emit woundChanged(wound());
}

void Character::characteristicsChanged()
{
	Weapons::instance.setDataChanged();
	GeneralSkills::instance.setDataChanged();
	CombatSkills::instance.setDataChanged();
	KnowledgeSkills::instance.setDataChanged();
	SpecialSkills::instance.setDataChanged();
}

void Character::emitStimPacksChanged()
{
	emit stimPacksChanged(stimPacks());
}

void Character::emitErpsChanged()
{
	emit erpsChanged(erps());
}

void Character::emitCharacterCountChanged()
{
	emit characterCountChanged(characterCount());
}

void Character::emitBrawnChanged()
{
	emit brawnChanged(brawn());
}

void Character::emitAgilityChanged()
{
	emit agilityChanged(agility());
}

void Character::emitForceCommittedChanged()
{
	emit forceCommittedChanged(forceCommitted());
}

void Character::emitCurrentStrainChanged()
{
	emit currentStrainChanged(currentStrain());
}

void Character::reload()
{
	setLoading(true);

	if (iFile.isEmpty()) {
		if (!CurrentData::instance->characterFile.isEmpty())
			CurrentData::instance = CharCache::instance.loadCharFile(iFile);
	}
	else {
		if (CurrentData::instance->characterFile.isEmpty())
			delete CurrentData::instance;
		CurrentData::instance = NULL; // Crash if addressed during load!
		CurrentData::instance = CharCache::instance.loadCharFile(iFile);
		Character::instance->inventoryChanged();
		Character::instance->emitExperienceChanged();
	}

	/*
	emit activeSkillChanged(activeSkill());
	emit dicePoolChanged(dicePool());
	emit negativePoolChanged(negativePool());
	emit itemUuidChanged(itemUuid());
	emit itemItemKeyChanged(itemItemKey());
	emit itemNameChanged(itemName());
	emit itemRangeChanged(itemRange());
	emit itemSkillChanged(itemSkill());
	emit itemDamageChanged(itemDamage());
	emit itemCriticalChanged(itemCritical());
	emit itemQualitiesChanged(itemQualities());
	emit itemAttachmentsChanged(itemAttachments());
	emit itemManeuversChanged(itemManeuvers());
	emit itemStrainChanged(itemStrain());
	*/
	emit currentWoundsChanged(currentWounds());
	emit currentStrainChanged(currentStrain());
	emit currentConflictChanged(currentConflict());
	emit woundHistoryChanged(woundHistory());
	emit strainHistoryChanged(strainHistory());
	emit conflictHistoryChanged(conflictHistory());
	emit woundDeltaChanged(woundDelta());
	emit strainDeltaChanged(strainDelta());
	emit stimPacksChanged(stimPacks());
	emit erpsChanged(erps());
	emit stimPacksUsedChanged(stimPacksUsed());
	emit erpsUsedChanged(erpsUsed());

	emit nameChanged(name());
	emit playerChanged(player());
	emit genderChanged(gender());
	emit ageChanged(age());
	emit heightChanged(height());
	emit buildChanged(build());
	emit hairChanged(hair());
	emit eyesChanged(eyes());
	emit featuresChanged(features());
	emit storyChanged(story());
	emit speciesChanged(species());
	emit careerChanged(career());
	emit specializationsChanged(specializations());
	emit portraitChanged(portrait());
	emit creditsChanged(credits());
	emit encValueChanged(encValue());
	emit encThresholdChanged(encThreshold());
	emit cumbValueChanged(cumbValue());
	emit cumbThresholdChanged(cumbThreshold());
	emit encTextChanged(encText());

	emit moralityChanged(morality());

	emit brawnChanged(brawn());
	emit agilityChanged(agility());
	emit intellectChanged(intellect());
	emit cunningChanged(cunning());
	emit willpowerChanged(willpower());
	emit presenceChanged(presence());
	emit soakChanged(soak());
	emit woundChanged(wound());
	emit strainChanged(strain());
	emit defenseRangedChanged(defenseRanged());
	emit defenseMeleeChanged(defenseMelee());
	emit forceChanged(force());
	emit forceCommittedChanged(forceCommitted());
	emit totalXPChanged(totalXP());
	emit newXPChanged(newXP());
	emit usedXPChanged(usedXP());

	GeneralSkills::instance.setDataChanged();
	CombatSkills::instance.setDataChanged();
	KnowledgeSkills::instance.setDataChanged();
	SpecialSkills::instance.setDataChanged();
	CustomSkills::instance.setDataChanged();

	SpecialFeaturesList::instance.setRowCountChanged();
	ObligationList::instance.setRowCountChanged();
	DutyList::instance.setRowCountChanged();
	Talents::instance.setRowCountChanged();
	Weapons::instance.setRowCountChanged();
	Armor::instance.setRowCountChanged();
	Gear::instance.setRowCountChanged();
	InjuryList::instance.setRowCountChanged();
	ExperienceList::instance.setRowCountChanged();
	MotivationList::instance.setRowCountChanged();
	MoralityList::instance.setRowCountChanged();
	InventoryLog::instance.setRowCountChanged();

	setLoading(false);
}

QString Character::processDownload(const QString& url, const QByteArray& data, const QString& to_dir, QString& err_msg)
{
	QString file = DatUtil::lastNameOfPath(url);
	QString tmp_file = DatUtil::getTempFolder() + file;
	DataAccess::writeFile(tmp_file, data);

	QString unpack_dir = DatUtil::addDirChar(to_dir) + file;
	QDir dir;
	if (!dir.exists(unpack_dir) && !dir.mkdir(unpack_dir)) {
		err_msg = "Unable to created directory: "+unpack_dir;
		return QString();
	}

	QString dest = DatZip::uncompress(tmp_file, unpack_dir, err_msg);

	DataAccess::deleteFile(tmp_file);
	return file + "/" + dest;
}
