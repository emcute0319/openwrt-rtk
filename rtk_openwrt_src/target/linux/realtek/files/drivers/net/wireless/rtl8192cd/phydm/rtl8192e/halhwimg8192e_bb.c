/****************************************************************************** 
* 
* Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved. 
* 
* This program is free software; you can redistribute it and/or modify it 
* under the terms of version 2 of the GNU General Public License as 
* published by the Free Software Foundation. 
* 
* This program is distributed in the hope that it will be useful, but WITHOUT 
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
* more details. 
* 
* You should have received a copy of the GNU General Public License along with 
* this program; if not, write to the Free Software Foundation, Inc., 
* 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA 
* 
* 
******************************************************************************/

/*Image2HeaderVersion: 2.14*/
#if !defined(__ECOS) && !defined(CPTCFG_CFG80211_MODULE)
#include "mp_precomp.h"
#else
#include "../mp_precomp.h"
#endif
#include "../phydm_precomp.h"

#if (RTL8192E_SUPPORT == 1)
static BOOLEAN
CheckPositive(
	IN  PDM_ODM_T     pDM_Odm,
	IN  const u4Byte  Condition1,
	IN  const u4Byte  Condition2,
	IN	const u4Byte  Condition3,
	IN	const u4Byte  Condition4
)
{
	u1Byte    _BoardType = ((pDM_Odm->BoardType & BIT4) >> 4) << 0 | /* _GLNA*/
				((pDM_Odm->BoardType & BIT3) >> 3) << 1 | /* _GPA*/ 
				((pDM_Odm->BoardType & BIT7) >> 7) << 2 | /* _ALNA*/
				((pDM_Odm->BoardType & BIT6) >> 6) << 3 | /* _APA */
				((pDM_Odm->BoardType & BIT2) >> 2) << 4;  /* _BT*/  

	u4Byte	cond1   = Condition1, cond2 = Condition2, cond3 = Condition3, cond4 = Condition4;
	u4Byte    driver1 = pDM_Odm->CutVersion       << 24 | 
				(pDM_Odm->SupportInterface & 0xF0) << 16 | 
				pDM_Odm->SupportPlatform  << 16 | 
				pDM_Odm->PackageType      << 12 | 
				(pDM_Odm->SupportInterface & 0x0F) << 8  |
				_BoardType;

	u4Byte    driver2 = (pDM_Odm->TypeGLNA & 0xFF) <<  0 |  
				(pDM_Odm->TypeGPA & 0xFF)  <<  8 | 
				(pDM_Odm->TypeALNA & 0xFF) << 16 | 
				(pDM_Odm->TypeAPA & 0xFF)  << 24; 

u4Byte    driver3 = 0;

	u4Byte    driver4 = (pDM_Odm->TypeGLNA & 0xFF00) >>  8 |
				(pDM_Odm->TypeGPA & 0xFF00) |
				(pDM_Odm->TypeALNA & 0xFF00) << 8 |
				(pDM_Odm->TypeAPA & 0xFF00)  << 16;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("===> CheckPositive (cond1, cond2, cond3, cond4) = (0x%X 0x%X 0x%X 0x%X)\n", cond1, cond2, cond3, cond4));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("===> CheckPositive (driver1, driver2, driver3, driver4) = (0x%X 0x%X 0x%X 0x%X)\n", driver1, driver2, driver3, driver4));

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("	(Platform, Interface) = (0x%X, 0x%X)\n", pDM_Odm->SupportPlatform, pDM_Odm->SupportInterface));
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_TRACE, 
	("	(Board, Package) = (0x%X, 0x%X)\n", pDM_Odm->BoardType, pDM_Odm->PackageType));


	/*============== Value Defined Check ===============*/
	/*QFN Type [15:12] and Cut Version [27:24] need to do value check*/
	
	if (((cond1 & 0x0000F000) != 0) && ((cond1 & 0x0000F000) != (driver1 & 0x0000F000)))
		return FALSE;
	if (((cond1 & 0x0F000000) != 0) && ((cond1 & 0x0F000000) != (driver1 & 0x0F000000)))
		return FALSE;

	/*=============== Bit Defined Check ================*/
	/* We don't care [31:28] */

	cond1   &= 0x00FF0FFF; 
	driver1 &= 0x00FF0FFF; 

	if ((cond1 & driver1) == cond1) {
		u4Byte bitMask = 0;

		if ((cond1 & 0x0F) == 0) /* BoardType is DONTCARE*/
			return TRUE;

		if ((cond1 & BIT0) != 0) /*GLNA*/
			bitMask |= 0x000000FF;
		if ((cond1 & BIT1) != 0) /*GPA*/
			bitMask |= 0x0000FF00;
		if ((cond1 & BIT2) != 0) /*ALNA*/
			bitMask |= 0x00FF0000;
		if ((cond1 & BIT3) != 0) /*APA*/
			bitMask |= 0xFF000000;

		if (((cond2 & bitMask) == (driver2 & bitMask)) && ((cond4 & bitMask) == (driver4 & bitMask)))  /* BoardType of each RF path is matched*/
			return TRUE;
		else
			return FALSE;
	} else
		return FALSE;
}
static BOOLEAN
CheckNegative(
	IN  PDM_ODM_T     pDM_Odm,
	IN  const u4Byte  Condition1,
	IN  const u4Byte  Condition2
)
{
	return TRUE;
}

/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

