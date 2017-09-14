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

// Character -------------------------------------

Character* Character::instance;

Character::Character(QObject *parent) :
	QObject(parent)
{
	if (!CurrentData::instance)
		CurrentData::instance = new CurrentData();

	iLocked = 0;
	iHideCodedTalents = 0;

	iCurrentSkillKey.clear();
	iCurrentTalentKey.clear();
	iDicePool.clear();
	iItem.clear();

	iChangeDicePool.clear();
	//iModDicePool.clear();
	iChecklistDamage = 0;
	iChecklistPierce = 0;
	iChecklistCrit = 0;
	iChecklistCritRating = 0;
	iChecklistRange = 0;
	iChecklistMagnitude = 0;
	iChecklistStrength = 0;
	iChecklistDuration = 0;
	iChecklistExtra = 0;

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

QString Character::skillName()
{
	Skill* skill = Skill::getSkill(iCurrentSkillKey);
	if (skill)
		return skill->name;
	MethodID method_id = KeyMethod::instance.getID(iCurrentSkillKey);
	switch (method_id) {
		case KM_FORCEPOWER:
		case KM_FORCECOMMIT:
			return "FORCE";
		case KM_BR:
			return "Brawn";
		case KM_AG:
			return "Agility";
		case KM_INT:
			return "Intelect";
		case KM_CUN:
			return "Cunning";
		case KM_WIL:
			return "Willpower";
		case KM_PR:
			return "Presence";
		default:
			break;
	}
	return iCurrentSkillKey + "?";
}

QString Character::talentName()
{
	Talent talent = AllTalents::instance()->getTalent(iCurrentTalentKey);
	return talent.name();
}

QString Character::itemName()
{
	return iItem.name();
}

QString Character::dicePool()
{
	return DatUtil::normalizeDice(iDicePool + iChangeDicePool);
}

int Character::negativePool()
{
	return CurrentData::instance->negativePool();
}

void Character::getItemRange(int& range1, int& range2)
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
	range1 = RANGE_ENGAGED, range2 = RANGE_NA;

	// see QString Weapons::toRangeText(int range)
	switch (talent_id) {
		case KM_INFLUENCEBASIC:
		case KM_INFLUENCECONTROL1:
		case KM_SENSECONTROL2:
		case KM_BATMEDBASIC:
		case KM_BATMEDCONTROL1:
		case KM_BATMEDCONTROL2:
		case KM_BATMEDMASTERY:
		case KM_BATMEDDURATION:
		case KM_FORSEECONTROL1:
			range1 = RANGE_ENGAGED;
			break;
		case KM_WARFORBASIC:
		case KM_WARFORCONTROL2:
		case KM_BINDBASIC:
		case KM_BINDCONTROL1:
		case KM_BINDCONTROL2:
		case KM_BINDMASTERY:
		case KM_MISDIRBASIC:
		case KM_MISDIRCONTROL1:
		case KM_MISDIRCONTROL2:
		case KM_MISDIRMASTERY:
		case KM_MISDIRDURATION:
		case KM_MOVEBASIC:
		case KM_MOVECONTROL1:
		case KM_MOVECONTROL2:
		case KM_MOVECONTROL3:
		case KM_ENHANCECONT3:
		case KM_ENHANCECONT6: // Control: Force Leap (Horizontal)
		case KM_SUPPRESSBASIC:
		case KM_SUPPRESSDURATION:
		case KM_SUPPRESSCONTROL2:
		case KM_SUPPRESSCONTROL3:
		case KM_SUPPRESSMASTERY:
			range1 = RANGE_SHORT;
			break;
		case KM_FARSIGHTBASIC:
		case KM_FARSIGHTCONTROL1:
		case KM_FARSIGHTCONTROL2:
		case KM_FARSIGHTCONTROL4: // -- ongoing
		case KM_FARSIGHTCONTROL5:
		case KM_FARSIGHTCONTROL6:
		case KM_FARSIGHTMASTERY:
			range1 = RANGE_MEDIUM;
			break;
		case KM_ENHANCEBASIC:
		case KM_ENHANCECONT1:
		case KM_ENHANCECONT2:
		case KM_ENHANCECONT4:
		case KM_ENHANCECONT5:
		case KM_ENHANCECONT7:
		case KM_ENHANCECONT8:
		case KM_ENHANCECONT9:
			range1 = RANGE_NA; // Range n/a
			break;
		case KM_SEEKBASIC:
		case KM_SEEKSTRENGTH:
		case KM_SEEKDURATION:
		case KM_SEEKMASTERY:
			range1 = RANGE_UNLIMITED; // Range Unlimited
			break;
		case KM_SENSEBASIC:
			range1 = RANGE_SHORT;
			range2 = RANGE_ENGAGED;
			break;
		default: {
			if (iItem.uuid.isEmpty()) {
				// Skill range?!
				MethodID skill_id = KeyMethod::instance.getID(iCurrentSkillKey);
				switch (skill_id) {
					case KM_ICOOL:
					case KM_IVIG:
						range1 = RANGE_ENGAGED;
						break;
					default:
						range1 = RANGE_NA;
						break;
				}
			}
			else {
				// Weapon range:
				QString range = iItem.range();
				switch (range[0].unicode()) {
					case 'E':
						if (range[1].unicode() == 'x')
							range1 = RANGE_EXTREME;
						else
							range1 = RANGE_ENGAGED; // Engaged
						break;
					case 'S':
						range1 = RANGE_SHORT;
						break;
					case 'M':
						range1 = RANGE_MEDIUM;
						break;
					case 'L':
						range1 = RANGE_LONG;
						break;
				}
			}

			break;
		}
	}
}

