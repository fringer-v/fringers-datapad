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

class Character;

class CharSkill {
public:
	QString key;
	int		ranks;
	bool	isCareer;

	CharSkill() {
		clear("");
	}

	void clear(QString k) {
		key = k;
		ranks = 0;
		isCareer = false;
	}

	QString getDicePool(Skill* skill = NULL, QString ch = QString());
};

class CharItem {
public:
	QString key;
	QString name;
	int		size;
	QString notes;

	void clear() {
		key.clear();
		name.clear();
		size = 0;
		notes.clear();
	}
};

class CharItemList {
public:
	QList<CharItem> items;

	CharItem findItem(const QString& key);
};

//#define DEFAULT_HOST	"localhost:8080/starwars"
#define DEFAULT_HOST	"fringer.space/"

class Character : public QObject
{
	Q_OBJECT

public:
	static Character* instance;

	explicit Character(QObject *parent = 0);
	void clear(bool signal);

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
	Q_PROPERTY(QString dataSet READ dataSet WRITE setDataSet NOTIFY dataSetChanged)
	Q_PROPERTY(int characterCount READ characterCount NOTIFY characterCountChanged)

	Q_INVOKABLE void downloadCharacter(const QString& url);
	Q_INVOKABLE void downloadDataSet(const QString& url);
	Q_INVOKABLE void downloadSystemData(const QString& url);
	Q_INVOKABLE void syncWithServer();

	QString host();
	QString email();
	QString currentEmail();
	QString password();
	QString currentPassword();
	int locked();
	int hideCodedTalents();

	bool loading();
	bool isIOS();
	QString lastSystemDataUpdate();
	QString dataSet();
	int characterCount();
	void setCurrentEmailAndPassword(const QString& v, const QString& p);

	Q_INVOKABLE void setHost(const QString& v);
	Q_INVOKABLE void setEmail(const QString& v);
	Q_INVOKABLE void setPassword(const QString& v);
	Q_INVOKABLE void setLocked(int t);
	Q_INVOKABLE void setHideCodedTalents(int t);

	Q_INVOKABLE void setLoading(bool v);
	Q_INVOKABLE void setLastSystemDataUpdate(const QString& la);
	Q_INVOKABLE void setDataSet(const QString& file);

signals:
	void hostChanged(QString v);
	void emailChanged(QString v);
	void passwordChanged(QString v);
	void loadingChanged(bool v);
	void isIOSChanged(bool v);
	void lastSystemDataUpdateChanged(const QString& lastup);
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

	Q_PROPERTY(int brawnDelta READ brawnDelta NOTIFY brawnDeltaChanged)
	Q_PROPERTY(int agilityDelta READ agilityDelta NOTIFY agilityDeltaChanged)
	Q_PROPERTY(int intellectDelta READ intellectDelta NOTIFY intellectDeltaChanged)
	Q_PROPERTY(int cunningDelta READ cunningDelta NOTIFY cunningDeltaChanged)
	Q_PROPERTY(int willpowerDelta READ willpowerDelta NOTIFY willpowerDeltaChanged)
	Q_PROPERTY(int presenceDelta READ presenceDelta NOTIFY presenceDeltaChanged)

	Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
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
	Q_PROPERTY(QString lastInvLine READ lastInvLine WRITE setLastInvLine NOTIFY lastInvLineChanged)
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
	Q_PROPERTY(int totalXP READ totalXP NOTIFY totalXPChanged)
	Q_PROPERTY(int newXP READ newXP NOTIFY newXPChanged)
	Q_PROPERTY(int usedXP READ usedXP NOTIFY usedXPChanged)