u4Byte Array_MP_8192E_AGC_TAB[] = { 
	0x80000400,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFB060001,
		0xC78, 0xFB070001,
		0xC78, 0xFA080001,
		0xC78, 0xF9090001,
		0xC78, 0xF80A0001,
		0xC78, 0xF70B0001,
		0xC78, 0xF60C0001,
		0xC78, 0xF50D0001,
		0xC78, 0xF40E0001,
		0xC78, 0xF30F0001,
		0xC78, 0xF2100001,
		0xC78, 0xF1110001,
		0xC78, 0xF0120001,
		0xC78, 0xEF130001,
		0xC78, 0xEE140001,
		0xC78, 0xED150001,
		0xC78, 0xEC160001,
		0xC78, 0xEB170001,
		0xC78, 0xEA180001,
		0xC78, 0xE9190001,
		0xC78, 0xC81A0001,
		0xC78, 0xC71B0001,
		0xC78, 0xC61C0001,
		0xC78, 0x071D0001,
		0xC78, 0x061E0001,
		0xC78, 0x051F0001,
		0xC78, 0x04200001,
		0xC78, 0x03210001,
		0xC78, 0xAA220001,
		0xC78, 0xA9230001,
		0xC78, 0xA8240001,
		0xC78, 0xA7250001,
		0xC78, 0xA6260001,
		0xC78, 0x85270001,
		0xC78, 0x84280001,
		0xC78, 0x83290001,
		0xC78, 0x252A0001,
		0xC78, 0x242B0001,
		0xC78, 0x232C0001,
		0xC78, 0x222D0001,
		0xC78, 0x672E0001,
		0xC78, 0x662F0001,
		0xC78, 0x65300001,
		0xC78, 0x64310001,
		0xC78, 0x63320001,
		0xC78, 0x62330001,
		0xC78, 0x61340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFA020001,
		0xC78, 0xF9030001,
		0xC78, 0xF8040001,
		0xC78, 0xF7050001,
		0xC78, 0xF6060001,
		0xC78, 0xF5070001,
		0xC78, 0xF4080001,
		0xC78, 0xF3090001,
		0xC78, 0xF20A0001,
		0xC78, 0xF10B0001,
		0xC78, 0xF00C0001,
		0xC78, 0xEF0D0001,
		0xC78, 0xEE0E0001,
		0xC78, 0xED0F0001,
		0xC78, 0xEC100001,
		0xC78, 0xEB110001,
		0xC78, 0xEA120001,
		0xC78, 0xE9130001,
		0xC78, 0xE8140001,
		0xC78, 0xE7150001,
		0xC78, 0xE6160001,
		0xC78, 0xE5170001,
		0xC78, 0xE4180001,
		0xC78, 0xE3190001,
		0xC78, 0xE21A0001,
		0xC78, 0xE11B0001,
		0xC78, 0x8A1C0001,
		0xC78, 0x891D0001,
		0xC78, 0x881E0001,
		0xC78, 0x871F0001,
		0xC78, 0x86200001,
		0xC78, 0x85210001,
		0xC78, 0x84220001,
		0xC78, 0x83230001,
		0xC78, 0x82240001,
		0xC78, 0x6A250001,
		0xC78, 0x69260001,
		0xC78, 0x68270001,
		0xC78, 0x67280001,
		0xC78, 0x66290001,
		0xC78, 0x652A0001,
		0xC78, 0x642B0001,
		0xC78, 0x632C0001,
		0xC78, 0x622D0001,
		0xC78, 0x612E0001,
		0xC78, 0x602F0001,
		0xC78, 0x47300001,
		0xC78, 0x46310001,
		0xC78, 0x45320001,
		0xC78, 0x44330001,
		0xC78, 0x43340001,
		0xC78, 0x42350001,
		0xC78, 0x41360001,
		0xC78, 0x40370001,
		0xC78, 0x40380001,
		0xC78, 0x40390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFA040001,
		0xC78, 0xF9050001,
		0xC78, 0xF8060001,
		0xC78, 0xF7070001,
		0xC78, 0xF6080001,
		0xC78, 0xF5090001,
		0xC78, 0xF40A0001,
		0xC78, 0xF30B0001,
		0xC78, 0xF20C0001,
		0xC78, 0xF10D0001,
		0xC78, 0xF00E0001,
		0xC78, 0xEF0F0001,
		0xC78, 0xEE100001,
		0xC78, 0xED110001,
		0xC78, 0xEC120001,
		0xC78, 0xEB130001,
		0xC78, 0xEA140001,
		0xC78, 0xE9150001,
		0xC78, 0xE8160001,
		0xC78, 0xE7170001,
		0xC78, 0xE6180001,
		0xC78, 0xE5190001,
		0xC78, 0xE41A0001,
		0xC78, 0xE31B0001,
		0xC78, 0xE21C0001,
		0xC78, 0xE11D0001,
		0xC78, 0x8A1E0001,
		0xC78, 0x891F0001,
		0xC78, 0x88200001,
		0xC78, 0x87210001,
		0xC78, 0x86220001,
		0xC78, 0x85230001,
		0xC78, 0x84240001,
		0xC78, 0x83250001,
		0xC78, 0x82260001,
		0xC78, 0x6A270001,
		0xC78, 0x69280001,
		0xC78, 0x68290001,
		0xC78, 0x672A0001,
		0xC78, 0x662B0001,
		0xC78, 0x652C0001,
		0xC78, 0x642D0001,
		0xC78, 0x632E0001,
		0xC78, 0x622F0001,
		0xC78, 0x61300001,
		0xC78, 0x60310001,
		0xC78, 0x47320001,
		0xC78, 0x46330001,
		0xC78, 0x45340001,
		0xC78, 0x44350001,
		0xC78, 0x43360001,
		0xC78, 0x42370001,
		0xC78, 0x41380001,
		0xC78, 0x40390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFA050001,
		0xC78, 0xF9060001,
		0xC78, 0xF8070001,
		0xC78, 0xF7080001,
		0xC78, 0xF6090001,
		0xC78, 0xF50A0001,
		0xC78, 0xF40B0001,
		0xC78, 0xF30C0001,
		0xC78, 0xF20D0001,
		0xC78, 0xF10E0001,
		0xC78, 0xF00F0001,
		0xC78, 0xEF100001,
		0xC78, 0xEE110001,
		0xC78, 0xED120001,
		0xC78, 0xEC130001,
		0xC78, 0xEB140001,
		0xC78, 0xEA150001,
		0xC78, 0xE9160001,
		0xC78, 0xE8170001,
		0xC78, 0xE7180001,
		0xC78, 0xE6190001,
		0xC78, 0xE51A0001,
		0xC78, 0xE41B0001,
		0xC78, 0xE31C0001,
		0xC78, 0xE21D0001,
		0xC78, 0xE11E0001,
		0xC78, 0x8A1F0001,
		0xC78, 0x89200001,
		0xC78, 0x88210001,
		0xC78, 0x87220001,
		0xC78, 0x86230001,
		0xC78, 0x85240001,
		0xC78, 0x84250001,
		0xC78, 0x83260001,
		0xC78, 0x82270001,
		0xC78, 0x6A280001,
		0xC78, 0x69290001,
		0xC78, 0x682A0001,
		0xC78, 0x672B0001,
		0xC78, 0x662C0001,
		0xC78, 0x652D0001,
		0xC78, 0x642E0001,
		0xC78, 0x632F0001,
		0xC78, 0x62300001,
		0xC78, 0x61310001,
		0xC78, 0x60320001,
		0xC78, 0x47330001,
		0xC78, 0x46340001,
		0xC78, 0x45350001,
		0xC78, 0x44360001,
		0xC78, 0x43370001,
		0xC78, 0x42380001,
		0xC78, 0x41390001,
		0xC78, 0x403A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFA060001,
		0xC78, 0xF9070001,
		0xC78, 0xF8080001,
		0xC78, 0xF7090001,
		0xC78, 0xF60A0001,
		0xC78, 0xF50B0001,
		0xC78, 0xF40C0001,
		0xC78, 0xF30D0001,
		0xC78, 0xF20E0001,
		0xC78, 0xF10F0001,
		0xC78, 0xF0100001,
		0xC78, 0xEF110001,
		0xC78, 0xEE120001,
		0xC78, 0xED130001,
		0xC78, 0xEC140001,
		0xC78, 0xEB150001,
		0xC78, 0xEA160001,
		0xC78, 0xE9170001,
		0xC78, 0xE8180001,
		0xC78, 0xE7190001,
		0xC78, 0xE61A0001,
		0xC78, 0xE51B0001,
		0xC78, 0xE41C0001,
		0xC78, 0xE31D0001,
		0xC78, 0xE21E0001,
		0xC78, 0xE11F0001,
		0xC78, 0x8A200001,
		0xC78, 0x89210001,
		0xC78, 0x88220001,
		0xC78, 0x87230001,
		0xC78, 0x86240001,
		0xC78, 0x85250001,
		0xC78, 0x84260001,
		0xC78, 0x83270001,
		0xC78, 0x82280001,
		0xC78, 0x6A290001,
		0xC78, 0x692A0001,
		0xC78, 0x682B0001,
		0xC78, 0x672C0001,
		0xC78, 0x662D0001,
		0xC78, 0x652E0001,
		0xC78, 0x642F0001,
		0xC78, 0x63300001,
		0xC78, 0x62310001,
		0xC78, 0x61320001,
		0xC78, 0x60330001,
		0xC78, 0x47340001,
		0xC78, 0x46350001,
		0xC78, 0x45360001,
		0xC78, 0x44370001,
		0xC78, 0x43380001,
		0xC78, 0x42390001,
		0xC78, 0x413A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xA0000000,	0x00000000,
		0xC78, 0xFB000001,
		0xC78, 0xFB010001,
		0xC78, 0xFB020001,
		0xC78, 0xFB030001,
		0xC78, 0xFB040001,
		0xC78, 0xFB050001,
		0xC78, 0xFB060001,
		0xC78, 0xFA070001,
		0xC78, 0xF9080001,
		0xC78, 0xF8090001,
		0xC78, 0xF70A0001,
		0xC78, 0xF60B0001,
		0xC78, 0xF50C0001,
		0xC78, 0xF40D0001,
		0xC78, 0xF30E0001,
		0xC78, 0xF20F0001,
		0xC78, 0xF1100001,
		0xC78, 0xF0110001,
		0xC78, 0xEF120001,
		0xC78, 0xEE130001,
		0xC78, 0xED140001,
		0xC78, 0xEC150001,
		0xC78, 0xEB160001,
		0xC78, 0xEA170001,
		0xC78, 0xCD180001,
		0xC78, 0xCC190001,
		0xC78, 0xCB1A0001,
		0xC78, 0xCA1B0001,
		0xC78, 0xC91C0001,
		0xC78, 0xC81D0001,
		0xC78, 0x071E0001,
		0xC78, 0x061F0001,
		0xC78, 0x05200001,
		0xC78, 0x04210001,
		0xC78, 0x03220001,
		0xC78, 0xAA230001,
		0xC78, 0xA9240001,
		0xC78, 0xA8250001,
		0xC78, 0xA7260001,
		0xC78, 0xA6270001,
		0xC78, 0x85280001,
		0xC78, 0x84290001,
		0xC78, 0x832A0001,
		0xC78, 0x252B0001,
		0xC78, 0x242C0001,
		0xC78, 0x232D0001,
		0xC78, 0x222E0001,
		0xC78, 0x672F0001,
		0xC78, 0x66300001,
		0xC78, 0x65310001,
		0xC78, 0x64320001,
		0xC78, 0x63330001,
		0xC78, 0x62340001,
		0xC78, 0x61350001,
		0xC78, 0x45360001,
		0xC78, 0x44370001,
		0xC78, 0x43380001,
		0xC78, 0x42390001,
		0xC78, 0x413A0001,
		0xC78, 0x403B0001,
		0xC78, 0x403C0001,
		0xC78, 0x403D0001,
		0xC78, 0x403E0001,
		0xC78, 0x403F0001,
	0xB0000000,	0x00000000,
	0x80000400,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0xE25E0001,
		0xC78, 0xE15F0001,
		0xC78, 0x8A600001,
		0xC78, 0x89610001,
		0xC78, 0x88620001,
		0xC78, 0x87630001,
		0xC78, 0x86640001,
		0xC78, 0x85650001,
		0xC78, 0x84660001,
		0xC78, 0x83670001,
		0xC78, 0x82680001,
		0xC78, 0x6B690001,
		0xC78, 0x6A6A0001,
		0xC78, 0x696B0001,
		0xC78, 0x686C0001,
		0xC78, 0x676D0001,
		0xC78, 0x666E0001,
		0xC78, 0x656F0001,
		0xC78, 0x64700001,
		0xC78, 0x63710001,
		0xC78, 0x62720001,
		0xC78, 0x61730001,
		0xC78, 0x49740001,
		0xC78, 0x48750001,
		0xC78, 0x47760001,
		0xC78, 0x46770001,
		0xC78, 0x45780001,
		0xC78, 0x44790001,
		0xC78, 0x437A0001,
		0xC78, 0x427B0001,
		0xC78, 0x417C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040020,
		0xC58, 0x00000020,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
		0xC78, 0xFE400001,
		0xC78, 0xFD410001,
		0xC78, 0xFC420001,
		0xC78, 0xFB430001,
		0xC78, 0xFA440001,
		0xC78, 0xF9450001,
		0xC78, 0xF8460001,
		0xC78, 0xF7470001,
		0xC78, 0xF6480001,
		0xC78, 0xF5490001,
		0xC78, 0xF44A0001,
		0xC78, 0xF34B0001,
		0xC78, 0xF24C0001,
		0xC78, 0xF14D0001,
		0xC78, 0xF04E0001,
		0xC78, 0xEF4F0001,
		0xC78, 0xEE500001,
		0xC78, 0xED510001,
		0xC78, 0xEC520001,
		0xC78, 0xEB530001,
		0xC78, 0xEA540001,
		0xC78, 0xE9550001,
		0xC78, 0xE8560001,
		0xC78, 0xE7570001,
		0xC78, 0xE6580001,
		0xC78, 0xE5590001,
		0xC78, 0xE45A0001,
		0xC78, 0xE35B0001,
		0xC78, 0x885C0001,
		0xC78, 0x875D0001,
		0xC78, 0xAA5E0001,
		0xC78, 0xA95F0001,
		0xC78, 0xA8600001,
		0xC78, 0xA7610001,
		0xC78, 0xA6620001,
		0xC78, 0xA5630001,
		0xC78, 0x66640001,
		0xC78, 0x65650001,
		0xC78, 0x64660001,
		0xC78, 0x63670001,
		0xC78, 0x62680001,
		0xC78, 0x49690001,
		0xC78, 0x486A0001,
		0xC78, 0x476B0001,
		0xC78, 0x466C0001,
		0xC78, 0x456D0001,
		0xC78, 0x446E0001,
		0xC78, 0x436F0001,
		0xC78, 0x42700001,
		0xC78, 0x41710001,
		0xC78, 0x40720001,
		0xC78, 0x40730001,
		0xC78, 0x40740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040220,
		0xC58, 0x00000220,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xC78, 0xFE400001,
		0xC78, 0xFE410001,
		0xC78, 0xFE420001,
		0xC78, 0xFD430001,
		0xC78, 0xFC440001,
		0xC78, 0xFB450001,
		0xC78, 0xFA460001,
		0xC78, 0xF9470001,
		0xC78, 0xF8480001,
		0xC78, 0xF7490001,
		0xC78, 0xF64A0001,
		0xC78, 0xF54B0001,
		0xC78, 0xF44C0001,
		0xC78, 0xF34D0001,
		0xC78, 0xF24E0001,
		0xC78, 0xF14F0001,
		0xC78, 0xF0500001,
		0xC78, 0xEF510001,
		0xC78, 0xEE520001,
		0xC78, 0xED530001,
		0xC78, 0xEC540001,
		0xC78, 0xEB550001,
		0xC78, 0xEA560001,
		0xC78, 0xE9570001,
		0xC78, 0xE8580001,
		0xC78, 0xE7590001,
		0xC78, 0xE65A0001,
		0xC78, 0xE55B0001,
		0xC78, 0xE45C0001,
		0xC78, 0xE35D0001,
		0xC78, 0x885E0001,
		0xC78, 0x875F0001,
		0xC78, 0xAA600001,
		0xC78, 0xA9610001,
		0xC78, 0xA8620001,
		0xC78, 0xA7630001,
		0xC78, 0xA6640001,
		0xC78, 0xA5650001,
		0xC78, 0x66660001,
		0xC78, 0x65670001,
		0xC78, 0x64680001,
		0xC78, 0x63690001,
		0xC78, 0x626A0001,
		0xC78, 0x496B0001,
		0xC78, 0x486C0001,
		0xC78, 0x476D0001,
		0xC78, 0x466E0001,
		0xC78, 0x456F0001,
		0xC78, 0x44700001,
		0xC78, 0x43710001,
		0xC78, 0x42720001,
		0xC78, 0x41730001,
		0xC78, 0x40740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040220,
		0xC58, 0x00000220,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xC78, 0xFE400001,
		0xC78, 0xFE410001,
		0xC78, 0xFE420001,
		0xC78, 0xFE430001,
		0xC78, 0xFD440001,
		0xC78, 0xFC450001,
		0xC78, 0xFB460001,
		0xC78, 0xFA470001,
		0xC78, 0xF9480001,
		0xC78, 0xF8490001,
		0xC78, 0xF74A0001,
		0xC78, 0xF64B0001,
		0xC78, 0xF54C0001,
		0xC78, 0xF44D0001,
		0xC78, 0xF34E0001,
		0xC78, 0xF24F0001,
		0xC78, 0xF1500001,
		0xC78, 0xF0510001,
		0xC78, 0xEF520001,
		0xC78, 0xEE530001,
		0xC78, 0xED540001,
		0xC78, 0xEC550001,
		0xC78, 0xEB560001,
		0xC78, 0xEA570001,
		0xC78, 0xE9580001,
		0xC78, 0xE8590001,
		0xC78, 0xE75A0001,
		0xC78, 0xE65B0001,
		0xC78, 0xE55C0001,
		0xC78, 0xE45D0001,
		0xC78, 0xE35E0001,
		0xC78, 0x885F0001,
		0xC78, 0x87600001,
		0xC78, 0xAA610001,
		0xC78, 0xA9620001,
		0xC78, 0xA8630001,
		0xC78, 0xA7640001,
		0xC78, 0xA6650001,
		0xC78, 0xA5660001,
		0xC78, 0x66670001,
		0xC78, 0x65680001,
		0xC78, 0x64690001,
		0xC78, 0x636A0001,
		0xC78, 0x626B0001,
		0xC78, 0x496C0001,
		0xC78, 0x486D0001,
		0xC78, 0x476E0001,
		0xC78, 0x466F0001,
		0xC78, 0x45700001,
		0xC78, 0x44710001,
		0xC78, 0x43720001,
		0xC78, 0x42730001,
		0xC78, 0x41740001,
		0xC78, 0x40750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040220,
		0xC58, 0x00000220,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xC78, 0xFE400001,
		0xC78, 0xFE410001,
		0xC78, 0xFE420001,
		0xC78, 0xFE430001,
		0xC78, 0xFE440001,
		0xC78, 0xFD450001,
		0xC78, 0xFC460001,
		0xC78, 0xFB470001,
		0xC78, 0xFA480001,
		0xC78, 0xF9490001,
		0xC78, 0xF84A0001,
		0xC78, 0xF74B0001,
		0xC78, 0xF64C0001,
		0xC78, 0xF54D0001,
		0xC78, 0xF44E0001,
		0xC78, 0xF34F0001,
		0xC78, 0xF2500001,
		0xC78, 0xF1510001,
		0xC78, 0xF0520001,
		0xC78, 0xEF530001,
		0xC78, 0xEE540001,
		0xC78, 0xED550001,
		0xC78, 0xEC560001,
		0xC78, 0xEB570001,
		0xC78, 0xEA580001,
		0xC78, 0xE9590001,
		0xC78, 0xE85A0001,
		0xC78, 0xE75B0001,
		0xC78, 0xE65C0001,
		0xC78, 0xE55D0001,
		0xC78, 0xE45E0001,
		0xC78, 0xE35F0001,
		0xC78, 0x88600001,
		0xC78, 0x87610001,
		0xC78, 0xAA620001,
		0xC78, 0xA9630001,
		0xC78, 0xA8640001,
		0xC78, 0xA7650001,
		0xC78, 0xA6660001,
		0xC78, 0xA5670001,
		0xC78, 0x66680001,
		0xC78, 0x65690001,
		0xC78, 0x646A0001,
		0xC78, 0x636B0001,
		0xC78, 0x626C0001,
		0xC78, 0x496D0001,
		0xC78, 0x486E0001,
		0xC78, 0x476F0001,
		0xC78, 0x46700001,
		0xC78, 0x45710001,
		0xC78, 0x44720001,
		0xC78, 0x43730001,
		0xC78, 0x42740001,
		0xC78, 0x41750001,
		0xC78, 0x40760001,
		0xC78, 0x40770001,
		0xC78, 0x40780001,
		0xC78, 0x40790001,
		0xC78, 0x407A0001,
		0xC78, 0x407B0001,
		0xC78, 0x407C0001,
		0xC78, 0x407D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040220,
		0xC58, 0x00000220,
	0xA0000000,	0x00000000,
		0xC78, 0xFB400001,
		0xC78, 0xFB410001,
		0xC78, 0xFB420001,
		0xC78, 0xFB430001,
		0xC78, 0xFB440001,
		0xC78, 0xFB450001,
		0xC78, 0xFB460001,
		0xC78, 0xFA470001,
		0xC78, 0xF9480001,
		0xC78, 0xF8490001,
		0xC78, 0xF74A0001,
		0xC78, 0xF64B0001,
		0xC78, 0xF54C0001,
		0xC78, 0xF44D0001,
		0xC78, 0xF34E0001,
		0xC78, 0xF24F0001,
		0xC78, 0xF1500001,
		0xC78, 0xF0510001,
		0xC78, 0xEF520001,
		0xC78, 0xEE530001,
		0xC78, 0xED540001,
		0xC78, 0xEC550001,
		0xC78, 0xEB560001,
		0xC78, 0xEA570001,
		0xC78, 0xE9580001,
		0xC78, 0xE8590001,
		0xC78, 0xE75A0001,
		0xC78, 0xE65B0001,
		0xC78, 0xE55C0001,
		0xC78, 0xE45D0001,
		0xC78, 0xE35E0001,
		0xC78, 0xE25F0001,
		0xC78, 0xE1600001,
		0xC78, 0x8A610001,
		0xC78, 0x89620001,
		0xC78, 0x88630001,
		0xC78, 0x87640001,
		0xC78, 0x86650001,
		0xC78, 0x85660001,
		0xC78, 0x84670001,
		0xC78, 0x83680001,
		0xC78, 0x82690001,
		0xC78, 0x6B6A0001,
		0xC78, 0x6A6B0001,
		0xC78, 0x696C0001,
		0xC78, 0x686D0001,
		0xC78, 0x676E0001,
		0xC78, 0x666F0001,
		0xC78, 0x65700001,
		0xC78, 0x64710001,
		0xC78, 0x63720001,
		0xC78, 0x62730001,
		0xC78, 0x61740001,
		0xC78, 0x49750001,
		0xC78, 0x48760001,
		0xC78, 0x47770001,
		0xC78, 0x46780001,
		0xC78, 0x45790001,
		0xC78, 0x447A0001,
		0xC78, 0x437B0001,
		0xC78, 0x427C0001,
		0xC78, 0x417D0001,
		0xC78, 0x407E0001,
		0xC78, 0x407F0001,
		0xC50, 0x00040020,
		0xC58, 0x00000020,
	0xB0000000,	0x00000000,

};