// showChecklist() <---- Initialise power
QString Character::checkRange()
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
	int range1, range2;

	getItemRange(range1, range2); // Initial range
	if (range1 >= RANGE_ENGAGED) {
		range1 += iChecklistRange;
		if (range1 < RANGE_ENGAGED)
			range1 = RANGE_ENGAGED;
		else if (range1 > RANGE_EXTREME)
			range1 = RANGE_EXTREME;
	}
	if (range2 >= RANGE_ENGAGED) {
		range2 += iChecklistRange;
		if (range2 < RANGE_ENGAGED)
			range2 = RANGE_ENGAGED;
		else if (range2 > RANGE_EXTREME)
			range2 = RANGE_EXTREME;
	}

	bool planetary_scale = false;
	if ((talent_id == KM_BATMEDBASIC ||
		 talent_id == KM_BATMEDCONTROL1 ||
		 talent_id == KM_BATMEDCONTROL2 ||
		 talent_id == KM_BATMEDMASTERY ||
		 talent_id == KM_BATMEDDURATION) && ((iChecklistExtra) & 1) != 0)
		planetary_scale = true;

	return Weapons::toRangeText(range1, range2, planetary_scale);
}

QString Character::checkDamage()
{
	QString damage = iItem.damageTotal();
	int pierce = iItem.pierce();

	if (iChecklistDamage) {
		QString val = damage;
		QString rig;

		if (val.contains("[")) {
			val = DatUtil::left(val, "[");
			rig = DatUtil::right(val, "[");
		}

		if (val.contains("+")) {
			val = DatUtil::left(val, "+");
			rig = DatUtil::right(val, "+") + rig;
		}

		damage = QString("%1%2").arg(val.toInt() + iChecklistDamage).arg(rig);
	}

	if (pierce + iChecklistPierce)
		damage = damage + QString(" ➤ %1").arg(pierce + iChecklistPierce);

	return damage;
}

QString Character::checkCritLevel()
{
	int crit_tot = iItem.critTotal();
	if (crit_tot > 0) {
		crit_tot += iChecklistCritRating;
		if (crit_tot < 1)
			crit_tot = 1;
	}

	QString crit_dice = DatUtil::repeat("a", crit_tot);
	int crit_plus = iItem.critPlus();

	if (!crit_dice.isEmpty() && crit_plus > 0)
		return crit_dice + QString(" +%1%").arg(crit_plus*10+iChecklistCrit);
	return crit_dice;
}

