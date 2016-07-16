
#include "md5.h"

#include "Item.h"
#include "Character.h"
#include "Talents.h"
#include "ItemDescriptors.h"
#include "DatUtil.h"

// Quality ----------------------------------------

Quality::Quality()
{
	clear();
}

void Quality::clear(const QString& k)
{
	key = k;
	count = 0;
}

QString Quality::quality()
{
	ItemDescriptor desc = ItemDescriptors::instance()->descriptor(key);
	QString text;

	if (desc.isQuality) {
		text = desc.qualDesc;
		if (count > 1) {
			if (text.contains("{0}"))
				text.replace("{0}", QString::number(count));
			else
				text = QString("%1 %1").arg(count).arg(text);
		}
		else
			text.replace("{0}", QString::number(count));
	}
	return text;
}

// Mod ----------------------------------------

Mod::Mod()
{
	clear();
}

void Mod::clear(const QString& k)
{
	key = k;
	count = 0;
	number = 1;
	miscDesc.clear();
}

QString Mod::qualityText()
{
	if (!miscDesc.isEmpty())
		return miscDesc;

	QString mod_desc = ItemDescriptors::instance()->descriptor(key).modDesc;
	int n_number = number * count;
	if (mod_desc.contains("{0}"))
		mod_desc.replace("{0}", QString::number(n_number));
	else {
		if (n_number > 1)
			mod_desc = QString("%1 %2").arg(n_number).arg(mod_desc);

	}
	return mod_desc;
}

bool Mod::isQuality()
{
	return ItemDescriptors::instance()->descriptor(key).isQuality;
}

QString Mod::modText()
{
	if (!miscDesc.isEmpty())
		return miscDesc;

	QString mod_desc = ItemDescriptors::instance()->descriptor(key).modDesc;

	if (mod_desc.isEmpty()) {
		Skill* skill;
		if ((skill = Skill::getSkill(key))) {
			mod_desc = QString("Skill (%1)").arg(skill->name);
		}
		else {
			Talent talent = AllTalents::instance()->getTalent(key);

			if (!talent.key.isEmpty()) {
				mod_desc = QString("Talent (%1)").arg(talent.name);
			}
			else {
				mod_desc = miscDesc;
				if (mod_desc.isEmpty())
					mod_desc = "?";
			}
		}
	}

	int n_number = number;
	if (mod_desc.contains("{0}"))
		mod_desc.replace("{0}", QString::number(count));
	else
		n_number = count*number;

	if (n_number == 0)
		mod_desc = QString("%1 Mod").arg(mod_desc);
	else if (n_number == 1)
		mod_desc = QString("1 %2 Mod").arg(mod_desc);
	else
		mod_desc = QString("%1 %2 Mods").arg(n_number).arg(mod_desc);
	return mod_desc;
}

// QualityList ----------------------------------------

void QualityList::clear()
{
	iModMap.clear();
	iOrder.clear();
}

int QualityList::count()
{
	return iModMap.count();
}

bool QualityList::contains(const QString& key)
{
	return iModMap.contains(key);
}

Quality QualityList::itemAt(int i)
{
	Quality result;

	if (i<0 || i>=count())
		return result;
	return iModMap[iOrder[i]];
}

Quality QualityList::get(const QString& key)
{
	Quality qual;

	if (iModMap.contains(key))
		return iModMap[key];
	qual.key = key;
	return qual;
}

void QualityList::addItem(Quality qual)
{
	if (iModMap.contains(qual.key)) {
		iModMap[qual.key].count += qual.count;
	}
	else {
		iOrder.append(qual.key);
		iModMap[qual.key] = qual;
	}
}

void QualityList::addMod(Mod mod)
{
	Quality qual;

	if (mod.key.isEmpty())
		return;
	qual.key = mod.key;
	qual.count = mod.count * mod.number;
	addItem(qual);
}

// DieMod --------------------------------------------

DieMod::DieMod() :
	skillKey(),
	boostCount(0),
	setbackCount(0),
	forceCount(0),
	advantageCount(0),
	threatCount(0),
	addSetbackCount(0),
	upgradeCount(0)
{
}

void DieMod::clear(const QString& k)
{
	skillKey = k;
	boostCount = 0;
	setbackCount = 0;
	forceCount = 0;
	advantageCount = 0;
	threatCount = 0;
	addSetbackCount = 0;
	upgradeCount = 0;
}

// DieModList ----------------------------------------

void DieModList::clear()
{
	iDieModMap.clear();
}

int DieModList::count()
{
	return iDieModMap.count();
}

bool DieModList::contains(const QString& key)
{
	return iDieModMap.contains(key);
}

DieMod DieModList::get(const QString& key)
{
	return iDieModMap[key];
}

