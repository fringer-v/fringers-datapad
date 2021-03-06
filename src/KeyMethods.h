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

#ifndef __KeyMethod_h__
#define __KeyMethod_h__

#include <QMap>

enum MethodID {
	KM_FORCECOMMIT = -4,
	KM_FORCEPOWER = -3,
	KM_UNKNOWN = -2,

	// Skills (first skill must be 0 so it can be used as an index!)
	START_OF_SKILLS = -1,
	KM_ASTRO = 0,
	KM_ATHL,
	KM_BRAWL,
	KM_CHARM,
	KM_COERC,
	KM_COMP,
	KM_COOL,
	KM_COORD,
	KM_CORE,
	KM_DECEP,
	KM_DISC,
	KM_EDU,
	KM_GUNN,
	KM_LEAD,
	KM_LTSABER,
	KM_LORE,
	KM_MECH,
	KM_MED,
	KM_MELEE,
	KM_NEG,
	KM_OUT,
	KM_PERC,
	KM_PILOTPL,
	KM_PILOTSP,
	KM_RANGHVY,
	KM_RANGLT,
	KM_RESIL,
	KM_SKUL,
	KM_STEAL,
	KM_SW,
	KM_SURV,
	KM_UND,
	KM_VIGIL,
	KM_XEN,
	KM_WARF,
	KM_ICOOL,
	KM_IVIG,
	KM_DEFM,
	KM_DEFR,
	KM_REC,
	KM_FDISC,
	KM_FCOOL,
	END_OF_SKILLS,

	// Attributes:
	KM_BR,
	KM_AG,
	KM_INT,
	KM_CUN,
	KM_WIL,
	KM_PR,

	// Implemented Talents:
	KM_BINDBASIC,
	KM_BRI,
	KM_INFLUENCEBASIC,
	KM_INFLUENCECONTROL1,
	KM_MISDIRBASIC,
	KM_MISDIRCONTROL1,
	KM_MISDIRCONTROL2,
	KM_MISDIRMASTERY,
	KM_SENSEBASIC,
	KM_SENSECONTROL2,
	KM_DONTSHOOT,
	KM_FEARSOME,
	KM_MOVEBASIC,
	KM_MOVECONTROL1,
	KM_MOVECONTROL2,
	KM_MOVECONTROL3,
	KM_TECHAPT,
	KM_INDIS,
	KM_ENHANCECONT6,
	KM_QUICKDR,
	KM_TRICK,
	KM_WARFORBASIC,
	KM_WARFORCONTROL1,
	KM_JUSTKID,
	KM_HERO,
	KM_HEROICRES,
	KM_INTUITEVA,
	KM_NATBLADE,
	KM_SEEKBASIC,
	KM_SEEKSTRENGTH,
	KM_SEEKCONTROL1,
	KM_SEEKMASTERY,
	KM_SEEKDURATION,
	KM_BATMEDBASIC,
	KM_SUPPRESSBASIC,
	KM_SUPPRESSCONTROL1,
	KM_SUPPRESSCONTROL2,
	KM_SUPPRESSMASTERY,
	KM_FARSIGHTBASIC,
	KM_FARSIGHTCONTROL1, // Micro detail  "Control: Microscopic Detail"
	KM_FARSIGHTCONTROL2, // Transparent "Control: Transparent"
	KM_FARSIGHTCONTROL5, // Panarmic vision "Control: Ongoing Effect"
	KM_FARSIGHTCONTROL6, // Fine details "Control: 360 Degrees"
	KM_FARSIGHTMASTERY,
	KM_FORSEEBASIC,
	KM_INTERJECT,
	KM_FLDCOMM,