void
ODM_ReadAndConfig_MP_8192E_AGC_TAB(
	IN   PDM_ODM_T  pDM_Odm
)
{
	u4Byte     i         = 0;
	u1Byte     cCond;
	BOOLEAN bMatched = TRUE, bSkipped = FALSE;
	u4Byte     ArrayLen    = sizeof(Array_MP_8192E_AGC_TAB)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8192E_AGC_TAB;
	
	u4Byte	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8192E_AGC_TAB\n"));

	while ((i + 1) < ArrayLen) {
		v1 = Array[i];
		v2 = Array[i + 1];

		if (v1 & (BIT31 | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT31) {/* positive condition*/
				cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
				if (cCond == COND_ENDIF) {/*end*/
					bMatched = TRUE;
					bSkipped = FALSE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ENDIF\n"));
				} else if (cCond == COND_ELSE) { /*else*/
					bMatched = bSkipped?FALSE:TRUE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ELSE\n"));
				}
				else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT30) { /*negative condition*/
				if (bSkipped == FALSE) {
					if (CheckPositive(pDM_Odm, pre_v1, pre_v2, v1, v2)) {
						bMatched = TRUE;
						bSkipped = TRUE;
					} else {
						bMatched = FALSE;
						bSkipped = FALSE;
					}
				} else
					bMatched = FALSE;
			}
		} else {
			if (bMatched)
				odm_ConfigBB_AGC_8192E(pDM_Odm, v1, bMaskDWord, v2);
		}
		i = i + 2;
	}
}

