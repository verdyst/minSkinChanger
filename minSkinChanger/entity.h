#pragma once
#include "offsets.h"

// defined global.h in here and included entity in global.h, circular inclusion.
// if I ever need a global for this that'll be interesting.

class Entity
{
public:

	uintptr_t GetThis()
	{
		return (uintptr_t)this;
	}
	Vec3* GetVecOrigin() {
		// why is this correct here but not on prev project? Because ClientList dereferences automatically unlike the entity list
		return (Vec3*)(this + offset.m_vecOrigin);
	} // inlined to boost performance

	Vec3* GetVecViewEyes() { return (Vec3*)(this + offset.m_vecViewOffset); }

	int* GetHealth() { return (int*)(this + offset.m_iHealth); }

};