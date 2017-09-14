#ifndef __Character_h__
#define __Character_h__

#include <QObject>
#include <QPixmap>

#include "Downloader.h"
#include "Motivations.h"
#include "Skills.h"
#include "DatUtil.h"
#include "Item.h"
#include "CurrentData.h"
#include "DataList.h"
#include "Syncer.h"
#include "Talents.h"
#include "Shop.h"
#include "DataAccess.h"

class Character;

//#define DEFAULT_HOST	"localhost:8080/starwars"
#define DEFAULT_HOST	"fringer.space/"

#define RANGE_NA		-1
#define RANGE_UNLIMITED	-2
#define RANGE_ENGAGED	0
#define RANGE_SHORT		1
#define RANGE_MEDIUM	2
#define RANGE_LONG		3
#define RANGE_EXTREME	4

class Character : public QObject
{
	Q_OBJECT

public:
	static Character* instance;

	explicit Character(QObject *parent = 0);

public:
	// Data management ------------------
	Q_INVOKABLE void init(void);

	Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
	Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
	Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
	Q_PROPERTY(int locked READ locked WRITE setLocked NOTIFY lockedChanged)
	Q_PROPERTY(int hideCodedTalents READ hideCodedTalents WRITE setHideCodedTalents NOTIFY hideCodedTalentsChanged)
	Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
	Q_PROPERTY(bool isIOS READ isIOS NOTIFY isIOSChanged)
	Q_PROPERTY(QString lastSystemDataUpdate
		READ lastSystemDataUpdate
		WRITE setLastSystemDataUpdate
		NOTIFY lastSystemDataUpdateChanged)
	Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
	Q_PROPERTY(QString dataSet READ dataSet WRITE setDataSet NOTIFY dataSetChanged)
	Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)

	QString host() {
		if (iHost.isEmpty())
			return DEFAULT_HOST;
		return iHost;
	}
	QString email() { return iEmail; }
	QString currentEmail() { return iCurrentEmail; }
	QString password() { return iPassword; }
	QString currentPassword() { return iCurrentPassword; }
	int locked() { return iLocked; }
	int hideCodedTalents() { return iHideCodedTalents; }
	bool loading() { return iLoading != 0; }
	bool isIOS() {
#if defined(Q_OS_IOS)
		return true;
#else
		return false;
#endif
	}
	QString lastSystemDataUpdate() { return iLastSystemDataUpdate; }
	QString file() { return iFile; }
	QString dataSet() { return iDataSet; }
	int characterCount() { return DataAccess::characters.rowCount(); }

	void setCurrentEmailAndPassword(const QString& v, const QString& p);

	Q_INVOKABLE void setHost(const QString& v);
	Q_INVOKABLE void setEmail(const QString& v);
	Q_INVOKABLE void setPassword(const QString& v);
	Q_INVOKABLE void setLocked(int t);
	Q_INVOKABLE void setHideCodedTalents(int t);

	Q_INVOKABLE void setLoading(bool v);
	Q_INVOKABLE void setLastSystemDataUpdate(const QString& la);
	Q_INVOKABLE void setFile(const QString& value);
	Q_INVOKABLE void setDataSet(const QString& file);

	Q_INVOKABLE void downloadCharacter(const QString& url);
	Q_INVOKABLE void downloadDataSet(const QString& url);
	Q_INVOKABLE void downloadSystemData(const QString& url);
	Q_INVOKABLE void syncWithServer();

signals:
	void hostChanged(QString v);
	void emailChanged(QString v);
	void passwordChanged(QString v);
	void lockedChanged(int value);
	void hideCodedTalentsChanged(bool value);
	void loadingChanged(bool v);
	void isIOSChanged(bool v);
	void lastSystemDataUpdateChanged(const QString& lastup);
	void fileChanged(const QString& file);
	void dataSetChanged(const QString& dset);
	void characterCountChanged(int v);

private slots:
	void characterDownloaded(bool ok);
	void dataSetDownloaded(bool ok);
	void systemDataDownloaded(bool ok);

