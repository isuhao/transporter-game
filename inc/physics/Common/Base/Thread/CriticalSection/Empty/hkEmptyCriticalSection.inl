/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  Not for disclosure or distribution without Havok's
 * prior written consent.This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2008 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

inline hkCriticalSection::hkCriticalSection( int spinCount, hkBool32 addToList ) {}
inline hkCriticalSection::~hkCriticalSection() {}
inline void hkCriticalSection::enter(){}
inline bool hkCriticalSection::haveEntered (){ return true; }
inline bool hkCriticalSection::isEntered () const { return false; }
inline void hkCriticalSection::leave() {}
inline void HK_CALL hkCriticalSection::setTimersEnabled(){}
inline void HK_CALL hkCriticalSection::setTimersDisabled(){}

hkUint32 HK_CALL hkCriticalSection::atomicExchangeAdd(hkUint32* var, int value)
{
	hkUint32 r = *var;
	*var += value;
	return r;
}

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20080529)
*
* Confidential Information of Havok.  (C) Copyright 1999-2008 
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
*
* Use of this software for evaluation purposes is subject to and indicates 
* acceptance of the End User licence Agreement for this product. A copy of 
* the license is included with this software and is also available at
* www.havok.com/tryhavok
*
*/