QString Character::checkQualMag()
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);

	switch (talent_id) {
		case KM_ENHANCEBASIC:
		case KM_ENHANCECONT1:
		case KM_ENHANCECONT2:
		case KM_ENHANCECONT3:
		case KM_ENHANCECONT4:
		case KM_ENHANCECONT5:
		case KM_ENHANCECONT6: // Control: Force Leap (Horizontal)
		case KM_ENHANCECONT7:
		case KM_ENHANCECONT8:
		case KM_ENHANCECONT9:
			return "n/a";
		case KM_WARFORBASIC:
		case KM_WARFORCONTROL2:
		case KM_BATMEDBASIC:
		case KM_BATMEDCONTROL1:
		case KM_BATMEDCONTROL2:
		case KM_BATMEDMASTERY:
		case KM_BATMEDDURATION:
		case KM_BINDBASIC:
		case KM_BINDCONTROL1:
		case KM_BINDCONTROL2:
		case KM_BINDMASTERY:
		case KM_INFLUENCEBASIC:
		case KM_INFLUENCECONTROL1:
		case KM_MISDIRBASIC:
		case KM_MISDIRCONTROL1:
		case KM_MISDIRCONTROL2:
		case KM_MISDIRMASTERY:
		case KM_MISDIRDURATION:
		case KM_SENSEBASIC:
		case KM_SENSECONTROL2:
		case KM_MOVEBASIC:
		case KM_MOVECONTROL1:
		case KM_MOVECONTROL2:
		case KM_MOVECONTROL3:
		case KM_FORSEECONTROL1:
			if (iChecklistMagnitude+1 == 1)
				return "1 Target";
			return QString("%1 Targets").arg(iChecklistMagnitude+1);
		case KM_SEEKBASIC:
		case KM_SEEKSTRENGTH:
		case KM_SEEKDURATION:
		case KM_SEEKMASTERY:
		{
			QString val;

			if (iChecklistDuration == 0)
				val = "1 target";
			else
				val = QString("%1 targets").arg(iChecklistDuration+1);
			if (iChecklistMagnitude == 0)
				val += ", - Details";
			else if (iChecklistMagnitude == 1)
				val += ", 1 Detail";
			else
				val += QString(", %1 Details").arg(iChecklistMagnitude);
			return val;
		}
		case KM_SUPPRESSBASIC:
		case KM_SUPPRESSDURATION:
		case KM_SUPPRESSCONTROL2:
		case KM_SUPPRESSCONTROL3:
		case KM_SUPPRESSMASTERY:
			if ((iChecklistExtra & 1) != 0)
				return "1 Force User";
			return "All Allies";
			break;
		default:
			break;
	}	
	return "";
}