u4Byte
ODM_GetVersion_MP_8192E_AGC_TAB(void)
{
	   return 50;
}

/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

u4Byte Array_MP_8192E_PHY_REG[] = { 
		0x800, 0x80040000,
		0x804, 0x00000003,
		0x808, 0x0000FC00,
		0x80C, 0x0000000A,
		0x810, 0x10001331,
		0x814, 0x020C3D10,
		0x818, 0x02220385,
		0x81C, 0x00000000,
		0x820, 0x01000100,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x824, 0x00390004,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0x824, 0x00390004,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0x824, 0x00390004,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0x824, 0x00390004,
	0xA0000000,	0x00000000,
		0x824, 0x00390204,
	0xB0000000,	0x00000000,
		0x828, 0x01000100,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0x82C, 0x00390004,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0x82C, 0x00390004,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0x82C, 0x00390004,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0x82C, 0x00390004,
	0xA0000000,	0x00000000,
		0x82C, 0x00390204,
	0xB0000000,	0x00000000,
		0x830, 0x32323232,
		0x834, 0x30303030,
		0x838, 0x30303030,
		0x83C, 0x30303030,
		0x840, 0x00010000,
		0x844, 0x00010000,
		0x848, 0x28282828,
		0x84C, 0x28282828,
		0x850, 0x00000000,
		0x854, 0x00000000,
		0x858, 0x009A009A,
		0x85C, 0x01000014,
		0x860, 0x66F60000,
		0x864, 0x061F0000,
		0x868, 0x30303030,
		0x86C, 0x30303030,
		0x870, 0x00000000,
		0x874, 0x55004200,
		0x878, 0x08080808,
		0x87C, 0x00000000,
		0x880, 0xB0000C1C,
		0x884, 0x00000001,
		0x888, 0x00000000,
		0x88C, 0xCC0000C0,
		0x890, 0x00000800,
		0x894, 0xFFFFFFFE,
		0x898, 0x40302010,
		0x900, 0x00000000,
		0x904, 0x00000023,
		0x908, 0x00000000,
		0x90C, 0x81121313,
		0x910, 0x806C0001,
		0x914, 0x00000001,
		0x918, 0x00000000,
		0x91C, 0x00010000,
		0x924, 0x00000001,
		0x928, 0x00000000,
		0x92C, 0x00000000,
		0x930, 0x00000000,
		0x934, 0x00000000,
		0x938, 0x00000000,
		0x93C, 0x00000000,
		0x940, 0x00000000,
		0x944, 0x00000000,
		0x94C, 0x00000008,
		0xA00, 0x00D0C7C8,
		0xA04, 0x81FF800C,
		0xA08, 0x8C838300,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0xA0C, 0x2E2E120F,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xA0C, 0x2E2E120F,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xA0C, 0x2E2E120F,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xA0C, 0x2E2E120F,
	0xA0000000,	0x00000000,
		0xA0C, 0x2E68120F,
	0xB0000000,	0x00000000,
		0xA10, 0x95009B78,
		0xA14, 0x1114D028,
		0xA18, 0x00881117,
		0xA1C, 0x89140F00,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
		0xA20, 0x12130000,
		0xA24, 0x060A0D10,
		0xA28, 0x00000103,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
		0xA20, 0x12130000,
		0xA24, 0x060A0D10,
		0xA28, 0x00000103,
	0xA0000000,	0x00000000,
		0xA20, 0x1A1B0000,
		0xA24, 0x090E1317,
		0xA28, 0x00000204,
	0xB0000000,	0x00000000,
		0xA2C, 0x00D30000,
		0xA70, 0x101FFF80,
		0xA74, 0x00000007,
		0xA78, 0x00000900,
		0xA7C, 0x225B0606,
	0x80000001,	0x00000000,	0x40000000,	0x00000000,
		0xA80, 0x21807531,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xA80, 0x21807531,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xA80, 0x21807531,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xA80, 0x21807531,
	0xA0000000,	0x00000000,
		0xA80, 0x218075B1,
	0xB0000000,	0x00000000,
		0xB38, 0x00000000,
		0xC00, 0x48071D40,
		0xC04, 0x03A05633,
		0xC08, 0x000000E4,
		0xC0C, 0x6C6C6C6C,
		0xC10, 0x08800000,
		0xC14, 0x40000100,
		0xC18, 0x08800000,
		0xC1C, 0x40000100,
		0xC20, 0x00000000,
		0xC24, 0x00000000,
		0xC28, 0x00000000,
		0xC2C, 0x00000000,
		0xC30, 0x69E9AC47,
		0xC34, 0x469652AF,
		0xC38, 0x49795994,
		0xC3C, 0x0A97971C,
		0xC40, 0x1F7C403F,
		0xC44, 0x000100B7,
		0xC48, 0xEC020107,
		0xC4C, 0x007F037F,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xC50, 0x00340220,
	0xA0000000,	0x00000000,
		0xC50, 0x00340020,
	0xB0000000,	0x00000000,
		0xC54, 0x0080801F,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0x90000001,	0x00000000,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0x90000001,	0x00000005,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0x90000001,	0x0000000a,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0x90000001,	0x0000000f,	0x40000000,	0x00000000,
		0xC58, 0x00000220,
	0xA0000000,	0x00000000,
		0xC58, 0x00000020,
	0xB0000000,	0x00000000,
		0xC5C, 0x00248492,
		0xC60, 0x00000000,
		0xC64, 0x7112848B,
		0xC68, 0x47C00BFF,
		0xC6C, 0x00000036,
		0xC70, 0x00000600,
		0xC74, 0x02013169,
		0xC78, 0x0000001F,
		0xC7C, 0x00B91612,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
		0xC80, 0x2D4000B5,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
		0xC80, 0x2D4000B5,
	0xA0000000,	0x00000000,
		0xC80, 0x40000100,
	0xB0000000,	0x00000000,
		0xC84, 0x21F60000,
	0x80000003,	0x00000000,	0x40000000,	0x00000000,
		0xC88, 0x2D4000B5,
	0x90000002,	0x00000000,	0x40000000,	0x00000000,
		0xC88, 0x2D4000B5,
	0xA0000000,	0x00000000,
		0xC88, 0x40000100,
	0xB0000000,	0x00000000,
		0xC8C, 0xA0E40000,
		0xC90, 0x00121820,
		0xC94, 0x00000000,
		0xC98, 0x00121820,
		0xC9C, 0x00007F7F,
		0xCA0, 0x00000000,
		0xCA4, 0x000300A0,
		0xCA8, 0x00000000,
		0xCAC, 0x00000000,
		0xCB0, 0x00000000,
		0xCB4, 0x00000000,
		0xCB8, 0x00000000,
		0xCBC, 0x28000000,
		0xCC0, 0x00000000,
		0xCC4, 0x00000000,
		0xCC8, 0x00000000,
		0xCCC, 0x00000000,
		0xCD0, 0x00000000,
		0xCD4, 0x00000000,
		0xCD8, 0x64B22427,
		0xCDC, 0x00766932,
		0xCE0, 0x00222222,
		0xCE4, 0x00040000,
		0xCE8, 0x77644302,
		0xCEC, 0x2F97D40C,
		0xD00, 0x00080740,
		0xD04, 0x00020403,
		0xD08, 0x0000907F,
		0xD0C, 0x20010201,
		0xD10, 0xA0633333,
		0xD14, 0x3333BC43,
		0xD18, 0x7A8F5B6B,
		0xD1C, 0x0000007F,
		0xD2C, 0xCC979975,
		0xD30, 0x00000000,
		0xD34, 0x80608000,
		0xD38, 0x00000000,
		0xD3C, 0x00127353,
		0xD40, 0x00000000,
		0xD44, 0x00000000,
		0xD48, 0x00000000,
		0xD4C, 0x00000000,
		0xD50, 0x6437140A,
		0xD54, 0x00000000,
		0xD58, 0x00000282,
		0xD5C, 0x30032064,
		0xD60, 0x4653DE68,
		0xD64, 0x04518A3C,
		0xD68, 0x00002101,
		0xD6C, 0x2A201C16,
		0xD70, 0x1812362E,
		0xD74, 0x322C2220,
		0xD78, 0x000E3C24,
		0xD80, 0x01081008,
		0xD84, 0x00000800,
		0xD88, 0xF0B50000,
		0xE00, 0x30303030,
		0xE04, 0x30303030,
		0xE08, 0x03903030,
		0xE10, 0x30303030,
		0xE14, 0x30303030,
		0xE18, 0x30303030,
		0xE1C, 0x30303030,
		0xE28, 0x00000000,
		0xE30, 0x1000DC1F,
		0xE34, 0x10008C1F,
		0xE38, 0x02140102,
		0xE3C, 0x681604C2,
		0xE40, 0x01007C00,
		0xE44, 0x01004800,
		0xE48, 0xFB000000,
		0xE4C, 0x000028D1,
		0xE50, 0x1000DC1F,
		0xE54, 0x10008C1F,
		0xE58, 0x02140102,
		0xE5C, 0x28160D05,
		0xE60, 0x00000048,
		0xE68, 0x0FC05656,
		0xE6C, 0x03C09696,
		0xE70, 0x03C09696,
		0xE74, 0x0C005656,
		0xE78, 0x0C005656,
		0xE7C, 0x0C005656,
		0xE80, 0x0C005656,
		0xE84, 0x03C09696,
		0xE88, 0x0C005656,
		0xE8C, 0x03C09696,
		0xED0, 0x03C09696,
		0xED4, 0x03C09696,
		0xED8, 0x03C09696,
		0xEDC, 0x0000D6D6,
		0xEE0, 0x0000D6D6,
		0xEEC, 0x0FC01616,
		0xEE4, 0xB0000C1C,
		0xEE8, 0x00000001,
		0xF14, 0x00000003,
		0xF4C, 0x00000000,
		0xF00, 0x00000300,

};