	// Coded Talents (must be last)
	FIRST_CODED_TALENT,
	KM_MASLEAD,
	KM_FLDCOMMIMP,
	KM_SORRYMESS,
	KM_GUNSBLAZING,
	KM_CALLEM,
	KM_FRENZ,
	KM_NATMYSTIC,
	KM_NATBRAW,
	KM_FORSEECONTROL1, // Init basic
	KM_FORSEECONTROL2, // Increase defence
	KM_FORSEECONTROL3, // Free move
	KM_FARSIGHTCONTROL3, // -- VIG & PERC boost "Control: Fine Detail"
	KM_FARSIGHTCONTROL4, // -- ongoing "Control: Vigilance/Perception"
	KM_FORCEALLY,
	KM_FORAG,
	KM_EXTRACK,
	KM_NOWYOUSEE,
	KM_SUPPRESSDURATION,
	KM_SUPPRESSCONTROL3,
	KM_MASSHAD,
	KM_BATMEDCONTROL1,
	KM_BATMEDCONTROL2,
	KM_BATMEDMASTERY,
	KM_BATMEDDURATION,
	KM_IRONBODY,
	KM_ANAT,
	KM_PLAUSDEN,
	KM_CRIPV,
	KM_PREYWEAK,
	KM_SEEKCONTROL2,
	KM_SEEKMAGNITUDE,
	KM_SEEKCONTROL3,
	KM_SARSWEEP,
	KM_MULTOPP,
	KM_GALMAP,
	KM_EXHPORT,
	KM_CONF,
	KM_DEFTRAIN,
	KM_COD,
	KM_BYP,
	KM_CONDITIONED,
	KM_ARM,
	KM_ARMIMP,
	KM_DEADACC,
	KM_DEDI,
	KM_GRIT,
	KM_JURY,
	KM_LETHALBL,
	KM_PHYSTRAIN,
	KM_SKILLJOCK,
	KM_TINK,
	KM_TOUGH,
	KM_KILL,
	KM_INTIM,
	KM_CONGENIAL,
	KM_TRUEAIM,
	KM_QUICKST,
	KM_PRECAIM,
	KM_TARGBL,
	KM_SNIPSHOT,
	KM_UNCANREAC,
	KM_STRSMART,
	KM_INTENSFOC,
	KM_SIXSENSE,
	KM_FORCERAT,
	KM_BAL,
	KM_DODGE,
	KM_DISARMSMILE,
	KM_TOUCH,
	KM_INSIGHT,
	KM_MASPIL,
	KM_FULLTH,
	KM_FULLTHIMP,
	KM_FULLTHSUP,
	KM_SLEIGHTMIND,
	KM_HAWKSWOOP,
	KM_UNCANSENS,
	KM_PARRY,
	KM_PARRYIMP,
	KM_UNARMPARRY,
	KM_RAPREA,
	KM_REFLECT,
	KM_REFLECTIMP,
	KM_SABERSW,
	KM_RAINDEATH,
	KM_ENDUR,
	KM_HEAVYHITTER,
	KM_BRA,
	KM_BURLY,
	KM_BAR,
	KM_FERSTR,
	KM_SENSDANG,
	KM_POINTBL,
	KM_SIDESTEP,
	KM_DEFSTA,
	KM_DURA,
	KM_SENSEADV,
	KM_KEENEYED,
	KM_SUPREF,
	KM_DEAD,
	KM_DEADIMP,
	KM_HARDHD,
	KM_HARDHDIMP,
	KM_RESEARCH,
	KM_SORESUTECH,
	KM_WELLROUND,

	KM_SENSECONTROL1,
	KM_SENSECONTROL3,
	KM_SENSESTRENGTH,
	KM_SENSEDURATION,
	KM_SENSERANGE,
	KM_SENSEMAGNITUDE,

	KM_INFLUENCECONTROL2,
	KM_INFLUENCERANGE,
	KM_INFLUENCEMAGNITUDE,
	KM_INFLUENCESTRENGTH,
	KM_INFLUENCEDURATION,

	KM_ATARU,

	KM_ENHANCEBASIC,
	KM_ENHANCERANGE,
	KM_ENHANCECONT0,
	KM_ENHANCECONT1,
	KM_ENHANCECONT2,
	KM_ENHANCECONT3,
	KM_ENHANCECONT4,
	KM_ENHANCECONT5,
	KM_ENHANCECONT7,
	KM_ENHANCECONT8,
	KM_ENHANCECONT9,

	KM_MOVESTRENGTH,
	KM_MOVERANGE,
	KM_MOVEMAGNITUDE,

	KM_MISDIRRANGE,
	KM_MISDIRMAGNITUDE,
	KM_MISDIRSTRENGTH,
	KM_MISDIRCONTROL3,
	KM_MISDIRDURATION,

	KM_WARFORRANGE,
	KM_WARFORCONTROL2,
	KM_WARFORCONTROL3,
	KM_WARFORCONTROL4,
	KM_WARFORMAGNITUDE,

	KM_BINDRANGE,
	KM_BINDMAGNITUDE,
	KM_BINDSTRENGTH,
	KM_BINDCONTROL1,
	KM_BINDDURATION,
	KM_BINDCONTROL2,
	KM_BINDMASTERY
};

class KeyMethod
{
public:
	static KeyMethod instance;

	KeyMethod();

	void append(const QString& key, MethodID id) {
		iKeys[key] = id;
	}

	MethodID getID(const QString& key) {
		if (!iKeys.contains(key))
			return KM_UNKNOWN;
		return iKeys[key];
	}

	bool isSkill(const QString& key) {
		if (!iKeys.contains(key))
			return false;
		int k = iKeys[key];
		return k > (int) START_OF_SKILLS && k < (int) END_OF_SKILLS;
	}

	static bool isSkillID(MethodID k)
	{
		return (int) k > (int) START_OF_SKILLS && (int) k < (int) END_OF_SKILLS;
	}

	bool isCodedTalent(const QString& key) {
		if (!iKeys.contains(key))
			return false;
		int k = iKeys[key];
		return k > (int) FIRST_CODED_TALENT;
	}

protected:
	QMap<QString, MethodID> iKeys;
};


#endif // __KeyMethod_h__