QString Character::checkPowerStr()
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
	int silhouette = 0;

	switch (talent_id) {
		case KM_BINDBASIC:
		case KM_BINDCONTROL1:
		case KM_BINDCONTROL2:
		case KM_BINDMASTERY:
			if (iChecklistStrength == 0)
				return "-";
			return QString("Disorient target(s) for %1 rounds").arg(iChecklistStrength+1);
		case KM_INFLUENCEBASIC:
			if (CurrentData::instance->talents.contains("INFLUENCESTRENGTH"))
				return QString("Inflict 2 strain");
			return "Inflict 1 strain";
		case KM_MISDIRBASIC:
		case KM_MISDIRCONTROL1:
		case KM_MISDIRCONTROL2:
		case KM_MISDIRMASTERY:
		case KM_MISDIRDURATION:
			silhouette = 1;
			// No break!
		case KM_MOVEBASIC:
		case KM_MOVECONTROL1:
		case KM_MOVECONTROL2:
		case KM_MOVECONTROL3:
			return QString("Effect silhouette %1 or smaller").arg(silhouette+iChecklistStrength > 9 ? 9 : silhouette+iChecklistStrength);
		case KM_SEEKBASIC:
		case KM_SEEKSTRENGTH:
		case KM_SEEKDURATION:
		case KM_SEEKMASTERY:
			if (iChecklistRange > 0)
				return "Add [TR] to combat checks vs target";
			if (iChecklistStrength == 0)
				return "Eliminate 0 Force-based illusions";
			if (iChecklistStrength == 1)
				return "Eliminate 1 Force-based illusion";
			return QString("Eliminate %1 Force-based illusions").arg(iChecklistStrength);
		case KM_BATMEDBASIC:
		case KM_BATMEDCONTROL1:
		case KM_BATMEDCONTROL2:
		case KM_BATMEDMASTERY:
		case KM_BATMEDDURATION:
		{
			QString stars = QString("[SU]").repeated(iChecklistStrength+1);
			if (((iChecklistExtra) & 2) != 0)
				return QString("Add %1 to all checks and boost 1 Skill").arg(stars);
			else if (((iChecklistExtra) & 4) != 0)
				return QString("Add %1 to all checks and force orders").arg(stars);
			return QString("Add %1 to all checks made by targets").arg(stars);
		}
		case KM_WARFORBASIC:
		case KM_WARFORCONTROL2:
			if (iChecklistStrength > 0)
				return QString("Add %1 to next check against target").arg(QString("[BO]").repeated(iChecklistStrength));
			break;
		case KM_SUPPRESSBASIC:
		case KM_SUPPRESSDURATION:
		case KM_SUPPRESSCONTROL2:
		case KM_SUPPRESSCONTROL3:
		case KM_SUPPRESSMASTERY:
			return QString("Add %1 to incoming Force attacks").arg(QString("[FA]").repeated(iChecklistStrength+1));
		case KM_FORSEEBASIC:
			if (iChecklistStrength == 0)
				return "No specific details";
			return CurrentData::plurize("Pick out %1 specific detail", iChecklistStrength);
		default:
			break;
	}
	return "";
}

QString Character::checkDuration()
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
	switch (talent_id) {
		case KM_BINDBASIC:
		case KM_BINDCONTROL1:
		case KM_BINDCONTROL2:
		case KM_BINDMASTERY:
		case KM_BATMEDBASIC:
		case KM_BATMEDCONTROL1:
		case KM_BATMEDCONTROL2:
		case KM_BATMEDMASTERY:
		case KM_BATMEDDURATION:
		case KM_SUPPRESSBASIC:
		case KM_SUPPRESSDURATION:
		case KM_SUPPRESSCONTROL2:
		case KM_SUPPRESSCONTROL3:
		case KM_SUPPRESSMASTERY:
			if (iChecklistDuration == 0)
				return "End of user's next turn";
			return QString("Sustained until %1 uncommitted").arg(QString("[FO]").repeated(iChecklistDuration));
		case KM_INFLUENCECONTROL1:
			if (iChecklistDuration == 0)
				return "1 round or 5 minutes";
			return QString("%1 rounds or %2 minutes").arg(iChecklistDuration+1).arg(iChecklistDuration+5);
		case KM_MISDIRBASIC:
		case KM_MISDIRCONTROL1:
		case KM_MISDIRCONTROL2:
		case KM_MISDIRMASTERY:
		case KM_MISDIRDURATION:
			if (iChecklistDuration == 0)
				return "Beginning of user's next turn";
			return "Sustained until [FO][FO] uncommitted";
		case KM_WARFORBASIC:
		case KM_WARFORCONTROL2:
			if (iChecklistDuration > 0)
				return CurrentData::plurize("Bonuses apply to all checks for %1 round", iChecklistDuration);
			break;
		case KM_SEEKBASIC:
		case KM_SEEKSTRENGTH:
		case KM_SEEKDURATION:
		case KM_SEEKMASTERY:
			if (iChecklistDuration == 0)
				return "";
			return QString("Track moving target until %1 uncommitted").arg(QString("[FO]").repeated(iChecklistDuration));
		case KM_FARSIGHTBASIC:
		case KM_FARSIGHTCONTROL1:
		case KM_FARSIGHTCONTROL2:
		case KM_FARSIGHTCONTROL4: // -- ongoing
		case KM_FARSIGHTCONTROL5:
		case KM_FARSIGHTCONTROL6:
		case KM_FARSIGHTMASTERY: {
			int cc = CurrentData::instance->getCommitCount("FARSIGHTCONTROL4");
			if (cc > 0)
				return QString("Sustained until %1 uncommitted").arg(QString("[FO]").repeated(cc));
			if (iChecklistDuration == 0)
				return "1 round or 1 minute";
			return QString("%1 rounds or %2 minutes").arg(iChecklistDuration+1).arg(iChecklistDuration+1);
		}
		case KM_FORSEEBASIC:
			if (iChecklistDuration == 0)
				return "1 Day Ahead";
			return QString("%1 Days Ahead").arg(iChecklistDuration+1);
		default:
			break;
	}
	return "";
}