void
ODM_ReadAndConfig_MP_8192E_PHY_REG(
	IN   PDM_ODM_T  pDM_Odm
)
{
	u4Byte     i         = 0;
	u1Byte     cCond;
	BOOLEAN bMatched = TRUE, bSkipped = FALSE;
	u4Byte     ArrayLen    = sizeof(Array_MP_8192E_PHY_REG)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8192E_PHY_REG;
	
	u4Byte	v1 = 0, v2 = 0, pre_v1 = 0, pre_v2 = 0;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8192E_PHY_REG\n"));

	while ((i + 1) < ArrayLen) {
		v1 = Array[i];
		v2 = Array[i + 1];

		if (v1 & (BIT31 | BIT30)) {/*positive & negative condition*/
			if (v1 & BIT31) {/* positive condition*/
				cCond  = (u1Byte)((v1 & (BIT29|BIT28)) >> 28);
				if (cCond == COND_ENDIF) {/*end*/
					bMatched = TRUE;
					bSkipped = FALSE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ENDIF\n"));
				} else if (cCond == COND_ELSE) { /*else*/
					bMatched = bSkipped?FALSE:TRUE;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ELSE\n"));
				}
				else {/*if , else if*/
					pre_v1 = v1;
					pre_v2 = v2;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("IF or ELSE IF\n"));
				}
			} else if (v1 & BIT30) { /*negative condition*/
				if (bSkipped == FALSE) {
					if (CheckPositive(pDM_Odm, pre_v1, pre_v2, v1, v2)) {
						bMatched = TRUE;
						bSkipped = TRUE;
					} else {
						bMatched = FALSE;
						bSkipped = FALSE;
					}
				} else
					bMatched = FALSE;
			}
		} else {
			if (bMatched)
				odm_ConfigBB_PHY_8192E(pDM_Odm, v1, bMaskDWord, v2);
		}
		i = i + 2;
	}
}