public:
	// Character data -------------------
	Q_PROPERTY(int currentWounds READ currentWounds NOTIFY currentWoundsChanged)
	Q_PROPERTY(int currentStrain READ currentStrain NOTIFY currentStrainChanged)
	Q_PROPERTY(int currentConflict READ currentConflict NOTIFY currentConflictChanged)
	Q_PROPERTY(QString woundHistory READ woundHistory NOTIFY woundHistoryChanged)
	Q_PROPERTY(QString strainHistory READ strainHistory NOTIFY strainHistoryChanged)
	Q_PROPERTY(QString conflictHistory READ conflictHistory NOTIFY conflictHistoryChanged)
	Q_PROPERTY(int woundDelta READ woundDelta WRITE setWoundDelta NOTIFY woundDeltaChanged)
	Q_PROPERTY(int strainDelta READ strainDelta WRITE setStrainDelta NOTIFY strainDeltaChanged)
	Q_PROPERTY(QString stimPacks READ stimPacks NOTIFY stimPacksChanged)
	Q_PROPERTY(QString erps READ erps NOTIFY erpsChanged)
	Q_PROPERTY(int stimPacksUsed READ stimPacksUsed NOTIFY stimPacksUsedChanged)
	Q_PROPERTY(int erpsUsed READ erpsUsed NOTIFY erpsUsedChanged)

	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString player READ player WRITE setPlayer NOTIFY playerChanged)
	Q_PROPERTY(QString gender READ gender WRITE setGender NOTIFY genderChanged)
	Q_PROPERTY(QString age READ age WRITE setAge NOTIFY ageChanged)
	Q_PROPERTY(QString height READ height WRITE setHeight NOTIFY heightChanged)
	Q_PROPERTY(QString build READ build WRITE setBuild NOTIFY buildChanged)
	Q_PROPERTY(QString hair READ hair WRITE setHair NOTIFY hairChanged)
	Q_PROPERTY(QString eyes READ eyes WRITE setEyes NOTIFY eyesChanged)
	Q_PROPERTY(QString features READ features WRITE setFeatures NOTIFY featuresChanged)
	Q_PROPERTY(QString story READ story WRITE setStory NOTIFY storyChanged)
	Q_PROPERTY(QString species READ species WRITE setSpecies NOTIFY speciesChanged)
	Q_PROPERTY(QString career READ career WRITE setCareer NOTIFY careerChanged)
	Q_PROPERTY(QString specializations READ specializations WRITE setSpecializations NOTIFY specializationsChanged)
	Q_PROPERTY(QString portrait READ portrait WRITE setPortrait NOTIFY portraitChanged)
	Q_PROPERTY(int credits READ credits WRITE setCredits NOTIFY creditsChanged)
	Q_PROPERTY(int encValue READ encValue WRITE setEncValue NOTIFY encValueChanged)
	Q_PROPERTY(int encThreshold READ encThreshold WRITE setEncThreshold NOTIFY encThresholdChanged)
	Q_PROPERTY(int cumbValue READ cumbValue WRITE setCumbValue NOTIFY cumbValueChanged)
	Q_PROPERTY(int cumbThreshold READ cumbThreshold WRITE setCumbThreshold NOTIFY cumbThresholdChanged)
	Q_PROPERTY(QString encText READ encText WRITE setEncText NOTIFY encTextChanged)
	Q_PROPERTY(int morality READ morality NOTIFY moralityChanged)

	Q_PROPERTY(int brawn READ brawn NOTIFY brawnChanged)
	Q_PROPERTY(int agility READ agility NOTIFY agilityChanged)
	Q_PROPERTY(int intellect READ intellect NOTIFY intellectChanged)
	Q_PROPERTY(int cunning READ cunning NOTIFY cunningChanged)
	Q_PROPERTY(int willpower READ willpower NOTIFY willpowerChanged)
	Q_PROPERTY(int presence READ presence NOTIFY presenceChanged)
	Q_PROPERTY(int soak READ soak NOTIFY soakChanged)
	Q_PROPERTY(int wound READ wound NOTIFY woundChanged)
	Q_PROPERTY(int strain READ strain NOTIFY strainChanged)
	Q_PROPERTY(int defenseRanged READ defenseRanged NOTIFY defenseRangedChanged)
	Q_PROPERTY(int defenseMelee READ defenseMelee NOTIFY defenseMeleeChanged)
	Q_PROPERTY(int force READ force NOTIFY forceChanged)
	Q_PROPERTY(int forceCommitted READ forceCommitted NOTIFY forceCommittedChanged)
	Q_PROPERTY(int totalXP READ totalXP NOTIFY totalXPChanged)
	Q_PROPERTY(int newXP READ newXP NOTIFY newXPChanged)
	Q_PROPERTY(int usedXP READ usedXP NOTIFY usedXPChanged)

	Q_PROPERTY(QString skillName READ skillName NOTIFY skillNameChanged)
	Q_PROPERTY(QString talentName READ talentName NOTIFY talentNameChanged)
	Q_PROPERTY(QString itemName READ itemName NOTIFY itemNameChanged) // Name
	Q_PROPERTY(QString dicePool READ dicePool WRITE setDicePool NOTIFY dicePoolChanged)
	Q_PROPERTY(int negativePool READ negativePool WRITE setNegativePool NOTIFY negativePoolChanged)
	Q_PROPERTY(QString checkRange READ checkRange NOTIFY checkRangeChanged) // Range
	Q_PROPERTY(QString checkDamage READ checkDamage NOTIFY checkDamageChanged)
	Q_PROPERTY(QString checkCritLevel READ checkCritLevel NOTIFY checkCritLevelChanged)  // Strength
	Q_PROPERTY(QString checkQualMag READ checkQualMag NOTIFY checkQualMagChanged) // Magnitude
	Q_PROPERTY(QString checkPowerStr READ checkPowerStr NOTIFY checkPowerStrChanged) // Strength
	Q_PROPERTY(QString checkDuration READ checkDuration NOTIFY checkDurationChanged) // Duration
	Q_PROPERTY(QString checkAttachDesc READ checkAttachDesc NOTIFY checkAttachDescChanged) //
	Q_PROPERTY(QString checkManeuvers READ checkManeuvers NOTIFY checkManeuversChanged) // Maneuvers
	Q_PROPERTY(QString checkStrain READ checkStrain NOTIFY checkStrainChanged) // Strain

	Q_PROPERTY(int imageProviderCount READ imageProviderCount WRITE setImageProviderCount NOTIFY imageProviderCountChanged)

	int currentWounds() { return CurrentData::instance->wounds; }
	int currentStrain() { return CurrentData::instance->strain+CurrentData::instance->temporaryStrain; }
	int currentConflict() { return CurrentData::instance->conflict; }
	QString woundHistory() { return CurrentData::instance->woundHistory; }
	QString strainHistory() { return CurrentData::instance->strainHistory; }
	QString conflictHistory() { return CurrentData::instance->conflictHistory; }
	int woundDelta() { return CurrentData::instance->woundDelta; }
	int strainDelta() { return CurrentData::instance->strainDelta; }
	QString stimPacks() { return CurrentData::instance->stimPacks(); }
	QString erps() { return CurrentData::instance->erps(); }
	int stimPacksUsed() { return CurrentData::instance->stimPacksUsed; }
	int erpsUsed() { return CurrentData::instance->erpsUsed; }

	QString name() { return CurrentData::instance->name; }
	QString player() { return CurrentData::instance->player; }
	QString gender() { return CurrentData::instance->gender; }
	QString age() { return CurrentData::instance->age; }
	QString height() { return CurrentData::instance->height; }
	QString build() { return CurrentData::instance->build; }
	QString hair() { return CurrentData::instance->hair; }
	QString eyes() { return CurrentData::instance->eyes; }
	QString features() { return CurrentData::instance->features; }
	QString story() { return CurrentData::instance->story; }
	QString species() { return CurrentData::instance->species; }
	QString career() { return CurrentData::instance->career; }
	QString specializations() { return CurrentData::instance->specializations; }
	QString portrait() { return CurrentData::instance->portrait; }
	int credits() { return CurrentData::instance->credits; }
	int encValue() { return CurrentData::instance->encumbranceValue; }
	int encThreshold() { return CurrentData::instance->encumbranceThreshold; }
	int cumbValue() { return CurrentData::instance->cumbersomeValue; }
	int cumbThreshold() { return CurrentData::instance->cumbersomeThreshold; }
	QString encText() { return CurrentData::instance->encumbranceText; }
	int morality() { return getAttribute(MORALITY); }
	int brawn() { return getAttribute(BRAWN); } // Modifications must go into getAttribute()
	int agility() { return getAttribute(AGILITY); }
	int intellect() { return getAttribute(INTELLECT); }
	int cunning() { return getAttribute(CUNNING); }
	int willpower() { return getAttribute(WILLPOWER); }
	int presence() { return getAttribute(PRESENCE); }
	int soak() { return getAttribute(SOAK); }
	int wound() { return getAttribute(WOUND); }
	int strain() { return getAttribute(STRAIN); }
	int defenseRanged() { return getAttribute(DRANGED); }
	int defenseMelee() { return getAttribute(DMELEE); }
	int force() { return getAttribute(FORCE); }
	int forceCommitted() { return CurrentData::instance->forceCommitCount(); }
	int totalXP() { return getAttribute(XP); }
	int newXP() { return getAttribute(NEWXP); }
	int usedXP() { return getAttribute(USEDXP); }
	QString skillName();
	QString talentName();
	QString itemName();
	QString dicePool();
	int negativePool();
	void getItemRange(int& range1, int& range2);
	QString checkRange();
	QString checkDamage();
	QString checkCritLevel();
	QString checkQualMag();
	QString checkPowerStr();
	QString checkDuration();
	QString checkAttachDesc();
	QString checkManeuvers();
	QString checkStrain();
	int imageProviderCount();

	Q_INVOKABLE void adjustWounds(int delta);
	Q_INVOKABLE void adjustStrain(int delta);
	Q_INVOKABLE void adjustConflict(int delta);

	Q_INVOKABLE void useStimPack(int delta);
	Q_INVOKABLE void useErp(int delta);

	Q_INVOKABLE void setWoundDelta(int val);
	Q_INVOKABLE void setStrainDelta(int val);

	Q_INVOKABLE void addCriticalWound(int perc, int type);
	Q_INVOKABLE void removeCriticalWound(int ref);

	Q_INVOKABLE void addExperience(const QString& type, const QString& key, const QString& name, const QString& desc, int amount);
	Q_INVOKABLE void changeExperience(int ref, const QString& desc, int amount);
	Q_INVOKABLE void removeExperience(int ref);

	Q_INVOKABLE void addCustomSkill(const QString& name, const QString& characteristic, int ranks);
	Q_INVOKABLE void removeCustomSkill(const QString& name);

	Q_INVOKABLE void addInvItem(int count, const QString& itemkey, const QString& desc, int amount);
	Q_INVOKABLE void updateInvItem(int ref, int count, const QString& desc, int amount);
	Q_INVOKABLE bool removeInvItem(int ref);
	Q_INVOKABLE int getPrice(const QString& key);
	Q_INVOKABLE int getRarity(const QString& key);
	Q_INVOKABLE void searchShop(QString search_string);

	Q_INVOKABLE void appendCheckItem(const QString& pool, const QString& desc);
	Q_INVOKABLE void updateCheckItem(int ref, const QString& pool, const QString& desc);
	Q_INVOKABLE void removeCheckItem(int ref);
	Q_INVOKABLE void checkItem(int ref);

	Q_INVOKABLE void changeEquipment(const QString& uuid, int state, int stored);
	Q_INVOKABLE void showCharacteristics(); // characteristics
	Q_INVOKABLE void showDescription(); // description
	Q_INVOKABLE void showSkills(); // skills
	Q_INVOKABLE void showTalents(); // experience
	Q_INVOKABLE void showWeaponsAndArmor(); // weapons
	Q_INVOKABLE void showGear(); // inventory
	Q_INVOKABLE void showInventory(); // cashout
	Q_INVOKABLE QString showChecklist(QString skill_key, QString talent_key, QString uuid);
	Q_INVOKABLE void hideCheckList();
	Q_INVOKABLE void fillCheckList(const QString& check_list_type);

	Q_INVOKABLE void setName(const QString& value);
	Q_INVOKABLE void setPlayer(const QString& value);
	Q_INVOKABLE void setGender(const QString& value);
	Q_INVOKABLE void setAge(const QString& value);
	Q_INVOKABLE void setHeight(const QString& value);
	Q_INVOKABLE void setBuild(const QString& value);
	Q_INVOKABLE void setHair(const QString& value);
	Q_INVOKABLE void setEyes(const QString& value);
	Q_INVOKABLE void setFeatures(const QString& value);
	Q_INVOKABLE void setStory(const QString& value);
	Q_INVOKABLE void setSpecies(const QString& value);
	Q_INVOKABLE void setCareer(const QString& value);
	Q_INVOKABLE void setSpecializations(const QString& value);
	Q_INVOKABLE void setPortrait(const QString& value);
	Q_INVOKABLE void setCredits(int c);
	Q_INVOKABLE void setEncValue(int c);
	Q_INVOKABLE void setEncThreshold(int c);
	Q_INVOKABLE void setCumbValue(int c);
	Q_INVOKABLE void setCumbThreshold(int c);
	Q_INVOKABLE void setEncText(const QString& t);
	Q_INVOKABLE void setDicePool(const QString& t);
	Q_INVOKABLE void setNegativePool(int t);
	Q_INVOKABLE void setImageProviderCount(int t);