QString Character::checkAttachDesc()
{
	if (!iCurrentTalentKey.isEmpty()) {
		MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
		Talent talent = AllTalents::instance()->getTalent(iCurrentTalentKey);
		QString desc = talent.description;
		QString desc2;

		switch (talent_id) {
			case KM_SUPPRESSBASIC:
			case KM_SUPPRESSDURATION:
			case KM_SUPPRESSCONTROL2:
			case KM_SUPPRESSMASTERY:
				if (CurrentData::instance->talents.contains("SUPPRESSCONTROL3")) {
					CharSkill char_skill = CurrentData::instance->getCharSkill(KM_DISC);
					desc2 = QString("[B]Control:[b] Attacker suffers %1 strain if they use [DA] to generate [FP].").arg(char_skill.skillRanks());
				}
				break;
			case KM_SUPPRESSCONTROL3:
			default:
				break;
		}

		DatUtil::addDescription(desc, desc2, talent.books);
		return desc;
	}
	return iItem.attachments;
}

QString Character::checkManeuvers()
{
	MethodID talent_id = KeyMethod::instance.getID(iCurrentTalentKey);
	int add_man = 0;

	switch (talent_id) {
		case KM_ENHANCECONT0:
		case KM_ENHANCECONT6:
		case KM_ENHANCECONT3:
			if (CurrentData::instance->talents.contains("ENHANCECONT0"))
				add_man = 1;
			break;
		default:
			break;
	}
	QString val = QString("%1|/ 2").arg(add_man + CurrentData::instance->checklistItems.movesUsed());
	return val;
}

QString Character::checkStrain()
{
	QString cons;

	QString val = QString("%1|(%2 / %3)")
		.arg(CurrentData::instance->checklistItems.strainUsed(cons))
		.arg(currentStrain())
		.arg(strain());
	return val;
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
	return Shop::instance.getItem(key).price;
}

int Character::getRarity(const QString& key)
{
	return Shop::instance.getItem(key).rarity;
}

void Character::searchShop(QString search_string)
{
	Shop::instance.search(search_string);
}

void Character::appendCheckItem(const QString& pool, const QString& desc)
{
	return CurrentData::instance->appendCheckItem(this, pool, desc);
}

void Character::updateCheckItem(int ref, const QString& pool, const QString& desc)
{
	return CurrentData::instance->updateCheckItem(this, ref, pool, desc);
}

void Character::removeCheckItem(int ref)
{
	CurrentData::instance->removeCheckItem(this, ref);
}