	Q_PROPERTY(QString activeSkill READ activeSkill WRITE setActiveSkill NOTIFY activeSkillChanged)
	Q_PROPERTY(QString dicePool READ dicePool WRITE setDicePool NOTIFY dicePoolChanged)
	Q_PROPERTY(int negativePool READ negativePool WRITE setNegativePool NOTIFY negativePoolChanged)
	Q_PROPERTY(QString itemUuid READ itemUuid WRITE setItemUuid NOTIFY itemUuidChanged)
	Q_PROPERTY(QString itemItemKey READ itemItemKey WRITE setItemItemKey NOTIFY itemItemKeyChanged)
	Q_PROPERTY(QString itemName READ itemName WRITE setItemName NOTIFY itemNameChanged)
	Q_PROPERTY(QString itemRange READ itemRange WRITE setItemRange NOTIFY itemRangeChanged)
	Q_PROPERTY(QString itemSkill READ itemSkill WRITE setItemSkill NOTIFY itemSkillChanged)
	Q_PROPERTY(QString itemDamage READ itemDamage WRITE setItemDamage NOTIFY itemDamageChanged)
	Q_PROPERTY(QString itemCritical READ itemCritical WRITE setItemCritical NOTIFY itemCriticalChanged)
	Q_PROPERTY(QString itemQualities READ itemQualities WRITE setItemQualities NOTIFY itemQualitiesChanged)
	Q_PROPERTY(QString itemAttachments READ itemAttachments WRITE setItemAttachments NOTIFY itemAttachmentsChanged)
	Q_PROPERTY(QString itemManeuvers READ itemManeuvers WRITE setItemManeuvers NOTIFY itemManeuversChanged)
	Q_PROPERTY(QString itemStrain READ itemStrain WRITE setItemStrain NOTIFY itemStrainChanged)
	Q_PROPERTY(int itemCritPlus READ itemCritPlus WRITE setItemCritPlus NOTIFY itemCritPlusChanged)
	Q_PROPERTY(int itemPierce READ itemPierce WRITE setItemPierce NOTIFY itemPierceChanged)
	Q_PROPERTY(int imageProviderCount READ imageProviderCount WRITE setImageProviderCount NOTIFY imageProviderCountChanged)

	int currentWounds() { return iCurrentData.wounds; }
	int currentStrain() { return iCurrentData.strain+iCurrentData.temporaryStrain; }
	int currentConflict() { return iCurrentData.conflict; }
	QString woundHistory() { return iCurrentData.woundHistory; }
	QString strainHistory() { return iCurrentData.strainHistory; }
	QString conflictHistory() { return iCurrentData.conflictHistory; }
	int woundDelta() { return iCurrentData.woundDelta; }
	int strainDelta() { return iCurrentData.strainDelta; }
	QString stimPacks() { return iCurrentData.stimPacks(); }
	QString erps() { return iCurrentData.erps(); }
	int stimPacksUsed() { return iCurrentData.stimPacksUsed; }
	int erpsUsed() { return iCurrentData.erpsUsed; }

	int brawnDelta() { return iChDelta.get(V_BR); }
	int agilityDelta() { return iChDelta.get(V_AG); }
	int intellectDelta() { return iChDelta.get(V_INT); }
	int cunningDelta() { return iChDelta.get(V_CUN); }
	int willpowerDelta() { return iChDelta.get(V_WIL); }
	int presenceDelta() { return iChDelta.get(V_PR); }