void DieModList::addMod(DieMod mod)
{
	Quality qual;

	if (mod.skillKey.isEmpty())
		return;
	iDieModMap[mod.skillKey] = mod;
}

// ModList ----------------------------------------

void ModList::clear()
{
	iModMap.clear();
	iOrder.clear();
}

int ModList::count() const
{
	return iModMap.count();
}

bool ModList::contains(const QString& key)
{
	return iModMap.contains(key);
}

Mod ModList::itemAt(int i) const
{
	Mod result;

	if (i<0 || i>=count())
		return result;
	return iModMap[iOrder[i]];
}

Mod ModList::get(const QString& key)
{
	Mod mod;

	if (iModMap.contains(key))
		return iModMap[key];
	mod.key = key;
	return mod;
}

QString ModList::addItem(Mod mod)
{
	if (mod.key.isEmpty())
		mod.key = Md5::hash(mod.miscDesc.toUtf8());	// This mod only has a text
	if (iModMap.contains(mod.key))
		iModMap[mod.key].number++;
	else  {
		iOrder.append(mod.key);
		iModMap[mod.key] = mod;
	}
	return mod.key;
}

void ModList::attachments(QString& list)
{
	for (int i=0; i<count(); i++) {
		Mod mod = itemAt(i);

		if (!list.isEmpty()) {
			if (!list.endsWith(".") && !list.endsWith(":"))
				list.append(",");
			if (!list.endsWith(" "))
				list.append(" ");
		}
		list.append(mod.modText());
	}
}

// Item ----------------------------------------

QString Item::name() const
{
	ShopItem shop = Shop::instance.getItem(key);
	if (shop.name.isEmpty())
		return key;
	return shop.name;
}

bool Item::restricted()
{
	ShopItem shop = Shop::instance.getItem(key);
	return false;
}

// For shown weapons, if you have a talent that adds damage to only one hit of an attack,
// and you're using either a two-weapon set, or a weapon with the auto fire, blast, or linked
// qualities (any of which can generate multiple hits), the extra damage will now be shown
// in brackets and not included with the main damage, indicating that it is not necessarily
// applied to all hits.
// Although I would exclude blast because other hits have fixed damage.
QString Item::damageTotal()
{
	Character*	character = Character::instance;
	ShopItem	shop = Shop::instance.getItem(key);
	int			t;
	int			oneHitDamage = 0;
	QString		skillKey = shop.skillKey;
	int			baseDamage = shop.damage;
	int			addDamage = shop.addDamage;

	if (baseDamage == 0 && (skillKey == "MELEE" || skillKey == "BRAWL"))
		t = character->brawn();
	else
		t = baseDamage;
	if (hasQuality("SUPERIOR"))
		t += 1;

	if (hasQuality("DAMADD"))
		t += getQuality("DAMADD").count;

	for (int i = 0; i < character->talents.size(); i++) {
		CharTalent& char_talent = character->talents.at(i);
		Talent talent = AllTalents::instance()->getTalent(char_talent.key);

		if (talent.addDamagePerSkillRank && char_talent.selectedSkills.contains(skillKey)) {
			if (hasQuality("AUTOFIRE"))
				oneHitDamage += character->skills[skillKey].ranks;
			else
				t += character->skills[skillKey].ranks;
		}

		if (talent.damageBonusSkills.contains(skillKey))
			t += talent.damageBonus * char_talent.ranks;
	}

	foreach (SpeciesTalent st, character->speciesTalents) {
		if (st.damageBonusSkill == skillKey)
			t += st.damageBonus;
	}

	QString damage;
	if (damage > 0 && addDamage)
		damage = QString("%1+%2").arg(t).arg(addDamage);
	else
		damage = QString::number(t + addDamage);
	if (oneHitDamage)
		damage += QString("[+%1]").arg(oneHitDamage);
	return damage;
}

QString Item::dicePool()
{
	Character*	character = Character::instance;
	CharSkill	skill;
	int			accuracy = 0;
	int			superior = 0;
	int			setback = 0;
	int			adv_add = 0;
	int			remove_setback = 0;
	QString		pool;
	ShopItem	shop = Shop::instance.getItem(key);
	QString		skillKey = shop.skillKey;

	if (character->skills.contains(skillKey))
		skill = character->skills[skillKey];
	else
		skill.key = skillKey;

	if (hasQuality("ACCURATE"))
		accuracy = getQuality("ACCURATE").count;

	// Mods are included in the qualities!
	//if (hasMod("ACCURATE")) {
	//	Mod mod = getMod("ACCURATE");
	//	accuracy += mod.count * mod.number;
	//}

	if (hasQuality("SETBACKADD"))
		setback = getQuality("SETBACKADD").count;

	// Mods are included in the qualities!
	//if (hasMod("SETBACKADD")) {
	//	Mod mod = getMod("SETBACKADD");
	//	setback += mod.count * mod.number;
	//}

	if (hasQuality("SUPERIOR"))
		superior = 1;

	//if (hasMod("SUPERIOR"))
	//	superior = 1;

	if (hasQuality("ADVADD"))
		adv_add = getQuality("ADVADD").count;

	if (attachments.contains("Custom Grip")) {
		if (setback > 0)
			setback--;
		else
			remove_setback = 1;
	}

	pool = skill.getDicePool();

	return pool + DatUtil::repeat("B", accuracy) + DatUtil::repeat("S", setback) +
		DatUtil::repeat("N", remove_setback) + DatUtil::repeat("a", superior+adv_add);
}