u4Byte
ODM_GetVersion_MP_8192E_PHY_REG(void)
{
	   return 50;
}

/******************************************************************************
*                           PHY_REG_PG.TXT
******************************************************************************/

u4Byte Array_MP_8192E_PHY_REG_PG[] = { 
	0, 0, 0, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 1, 0x00000e08, 0x0000ff00, 0x00003200,
	0, 0, 0, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 1, 0x0000086c, 0xffffff00, 0x32323200,
	0, 0, 0, 0x00000e00, 0xffffffff, 0x36364040,
	0, 0, 1, 0x00000e00, 0xffffffff, 0x34343636,
	0, 0, 0, 0x00000e04, 0xffffffff, 0x28283234,
	0, 0, 1, 0x00000e04, 0xffffffff, 0x28283032,
	0, 0, 0, 0x00000e10, 0xffffffff, 0x38383840,
	0, 0, 1, 0x00000e10, 0xffffffff, 0x34363840,
	0, 0, 0, 0x00000e14, 0xffffffff, 0x26283038,
	0, 0, 1, 0x00000e14, 0xffffffff, 0x26283032,
	0, 0, 1, 0x00000e18, 0xffffffff, 0x36384040,
	0, 0, 1, 0x00000e1c, 0xffffffff, 0x24262832,
	0, 1, 0, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 1, 0x00000838, 0xffffff00, 0x32323200,
	0, 1, 0, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 1, 0x0000086c, 0x000000ff, 0x00000032,
	0, 1, 0, 0x00000830, 0xffffffff, 0x36364040,
	0, 1, 1, 0x00000830, 0xffffffff, 0x34343636,
	0, 1, 0, 0x00000834, 0xffffffff, 0x28283234,
	0, 1, 1, 0x00000834, 0xffffffff, 0x28283032,
	0, 1, 0, 0x0000083c, 0xffffffff, 0x38383840,
	0, 1, 1, 0x0000083c, 0xffffffff, 0x34363840,
	0, 1, 0, 0x00000848, 0xffffffff, 0x26283038,
	0, 1, 1, 0x00000848, 0xffffffff, 0x26283032,
	0, 1, 1, 0x0000084c, 0xffffffff, 0x36384040,
	0, 1, 1, 0x00000868, 0xffffffff, 0x24262832
};