	QString file() { return iFile; }
	QString name() { return iName; }
	QString player() { return iPlayer; }
	QString gender() { return iGender; }
	QString age() { return iAge; }
	QString height() { return iHeight; }
	QString build() { return iBuild; }
	QString hair() { return iHair; }
	QString eyes() { return iEyes; }
	QString features() { return iFeatures; }
	QString story() { return iStory; }
	QString species() { return iSpecies; }
	QString career() { return iCareer; }
	QString specializations() { return iSpecializations; }
	QString portrait() { return iPortrait; }
	int credits() { return iCredits; }
	QString lastInvLine() { return iLastInvLine; }
	int encValue() { return iEncValue; }
	int encThreshold() { return iEncThreshold; }
	int cumbValue() { return iCumbValue; }
	int cumbThreshold() { return iCumbThreshold; }
	QString encText() { return iEncText; }
	int morality() {
		if (iCurrentData.experienceTotal.contains(EXP_TOT_MORALITY))
			return iCurrentData.experienceTotal[EXP_TOT_MORALITY].value;
		return getAttribute(MORALITY);
	}
	int brawn() { return getAttribute(BRAWN); } // Modifications must go into getAttribute()
	int agility() { return getAttribute(AGILITY); }
	int intellect() { return getAttribute(INTELLECT); }
	int cunning() { return getAttribute(CUNNING); }
	int willpower() { return getAttribute(WILLPOWER); }
	int presence() { return getAttribute(PRESENCE); }
	int soak() { return getAttribute(SOAK); }
	int wound() {
		int wound_adj = 0;

		if (getAttribute(FORCE) > 0) {
			int mor = morality();

			if (mor < 10)
				wound_adj = 2;
			else if (mor < 20)
				wound_adj = 1;
		}
		return getAttribute(WOUND) + wound_adj;
	}
	int  strain(){
		int strain_adj = 0;

		if (getAttribute(FORCE) > 0) {
			int mor = morality();

			if (mor > 90)
				strain_adj = 2;
			else if (mor > 80)
				strain_adj = 1;
			else if (mor < 10)
				strain_adj = -2;
			else if (mor < 20)
				strain_adj = -1;
		}
		return getAttribute(STRAIN) + strain_adj;
	}
	int defenseRanged() { return getAttribute(DRANGED); }
	int defenseMelee() { return getAttribute(DMELEE); }
	int force() { return getAttribute(FORCE); }
	int totalXP() {
		if (iCurrentData.experienceTotal.contains(EXP_TOT_XP))
			return iCurrentData.experienceTotal[EXP_TOT_XP].value;
		return getAttribute(XP);
	}
	int newXP() { return getAttribute(NEWXP); }
	int usedXP() { return getAttribute(USEDXP); }
	QString activeSkill();
	QString activeSkillKey();
	QString dicePool();
	int negativePool();
	QString itemUuid();
	QString itemItemKey();
	QString itemName();
	QString itemRange();
	QString itemSkill();
	QString itemDamage();
	QString itemCritical();
	QString itemQualities();
	QString itemAttachments();
	QString itemManeuvers();
	QString itemStrain();
	int itemCritPlus();
	int itemPierce();
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

	Q_INVOKABLE void addCustomSkill(const QString& name, const QString& charac, int ranks);
	Q_INVOKABLE void removeCustomSkill(const QString& name);

	Q_INVOKABLE void addInvItem(int count, const QString& itemkey, const QString& desc, int amount);
	Q_INVOKABLE void updateInvItem(int ref, int count, const QString& desc, int amount);
	Q_INVOKABLE bool removeInvItem(int ref);
	Q_INVOKABLE int getPrice(const QString& key);
	Q_INVOKABLE int getRarity(const QString& key);
	Q_INVOKABLE void searchShop(QString search_string);

	Q_INVOKABLE void addCheckItem(const QString& pool, const QString& desc);
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
	Q_INVOKABLE void showCheckList();
	Q_INVOKABLE void hideCheckList();
	Q_INVOKABLE void fillCheckList();

	Q_INVOKABLE void setFile(const QString& value);
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
	Q_INVOKABLE void setLastInvLine(const QString& v);
	Q_INVOKABLE void setEncValue(int c);
	Q_INVOKABLE void setEncThreshold(int c);
	Q_INVOKABLE void setCumbValue(int c);
	Q_INVOKABLE void setCumbThreshold(int c);
	Q_INVOKABLE void setEncText(const QString& t);
	Q_INVOKABLE void setAttribute(QString ch, int val);
	Q_INVOKABLE void setActiveSkill(const QString& t);
	Q_INVOKABLE void setDicePool(const QString& t);
	Q_INVOKABLE void setNegativePool(int t);
	Q_INVOKABLE void setItemUuid(const QString& t);
	Q_INVOKABLE void setItemItemKey(const QString& t);
	Q_INVOKABLE void setItemName(const QString& t);
	Q_INVOKABLE void setItemRange(const QString& t);
	Q_INVOKABLE void setItemSkill(const QString& t);
	Q_INVOKABLE void setItemDamage(const QString& t);
	Q_INVOKABLE void setItemCritical(const QString& t);
	Q_INVOKABLE void setItemQualities(const QString& t);
	Q_INVOKABLE void setItemAttachments(const QString& t);
	Q_INVOKABLE void setItemManeuvers(const QString& t);
	Q_INVOKABLE void setItemStrain(const QString& t);
	Q_INVOKABLE void setItemCritPlus(int t);
	Q_INVOKABLE void setItemPierce(int t);
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