int Item::critPlus()
{
	Character*	character = Character::instance;
	int			crit_plus = 0;

	if (hasQuality("VICIOUS"))
		crit_plus = getQuality("VICIOUS").count;

	if (character->talents.contains("LETHALBL"))
		crit_plus += character->talents.get("LETHALBL").ranks;

	return crit_plus;
}

int Item::pierce()
{
	int pierce = 0;

	if (hasQuality("PIERCE"))
		pierce = getQuality("PIERCE").count;
	if (hasQuality("BREACH"))
		pierce += getQuality("BREACH").count*10;

	//if (character->talents.contains("LETHALBL"))
		//pierce += character->talents["LETHALBL"].ranks;

	return pierce;
}

int Item::cumbersome(int burly)
{
	int cumb = 0;

	// I assume that the cumbersome penalty applies if you equip it or not!
	if (!carried())
		return 0;

	if (hasQuality("CUMBERSOME"))
		cumb = getQuality("CUMBERSOME").count;

	return removeBurly(cumb, burly);
}

QString Item::qualities()
{
	QualityList	q = qualityList;
	QString		list;

	for (int i=0; i<modList.count(); i++)
		q.addMod(modList.itemAt(i));

	if (Shop::instance.contains(key)) {
		ShopItem shop = Shop::instance.getItem(key);

		foreach (Quality qual, shop.qualityList)
			q.addItem(qual);
	}

	for (int i=0; i<q.count(); i++)
		DatUtil::appendToList(list, q.itemAt(i).quality());

	return list;
}

QString Item::features()
{
	ModList		m;
	//ModList*	m_ptr = &modList;
	QString		list;

	if (Shop::instance.contains(key)) {
		ShopItem shop = Shop::instance.getItem(key);
		//m = modList;

		foreach (Mod mod, shop.modList)
			m.addItem(mod);
		//m_ptr = &m;
	}

	for (int i=0; i<m.count(); i++)
		DatUtil::appendToList(list, m.itemAt(i).modText());

	return list;
}

// Used for gear:
int Item::encCarriedVal(int burly)
{
	return removeBurly(encumberance(), burly);
}

int Item::removeBurly(int val, int burly)
{
	if (val > 1) {
		val -= burly;
		if (val < 1)
			val = 1;
	}
	return val;
}

// Used for armor:
int Item::encWornValue()
{
	if (carried()) {
		int enc = encumberance();

		return enc - 3 > 0 ? enc - 3 : 0;
	}
	return 0;
}

QString Item::encArmorValue()
{
	return QString("%1/%2").arg(encWornValue()).arg(encumberance());
}

int Item::soakVal()
{
	ShopItem shop = Shop::instance.getItem(key);

	int s = shop.soak;
	if (hasQuality("SUPERIOR"))
		s += 1;
	return s;
}

int Item::meleeDef()
{
	ShopItem shop = Shop::instance.getItem(key);

	int def = shop.mdef;
	if (hasQuality("MELEEDEFADD"))
		def += getQuality("MELEEDEFADD").count;
	return def;
}

int Item::rangeDef()
{
	ShopItem shop = Shop::instance.getItem(key);

	int def = shop.rdef;
	if (hasQuality("RANGEDEFADD"))
		def += getQuality("RANGEDEFADD").count;
	return def;
}

void Item::characteristicDelta(CharMods& mods)
{
	if (hasQuality(BRAWN))
		mods.inc(V_BR, getQuality(BRAWN).count);
	if (hasQuality(AGILITY))
		mods.inc(V_AG, getQuality(AGILITY).count);
	if (hasQuality(INTELLECT))
		mods.inc(V_INT,  getQuality(INTELLECT).count);
	if (hasQuality(CUNNING))
		mods.inc(V_CUN, getQuality(CUNNING).count);
	if (hasQuality(WILLPOWER))
		mods.inc(V_WIL, getQuality(WILLPOWER).count);
	if (hasQuality(PRESENCE))
		mods.inc(V_PR, getQuality(PRESENCE).count);
}