signals:
	void currentWoundsChanged(int val);
	void currentStrainChanged(int val);
	void currentConflictChanged(int val);
	void woundHistoryChanged(const QString& val);
	void strainHistoryChanged(const QString& val);
	void conflictHistoryChanged(const QString& val);
	void woundDeltaChanged(int val);
	void strainDeltaChanged(int val);
	void stimPacksChanged(QString val);
	void erpsChanged(QString val);
	void stimPacksUsedChanged(int val);
	void erpsUsedChanged(int val);

	void nameChanged(const QString& name);
	void playerChanged(const QString& player);
	void genderChanged(const QString& gender);
	void ageChanged(const QString& value);
	void heightChanged(const QString& value);
	void buildChanged(const QString& value);
	void hairChanged(const QString& value);
	void eyesChanged(const QString& value);
	void featuresChanged(const QString& value);
	void storyChanged(const QString& value);
	void speciesChanged(const QString& value);
	void careerChanged(const QString& value);
	void specializationsChanged(const QString& value);
	void portraitChanged(const QString& value);
	void creditsChanged(int c);
	void encValueChanged(int value);
	void encThresholdChanged(int value);
	void cumbValueChanged(int value);
	void cumbThresholdChanged(int value);
	void encTextChanged(const QString& t);
	void moralityChanged(int value);

	void brawnChanged(int value);
	void agilityChanged(int value);
	void intellectChanged(int value);
	void cunningChanged(int value);
	void willpowerChanged(int value);
	void presenceChanged(int value);
	void soakChanged(int value);
	void woundChanged(int value);
	void strainChanged(int value);
	void defenseRangedChanged(int value);
	void defenseMeleeChanged(int value);
	void forceChanged(int value);
	void forceCommittedChanged(int value);
	void totalXPChanged(int value);
	void newXPChanged(int value);
	void usedXPChanged(int value);

	void skillNameChanged(const QString& value);
	void talentNameChanged(const QString& value);
	void itemNameChanged(const QString& value);
	void dicePoolChanged(const QString& value);
	void negativePoolChanged(int value);
	void checkRangeChanged(const QString& value);
	void checkDamageChanged(const QString& value);
	void checkCritLevelChanged(const QString& value);
	void checkQualMagChanged(const QString& value);
	void checkPowerStrChanged(const QString& value);
	void checkDurationChanged(const QString& value);
	void checkAttachDescChanged(const QString& value);
	void checkManeuversChanged(const QString& value);
	void checkStrainChanged(const QString& value);
	void itemCritPlusChanged(int value);
	void imageProviderCountChanged(int value);

	void alert(const QString& title, const QString& message);