	void brawnDeltaChanged(int val);
	void agilityDeltaChanged(int val);
	void intellectDeltaChanged(int val);
	void cunningDeltaChanged(int val);
	void willpowerDeltaChanged(int val);
	void presenceDeltaChanged(int val);

	void fileChanged(const QString& file);
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
	void lastInvLineChanged(const QString& v);
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
	void totalXPChanged(int value);
	void newXPChanged(int value);
	void usedXPChanged(int value);
	void activeSkillChanged(const QString& value);
	void dicePoolChanged(const QString& value);
	void negativePoolChanged(int value);
	void itemUuidChanged(const QString& value);
	void itemItemKeyChanged(const QString& value);
	void itemNameChanged(const QString& value);
	void itemRangeChanged(const QString& value);
	void itemSkillChanged(const QString& value);
	void itemDamageChanged(const QString& value);
	void itemCriticalChanged(const QString& value);
	void itemQualitiesChanged(const QString& value);
	void itemAttachmentsChanged(const QString& value);
	void itemManeuversChanged(const QString& value);
	void itemStrainChanged(const QString& value);
	void itemCritPlusChanged(int value);
	void itemPierceChanged(int value);
	void imageProviderCountChanged(int value);
	void lockedChanged(int value);
	void hideCodedTalentsChanged(bool value);

	void alert(const QString& title, const QString& message);

public:
	int setChMod(const CharMods& mods);
	//void setChDelta(const CharMods& mods);
	void setChangeDicePool(const QString& t);
	void inventoryChanged();
	void experienceChanged();
	void characteristicsChanged();
	void loadCurrentData();
	QString getCurrentDataFile();
	void emitStimPacksChanged();
	void emitErpsChanged();
	void emitCharacterCountChanged();
	void emitBrawnChanged();
	void emitAgilityChanged();
	int getAttribute(const QString& val);
	CurrentData* currentData();
	void setTemporaryStrain(int value);
	int nonCommitedForce();

private:
	void excludeMorality(int mor);
	void includeMorality(int mor);
	void reload();
	QString processDownload(const QString& url, const QByteArray& data, const QString& to_dir, QString& err_msg);

	QString iHost;
	QString iEmail;
	QString iCurrentEmail; // The user from which we last loaded data
	QString iPassword;
	QString iCurrentPassword;
	int		iLocked;
	int		iHideCodedTalents;

	static int iLoading;
	QString iLastSystemDataUpdate;
	QString iDataSet;

	// Current Data:
	CharMods iChMod;
	CharMods iChDelta;

	// Fixed (Exported) Data:
	QString iFile;
	QString iName;
	QString iPlayer;
	QString iGender;
	QString iAge;
	QString iHeight;
	QString iBuild;
	QString iHair;
	QString iEyes;
	QString iFeatures;
	QString iStory;
	QString iSpecies;
	QString iCareer;
	QString iSpecializations;
	QString iPortrait;
	int		iCredits;
	QString	iLastInvLine;
	int		iEncValue;
	int		iEncThreshold;
	int		iCumbValue;
	int		iCumbThreshold;
	QString iEncText;
	int		iMorality;
	QMap<QString, int> iAttributes;
	QString iActiveSkill;
	QString iActiveSkillKey;
	QString iDicePool;
	QString iItemUuid;
	QString iItemItemKey;
	QString iItemName;
	QString iItemRange;
	QString iItemSkill;
	QString iItemDamage;
	QString iItemCritical;
	QString iItemQualities;
	QString iItemAttachments;
	int iItemCritPlus;
	int iItemPierce;
	int iImageProviderCount;
	QString iChangeDicePool;
	QString iModDicePool;
	int iModItemDamage;
	int iModItemPierce;
	int iModItemCrit;
	int iModItemRange;
	CurrentData iCurrentData;

	Downloader iCharacterDownloader;
	Downloader iDataSetDownloader;
	Downloader iSystemDataDownloader;
	Syncer iSyncer;

public:
	int originalCredits;
	CharTalentMap talents;
	QList<SpeciesTalent> speciesTalents;
	QMap<QString, CharSkill> skills;
	CharItemList obligations;
	CharItemList duties;
	//QMap<QString, ShopItem> shopItems;

};

#endif // __Character_h__
