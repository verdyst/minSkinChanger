#pragma once
#include <Windows.h>
#include <vector>
#include "Vec.h"
#include "offsets.h"
#include "QAngles.h"
#include "entity.h"

#define STR_MERGE_IMPL(a, b) a##b
#define STR_MERGE(a, b) STR_MERGE_IMPL(a,b)
#define MAKE_PAD(size) STR_MERGE(_pad, __COUNTER__) [size]
#define DEFINE_MEMBER_N(type, name, offset) struct {unsigned char MAKE_PAD(offset); type name;}




class QAngle;


class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};

typedef unsigned int CRC32_t;

void CRC32_Init(CRC32_t* pulCRC);
void CRC32_ProcessBuffer(CRC32_t* pulCRC, const void* p, int len);
void CRC32_Final(CRC32_t* pulCRC);
CRC32_t	CRC32_GetTableEntry(unsigned int slot);

inline CRC32_t CRC32_ProcessSingleBuffer(const void* p, int len)
{
	CRC32_t crc;

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, p, len);
	CRC32_Final(&crc);

	return crc;
}



struct CUserCmd
{
	int pad;
	// For matching server and client commands for debugging
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	QAngle viewangles;
	QAngle aimdirection; // let's just see if this works.
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	byte    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

	// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;

};

class IClientMode
{
	// Misc.
public:

	virtual			~IClientMode() {}

	// Called before the HUD is initialized.
	virtual void	InitViewport() = 0;

	// One time init when .dll is first loaded.
	virtual void	Init() = 0;

	// Called when vgui is shutting down.
	virtual void	VGui_Shutdown() = 0;

	// One time call when dll is shutting down
	virtual void	Shutdown() = 0;

	// Called when switching from one IClientMode to another.
	// This can re-layout the view and such.
	// Note that Enable and Disable are called when the DLL initializes and shuts down.
	virtual void	Enable() = 0;

	// Called when it's about to go into another client mode.
	virtual void	Disable() = 0;

	// Called when initializing or when the view changes.
	// This should move the viewport into the correct position.
	virtual void	Layout() = 0;

	// Gets at the viewport, if there is one...
	virtual void fn10() = 0;

	// Gets at the viewports vgui panel animation controller, if there is one...
	virtual void* GetViewportAnimationController() = 0;

	// called every time shared client dll/engine data gets changed,
	// and gives the cdll a chance to modify the data.
	virtual void	ProcessInput(bool bActive) = 0;

	// The mode can choose to draw/not draw entities.
	virtual bool	ShouldDrawDetailObjects() = 0;
	virtual bool	ShouldDrawEntity(void* pEnt) = 0;
	virtual bool	ShouldDrawLocalPlayer(void* pPlayer) = 0;
	virtual bool	ShouldDrawParticles() = 0;

	// The mode can choose to not draw fog
	virtual bool	ShouldDrawFog(void) = 0;

	virtual void	OverrideView(void* pSetup) = 0;
	virtual void fn20() = 0;
	virtual void	StartMessageMode(int iMessageModeType) = 0;
	virtual void fn22() = 0;
	virtual void	OverrideMouseInput(float* x, float* y) = 0;
	virtual bool	CreateMove(float flInputSampleTime, CUserCmd* cmd) = 0;
};

typedef void(__stdcall* tCreateMove)(float flInputSampleTime, CUserCmd* cmd);

// static tCreateMove oCreateMove;
// we'll keep function typedef's and declarations in here, as it makes sense since it
// pertains to the SDK.

void __stdcall hkCreateMove(float flInputSampleTime, CUserCmd* cmd);

extern IClientMode* g_pClientMode;

class IClientEntityList
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual void* GetClientNetworkable(int entnum) = 0;
	virtual void* GetClientNetworkableFromHandle(int hEnt) = 0;
	virtual void* GetClientUnknownFromHandle(int hEnt) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual void* GetClientEntity(int entnum) = 0;
	virtual void* GetClientEntityFromHandle(int hEnt) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities(bool bIncludeNonNetworkable) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex(void) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities(int maxents) = 0;
	virtual int					GetMaxEntities() = 0;
};

//typedef void* (__cdecl* tCreateInterface)(const char* name, int* returnCode);

//void* GetInterface(const char* dllname, const char* interfacename);
struct model_t;
struct trace_t;
struct studiohdr_t;

class IVModelInfo
{
public:
	virtual	~IVModelInfo(void) { }

	// Returns model_t* pointer for a model given a precached or dynamic model index.
	virtual const model_t* GetModel(int modelindex) = 0;

	// Returns index of model by name for precached or known dynamic models.
	// Does not adjust reference count for dynamic models.
	virtual int						GetModelIndex(const char* name) const = 0;

	// Returns name of model
	virtual const char* GetModelName(const model_t* model) const = 0;
	virtual void* GetVCollide(const model_t* model) const = 0;
	virtual void* GetVCollide(int modelindex) const = 0;
	virtual void					GetModelBounds(const model_t* model, Vec3& mins, Vec3& maxs) const = 0;
	virtual	void					GetModelRenderBounds(const model_t* model, Vec3& mins, Vec3& maxs) const = 0;
	virtual int						GetModelFrameCount(const model_t* model) const = 0;
	virtual int						GetModelType(const model_t* model) const = 0;
	virtual void* GetModelExtraData(const model_t* model) = 0;
	virtual bool					ModelHasMaterialProxy(const model_t* model) const = 0;
	virtual bool					IsTranslucent(model_t const* model) const = 0;
	virtual bool					IsTranslucentTwoPass(const model_t* model) const = 0;
	virtual void					Unused0() {};
	virtual void ComputeTranslucencyType(const model_t* model, int nSkin, int nBody) = 0;
	virtual int						GetModelMaterialCount(const model_t* model) const = 0;
	virtual int 					GetModelMaterials(const model_t* model, int count, void** ppMaterial) = 0;
	virtual bool					IsModelVertexLit(const model_t* model) const = 0;
	virtual const char* GetModelKeyValueText(const model_t* model) = 0;
	virtual bool					GetModelKeyValue(const model_t* model, int& buf) = 0; // supports keyvalue blocks in submodels
	virtual float					GetModelRadius(const model_t* model) = 0;

	virtual const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** cache, const char* modelname) const = 0;
	virtual const studiohdr_t* FindModel(void* cache) const = 0;
	virtual	void* GetVirtualModel(const studiohdr_t* pStudioHdr) const = 0;
	virtual byte* GetAnimBlock(const studiohdr_t* pStudioHdr, int nBlock, bool bPreloadIfMissing) const = 0;
	virtual bool					HasAnimBlockBeenPreloaded(const studiohdr_t* pStudioHdr, int nBlock) const = 0;

	// Available on client only!!!
	virtual void					GetModelMaterialColorAndLighting(const model_t* model, Vec3 const& origin,
		QAngle const& angles, trace_t* pTrace,
		Vec3& lighting, Vec3& matColor) = 0;
	virtual void					GetIlluminationPoint(const model_t* model, void* pRenderable, Vec3 const& origin,
		QAngle const& angles, Vec3* pLightingCenter) = 0;

	virtual int						GetModelContents(int modelIndex) const = 0;
	virtual studiohdr_t* GetStudiomodel(const model_t* mod) = 0;
};

//-----------------------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------