public:
	int getAttribute(const QString& val);
	int setAttributeMods(const CharMods& mods);
	void setChangeDicePool(const QString& dice_pool, ChecklistItem* item = NULL, bool list_setup = false);
	void inventoryChanged();
	void emitExperienceChanged();
	void characteristicsChanged();
	void emitStimPacksChanged();
	void emitErpsChanged();
	void emitCharacterCountChanged();
	void emitBrawnChanged();
	void emitAgilityChanged();
	void emitForceCommittedChanged();
	void emitCurrentStrainChanged();

	QString getCurrentSkillKey();
	QString getCurrentTalentKey();
	QString getForcePool(bool total_force = false);
	void adjustPoolForCommittedForce(int commit_count);

private:
	void reload();
	QString processDownload(const QString& url, const QByteArray& data, const QString& to_dir, QString& err_msg);
	void setupItem(const QString& uuid);

	// Fixed (Exported) Data:
	QString iHost;
	QString iEmail;
	QString iCurrentEmail; // The user from which we last loaded data
	QString iPassword;
	QString iCurrentPassword;
	int iLocked;
	int iHideCodedTalents;
	static int iLoading;
	QString iLastSystemDataUpdate;
	QString iFile;
	QString iDataSet;

	// Attributes used to run the Check List dialog:
	QString iActiveSkill;
	QString iCurrentSkillKey;
	QString iCurrentTalentKey;
	QString iDicePool;
	Item iItem;

	// Changes to the Check List dice pool, and other checklist items:
	QString iChangeDicePool;
	//QString iModDicePool;
	int iChecklistDamage;
	int iChecklistPierce;
	int iChecklistCrit;
	int iChecklistCritRating;
	int iChecklistRange;
	int iChecklistMagnitude;
	int iChecklistStrength;
	int iChecklistDuration;
	int iChecklistExtra;

	// A mod number used to indicate that the image database has changed
	int iImageProviderCount;

	QList<CurrentData*> iCharDataList;

	Downloader iCharacterDownloader;
	Downloader iDataSetDownloader;
	Downloader iSystemDataDownloader;
	Syncer iSyncer;
};

#endif // __Character_h__