void Character::checkItem(int ref)
{
	CurrentData::instance->checkItem(this, ref, false);
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

QString Character::showChecklist(QString skill_key, QString talent_key, QString uuid)
{
	QString check_list_type;
	QString pool_plus;
	QString dice_pool;

	iChangeDicePool.clear();
	iChecklistDamage = 0;
	iChecklistPierce = 0;
	iChecklistCrit = 0;
	iChecklistCritRating = 0;
	iChecklistRange = 0;
	iChecklistMagnitude = 0;
	iChecklistStrength = 0;
	iChecklistDuration = 0;
	iChecklistExtra = 0;

	if (!talent_key.isEmpty()) {
		Talent talent = AllTalents::instance()->getTalent(talent_key);
		MethodID talent_id = KeyMethod::instance.getID(talent_key);

		if (talent.force) {
			// FORCE POWERS:
			if (talent_key.endsWith("STRENGTH") ||
				talent_key.endsWith("DURATION") ||
				talent_key.endsWith("RANGE") ||
				talent_key.endsWith("MAGNITUDE"))
				check_list_type = "";
			else
				check_list_type = "force";

			switch (talent_id) {
				case KM_ENHANCEBASIC:
					skill_key = "ATHL";
					break;
				case KM_ENHANCECONT1:
					skill_key = "COORD";
					break;
				case KM_ENHANCECONT2:
					skill_key = "RESIL";
					break;
				case KM_ENHANCECONT4:
					skill_key = "PILOTPL";
					break;
				case KM_ENHANCECONT5:
					skill_key = "BRAWL";
					break;
				case KM_ENHANCECONT7:
					skill_key = "PILOTSP";
					break;
				case KM_ENHANCECONT8:
					skill_key = "BR";
					break;
				case KM_ENHANCECONT9:
					skill_key = "AG";
					break;
				case KM_MOVECONTROL1:
				case KM_INFLUENCECONTROL1:
					skill_key = "DISC";
					pool_plus += getForcePool() + ".";
					break;
				case KM_SEEKCONTROL1:
				case KM_SEEKCONTROL3:
				case KM_SENSECONTROL1:
				case KM_SUPPRESSCONTROL1:
					skill_key = "FORCECOMMIT";
					break;
				case KM_WARFORCONTROL1:
				case KM_WARFORCONTROL3:
				case KM_WARFORCONTROL4:
				case KM_WARFORMAGNITUDE:
				case KM_FORSEECONTROL1:
				case KM_FORSEECONTROL2:
				case KM_FORSEECONTROL3:
					skill_key = "ICOOL";
					break;
				case KM_SEEKCONTROL2:
				case KM_ENHANCECONT0:
				case KM_FARSIGHTCONTROL3:
					check_list_type = "";
					break;
				// STRENGTH, DURATION, etc, exceptions to the non-clickable rule:
				case KM_SEEKSTRENGTH:
				case KM_SEEKDURATION:
				case KM_BATMEDDURATION:
				case KM_SUPPRESSDURATION:
					// no break;
					check_list_type = "force";
				default:
					skill_key = "FORCEPOWER";
					dice_pool = getForcePool() + ".";
					break;
			}
		}
		else {
			// TALENTS:
			check_list_type = "talent";

			switch (talent_id) {
				case KM_BRI:
					skill_key = "PILOTSP";
					break;
				case KM_DONTSHOOT:
					skill_key = "CHARM";
					pool_plus = "DDD";
					break;
				case KM_FEARSOME: {
					int ranks = CurrentData::instance->talents.get("FEARSOME").ranks;
					pool_plus = QString("D").repeated(ranks);
					break;
				}
				case KM_MASSHAD:
					skill_key = DatUtil::betterThan(KM_STEAL, KM_SKUL, this) > 0 ? "STEAL" : "SKUL";
					break;
				case KM_BYP:
					skill_key = DatUtil::betterThan(KM_COMP, KM_SKUL, this) > 0 ? "COMP" : "SKUL";
					break;
				case KM_COD:
				case KM_TECHAPT:
					skill_key = "COMP";
					break;
				case KM_REFLECT:
				case KM_SIDESTEP:
					skill_key = "DEFR";
					break;
				case KM_CONF:
					skill_key = "FDISC";
					break;
				case KM_DEFTRAIN:
				case KM_PARRY:
				case KM_PARRYIMP:
				case KM_UNARMPARRY:
				case KM_DEFSTA:
					skill_key = "DEFM";
					break;
				case KM_EXHPORT:
					skill_key = "GUNN";
					break;
				case KM_INTIM:
					skill_key = "COERC";
					break;
				case KM_INTUITEVA:
					skill_key = "FORCECOMMIT";
					break;
				case KM_IRONBODY:
					skill_key = "BR";
					uuid = "UNARMED";
					check_list_type = "weapon";
					break;
				case KM_NOWYOUSEE:
					skill_key = "DECEP";
					break;
				case KM_EXTRACK:
				case KM_FORAG:
					skill_key = "SURV";
					break;
				case KM_RAPREA:
					skill_key = "ICOOL";
					break;
				case KM_BRA:
					skill_key = "RESIL";
					break;
				case KM_FERSTR:
				case KM_FRENZ:
				case KM_NATBRAW:
					skill_key = "BRAWL";
					break;
				case KM_HARDHD:
				case KM_HARDHDIMP:
					skill_key = "DISC";
					break;
				case KM_INTERJECT:
					skill_key = "VIGIL";
					break;
				default:
					if (skill_key.isEmpty()) {
						foreach (DieMod mod, talent.dieModList.modMap) {
							if (skill_key.isEmpty())
								skill_key = mod.skillKey;
							else if (DatUtil::betterThan(mod.skillKey, skill_key, this) > 0)
								skill_key = mod.skillKey;
							break;
						}
					}
					if (skill_key.isEmpty())
						check_list_type = "";
					break;
			}
		}

	}

	if (!skill_key.isEmpty()) {
		MethodID skill_id = KeyMethod::instance.getID(skill_key);
		switch (skill_id) {
			case KM_FORCECOMMIT:
				dice_pool = getForcePool();
				break;
			case KM_BR:
				dice_pool = QString("A").repeated(brawn());
				break;
			case KM_AG:
				dice_pool = QString("A").repeated(agility());
				break;
			case KM_INT:
				dice_pool = QString("A").repeated(intellect());
				break;
			case KM_CUN:
				dice_pool = QString("A").repeated(cunning());
				break;
			case KM_WIL:
				dice_pool = QString("A").repeated(willpower());
				break;
			case KM_PR:
				dice_pool = QString("A").repeated(presence());
				break;
			default:
				if (dice_pool.isEmpty()) {
					CharSkill char_skill = CurrentData::instance->getCharSkill(skill_id);
					dice_pool = char_skill.getDicePool();
				}
				break;
		}
	}

	setupItem(uuid);

	if (!uuid.isEmpty()) {
		if (skill_key.isEmpty())
			skill_key = iItem.shopItem().skillKey;
		if (dice_pool.isEmpty())
			dice_pool = iItem.dicePool();
	}

	setDicePool(dice_pool + pool_plus);

	iCurrentSkillKey = skill_key;
	iCurrentTalentKey = talent_key;
	emit skillNameChanged(skillName());
	emit talentNameChanged(talentName());
	emit checkAttachDescChanged(checkAttachDesc());

	CurrentData::instance->clearChecklist(this);
	CheckList::instance.setRowCountChanged();
	CheckList::instance.makeClean();
	return check_list_type;
}

void Character::hideCheckList()
{
	CurrentData::instance->exitChecklist(this);
}

void Character::fillCheckList(const QString& check_list_type)
{
	CurrentData::instance->setChecklist(this, iCurrentSkillKey, iCurrentTalentKey, iItem);
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

void Character::setDicePool(const QString& t)
{
	QString pool = t;

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
	CurrentData::instance->setNegativePool(this, t, iCurrentSkillKey);
	emit negativePoolChanged(negativePool());
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

void Character::setChangeDicePool(const QString& dice_pool, ChecklistItem* item, bool list_setup)
{
	iChangeDicePool = dice_pool;

	if (item) {
		if (item->checked) {
			iChecklistDamage += item->plusDamage;
			iChecklistPierce += item->plusPierce;
			iChecklistCrit += item->plusCrit;
			iChecklistCritRating += item->plusCritRating;
			iChecklistRange += item->plusRange;
			iChecklistMagnitude += item->plusMagnitude;
			iChecklistStrength += item->plusStrength;
			iChecklistDuration += item->plusDuration;
			iChecklistExtra += item->plusExtra;
		}
		else if (!list_setup) {
			iChecklistDamage -= item->plusDamage;
			iChecklistPierce -= item->plusPierce;
			iChecklistCrit -= item->plusCrit;
			iChecklistCritRating -= item->plusCritRating;
			iChecklistRange -= item->plusRange;
			iChecklistMagnitude -= item->plusMagnitude;
			iChecklistStrength -= item->plusStrength;
			iChecklistDuration -= item->plusDuration;
			iChecklistExtra -= item->plusExtra;
		}
	}

	emit dicePoolChanged(dicePool());
	emit negativePoolChanged(negativePool());

	emit checkManeuversChanged(checkManeuvers());
	emit checkStrainChanged(checkStrain());

	emit checkDamageChanged(checkDamage());
	emit checkCritLevelChanged(checkCritLevel());
	emit checkRangeChanged(checkRange());
	emit checkQualMagChanged(checkQualMag());
	emit checkPowerStrChanged(checkPowerStr());
	emit checkDurationChanged(checkDuration());
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
	CharTalent& char_talent = current_data->talents.get("BURLY");

	burly = char_talent.ranks;

	if (current_data->gear.equipped(ShopGear::grenBandKey))
		grenade_bandolier = true;

	int mdef_ranks = CurrentData::instance->talents.ranks("DEFTRAIN");

	mods.clear();
	for (int i = 0; i < current_data->weapons.rowCount(); i++) {
		Item item = current_data->weapons.itemAt(i);
		quantity = item.carriedQuantity();
		if (quantity > 0) {
			val = item.encCarriedVal(burly);
			enc += val * quantity;
			cumb += item.cumbersome(burly) * quantity;

			if (item.equipped()) {
				item.characteristicDelta(mods);
				if (mdef_ranks > 0) {
					const ShopItem shop = item.shopItem();
					if (!item.isUnarmed() && shop.isMeleeWeapon())
						mods.inc(V_DMELEE, mdef_ranks);
				}
			}

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

QString Character::getCurrentSkillKey()
{
	return iCurrentSkillKey;
}

QString Character::getCurrentTalentKey()
{
	return iCurrentTalentKey;
}

QString Character::getForcePool(bool total_force)
{
	int f = force();
	int u = forceCommitted();

	if (total_force)
		return QString("F").repeated(f);
	if (f == u)
		return "";
	if (f > u)
		return QString("F").repeated(f-u);
	return QString("g").repeated(u-f);
}

void Character::adjustPoolForCommittedForce(int commit_count)
{
	iDicePool = QString("F").repeated(commit_count) + iDicePool;
	iChangeDicePool += QString("g").repeated(commit_count);
	emit dicePoolChanged(dicePool());
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
	emit dicePoolChanged(dicePool());
	emit negativePoolChanged(negativePool());
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

void Character::setupItem(const QString& uuid)
{
	if (iItem.uuid != uuid) {
		iItem = CurrentData::instance->weapons.getItemByUuid(uuid);

		emit itemNameChanged(itemName());
		emit checkRangeChanged(checkRange());
		emit checkDamageChanged(checkDamage());
		emit checkCritLevelChanged(checkCritLevel());
		emit checkQualMagChanged(checkQualMag());
		emit checkAttachDescChanged(checkAttachDesc());
		emit checkManeuversChanged(checkManeuvers());
		emit checkStrainChanged(checkStrain());
	}
}