int Item::critTotal()
{
	ShopItem	shop = Shop::instance.getItem(key);
	int			t = shop.critical;

	foreach (SpeciesTalent st, Character::instance->speciesTalents) {
		if (st.weaponsCrit && st.damageBonusSkill == shop.skillKey && t > st.weaponsCrit)
			t = st.weaponsCrit;
	}

	if (t > 0 && hasQuality("CRITSUB")) {
		t -= getQuality("CRITSUB").count;
		if (t <= 0)
			t = 1;
	}

	return t;
}

bool Item::held()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return state == IS_HELD;
}

bool Item::equipped()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return state == IS_EQUIPPED;
}

bool Item::carried()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return state != NOT_CARRIED;
}

int Item::state()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return state;
}

int Item::quantity()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return quantity;
}

int Item::carriedQuantity()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	if (state == NOT_CARRIED)
		return 0;
	return quantity - stored;
}

int Item::stored()
{
	int quantity;
	int stored;
	int state;

	storageData(quantity, stored, state);
	return stored;
}

void Item::setHeld(bool h)
{
	iHeld = h;
}

void Item::setEquipped(bool h)
{
	iEquipped = h;
}

void Item::setQuantity(int v)
{
	iQuantity = v;
}

int Item::originalQuantity()
{
	return iQuantity;
}

int Item::originalState()
{
	if (iEquipped)
		return IS_EQUIPPED;
	if (iHeld)
		return IS_HELD;
	return NOT_CARRIED;
}

bool Item::isGrenade()
{
	ShopItem shop = Shop::instance.getItem(key);
	return (shop.gearType & (GEAR_TYPE_GRENADE | GEAR_TYPE_RANGED)) == (GEAR_TYPE_GRENADE | GEAR_TYPE_RANGED);
}

bool Item::canBeWorn()
{
	ShopItem shop = Shop::instance.getItem(key);
	return (shop.gearType & (GEAR_TYPE_CYBERNETICS | GEAR_TYPE_STORAGE)) != 0;
}

void Item::addMod(Mod mod)
{
	modList.addItem(mod);
}

void Item::addMod(const ModList& mod_list)
{
	for (int i=0; i<mod_list.count(); i++) {
		modList.addItem(mod_list.itemAt(i));
	}
}

bool Item::hasMod(const QString& qkey)
{
	if (!modList.contains(qkey)) {
		if (Shop::instance.contains(key)) {
			ShopItem shop = Shop::instance.getItem(key);

			return shop.modList.contains(qkey);
		}
	}

	return true;
}

Mod Item::getMod(const QString& qkey)
{
	Mod mod = modList.get(qkey);
	if (Shop::instance.contains(key)) {
		ShopItem shop = Shop::instance.getItem(key);

		if (shop.modList.contains(qkey)) {
			if (mod.key.isEmpty())
				mod = shop.modList[qkey];
			else
				mod.number++;
		}
	}
	return mod;
}

void Item::addQuality(Quality qual)
{
	qualityList.addItem(qual);
}

/*
void Item::addQualityFromMod(Mod mod)
{
	qualityList.addMod(mod);
}
*/

bool Item::hasQuality(const QString& qkey)
{
	if (qualityList.contains(qkey))
		return true;

	if (modList.contains(qkey))
		return true;

	if (Shop::instance.contains(key)) {
		ShopItem shop = Shop::instance.getItem(key);

		return shop.qualityList.contains(qkey);
	}

	return true;
}

Quality Item::getQuality(const QString& qkey)
{
	Quality qual = qualityList.get(qkey);
	qual.key = qkey;

	Mod mod = modList.get(qkey);
	qual.count += mod.count * mod.number;

	if (Shop::instance.contains(key)) {
		ShopItem shop = Shop::instance.getItem(key);
		if (shop.qualityList.contains(qkey))
			qual.count += shop.qualityList[qkey].count;
	}

	return qual;
}

int Item::encumberance()
{
	ShopItem shop = Shop::instance.getItem(key);
	return shop.encumberance;
}

void Item::storageData(int& quantity, int& stored, int& state)
{
	quantity = 0;
	if (iQuantity >= 0)
		quantity = iQuantity;
	stored = 0;
	if (iEquipped)
		state = IS_EQUIPPED;
	else if (iHeld)
		state = IS_HELD;
	else
		state = NOT_CARRIED;

	if (Character::instance->currentData()->invMod.contains(key)) {
		// Inventory overrides:
		InvModItem item = Character::instance->currentData()->invMod[key];

		if (item.quantity != UNKNOWN_QUANTITY)
			quantity = item.quantity;
		if (item.stored >= 0)
			stored = item.stored;
		if (item.state != UNDEFINED)
			state = item.state;

		// Cannot store more than we have:
		if (stored > quantity)
			stored = quantity;
	}

	// Nothing to carry:
	if ((quantity - stored) == 0)
		state = NOT_CARRIED;
}

// ShopItem --------------------------------------
