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

int Character::iLoading;

// CharSkill -------------------------------------

QString CharSkill::getDicePool(Skill* skill, QString ch)
{
	Character* character;

	if (!(character = Character::instance))
		return "";

	if (!skill) {
		skill = Skill::getSkill(key);
		if (!skill)
			return "EE";
		ch = skill->characteristic;
		if (strcmp(skill->key, "LTSABER") == 0)
			ch = character->talents.getLightSaberChar();
	}

	QString pool;
	int boostCount = 0;
	int setbackCount = 0;
	int forceCount = 0;
	int optionalRemoveSetback = 0;
	int optionalUpgradeCount = 0;
	int optionalDowngradeCount = 0;
	int optionalBoost = 0;
	int commitCount = 0;
	int threatCount = 0;
	bool defr = false;
	bool defm = false;

	if (strcmp(skill->key, "DEFM") == 0) {
		defm = true;
		pool = DatUtil::repeat("S", character->defenseMelee());
		commitCount = character->currentData()->isCommitted("SENSECONTROL1");
		threatCount = character->currentData()->isCommitted("MISDIRCONTROL3");
	}
	else if (strcmp(skill->key, "DEFR") == 0) {
		defr = true;
		pool = DatUtil::repeat("S", character->defenseRanged());
		commitCount = character->currentData()->isCommitted("SENSECONTROL1");
		threatCount = character->currentData()->isCommitted("MISDIRCONTROL3");
	}
	else {
		pool = DatUtil::getBasicDicePool(ranks, character->getAttribute(skill ? ch : 0));
		if (skill->type == COMBAT)
			commitCount = character->currentData()->isCommitted("SENSECONTROL3");
	}

	if (strcmp(skill->key, "REC") == 0) {
		if (character->talents.contains("BAL"))
			forceCount++;
	}
	else if (strcmp(skill->key, "ICOOL") == 0 || strcmp(skill->key, "IVIG") == 0) {
		if (character->talents.contains("FORSEECONTROL1") > 0)
			forceCount++;
	}

	for (int i = 0; i < character->talents.size(); i++) {
		CharTalent& ctalent = character->talents.at(i);
		Talent talent = AllTalents::instance()->getTalent(ctalent.key);

		foreach (DieModifier mod, talent.dieModifiers) {
			if (mod.skillKey == key) {
				if (talent.key == "SLEIGHTMIND")
					optionalBoost += mod.boostCount * ctalent.ranks;
				else
					boostCount += mod.boostCount * ctalent.ranks;
				if (talent.key == "KEENEYED")
					optionalRemoveSetback += mod.setbackCount * ctalent.ranks;
				else
					setbackCount += mod.setbackCount * ctalent.ranks;
				forceCount += mod.forceCount * ctalent.ranks;
			}
		}

		if (ctalent.key == "INTIM" && key == "COERC")
			optionalUpgradeCount += ctalent.ranks;
		if (ctalent.key == "CONGENIAL" &&
			(key == "CHARM" || key == "NEG"))
			optionalUpgradeCount += ctalent.ranks;
		if (ctalent.key == "DODGE" &&
			(key == "DEFM" || key == "DEFR"))
			optionalDowngradeCount += ctalent.ranks;
	}

	for (int i = 0; i<Armor::instance.rowCount(); i++) {
		Item item = Armor::instance.itemAt(i);
		if (item.equipped()) {
			if (item.dieModList.contains(key)) {
				DieMod mod = item.dieModList.get(key);
				pool += DatUtil::repeat("B", mod.boostCount);
				pool += DatUtil::repeat("N", mod.setbackCount);
				forceCount += mod.forceCount;
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

	pool += DatUtil::repeat("B", boostCount);
	pool += DatUtil::repeat("N", setbackCount);
	pool = DatUtil::normalizeDice(pool);

	if (defr || defm) {
		int r, m;

		character->currentData()->negetiveDefence(r, m);
		r = NEG_PURPLE_COUNT(r);
		m = NEG_PURPLE_COUNT(m);
		if (defr)
			pool = "|"+QString("D").repeated(r)+"|" + pool;
		if (defm)
			pool = "|"+QString("D").repeated(m)+"|" + pool;
	}

	if (forceCount + optionalRemoveSetback + optionalUpgradeCount + optionalDowngradeCount +
		commitCount + optionalBoost) {
		QString opt;
		// Mark the optional dice
		// Optional dice are just an indicator that something else
		// can be added to the pool
		opt += DatUtil::repeat("B", optionalBoost);
		if (forceCount > 0)
			opt += DatUtil::repeat("F", character->nonCommitedForce());
		opt += DatUtil::repeat("F", commitCount);
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
	clear(false);

	connect(&iCharacterDownloader, SIGNAL(downloaded(bool)), SLOT(characterDownloaded(bool)));
	connect(&iDataSetDownloader, SIGNAL(downloaded(bool)), SLOT(dataSetDownloaded(bool)));
	connect(&iSystemDataDownloader, SIGNAL(downloaded(bool)), SLOT(systemDataDownloaded(bool)));

	setLastSystemDataUpdate(DataAccess::getSystemDataLastUpdate());

	Character::instance = this;
}

void Character::clear(bool signal)
{
	iChMod.clear();
	iChDelta.clear();

	iName.clear();
	iPlayer.clear();
	iGender.clear();
	iAge.clear();
	iHeight.clear();
	iBuild.clear();
	iHair.clear();
	iEyes.clear();
	iFeatures.clear();
	iStory.clear();
	iSpecies.clear();
	iCareer.clear();
	iSpecializations.clear();
	iPortrait.clear();
	iCredits = 0;
	iLastInvLine.clear();
	iEncValue = 0;
	iEncThreshold = 0;
	iCumbValue = 0;
	iCumbThreshold = 0;
	iEncText.clear();

	iAttributes.clear();
	iActiveSkill.clear();
	iActiveSkillKey.clear();
	iDicePool.clear();
	iItemKey.clear();
	iItemName.clear();
	iItemSkill.clear();
	iItemDamage.clear();
	iItemCritical.clear();
	iItemQualities.clear();
	iItemAttachments.clear();
	iItemCritPlus = 0;
	iItemPierce = 0;
	iImageProviderCount = 0;
	iChangeDicePool.clear();
	iModDicePool.clear();
	iModItemDamage = 0;
	iModItemPierce = 0;
	iModItemCrit = 0;
	iModItemRange = 0;
	iCurrentData.clear();

	talents.clear();
	speciesTalents.clear();
	skills.clear();
	obligations.items.clear();
	duties.items.clear();
	Weapons::instance.clear();
	Armor::instance.clear();
	Gear::instance.clear();

	// These are not always set when reading XML, which
	// results in a missing signal!
	if (signal) {
		emit featuresChanged(iFeatures);
		emit storyChanged(iStory);

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
		emit totalXPChanged(totalXP());
		emit newXPChanged(newXP());
		emit usedXPChanged(usedXP());

		emit creditsChanged(credits());

		emit encValueChanged(encValue());
		emit encThresholdChanged(encThreshold());
		emit cumbValueChanged(cumbValue());
		emit cumbThresholdChanged(cumbThreshold());

		emit activeSkillChanged(activeSkill());
		emit dicePoolChanged(dicePool());
		emit negativePoolChanged(negativePool());
		emit itemKeyChanged(itemKey());
		emit itemNameChanged(itemName());
		emit itemRangeChanged(itemRange());
		emit itemSkillChanged(itemSkill());
		emit itemDamageChanged(itemDamage());
		emit itemCriticalChanged(itemCritical());
		emit itemQualitiesChanged(itemQualities());
		emit itemAttachmentsChanged(itemAttachments());
	}
}

void Character::init(void)
{
	DataAccess::serverAccessInfo(iHost, iEmail, iCurrentEmail, iPassword, iCurrentPassword);
	emit hostChanged(iHost);
	emit emailChanged(iEmail);
	emit passwordChanged(iPassword);
	setDataSet(DataAccess::selectedDataSet());
	setFile(DataAccess::selectedCharacter());
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

QString Character::host()
{
	if (iHost.isEmpty())
		return DEFAULT_HOST;
	return iHost;
}

QString Character::email()
{
	return iEmail;
}

QString Character::currentEmail()
{
	return iCurrentEmail;
}

QString Character::password()
{
	return iPassword;
}

QString Character::currentPassword()
{
	return iCurrentPassword;
}

bool Character::loading()
{
	return iLoading != 0;
}

bool Character::isIOS()
{
#if defined(Q_OS_IOS)
	return true;
#else
	return false;
#endif
}

QString Character::lastSystemDataUpdate()
{
	return iLastSystemDataUpdate;
}

QString Character::dataSet()
{
	return iDataSet;
}

int Character::characterCount()
{
	return DataAccess::characters.rowCount();
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
		DataList::motivation.setDataChanged();
		DataList::morality.setDataChanged();
		emit dataSetChanged(iDataSet);
	}
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
	return iCurrentData.negativePool();
}

QString Character::itemKey()
{
	return iItemKey;
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
	QString val = QString("%1 / 2").arg(Character::instance->currentData()->autoCheckItems.movesUsed());
	return val;
}

QString Character::itemStrain()
{
	QString cons;

	QString val = QString("%1 (%2 / %3)")
		.arg(Character::instance->currentData()->autoCheckItems.strainUsed(cons))
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

int Character::locked()
{
	return iCurrentData.locked;
}

int Character::hideAutoTalents()
{
	return iCurrentData.hideAutoTalents;
}

void Character::adjustWounds(int delta)
{
	iCurrentData.adjustWounds(delta);
	emit currentWoundsChanged(iCurrentData.wounds);
	emit woundHistoryChanged(iCurrentData.woundHistory);
}

void Character::adjustStrain(int delta)
{
	iCurrentData.adjustStrain(delta);
	emit currentStrainChanged(iCurrentData.strain+iCurrentData.temporaryStrain);
	emit strainHistoryChanged(iCurrentData.strainHistory);
}

void Character::adjustConflict(int delta)
{
	iCurrentData.adjustConflict(delta);
	emit currentConflictChanged(iCurrentData.conflict);
	emit conflictHistoryChanged(iCurrentData.conflictHistory);
}

void Character::useStimPack(int delta)
{
	iCurrentData.useStimPack(delta);
	emit stimPacksUsedChanged(iCurrentData.stimPacksUsed);
	emit currentWoundsChanged(iCurrentData.wounds);
	emit woundHistoryChanged(iCurrentData.woundHistory);
}

void Character::useErp(int delta)
{
	iCurrentData.useErp(delta);
	emit erpsUsedChanged(iCurrentData.erpsUsed);
	emit currentWoundsChanged(iCurrentData.wounds);
	emit woundHistoryChanged(iCurrentData.woundHistory);
}

void Character::setWoundDelta(int val)
{
	if (iCurrentData.setWoundDelta(val))
		emit woundDeltaChanged(val);
}

void Character::setStrainDelta(int val)
{
	if (iCurrentData.setStrainDelta(val))
		emit strainDeltaChanged(val);
}

void Character::addCriticalWound(int perc, int type)
{
	iCurrentData.addCriticalWound(perc, type);
}

void Character::removeCriticalWound(int ref)
{
	iCurrentData.removeCriticalWound(ref);
}

void Character::addExperience(const QString& type, const QString& key, const QString& name, const QString& desc, int amount)
{
	iCurrentData.addExperience(type, key, name, desc, amount);
}

void Character::changeExperience(int ref, const QString& desc, int amount)
{
	iCurrentData.changeExperience(ref, desc, amount);
}

void Character::removeExperience(int ref)
{
	iCurrentData.removeExperience(ref);
}

void Character::addCustomSkill(const QString& name, const QString& charac, int ranks)
{
	iCurrentData.addCustomSkill(name, charac, ranks, false);
}

void Character::removeCustomSkill(const QString& name)
{
	iCurrentData.removeCustomSkill(name);
}

void Character::addItem(int count, const QString& key, const QString& desc, int amount)
{
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
	iCurrentData.addItem(count, key, desc, amount);
}

void Character::updateItem(int ref, int count, const QString& desc, int amount)
{
	iCurrentData.updateItem(ref, count, desc, amount);
}

bool Character::removeItem(int ref)
{
	return iCurrentData.removeItem(ref);
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

void Character::addCheckItem(const QString& pool, const QString& desc)
{
	return iCurrentData.addCheckItem(iActiveSkillKey, pool, desc);
}

void Character::updateCheckItem(int ref, const QString& pool, const QString& desc)
{
	return iCurrentData.updateCheckItem(ref, iActiveSkillKey, pool, desc);
}

void Character::removeCheckItem(int ref)
{
	iCurrentData.removeCheckItem(ref, iActiveSkillKey);
}

void Character::checkItem(int ref)
{
	iCurrentData.checkItem(ref, iActiveSkillKey, false);
}

void Character::changeEquipment(const QString& key, int state, int stored)
{
	bool weapons_changed = Weapons::instance.changeEquipment(key, state, stored);
	bool armor_changed = Armor::instance.changeEquipment(key, state, stored);
	bool gear_changed = Gear::instance.changeEquipment(key, state, stored);

	inventoryChanged();

	if (weapons_changed || armor_changed) {
		Weapons::instance.setClean();
		Armor::instance.setClean();
	}

	if (gear_changed)
		Gear::instance.setClean();
}

void Character::showCharacteristics()
{
	DataList::injuries.setClean();
}

void Character::showDescription()
{
	ObligationList::instance.setClean();
	DutyList::instance.setClean();
	DataList::motivation.setClean();
	DataList::morality.setClean();
}

void Character::showSkills()
{
	GeneralSkills::instance.setClean();
	CombatSkills::instance.setClean();
	KnowledgeSkills::instance.setClean();
	SpecialSkills::instance.setClean();
	CustomSkills::instance.setClean();
}

void Character::showTalents()
{
	ExperienceList::instance.setClean();
	DataList::speciesFeatures.setClean();
	Talents::instance.setClean();
}

void Character::showWeaponsAndArmor()
{
	Weapons::instance.setClean();
	Armor::instance.setClean();
}

void Character::showGear()
{
	Gear::instance.setClean();
}

void Character::showInventory()
{
	InventoryLog::instance.setClean();
}

void Character::showCheckList()
{
	iModDicePool.clear();
	iCurrentData.clearAutoCheckItems(iActiveSkillKey);
	CheckList::instance.setRowCountChanged();
	CheckList::instance.setClean();
}

void Character::hideCheckList()
{
	iCurrentData.exitAutoCheckItems(iActiveSkillKey);
}

void Character::fillCheckList()
{
	iCurrentData.setupAutoCheckItems(iActiveSkillKey, iItemKey);
	CheckList::instance.setRowCountChanged();
	CheckList::instance.setClean();
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

void Character::setName(const QString& name)
{
	if (iName != name) {
		iName = name;
		emit nameChanged(name);
	}
}

void Character::setPlayer(const QString& player)
{
	if (iPlayer != player) {
		iPlayer = player;
		emit playerChanged(player);
	}
}

void Character::setGender(const QString& gender)
{
	if (iGender != gender) {
		iGender = gender;
		emit genderChanged(gender);
	}
}

void Character::setAge(const QString& age)
{
	if (iAge != age) {
		iAge = age;
		emit ageChanged(age);
	}
}

void Character::setHeight(const QString& height)
{
	if (iHeight != height) {
		iHeight = height;
		emit heightChanged(height);
	}
}

void Character::setBuild(const QString& build)
{
	if (iBuild != build) {
		iBuild = build;
		emit buildChanged(build);
	}
}

void Character::setHair(const QString& hair)
{
	if (iHair != hair) {
		iHair = hair;
		emit hairChanged(hair);
	}
}

void Character::setEyes(const QString& eyes)
{
	if (iEyes != eyes) {
		iEyes = eyes;
		emit eyesChanged(eyes);
	}
}

void Character::setFeatures(const QString& features)
{
	if (iFeatures != features) {
		iFeatures = features;
		emit featuresChanged(features);
	}
}

void Character::setStory(const QString& value)
{
	if (iStory != value) {
		iStory = value;
		emit storyChanged(value);
	}
}

void Character::setSpecies(const QString& species)
{
	if (iSpecies != species) {
		iSpecies = species;
		emit speciesChanged(species);
	}
}

void Character::setCareer(const QString& career)
{
	if (iCareer != career) {
		iCareer = career;
		emit careerChanged(career);
	}
}

void Character::setSpecializations(const QString& specializations)
{
	QString spec = specializations;

	if (spec.contains("Force Sensitive Exile") && spec.contains("Force Sensitive Emergent")) {
		spec = spec.replace("Force Sensitive Emergent", "Force Sensitive Emergent/Exile");
		spec = spec.replace(", Force Sensitive Exile", "");
		spec = spec.replace("Force Sensitive Exile", "");
	}
	if (iSpecializations != spec) {
		iSpecializations = spec;
		emit specializationsChanged(spec);
	}
}

void Character::setPortrait(const QString& portrait)
{
	if (iPortrait != portrait) {
		iPortrait = portrait;
		emit portraitChanged(portrait);
	}
}

void Character::setCredits(int c)
{
	if (iCredits != c) {
		iCredits = c;
		emit creditsChanged(c);
	}
}

void Character::setLastInvLine(const QString& v)
{
	if (iLastInvLine != v) {
		iLastInvLine = v;
		emit lastInvLineChanged(v);
	}
}

void Character::setEncValue(int c)
{
	if (iEncValue != c) {
		iEncValue = c;
		emit encValueChanged(c);
	}
}

void Character::setEncThreshold(int c)
{
	if (iEncThreshold != c) {
		iEncThreshold = c;
		emit encThresholdChanged(c);
	}
}

void Character::setCumbValue(int c)
{
	if (iCumbValue != c) {
		iCumbValue = c;
		emit cumbValueChanged(c);
	}
}

void Character::setCumbThreshold(int c)
{
	if (iCumbThreshold != c) {
		iCumbThreshold = c;
		emit cumbThresholdChanged(c);
	}
}

void Character::setEncText(const QString& t)
{
	if (iEncText != t) {
		iEncText = t;
		emit encTextChanged(t);
	}
}

void Character::setAttribute(QString ch, int val)
{
	if (!iAttributes.contains(ch) || iAttributes[ch] != val) {
		iAttributes[ch] = val;
		if (ch == BRAWN)
			emit brawnChanged(val);
		else if (ch == AGILITY)
			emit agilityChanged(val);
		else if (ch == INTELLECT)
			emit intellectChanged(val);
		else if (ch == CUNNING)
			emit cunningChanged(val);
		else if (ch == WILLPOWER)
			emit willpowerChanged(val);
		else if (ch == PRESENCE)
			emit presenceChanged(val);
		else if (ch == SOAK)
			emit soakChanged(val);
		else if (ch == WOUND)
			emit woundChanged(val);
		else if (ch == STRAIN)
			emit strainChanged(val);
		else if (ch == DRANGED)
			emit defenseRangedChanged(val);
		else if (ch == DMELEE)
			emit defenseMeleeChanged(val);
		else if (ch == FORCE)
			emit forceChanged(val);
		else if (ch == XP)
			emit totalXPChanged(val);
		else if (ch == NEWXP)
			emit newXPChanged(val);
		else if (ch == USEDXP)
			emit usedXPChanged(val);
		else if (ch == MORALITY) {
			emit moralityChanged(val);
			emit strainChanged(strain());
			emit woundChanged(wound());
		}
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

		if (skills.contains(iActiveSkillKey))
			char_skill = skills[iActiveSkillKey];
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
	iCurrentData.setNegativePool(t, iActiveSkillKey);
	emit negativePoolChanged(negativePool());
}

void Character::setItemKey(const QString& t)
{
	if (iItemKey != t) {
		iItemKey = t;
		emit itemKeyChanged(t);
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

void Character::setLocked(int t)
{
	if (iCurrentData.setLocked(t))
		emit lockedChanged(t);
}

void Character::setHideCodedTalents(int t)
{
	if (iCurrentData.setHideCodedTalents(t)) {
		emit hideCodedTalentsChanged(t);
		Talents::instance.rowCountChanged();
	}
}

int Character::setChMod(const CharMods& mods)
{
	int changed = iChMod.set(mods);
	if (changed & BR_BIT)
		emit brawnChanged(brawn());
	if (changed & AG_BIT)
		emit agilityChanged(agility() );
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

/*
void Character::setChDelta(const CharMods& mods)
{
	int changed = iChDelta.set(mods);
	if (changed & BR_BIT)
		emit brawnDeltaChanged(mods.get(V_BR));
	if (changed & AG_BIT)
		emit agilityDeltaChanged(mods.get(AG_BIT));
	if (changed & INT_BIT)
		emit intellectDeltaChanged(mods.get(INT_BIT));
	if (changed & CUN_BIT)
		emit cunningDeltaChanged(mods.get(CUN_BIT));
	if (changed & WIL_BIT)
		emit willpowerDeltaChanged(mods.get(WIL_BIT));
	if (changed & PR_BIT)
		emit presenceDeltaChanged(mods.get(PR_BIT));
}
*/

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
	int cumb = 0;
	CharMods mods;
	int burly = 0;
	int val;

	if (talents.contains("BURLY")) {
		CharTalent& ch_talent = talents.get("BURLY");
		Talent talent = AllTalents::instance()->getTalent("BURLY");

		burly = ch_talent.ranks * talent.burly;
	}

	if (Gear::instance.contains(ShopGear::grenBandKey)) {
		Item item = Gear::instance.getItem(ShopGear::grenBandKey);
		if (item.equipped())
			grenade_bandolier = true;
	}

	mods.clear();
	for (int i = 0; i < Weapons::instance.rowCount(); i++) {
		Item item = Weapons::instance.itemAt(i);
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
	for (int i = 0; i < Armor::instance.rowCount(); i++) {
		Item item = Armor::instance.itemAt(i);
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

	for (int i = 0; i < Gear::instance.rowCount(); i++) {
		Item item = Gear::instance.itemAt(i);
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
			}
			else {
				if (val == 0)
					light_items += quantity;
			}
		}
	}
	enc += (light_items + 9) / 10;

	setEncValue(enc);
	setCumbValue(cumb);

	// Add armor talents:
	for (int i = 0; i < talents.size(); i++) {
		CharTalent& char_talent = talents.at(i);
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
	for (int i = 0; i < talents.size(); i++) {
		CharTalent& char_talent = talents.at(i);
		Talent talent = AllTalents::instance()->getTalent(char_talent.key);

		if ((!talent.requirementWearingArmor || wearingArmor) &&
			(!talent.requirementSoakAtLeast || maxArmorSoak >= talent.requirementSoakAtLeast)) {
			if (talent.defenseRanged)
				mods.inc(V_DRANGED, talent.defenseRanged * (talent.ranked ? char_talent.ranks : 1));
			if (talent.defenseMelee)
				mods.inc(V_DMELEE, talent.defenseMelee * (talent.ranked ? char_talent.ranks : 1));
		}
	}

	setChMod(mods);
	characteristicsChanged();

	thr += brawn() + brawnDelta();
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
		if (amount > brawn() + brawnDelta())
			enc_string += "[BR]You also loose your free maneuver.";
	}

	if (cumb > brawn() + brawnDelta()) {
		int amount = cumb - (brawn() + brawnDelta());

		enc_string += "[BR] Your weapon is too cumbersome, add ";
		for (int i=0; i<amount; i++)
			enc_string += "[SE]";
		enc_string += " to combat checks.";
	}
	setEncText(enc_string);

	setCumbThreshold(brawn() + brawnDelta());
}

void Character::experienceChanged()
{
	emit totalXPChanged(totalXP());
	emit moralityChanged(morality());
	emit strainChanged(strain());
	emit woundChanged(wound());
}

void Character::characteristicsChanged()
{
	/*
	for (int i=0; i<DataList::weapons.rowCount(); i++) {
		Item item = weapons[DataList::weapons.getValueAsString(i, "key")];
		if (!item.key.isEmpty()) {
			DataList::weapons.setValue(i, "damage", item.damageTotal());
			DataList::weapons.setValue(i, "dicePool", item.dicePool());
		}
	}
	*/
	Weapons::instance.setDataChanged();
	GeneralSkills::instance.setDataChanged();
	CombatSkills::instance.setDataChanged();
	KnowledgeSkills::instance.setDataChanged();
	SpecialSkills::instance.setDataChanged();
}

void Character::loadCurrentData()
{
	iCurrentData.loadCurrentData(iName);
	emit currentWoundsChanged(iCurrentData.wounds);
	emit woundHistoryChanged(iCurrentData.woundHistory);
	emit currentStrainChanged(iCurrentData.strain+iCurrentData.temporaryStrain);
	emit strainHistoryChanged(iCurrentData.strainHistory);
	emit currentConflictChanged(iCurrentData.conflict);
	emit conflictHistoryChanged(iCurrentData.conflictHistory);
	emit woundDeltaChanged(iCurrentData.woundDelta);
	emit strainDeltaChanged(iCurrentData.strainDelta);
}

QString Character::getCurrentDataFile()
{
	return iCurrentData.getFile();
}

void Character::emitStimPacksChanged()
{
	emit stimPacksChanged(iCurrentData.stimPacks());
}

void Character::emitErpsChanged()
{
	emit erpsChanged(iCurrentData.erps());
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

int Character::getAttribute(const QString& val)
{
	if (!iAttributes.contains(val))
		return 0;
	return iAttributes[val] + iChMod.get(val);
}

CurrentData* Character::currentData()
{
	return &iCurrentData;
}

void Character::setTemporaryStrain(int value)
{
	iCurrentData.temporaryStrain = value;
	emit currentStrainChanged(iCurrentData.strain+iCurrentData.temporaryStrain);
}

int Character::nonCommitedForce()
{
	return force() - iCurrentData.commitCount();
}

void Character::reload()
{
	setLoading(true);
	QScopedPointer<CharacterXML> loader(new CharacterXML());
	QByteArray data = DataAccess::readFile(DatUtil::getCharacterFolder() + iFile);

	clear(true);

	//loader->parse(data);
	loader->readFromBuffer(data.constData(), data.length());
	if (data.isEmpty()) {
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

		emit brawnDeltaChanged(brawnDelta());
		emit agilityDeltaChanged(agilityDelta());
		emit intellectDeltaChanged(intellectDelta());
		emit cunningDeltaChanged(cunningDelta());
		emit willpowerDeltaChanged(willpowerDelta());
		emit presenceDeltaChanged(presenceDelta());

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
		emit lastInvLineChanged(lastInvLine());
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
		emit totalXPChanged(totalXP());
		emit newXPChanged(newXP());
		emit usedXPChanged(usedXP());
	}
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
