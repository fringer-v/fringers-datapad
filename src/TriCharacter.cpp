#include <QEventLoop>
#include <QDir>

#include "Character.h"
#include "TriUtil.h"
#include "DataAccess.h"
#include "CharacterXML.h"
#include "TriZip.h"
#include "ItemAttachments.h"
#include "ItemDescriptors.h"
#include "md5.h"

int Character::iLoading;

// CharSkill -------------------------------------

QString CharSkill::getDicePool(Character* character) {
	Skill* skill = TriUtil::getSkill(key);

	QString pool =  TriUtil::getBasicDicePool(ranks, character->getAttribute(skill->characteristic));

	int boostCount = 0;
	int setbackCount = 0;
	int forceCount = 0;
	QMap<QString, CharTalent>::iterator i;
	for (i = character->talents.begin(); i != character->talents.end(); i++) {
		CharTalent ctalent = i.value();
		Talent talent = Talents::instance()->getTalent(ctalent.key);

		foreach (DieModifier mod, talent.dieModifiers) {
			if (mod.skillKey == key) {
				boostCount += mod.boostCount * ctalent.ranks;
				setbackCount += mod.setbackCount * ctalent.ranks;
				forceCount += mod.forceCount * ctalent.ranks;
			}
		}
	}

	QMap<QString, Item>::iterator j;
	for (j = character->armor.begin(); j != character->armor.end(); j++) {
		Item item = j.value();
		if (item.equipped && item.dieModList.contains(key)) {
			DieMod mod = item.dieModList.get(key);
			pool += TriUtil::repeat("B", mod.boostCount);
			pool += TriUtil::repeat("N", mod.setbackCount);
			pool += TriUtil::repeat("Z", mod.forceCount);
			pool += TriUtil::repeat("a", mod.advantageCount);
			pool += TriUtil::repeat("t", mod.threadCount);
			pool += TriUtil::repeat("U", mod.upgradeCount);
		}
	}

	pool += TriUtil::repeat("B", boostCount);
	pool += TriUtil::repeat("N", setbackCount);
	pool += TriUtil::repeat("Z", forceCount);
	return TriUtil::normalizeDice(pool);
}

// Character -------------------------------------

Character::Character(QObject *parent) :
	QObject(parent)
{
	clear();

	connect(&iCharacterDownloader, SIGNAL(downloaded(bool)), SLOT(characterDownloaded(bool)));
	connect(&iDataSetDownloader, SIGNAL(downloaded(bool)), SLOT(dataSetDownloaded(bool)));
	connect(&iSystemDataDownloader, SIGNAL(downloaded(bool)), SLOT(systemDataDownloaded(bool)));

	setLastSystemDataUpdate(DataAccess::getSystemDataLastUpdate());
}

void Character::clear()
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
	iMorality = 0;

	iAttributes.clear();
	iDicePool.clear();
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
	iModItemCrit = 0;
	iCurrentData.clear();

	talents.clear();
	skills.clear();
	weapons.clear();
	armor.clear();
	gear.clear();
	shopItems.clear();
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
	QString error = iSyncer.sync(this);
	if (!error.isEmpty())
		emit alert("Sync with Server Failed", error);

	if (iSyncer.currentChanged()) {
		iDataSet = "x";
		iFile = "x";
		setDataSet(DataAccess::selectedDataSet());
		setFile(DataAccess::selectedCharacter());
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
	TriUtil::yield();
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
		setLoading(true);
		iDataSet = data_set;
		if (iDataSet.isEmpty())
			iDataSet = "";
		Talents::instance()->loadTalents(iDataSet);
		ItemAttachments::instance()->loadItemAttachments(iDataSet);
		ItemDescriptors::instance()->loadItemDescriptors(iDataSet);
		if (!iFile.isNull())
			reload();
		setLoading(false);
		DataAccess::setSelectedDataSet(iDataSet);
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
	QString file = TriUtil::lastNameOfPath(url);
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
		iDataSetDownloader.getData(), TriUtil::getDataSetFolder(), err_msg);

	if (dest.isEmpty()) {
		setLoading(false);
		emit alert("Unpack DataSet Failed", err_msg);
		return;
	}

	DataAccess::addDataSet(TriUtil::left(dest, "/"), TriUtil::right(dest, "/"));
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
		iSystemDataDownloader.getData(), TriUtil::getSystemDataFolder(), err_msg);

	if (dest.isEmpty()) {
		setLoading(false);
		emit alert("Unpack DataSet Failed", err_msg);
		return;
	}

	if (dest != "Data") {
		emit alert("Download System Data Failed", QString("Unpack DataSet Failed, the archived folder must be called \"Data\", not \"%1\"").arg(dest));
		DataAccess::removeDir(TriUtil::getSystemDataFolder() + dest);
		return;
	}

	setLastSystemDataUpdate(DataAccess::getSystemDataLastUpdate());
	reload();
	setLoading(false);
}

