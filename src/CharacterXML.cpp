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

#include <QtXml>
#include <QPixmap>

#include "CharacterXML.h"
#include "DataAccess.h"
#include "ImageProvider.h"
#include "DatUtil.h"
#include "ItemAttachments.h"
#include "ItemDescriptors.h"
#include "Gear.h"
#include "Weapons.h"
#include "DataSet.h"
#include "CharacterList.h"

// CharacterXML -------------------------

CharacterXML::CharacterXML(CurrentData* current_data) :
	iIsPC(false),
	iModUnique(0)
{
	iCurrentData = current_data;
	iSpeciesKey.clear();
	iSubSpeciesKey.clear();
	iChoices.clear();
}

void CharacterXML::start()
{
}

void CharacterXML::end()
{
	if (iSpecializations.contains("Force Sensitive Exile") && iSpecializations.contains("Force Sensitive Emergent")) {
		iSpecializations = iSpecializations.replace("Force Sensitive Emergent", "Force Sensitive Emergent/Exile");
		iSpecializations = iSpecializations.replace(", Force Sensitive Exile", "");
		iSpecializations = iSpecializations.replace("Force Sensitive Exile", "");
	}

	iCurrentData->specializations = iSpecializations;

	SpeciesXML species_xml(iCurrentData);

	// species, speciesTalents, specialFeatures
	iCurrentData->species = species_xml.loadSpecies(iSpeciesKey, iSubSpeciesKey, &iChoices);

	// Load Current Data:
	if (!iCurrentData->name.isEmpty()) {
		CurrentDataXML current_xml(iCurrentData);
		QString file_name;

		file_name = iCurrentData->name.toLower();
		file_name.replace(" ", "-");
		file_name.replace("\"", "-");
		while (file_name.contains("--"))
			file_name.replace("--", "-");

		if (QFile().exists(DatUtil::getCurrentFolder() + file_name + ".tri"))
			file_name += ".tri";
		else
			file_name += ".xpad";
		iCurrentData->fileName = file_name;
		QByteArray data = DataAccess::getCurrentData(file_name);
		current_xml.readFromBuffer(data.constData(), data.length());
	}

	// Add unarmed:
	if (!iCurrentData->weapons.containsByUuid("UNARMED")) {
		Item una;
		una.clear();
		una.itemkey = "UNARMED";
		una.originalQuantity = 1;
		iCurrentData->weapons.aquireItem(una, true);
	}
}