void
ODM_ReadAndConfig_MP_8192E_PHY_REG_PG(
	IN   PDM_ODM_T  pDM_Odm
)
{
	u4Byte     i         = 0;
	u4Byte     ArrayLen    = sizeof(Array_MP_8192E_PHY_REG_PG)/sizeof(u4Byte);
	pu4Byte    Array       = Array_MP_8192E_PHY_REG_PG;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	PlatformZeroMemory(pHalData->BufOfLinesPwrByRate, MAX_LINES_HWCONFIG_TXT*MAX_BYTES_LINE_HWCONFIG_TXT);
	pHalData->nLinesReadPwrByRate = ArrayLen/6;
#endif

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("===> ODM_ReadAndConfig_MP_8192E_PHY_REG_PG\n"));

	pDM_Odm->PhyRegPgVersion = 1;
	pDM_Odm->PhyRegPgValueType = PHY_REG_PG_EXACT_VALUE;

	for (i = 0; i < ArrayLen; i += 6) {
		u4Byte v1 = Array[i];
		u4Byte v2 = Array[i+1];
		u4Byte v3 = Array[i+2];
		u4Byte v4 = Array[i+3];
		u4Byte v5 = Array[i+4];
		u4Byte v6 = Array[i+5];

	    odm_ConfigBB_PHY_REG_PG_8192E(pDM_Odm, v1, v2, v3, v4, v5, v6);

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	rsprintf(pHalData->BufOfLinesPwrByRate[i/6], 100, "%s, %s, %s, 0x%X, 0x%08X, 0x%08X,",
		(v1 == 0?"2.4G":"  5G"), (v2 == 0?"A":"B"), (v3 == 0?"1Tx":"2Tx"), v4, v5, v6);
#endif
	}
}



#endif /* end of HWIMG_SUPPORT*/