QString Character::dicePool()
{
	return TriUtil::normalizeDice(iDicePool + iModDicePool);
}

int Character::negativePool()
{
	return iCurrentData.negativePool();
}

QString Character::itemName()
{
	return iItemName;
}

QString Character::itemRange()
{
	if (iModItemRange) {
		int range;

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
			val = TriUtil::left(val, "[");
			rig = TriUtil::right(val, "[");
		}

		if (val.contains("+")) {
			val = TriUtil::left(val, "+");
			rig = TriUtil::right(val, "+") + rig;
		}

		damage = QString("%1%2").arg(val.toInt() + iModItemDamage).arg(rig);
	}

	if (iItemPierce)
		damage = damage + QString(" ➤ %1").arg(iItemPierce);

	return damage;
}

QString Character::itemCritical()
{
	QString crit_dice = TriUtil::repeat("a", iItemCritical.toInt());

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

int Character::itemCritPlus()
{
	return iItemCritPlus;
}

int Character::itemPierce()
{
	return iItemPierce;
}

int Character::locked()
{
	return iCurrentData.locked;
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
	emit currentStrainChanged(iCurrentData.strain);
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

void Character::addXP(const QString& desc, int amount)
{
	iCurrentData.addXP(desc, amount);
}

void Character::removeXP(int ref)
{
	iCurrentData.removeXP(ref);
}

void Character::addCustomSkill(const QString& name, const QString& charac, int ranks)
{
	iCurrentData.addCustomSkill(name, charac, ranks);
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
	if (shopItems.contains(key))
		return shopItems[key].price;
	return 0;
}

void Character::addCheckItem(const QString& pool, const QString& desc)
{
	return iCurrentData.addCheckItem(pool, desc);
}

void Character::updateCheckItem(int ref, const QString& pool, const QString& desc)
{
	return iCurrentData.updateCheckItem(ref, pool, desc);
}

void Character::removeCheckItem(int ref)
{
	iCurrentData.removeCheckItem(ref);
}

void Character::checkItem(int ref)
{
	iCurrentData.checkItem(ref);
}

void Character::changeEquipment(const QString& key, bool equipped, bool held)
{
	if (weapons.contains(key)) {
		weapons[key].equipped = equipped;
		weapons[key].held = held;
		DataList::weapons.startChanges();
		for (int i=0; i<DataList::weapons.rowCount(); i++) {
			if (DataList::weapons.getValueAsString(i, "key") == key) {
				DataList::weapons.setValue(i, "equipped", equipped);
				DataList::weapons.setValue(i, "held", held);
				break;
			}
		}
		DataList::weapons.endChanges();
	}
	if (armor.contains(key)) {
		armor[key].equipped = equipped;
		armor[key].held = held;
		DataList::armor.startChanges();
		for (int i=0; i<DataList::armor.rowCount(); i++) {
			if (DataList::armor.getValueAsString(i, "key") == key) {
				DataList::armor.setValue(i, "equipped", equipped);
				DataList::armor.setValue(i, "held", held);
				DataList::armor.setValue(i, "encumberance", armor[key].encArmorValue());
				break;
			}
		}
		DataList::armor.endChanges();
	}
	if (gear.contains(key)) {
		gear[key].equipped = equipped;
		gear[key].held = held;
		DataList::gear.startChanges();
		for (int i=0; i<DataList::gear.rowCount(); i++) {
			if (DataList::gear.getValueAsString(i, "key") == key) {
				DataList::gear.setValue(i, "equipped", equipped);
				DataList::gear.setValue(i, "held", held);
				break;
			}
		}
		DataList::gear.endChanges();
	}
	inventoryChanged();
}

void Character::setFile(const QString& file)
{
	if (iFile.isNull() || iFile != file) {
		setLoading(true);
		iFile = file;
		if (iFile.isEmpty())
			iFile = "";
		if (!iDataSet.isNull())
			reload();
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
	if (iSpecializations != specializations) {
		iSpecializations = specializations;
		emit specializationsChanged(specializations);
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

void Character::setMorality(int c)
{
	if (iMorality != c) {
		iMorality = c;
		emit moralityChanged(c);
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
			emit defenceRangedChanged(val);
		else if (ch == DMELEE)
			emit defenceMeleeChanged(val);
		else if (ch == FORCE)
			emit forceChanged(val);
		else if (ch == XP)
			emit totalXPChanged(val);
		else if (ch == NEWXP)
			emit newXPChanged(val);
		else if (ch == USEDXP)
			emit usedXPChanged(val);
	}
}

void Character::setDicePool(const QString& t)
{
	if (iDicePool != t) {
		iDicePool = t;
		iCurrentData.checkItem(0);
		emit dicePoolChanged(dicePool());
	}
}

void Character::setNegativePool(int t)
{
	iCurrentData.setNegativePool(t);
	emit negativePoolChanged(negativePool());
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

void Character::setLocked(int t)
{
	if (iCurrentData.setLocked(t))
		emit lockedChanged(t);
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
		emit defenceRangedChanged(defenceRanged());
	if (changed & DMELEE_BIT)
		emit defenceMeleeChanged(defenceMelee());
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
		bool is_item;

		iChangeDicePool = dice;
		iModDicePool.clear();
		iModItemDamage = 0;
		iModItemCrit = 0;
		iModItemRange = 0;

		int i=0;
		while (i<dice.length()) {
			if (dice[i].isDigit())
				is_item = true;
			else
			if (((dice[i].unicode() == '-' || dice[i].unicode() == '+') &&
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
				else
					iModItemDamage += val;
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
}

void Character::addTalent(CharTalent talent)
{
	if (talents.contains(talent.key)) {
		talents[talent.key].ranks++;
		TriUtil::appendToList(talents[talent.key].aquisition, talent.aquisition, ", ");
	}
	else {
		talents[talent.key] = talent;
	}
}

void Character::inventoryChanged()
{
	QMap<QString, Item>::iterator i;
	int quantity;
	bool wearingArmor = false;
	int maxArmorSoak = 0;
	int thr = 5;
	int enc = 0;
	int cumb = 0;
	CharMods mods;
	int burly = 0;

	if (talents.contains("BURLY")) {
		CharTalent ch_talent = talents["BURLY"];
		Talent talent = Talents::instance()->getTalent("BURLY");

		burly = ch_talent.ranks * talent.burly;
	}

	mods.clear();
	for (i = weapons.begin(); i != weapons.end(); i++) {
		Item item = i.value();
		quantity = item.quantity;
		if (iCurrentData.inventory.contains(i.key()))
			quantity = iCurrentData.inventory[i.key()].count;
		if (quantity > 0) {
			enc += item.encCarriedVal(burly) * quantity;
			cumb += item.cumbersome(burly) * quantity;

			if (item.equipped)
				item.characteristicDelta(mods);
		}
	}

	maxArmorSoak = 0;
	for (i = armor.begin(); i != armor.end(); i++) {
		Item item = i.value();
		quantity = item.quantity;
		if (iCurrentData.inventory.contains(i.key()))
			quantity = iCurrentData.inventory[i.key()].count;
		if (quantity > 0) {
			enc += item.encWornValue();
			if (quantity > 1)
				enc += item.encCarriedVal(0) * (quantity-1);
			if (item.equipped) {
				wearingArmor = true;
				item.characteristicDelta(mods);
				if (maxArmorSoak < item.soakVal())
					maxArmorSoak = item.soakVal();
				mods.inc(V_SOAK, item.soakVal());
				mods.inc(V_DRANGED, item.rangeDef());
				mods.inc(V_DMELEE, item.meleeDef());
				if (item.qualityList.contains("ENCTADD"))
					thr += item.qualityList.get("ENCTADD").count;
			}
		}
	}

	for (i = gear.begin(); i != gear.end(); i++) {
		Item item = i.value();
		quantity = item.quantity;
		if (iCurrentData.inventory.contains(i.key()))
			quantity = iCurrentData.inventory[i.key()].count;
		if (quantity > 0) {
			enc += item.encCarriedVal(0) * quantity;
			if (item.equipped) {
				item.characteristicDelta(mods);
				if (item.qualityList.contains("ENCTADD"))
					thr += item.qualityList.get("ENCTADD").count;
				if (item.qualityList.contains("SOAKADD"))
					mods.inc(V_SOAK, item.qualityList.get("SOAKADD").count);
			}
		}
	}
	setEncValue(enc);
	setCumbValue(cumb);

	// Add armor talents:
	QMap<QString, CharTalent>::iterator j;
	for (j = talents.begin(); j != talents.end(); j++) {
		Talent talent = Talents::instance()->getTalent(j.key());

		if (talent.soakValue > 0) {
			if (!talent.requirementWearingArmor || wearingArmor) {
				int add_soak = talent.soakValue * (talent.ranked ? j.value().ranks : 1);
				mods.inc(V_SOAK, add_soak);
				maxArmorSoak += add_soak;
			}
		}
	}

	// Add defences (which may depend on armor!
	for (j = talents.begin(); j != talents.end(); j++) {
		Talent talent = Talents::instance()->getTalent(j.key());

		if ((!talent.requirementWearingArmor || wearingArmor) &&
			(!talent.requirementSoakAtLeast || maxArmorSoak >= talent.requirementSoakAtLeast)) {
			if (talent.defenseRanged)
				mods.inc(V_DRANGED, talent.defenseRanged * (talent.ranked ? j.value().ranks : 1));
			if (talent.defenseMelee)
				mods.inc(V_DMELEE, talent.defenseMelee * (talent.ranked ? j.value().ranks : 1));
		}
	}

	setChMod(mods);
	characteristicsChanged();

	thr += brawn() + brawnDelta();
	setEncThreshold(thr);

	QString enc_string;

	if (enc <= thr)
		enc_string = "You are unencumbered";
	else {
		int amount = enc - thr;

		enc_string = "You are encumbered, add ";
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

void Character::characteristicsChanged()
{
	DataList::weapons.startChanges();
	for (int i=0; i<DataList::weapons.rowCount(); i++) {
		Item item = weapons[DataList::weapons.getValueAsString(i, "key")];
		if (!item.key.isEmpty()) {
			DataList::weapons.setValue(i, "damage", item.damageTotal(this));
			DataList::weapons.setValue(i, "dicePool", item.dicePool(this));
		}
	}
	DataList::weapons.endChanges();
	skillsChanged(&DataList::generalSkills);
	skillsChanged(&DataList::combatSkills);
	skillsChanged(&DataList::knowledgeSkills);
}

void Character::skillsChanged(DataList* list)
{
	list->startChanges();
	for (int i=0; i<list->rowCount(); i++) {
		CharSkill skill = skills[list->getValueAsString(i, "key")];
		if (!skill.key.isEmpty())
			list->setValue(i, "dice", skill.getDicePool(this));
	}
	list->endChanges();
}

void Character::loadCurrentData()
{
	iCurrentData.loadCurrentData(this, iName);
	emit currentWoundsChanged(iCurrentData.wounds);
	emit woundHistoryChanged(iCurrentData.woundHistory);
	emit currentStrainChanged(iCurrentData.strain);
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

int Character::getAttribute(const QString& val)
{
	if (!iAttributes.contains(val))
		return 0;
	return iAttributes[val] + iChMod.get(val);
}

void Character::reload()
{
	setLoading(true);
	QScopedPointer<CharacterXML> loader(new CharacterXML(this));
	QByteArray data = DataAccess::readFile(TriUtil::getCharacterFolder() + iFile);

	clear();

	loader->parse(data);
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
		emit defenceRangedChanged(defenceRanged());
		emit defenceMeleeChanged(defenceMelee());
		emit forceChanged(force());
		emit totalXPChanged(totalXP());
		emit newXPChanged(newXP());
		emit usedXPChanged(usedXP());
	}
	setLoading(false);
}

QString Character::processDownload(const QString& url, const QByteArray& data, const QString& to_dir, QString& err_msg)
{
	QString file = TriUtil::lastNameOfPath(url);
	QString tmp_file = TriUtil::getTempFolder() + file;
	DataAccess::writeFile(tmp_file, data);

	QString unpack_dir = TriUtil::addDirChar(to_dir) + file;
	QDir dir;
	if (!dir.exists(unpack_dir) && !dir.mkdir(unpack_dir)) {
		err_msg = "Unabled to created directory: "+unpack_dir;
		return QString();
	}

	QString dest = TriZip::uncompress(tmp_file, unpack_dir, err_msg);

	DataAccess::deleteFile(tmp_file);
	return file + "/" + dest;
}