bool CharacterXML::xmlElement(const DatStringBuffer& path, const char* value)
{
	//if (path.contains("ExperienceRanks"))
	//qDebug() << "XML" << path.getCString() << value << "--" << iAttribute << iAttrValue;
	//if (strlen(value) > 200)
	//	qDebug() << "XML" << path.getCString() << "VALUE(" << strlen(value) << ")" << "--" << iAttribute << iAttrValue;
	//else
	//	qDebug() << "XML" << path.getCString() << value << "--" << iAttribute << iAttrValue;
	if (path.startsWith("/Character/Vehicles/"))
		// Ignore vehicle info
		;
	if (path.startsWith("/Character/NPCs/") && iIsPC)
		// Ignore NPC!
		;
	else if (path.endsWith("/Description/CharName/")) {
		iCurrentData->name = value;
		iIsPC = true;
	}
	else if (path.endsWith("/Adversary/Name/")) {
		if (!iIsPC) {
			iCurrentData->name = value;							// name
			iCurrentData->npc = true;
		}
	}
	else if (path.endsWith("/Description/PlayerName/"))
		iCurrentData->player = value;							// player
	else if (path.endsWith("/Description/Gender/"))
		iCurrentData->gender = value;				// gender
	else if (path.endsWith("/Description/Age/"))
		iCurrentData->age = value;					// age
	else if (path.endsWith("/Description/Height/"))
		iCurrentData->height = value;				// height
	else if (path.endsWith("/Description/Build/"))
		iCurrentData->build = value;				// build
	else if (path.endsWith("/Description/Hair/"))
		iCurrentData->hair = value;				// hair
	else if (path.endsWith("/Description/Eyes/"))
		iCurrentData->eyes = value;				// eyes
	else if (path.endsWith("/Description/OtherFeatures/"))
		iCurrentData->features = value;			// features
	else if (path.endsWith("/Character/Story/"))
		iCurrentData->story = value;				// story
	else if (path.endsWith("/Portrait/")) {
		//QByteArray data = QByteArray::fromBase64(value.toUtf8());
		QByteArray data = QByteArray::fromBase64(value);
		QString id = DatUtil::lastLeft(Character::instance->file(), ".");
		ImageProvider::setImage(id, data);
		iCurrentData->portrait = id;				// portrait
	}
	else if (path.endsWith("/Credits/")) {
		iCurrentData->credits = toInt(value);		// credits
		iCurrentData->originalCredits = toInt(value);
	}
	else if (path.endsWith("/Career/CareerKey/"))
		iCurrentData->career = DataSet::instance.careers[value];	// career
	else if (path.endsWith("/CharSpecialization/Name/")) {
		iSpecialization = value;
		if (!iSpecializations.isEmpty())
			iSpecializations.append(", ");
		iSpecializations.append(iSpecialization);			// specializations
	}
	else if (path.endsWith("/CharCharacteristic/Key/")) {
		iAttribute = value;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/SoakValue/#open")) {
		iAttribute = SOAK;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/WoundThreshold/#open")) {
		iAttribute = WOUND;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/StrainThreshold/#open")) {
		iAttribute = STRAIN;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/DefenseRanged/#open")) {
		iAttribute = DRANGED;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/DefenseMelee/#open")) {
		iAttribute = DMELEE;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Attributes/ForceRating/#open")) {
		iAttribute = FORCE;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Experience/ExperienceRanks/#open")) {
		iAttribute = XP;
		iAttrValue = 0;
	}
	else if (path.endsWith("/Experience/UsedExperience/"))
		iCurrentData->attributes[USEDXP] = toInt(value);
	else if (path.endsWith("/CharCharacteristic/#end")||
		path.endsWith("/Attributes/SoakValue/#end") ||
		path.endsWith("/Attributes/WoundThreshold/#end") ||
		path.endsWith("/Attributes/StrainThreshold/#end") ||
		path.endsWith("/Attributes/DefenseRanged/#end") ||
		path.endsWith("/Attributes/DefenseMelee/#end") ||
		path.endsWith("/Attributes/ForceRating/#end") ||
		path.endsWith("/Experience/ExperienceRanks/#end")) {
		iCurrentData->attributes[iAttribute] = iAttrValue; // attributes
	}

	// Skills -----------------------
	else if (path.endsWith("/CharSkill/Key/")) {
		iCharSkill.clear(value);
		iAttrValue = 0;
	}
	else if (path.endsWith("/CharSkill/isCareer/")) {
		iCharSkill.isCareer = isTrue(value) ? 1 : 0;
	}
	else if (path.endsWith("/CharSkill/#end")) {
		iCharSkill.internalRanks = iAttrValue;
		iCurrentData->setCharSkill(iCharSkill); // skills
	}

	else if (path.endsWith("/StartingRanks/"))
		iAttrValue += toInt(value);
	else if (path.endsWith("/SpeciesRanks/"))
		iAttrValue += toInt(value);
	else if (path.endsWith("/CareerRanks/"))
		iAttrValue += toInt(value);
	else if (path.endsWith("/TalentRanks/")) {
		// These will be added when inventory changes, because of conditions
		if (iAttribute != SOAK && iAttribute != DRANGED && iAttribute != DMELEE)
			iAttrValue += toInt(value);
	}
	else if (path.endsWith("/ObligationRanks/"))
		iAttrValue += toInt(value);
	else if (path.endsWith("/DutyRanks/"))
		iAttrValue += toInt(value);
	// Will add the attachments, gear (and items i have no example)!
	//else if (path.endsWith("/AttachRanks/"))
	//	iAttrValue += toInt(value);
	//else if (path.endsWith("/ItemRanks/"))
	//	iAttrValue += toInt(value);
	//else if (path.endsWith("/CyberRanks/"))
	//	iAttrValue += toInt(value);
	else if (path.endsWith("/PurchasedRanks/")) {
		// This appears to be used for "Item Ranks" in the case of
		// armor!
		if (iAttribute != SOAK && iAttribute != DRANGED && iAttribute != DMELEE)
			iAttrValue += toInt(value);
	}
	else if (path.endsWith("/CharRanks/"))
		iAttrValue += toInt(value);

	// Obligations -----------------------
	else if (path.endsWith("/CharObligation/#open"))
		iCharItem.clear();
	else if (path.endsWith("/CharObligation/ObKey/"))
		iCharItem.key = value;
	else if (path.endsWith("/CharObligation/Name/"))
		iCharItem.name = value;
	else if (path.endsWith("/CharObligation/Size/"))
		iCharItem.size = toInt(value);
	else if (path.endsWith("/CharObligation/Notes/"))
		iCharItem.notes = value;
	else if (path.endsWith("/CharObligation/#end"))
		iCurrentData->obligations.items.append(iCharItem);	// obligations

	// Duties -----------------------
	else if (path.endsWith("/CharDuty/#open"))
		iCharItem.clear();
	else if (path.endsWith("/CharDuty/DutKey/"))
		iCharItem.key = value;
	else if (path.endsWith("/CharDuty/Name/"))
		iCharItem.name = value;
	else if (path.endsWith("/CharDuty/Size/"))
		iCharItem.size = toInt(value);
	else if (path.endsWith("/CharDuty/Notes/"))
		iCharItem.notes = value;
	else if (path.endsWith("/CharDuty/#end"))
		iCurrentData->duties.items.append(iCharItem);		// duties

	// Motivations -----------------------
	else if (path.endsWith("/CharMotivation/#open"))
		iMotItem.clear();
	else if (path.endsWith("/CharMotivation/Notes/"))
		iMotItem.notes = value;
	else if (path.endsWith("/CharMotivation/MotiveKey/"))
		iMotItem.name1 = Motivations::instance.motivationList[value];
	else if (path.endsWith("/CharMotivation/SpecMotiveKey/"))
		iMotItem.name2 = Motivations::instance.motivationList[value];
	else if (path.endsWith("/CharMotivation/#end"))
		iCurrentData->motivations.append(iMotItem);			// motivations

	// Morality Pairs -----------------------
	else if (path.endsWith("/MoralityPair/#open"))
		iMorItem.clear();
	else if (path.endsWith("/MoralityPair/Notes/"))
		iMorItem.notes = value;
	else if (path.endsWith("/MoralityPair/StrengthKey/"))
		iMorItem.name1 = Moralities::instance.moralityList[value];
	else if (path.endsWith("/MoralityPair/WeaknessKey/"))
		iMorItem.name2 = Moralities::instance.moralityList[value];
	else if (path.endsWith("/MoralityPair/#end"))
		iCurrentData->moralities.append(iMorItem);			// moralities

	// Morality
	else if (path.endsWith("/MoralityValue/"))
		iCurrentData->attributes[MORALITY] = toInt(value);

	// Talents -----------------------
	else if (path.endsWith("/CharTalent/Purchased/"))
		iPurchased = iCurrentData->npc || isTrue(value);
	else if (path.endsWith("/CharTalent/Key/")) {
		iCharTalent.clear(value);
		iCharTalent.aquisition = iSpecialization;
	}
	else if (path.endsWith("/CharTalent/SelectedSkills/Key/"))
		iCharTalent.selectedSkills.append(value);
	else if (path.endsWith("/CharTalent/BonusChars/BonusChar/Key/"))
		DatUtil::appendToList(iCharTalent.selectedCharacteristics, value);
	else if (path.endsWith("/CharTalent/#end")) {
		if (iPurchased)
			iCurrentData->talents.addTalent(iCharTalent);
		iPurchased = false;
	}

	// Force Abilities -----------------------
	else if (path.endsWith("/CharForceAbility/Purchased/"))
		iPurchased = iCurrentData->npc || isTrue(value);
	else if (path.endsWith("/CharForceAbility/Key/")) {
		iCharTalent.clear(value);
		iCharTalent.aquisition = iSpecialization;
	}
	else if (path.endsWith("/CharForceAbility/#end")) {
		if (iPurchased)
			iCurrentData->talents.addTalent(iCharTalent);	// talents
		iPurchased = false;
	}

	// Species -----------------------
	else if (path.endsWith("/Species/SpeciesKey/"))
		iSpeciesKey = value;
	else if (path.endsWith("/Species/SubSpeciesKey/"))
		iSubSpeciesKey = value;
	else if (path.endsWith("/CharOption/ChoiceKey/"))
		iChoiceKey = value;
	else if (path.endsWith("/CharOption/OptionKey/"))
		iChoices[iChoiceKey] = value;

	// Weapons, Armor, Gear -----------------------
	else if (path.endsWith("/CharWeapon/#open") || path.endsWith("/CharArmor/#open") || path.endsWith("/CharGear/#open"))
		iItem.clear();
	else if (path.endsWith("/CharWeapon/Key/") || path.endsWith("/CharArmor/Key/") || path.endsWith("/CharGear/Key/"))
		iItem.uuid = value;
	else if (path.endsWith("/CharWeapon/ItemKey/") || path.endsWith("/CharArmor/ItemKey/") || path.endsWith("/CharGear/ItemKey/"))
		iItem.itemkey = value;
	else if (path.endsWith("/CharWeapon/Equipped/") || path.endsWith("/CharArmor/Equipped/") || path.endsWith("/CharGear/Equipped/")) {
		if (isTrue(value))
			iItem.originalState = IS_EQUIPPED;
	}
	else if (path.endsWith("/CharWeapon/Held/") || path.endsWith("/CharArmor/Held/") || path.endsWith("/CharGear/Held/")) {
		if (iItem.originalState != IS_EQUIPPED && isTrue(value))
			iItem.originalState = IS_HELD;
	}
	else if (path.endsWith("/CharWeapon/Count/") || path.endsWith("/CharArmor/Count/") || path.endsWith("/CharGear/Count/"))
		iItem.originalQuantity = toInt(value);
	else if (path.endsWith("/CharWeapon/Notes/") || path.endsWith("/CharArmor/Notes/") || path.endsWith("/CharGear/Notes/"))
		iItem.notes = value;
	else if (path.endsWith("/CharWeapon/Rename/") || path.endsWith("/CharArmor/Rename/") || path.endsWith("/CharGear/Rename/"))
		iItem.rename = value;
	else if (path.endsWith("/CharWeapon/Shown/"))
		iItem.shown = isTrue(value);	

	else if (path.endsWith("/PurchasedAttachments/CharItemAttachment/#open")) {
		iModList.clear();
		iAttachment.clear();
	}
	else if (path.endsWith("/PurchasedAttachments/CharItemAttachment/AttachKey/")) {
		QString attachment_name = ItemAttachments::instance()->attachment(value).name;

		iAttachment = value;
		iItem.attachList.append(iAttachment);
		DatUtil::makeList(iItem.attachments, QString("[I]%1[i]:").arg(attachment_name), ";");
	}
	else if (path.endsWith("/AllMods/Mod/#open"))
		iMod.clear();
	else if (path.endsWith("/AllMods/Mod/Key/"))
		iMod.key = value;
	else if (path.endsWith("/AllMods/Mod/Count/"))
		iMod.count = toInt(value);
	else if (path.endsWith("/AllMods/Mod/MiscDesc/"))
		iMod.miscDesc = value; //value.trimmed();
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/#open"))
		iDieMod.clear();
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/SkillKey/"))
		iDieMod.skillKey = value;
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/SkillType/"))
		iDieMod.skillKey = strcmp(value, "Knowledge") == 0 ? KNOWLEDGE : NO_SKILL_TYPE;
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/BoostCount/"))
		iDieMod.boostCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/SetbackCount/"))
		iDieMod.setbackCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/ForceCount/"))
		iDieMod.forceCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/AdvantageCount/"))
		iDieMod.advantageCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/ThreatCount/"))
		iDieMod.threatCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/AddSetbackCount/"))
		iDieMod.addSetbackCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/UpgradeAbilityCount/"))
		iDieMod.upgradeCount = toInt(value);
	else if (path.endsWith("/AllMods/Mod/DieModifiers/DieModifier/#end"))
		iItem.dieModList.addMod(iDieMod);

	else if (path.endsWith("/AllMods/Mod/#end")) {
		if (iMod.key.isEmpty()) {
			iModUnique++;
			iMod.key = QString("$SYS_%1").arg(iModUnique);
		}
		iModList.addItem(iMod);
		//iItem.addQualityFromMod(iMod);
	}
	else if (path.endsWith("/PurchasedAttachments/CharItemAttachment/#end")) {
		// Some attachments have no keys, only a description, not good!
		if (iAttachment == "SETTRIGGER") {
			ModList new_mod_list;
			Mod mod;

			for (int i=0; i<iModList.count(); i++) {
				mod = iModList.itemAt(i);
				if (mod.key.startsWith("$SYS_")) {
					if (mod.miscDesc.contains("[TH]"))
						mod.key = "SETTRIGGER_BASE_MOD";
					else
						mod.key = "SETTRIGGER_EXTRA_MOD";
				}
				new_mod_list.addItem(mod);
			}
			iModList = new_mod_list;
		}

		iModList.attachments(iItem.attachments);
		iItem.addMod(iModList);
	}

	else if (path.endsWith("/CharWeapon/#end")) {
		if (!iItem.uuid.isEmpty())
			iCurrentData->weapons.aquireItem(iItem, true);		// weapons
	}
	else if (path.endsWith("/CharArmor/#end")) {
		if (!iItem.uuid.isEmpty())
			iCurrentData->armor.aquireItem(iItem, true);		// armor
	}
	else if (path.endsWith("/CharGear/#end")) {
		if (!iItem.uuid.isEmpty())
			iCurrentData->gear.aquireItem(iItem, true);		// gear
	}

	// Custom Weapons --------------------------
	else if (path.endsWith("/CharWeapon/CustomWeap/#open"))
		iShopItem.clear(iItem.uuid);
	else if (path.endsWith("/CustomWeap/Name/"))
		iShopItem.name = value;
	else if (path.endsWith("/CustomWeap/SkillKey/")) {
		iShopItem.skillKey = value;
		if (strcmp(value, "RANGLT") == 0 || strcmp(value, "RANGHV") == 0 || strcmp(value, "GUNN") == 0)
			iShopItem.gearType |= GEAR_TYPE_RANGED;
	}
	else if (path.endsWith("/CustomWeap/Damage/")) {
		if (toInt(value) > 0)
			iShopItem.damage = toInt(value);
	}
	else if (path.endsWith("/CustomWeap/DamageAdd/")) {
		if (toInt(value) > 0)
			iShopItem.addDamage = toInt(value);
	}
	else if (path.endsWith("/CustomWeap/Crit/"))
		iShopItem.critical = toInt(value);
	else if (path.endsWith("/CustomWeap/RangeValue/")) {
		if (value[0] == 'w' && value[1] == 'r')
			value += 2;
		iShopItem.range = value;
	}
	else if (path.endsWith("/CustomWeap/Range/")) {
		if (*value && iShopItem.range.isEmpty())
			iShopItem.range = value;
	}
	else if (path.endsWith("/CustomWeap/Categories/Category/")) {
		if (strcmp(value, "Ranged") == 0)
			iShopItem.gearType |= GEAR_TYPE_RANGED;
	}
	else if (path.endsWith("/CustomWeap/Encumbrance/"))
		iShopItem.encumbrance = toInt(value);
	else if (path.endsWith("/CustomWeap/Price/"))
		iShopItem.price = toInt(value);
	else if (path.endsWith("/CustomWeap/Rarity/"))
		iShopItem.rarity = toInt(value);
	else if (path.endsWith("/CustomWeap/Qualities/Quality/Key/"))
		iQuality.clear(value);
	else if (path.endsWith("/CustomWeap/Qualities/Quality/Count/"))
		iQuality.count = toInt(value);
	else if (path.endsWith("/CustomWeap/Qualities/Quality/#end"))
		iShopItem.qualityList[iQuality.key] = iQuality;
	else if (path.endsWith("/CustomWeap/Type/"))
		iShopItem.type = value;
	else if (path.endsWith("/CustomWeap/BaseMods/Mod/#open"))
		iMod.clear();
	else if (path.endsWith("/CustomWeap/BaseMods/Mod/Key/"))
		iMod.clear(value);
	else if (path.endsWith("/CustomWeap/BaseMods/Mod/Count/"))
		iMod.count = toInt(value);
	else if (path.endsWith("/CustomWeap/BaseMods/Mod/MiscDesc/"))
		iMod.miscDesc = QString::fromUtf8(value).trimmed(); // value.trimmed();
	else if (path.endsWith("/CustomWeap/BaseMods/Mod/#end")) {
		if (!iMod.key.isEmpty()) {
			iShopItem.modList[iMod.key] = iMod;
			iShopItem.qualityList[iMod.key].key = iMod.key;
			iShopItem.qualityList[iMod.key].count = iMod.count * iMod.number;
		}
		else if (!iMod.miscDesc.isEmpty()) {
			// Add the MiscDesc to the description
			if (!iMod.miscDesc.endsWith(".") &&
				!iMod.miscDesc.endsWith(";") &&
				!iMod.miscDesc.endsWith(","))
				iMod.miscDesc += ".";

			QString mod_desc = iMod.miscDesc;
			if (iMod.count > 1)
				mod_desc = QString("%1 ").arg(iMod.count)+mod_desc;
			DatUtil::appendToList(iShopItem.description, mod_desc, " ");
		}
	}
	else if (path.endsWith("/CharWeapon/CustomWeap/#end")) {
		iItem.isCustom = true;
		iCurrentData->customItems[iShopItem.key] = iShopItem;
	}
	else if (path.endsWith("/AdvAbility/#open"))
		iTalent.clear(QString());
	else if (path.endsWith("/AdvAbility/Name/"))
		iTalent.internalName = value;
	else if (path.endsWith("/AdvAbility/Description/"))
		iTalent.description = value;
	else if (path.endsWith("/AdvAbility/#end")) {
		if (iTalent.internalName == "Droid") {
			iSpeciesKey = "DROID";
			iSubSpeciesKey = "DROIDSUB7";
			iChoices["DROIDAB"] = "DROIDABSK";
			iChoices["DROIDAB2"] = "DROIDAB2OP1";
			iChoices["DROIDAB3"] = "DROIDAB3OP1";
			iChoices["DROIDAB4"] = "DROIDAB4OP1";
			iChoices["DROIDAB5"] = "DROIDAB5OP1";
		}
		else {
			iTalent.key = "KEY_"+iTalent.internalName.toUpper();
			iCharTalent.key = iTalent.key;
			AllTalents::instance()->addTalent(iTalent);
			iCurrentData->talents.addTalent(iCharTalent);
		}
	}

	else if (path.endsWith("/AdvItem/#open")) {
		iItem.clear();
		iItem.originalQuantity = 1;
		iItem.isCustom = true;
		iShopItem.clear(QString());
		iShopItem.type = "GEAR";
	}
	else if (path.endsWith("/AdvItem/Name/"))
		iShopItem.name = value;
	else if (path.endsWith("/AdvItem/Description/"))
		iShopItem.description = value;
	else if (path.endsWith("/AdvItem/#end")) {
		if (iShopItem.description.isEmpty()) {
			QString word_1 = DatUtil::capitalize(DatUtil::left(iShopItem.name, " "));
			QString word_2 = DatUtil::capitalize(DatUtil::left(DatUtil::right(iShopItem.name, " "), " "));

			iShopItem.description = iShopItem.name;
			iShopItem.name = word_1 + (!word_2.isEmpty() ? " " + word_2 : "");
		}
		iItem.uuid = "KEY_"+iShopItem.name.toUpper();
		iShopItem.key = iItem.uuid;
		iCurrentData->gear.aquireItem(iItem, true);
		iCurrentData->customItems[iShopItem.key] = iShopItem;
	}

	return true;
}

void ListCharXML::load(const QByteArray& data, const QString& f)
{
	file = f;
	name.clear();
	player.clear();
	portrait.clear();
	iLastChanged.clear();
	readFromBuffer(data.constData(), data.length());
}

QString ListCharXML::lastChanged()
{
	QDateTime dt = QDateTime::fromString(iLastChanged, "yyyy-M-dTH:m:s.z");
	return dt.toString("d MMM yyyy H:m:s");
}

bool ListCharXML::xmlElement(const DatStringBuffer& path, const char* value)
{
	if (path.endsWith("/Description/CharName/")) {
		name = value;
		iIsPC = true;
	}
	else if (path.endsWith("/Adversary/Name/")) {
		if (!iIsPC)
			name = value;
	}
	else if (path.endsWith("/Description/PlayerName/"))
		player = value;
	else if (path.endsWith("/Character/LastChanged/"))
		iLastChanged = value;
	else if (path.endsWith("/Character/Portrait/")) {
		//QByteArray data = QByteArray::fromBase64(value.toUtf8());
		QByteArray data = QByteArray::fromBase64(value);
		portrait = DatUtil::lastLeft(file, ".");
		ImageProvider::setImage(portrait, data);
	}
	return true;
}
