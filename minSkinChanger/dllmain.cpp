// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <map>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_dx9.h"
#include "../ImGui/imgui_impl_win32.h"
#include "csgosdk.h"
#include "netvars.h"
#include "hook.h"
#include "globals.h"
#include "fastAlgo.h"

#include "skinchanger.h"
#include "entity.h"
#include "DefEnums.h"
#include "skinEnums.h"
#include "sigscan.h"
#include "dx.h"
#include <sstream>

#define NUM_KNIFE_SKINS 17


// TODO: Place these in seperate header file
extern IMGUI_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

WNDPROC oWndProc;

int count(const char** arr)
{
    int i = 0;

    while (true)
    {
        if (strcmp(arr[i], "END") == 0)
            break;
        else
            i++;
    }

    return i;
}

static int ak_skin_ids[17]{ (int)AK47::Asiimov, (int)AK47::Aquamarine_Revenge, (int)AK47::Bloodsport, (int)AK47::Blue_Laminate, (int)AK47::Case_Hardened,
(int)AK47::The_Empress, (int)AK47::Fire_Serpent, (int)AK47::Fuel_Injector, (int)AK47::Legion_of_Anubis, (int)AK47::Neon_Revolution, (int)AK47::Neon_Rider, (int)AK47::Phantom_Disruptor, (int)AK47::Point_Disarray, (int)AK47::Redline, (int)AK47::Vulcan, (int)AK47::Wasteland_Rebel, (int)AK47::Wild_Lotus };

static int awp_skin_ids[13]{ (int)AWP::Asiimov, (int)AWP::Containment_Breach, (int)AWP::Dragon_Lore, (int)AWP::Fade, (int)AWP::Gungnir, 
                            (int)AWP::Hyper_Beast, (int)AWP::Lightning_Strike, (int)AWP::Manowar,(int)AWP::Medusa, (int)AWP::NeoNoir, 
                            (int)AWP::Oni_Taiji, (int)AWP::The_Prince, (int)AWP::Wildfire };

int do_everything(int a, double b)
{
    int c = a + b;
    return a * a + b * c + a / c - b / c;
}
static int glock_skin_ids[2]{ (int)GLOCK18::Fade, (int)GLOCK18::Grinder };
static int deagle_skin_ids[6]{ (int)DEAGLE::Blaze, (int)DEAGLE::Cobalt_Disruption, (int)DEAGLE::Code_Red, (int)DEAGLE::Kumicho_Dragon, (int)DEAGLE::Mecha_Industries, (int)DEAGLE::Printstream };
static const char* deagle_skin_names[7]{ "Blaze", "Cobalt Disruption", "Code Red", "Kumicho Dragon", "Mecha Industries", "Printstream", "END" };

static int m4_skin_ids[11]{ (int)M4A4::Asiimov, (int)M4A4::The_Battlestar, (int)M4A4::Bullet_Rain, (int)M4A4::Buzz_Kill, (int)M4A4::DesertStrike, (int)M4A4::Desolate_Space, (int)M4A4::The_Emperor, (int)M4A4::Howl,
                            (int)M4A4::NeoNoir, (int)M4A4::Poseidon, (int)M4A4::Royal_Paladin };

static int m4a1s_skin_ids[19]{ (int)M4A1S::Blue_Phosphor, (int)M4A1S::Bright_Water, (int)M4A1S::ChanticosFire, (int)M4A1S::Control_Panel, (int)M4A1S::Cyrex, (int)M4A1S::Decimator, (int)M4A1S::Golden_Coil,
                               (int)M4A1S::Guardian, (int)M4A1S::Hot_Rod, (int)M4A1S::Hyper_Beast, (int)M4A1S::Icarus_Fell, (int)M4A1S::Knight, (int)M4A1S::Leaded_Glass, (int)M4A1S::Master_Piece,
                               (int)M4A1S::Mecha_Industries, (int)M4A1S::Nightmare, (int)M4A1S::Player_Two, (int)M4A1S::Printstream, (int)M4A1S::Welcome_to_the_Jungle };

static int usp_skin_ids[9]{ (int)USPS::Caiman, (int)USPS::Cyrex, (int)USPS::Guardian, (int)USPS::Kill_Confirmed, (int)USPS::NeoNoir, (int)USPS::Orion, (int)USPS::Road_Rash, (int)USPS::Serum, (int)USPS::Stainless };

static const char* ak_skin_names[18]{ "Asiimov", "Aquamarine Revenge", "Bloodsport", "Blue Laminate", "Case Hardened", "Empress", "Fire Serpent", "Fuel Injector", "Legion of Anubis", "Neon Revolution",
                                        "Neon Rider", "Phantom Disruptor", "Point Disarray", "Redline", "Vulcan", "Wasteland Revel", "Wild Lotus", "END" };
static const char* awp_skin_names[14]{ "Asiimov", "Containment Breach", "Dragon Lore", "Fade", "Gungnir", "Hyper Beast", "Lightning Strike", "Man-o-War", "Meduda", "Neo-Noir", "Oni-Taiji", 
                                       "The Prince", "Wildfire", "END" };
static const char* glock_skin_names[3]{ "Fade", "Grinder", "END" };

static const char* m4_skin_names[12]{ "Asiimov", "Battlestar", "Bullet Rain", "Buzz Kill", "Desert Strike", "Desolate Space", "Emperor", "Howl", "Neo-Noir", "Poseidon", "Royal Paladin", "END" };
static const char* m4a1s_skin_names[20]{ "Blue Phosphor", "Bright Water", "Chantico's Fire", "Control Panel", "Cyrex", "Decimator",
                                        "Golden Coil", "Guardian", "Hot Rod", "Hyper Beast", "Icarus Fell", "Knight", "Leaded Glass",
                                        "Master Piece", "Mecha Industries", "Nightmare", "Player Two", "Printstream", "Welcome to the Jungle", "END" };


static const char* usp_skin_names[10]{ "Caiman", "Cyrex", "Guardian", "Kill Confirmed", "Neo-Noir", "Orion", "Road Rash", "Serum", "Stainless", "END" };



static const char* weapon_names[7]{ "AK-47", "AWP", "Glock", "USP-S", "M4A4", "M4A1-S", "DEAGLE" };
static int weapons[7]{ WEAPON_AK47, WEAPON_AWP, WEAPON_GLOCK, WEAPON_USP_SILENCER, WEAPON_M4A1, WEAPON_M4A1_SILENCER, WEAPON_DEAGLE };

static int* weapon_id_list[7]{ ak_skin_ids, awp_skin_ids, glock_skin_ids, usp_skin_ids, m4_skin_ids, m4a1s_skin_ids, deagle_skin_ids };
static const char** skin_name_list[7]{ ak_skin_names, awp_skin_names, glock_skin_names, usp_skin_names, m4_skin_names, m4a1s_skin_names, deagle_skin_names };

static int current_weapon = 0;
static int current_skin = 0;

// ------------------------- knife section ------------------------------

static int knife_ids[4]{ WEAPON_KNIFE_KARAMBIT, WEAPON_KNIFE_M9_BAYONET, WEAPON_KNIFE_BAYONET, WEAPON_KNIFE_FLIP };

static const char* knife_names[4]{ "Karambit", "M9 Bayonet", "Bayonet", "Flip" };

static  int knife_skins[NUM_KNIFE_SKINS]{ BLUE_STEEL, CASE_HARDENED, CRIMSON_WEB, CRIMSON_WEB2, DOPPLER1, DOPPLER2, DOPPLER3, DOPPLER4, DOPPLER5, DOPPLER6, DOPPLER7, FADE, MARBLE_FADE, NIGHT, SLAUGHTER, TIGERTOOTH, ULTRAVIOLET };
static const char* knife_skin_names[NUM_KNIFE_SKINS]{ "Blue Steel", "Case Hardened", "Crimson Web", "Crimson Web Dark", "Ruby", "Sapphire", "Black Pearl", "Doppler 4", "Doppler 5", "Doppler 6", "Doppler 7", "Fade", "Marble Fade", "Night", "Slaughter", "Tiger Tooth", "Ultraviolet" };

static int current_knife = 0;
static int current_knife_skin = 0;


int modelIDs[4];

void updateModelIndex(int index)
{
    g_vars.modelIndex = modelIDs[index];
}

uintptr_t updateLocalPlayer()
{
    return (uintptr_t)altScanModIn("\x8D\x34\x85????\x89\x15????\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "client.dll", 3, 4);
}



// map weapon -> skins
// have it return a list based on weapon selected?

// could try using namespace in scope

// current method works. Problem is that for skins like elite build with multiple versions depending on skin it'd be ugly
// Fixed it to have seperate skins for each. The implementation is pretty ugly, I want to use structs in the future to be
// able to set a specific wear and stattrak or no stattrak for each item, but otherwise it works.
// WAIT you can just set the wear on a per weapon basis since we only update the array when we press the save button.

typedef void (*ForceUpdate)(void); // why does this typedef work?
uintptr_t FullUpdateAddr = altScanModIn("\xA1????\xB9????\x56\xFF\x50\x14\x8B\x34\x85", "engine.dll", 1, 0) + 0x178;
ForceUpdate FullUpdate = (ForceUpdate)FullUpdateAddr;
//ForceUpdate TestUpdate = (ForceUpdate)altScanModIn("\xA1????\xB9????\x56\xFF\x50\x14\x8B\x34\x85", "engine.dll", 0, 178); causes a loadLibrary error


// this is why you want a localPlayer class...

Entity* localPlayer; // will turn into global var
Vec3* viewAngles;
bool wasFiring = false;

typedef void* (__cdecl* tCreateInterface)(const char* name, int* returnCode);

tEndScene oEndScene = nullptr; // nullptr initialization as you can't read it if it's just declared. Undefined behavior.

std::stringstream intro;

static bool init = true;
static bool menu = false;
static bool destruct = false;


//ImGuiIO* ioCopy = nullptr;

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (!g_vars.pDevice)
    {
        
        g_vars.pDevice = o_pDevice;
    }


    SaveState(g_vars.pDevice);

    if (init) // I can change this into a function
    {
        init = false;
        ImGui::CreateContext(); // Error Line                                                                                                                                                                                                                                                                                                                                                                                                       
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();
        ImGui_ImplWin32_Init(FindWindow(L"Valve001", NULL)); // global window handle came in handy
        ImGui_ImplDX9_Init(g_vars.pDevice);
    }


    if (GetAsyncKeyState(VK_INSERT) & 1)
        menu = !menu;

    if (menu)
    {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("ImGui Window!");

        ImGui::Combo("Weapon", &current_weapon, weapon_names, IM_ARRAYSIZE(weapon_names));

        ImGui::Combo("Skins", &current_skin, skin_name_list[current_weapon], count(skin_name_list[current_weapon]));

        ImGui::Combo("Knife", &current_knife, knife_names, IM_ARRAYSIZE(knife_names));
        ImGui::Combo("Knife Skins", &current_knife_skin, knife_skin_names, IM_ARRAYSIZE(knife_skin_names));

        count(skin_name_list[current_weapon]);

        if (ImGui::Button("Save", ImVec2(50, 25)))
        {
            g_vars.paintKits[weapons[current_weapon]] = (weapon_id_list[current_weapon])[current_skin];
            g_vars.knife = knife_ids[current_knife];
            g_vars.knife_skin = knife_skins[current_knife_skin];
            updateModelIndex(current_knife);
        }

        ImGui::End();
        ImGui::EndFrame();

        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData()); // For some reason it's still calling this as we're shutting down.
    }

    if (destruct)
    {
        ImGui_ImplDX9_Shutdown(); // Shutdown's aren't even necessary. Will keep them for now.
        ImGui_ImplWin32_Shutdown(); // Maybe if you don't shutdown there's a memory leak, similar to release.
    }

    RestoreState(g_vars.pDevice);

    //oEndScene(g_vars.pDevice);
}

__declspec(naked) void midEndScene()
{
    __asm
    {
        mov[ebp - 0x20], ebx
        xor esi, esi

        mov ecx, [esp + 0x38]

        pushad
        pushfd

        push ecx
        call hkEndScene

        popfd
        popad

        jmp[g_vars.EndSceneRetAddr]
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    if (menu && ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam)) // are we going to implement the wndprochandler? We don't have to. ImGui has their own window handler since it's a menu library
        return true;
    return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}

void __stdcall hkFrameStageNotify(ClientFrameStage_t curStage)
{

    if (curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
    {
        skinChanger();
    }


    //g_vars.oFrameStageNotify(curStage);
}

__declspec(naked) void midFSM()
{
    __asm
    {
        mov eax, [eax + 0x174]

        pushad
        pushfd

        mov esi, [esp + 0x2C]

        push esi // edi's still same
        call hkFrameStageNotify

        popfd
        popad

        jmp [g_vars.FSMRetAddr]
    }
}

void* GetInterface(const char* dllname, const char* interfacename)
{
    tCreateInterface CreateInterface = (tCreateInterface)GetProcAddress(GetModuleHandleA(dllname), "CreateInterface");

    if (CreateInterface)
    {
        int returnCode = 0;
        void* mInterface = CreateInterface(interfacename, &returnCode);
        return mInterface;
    }

    return nullptr;
}


DWORD WINAPI HackThread(HMODULE hModule)
{
    
    HWND windowHndl = FindWindow(L"Valve001", NULL); // I have 0 clue how the handle for dx works but doesn't work for imgui
    oWndProc = (WNDPROC)SetWindowLongPtr(windowHndl, GWL_WNDPROC, (LONG_PTR)WndProc); // GWL_WNDPROC sets a new address for the window procedure


    g_vars.ClientEntityList = (IClientEntityList*)GetInterface("Client.dll", "VClientEntityList003"); // what is VClientEntityList formally called? What is it actually? It's the interface name.


    g_vars.modelInfo = (IVModelInfo*)GetInterface("Engine.dll", "VModelInfoClient004");

    updateModelIDs(modelIDs);

    
    g_vars.modelIndex = modelIDs[0];
   

    uintptr_t getSeqActOffset2 = altScanModIn("\x55\x8B\xEC\x53\x8B\x5D\x08\x56\x8B\xF1\x83", "client.dll", 0, 0);
    


    if (initD3D(g_vars.d3d9Device, sizeof(g_vars.d3d9Device)))
    {
        memcpy(g_vars.EndSceneBytes, (char*)g_vars.d3d9Device[42] + 26, 5);

        //oEndScene = (tEndScene)TrampHook((BYTE*)g_vars.d3d9Device[42], (BYTE*)hkEndScene, 7);

        g_vars.EndSceneRetAddr = (uintptr_t)g_vars.d3d9Device[42] + 31;

        Detour((BYTE*)g_vars.d3d9Device[42] + 26, (BYTE*)midEndScene, 5);
    }



    
    g_vars.localEnt = (Entity*)offset.localPlayer;
    
    localPlayer = (Entity*)g_vars.localEnt;


 



    // CreateMove Hook Start -------------------------------------------------------------------
    IBaseClientDll* BaseClientDll = (IBaseClientDll*)GetInterface("Client.dll", "VClient018"); // found in ida but idk why it's the only one there
    void** BaseClientDllVMT = *(void***)(BaseClientDll); // why do we want this as a pointer pointer? It points to function pointers.

    //--------------------------------- CreateMove Hook -----------------------------------------------
    IClientMode** ClientMode = *(IClientMode***)((DWORD)BaseClientDllVMT[10] + 0x5); // ClientMode is correct
    void** ClientModeVMT = *(void***)ClientMode; // allows us to access vTable

    void* createMoveAddr = *(void**)(*(uintptr_t*)(ClientModeVMT)+24 * 4);

    //memcpy(g_vars.createMoveBytes, createMoveAddr, 9);

    //memcpy(g_vars.createMoveBytes, (char*)createMoveAddr + 30, 6);

    //g_vars.oCreateMove = (tCreateMove)TrampHook((BYTE*)createMoveAddr, (BYTE*)hkCreateMove, 9);

    //g_vars.CMRetAddr = (uintptr_t)createMoveAddr + 36;
        
    //Detour((BYTE*)createMoveAddr + 30, (BYTE*)midCreateMove, 6);
    // createMove hook end ---------------------------------------------------------------------

    ClientClass* clientClass = BaseClientDll->GetAllClasses();

    //------------------------------------------------------ -----------------------------------------------------------------------------

    offset.m_nSequenceOffset = GetNetVarOffset("DT_BaseViewModel", "m_nSequence", clientClass);



    // FSN Netvars

    offset.m_hMyWeapons = GetNetVarOffset("DT_BasePlayer", "m_hMyWeapons", clientClass);
    offset.m_hActiveWeapon = GetNetVarOffset("DT_BasePlayer", "m_hActiveWeapon", clientClass);


    g_vars.m_iItemIDHigh = GetNetVarOffset("DT_BaseAttributableItem", "m_iItemIDHigh", clientClass);
    g_vars.m_nFallbackPaintKit = GetNetVarOffset("DT_BaseCombatWeapon", "m_nFallbackPaintKit", clientClass);
    g_vars.m_flFallbackWear = GetNetVarOffset("DT_BaseCombatWeapon", "m_flFallbackWear", clientClass);
    g_vars.m_nFallbackStatTrak = GetNetVarOffset("DT_BaseCombatWeapon", "m_nFallbackStatTrak", clientClass);
    g_vars.m_nFallbackSeed = GetNetVarOffset("DT_BaseCombatWeapon", "m_nFallbackSeed", clientClass);
    g_vars.m_iEntityQuality = GetNetVarOffset("DT_BaseCombatWeapon", "m_iEntityQuality", clientClass);
    offset.m_iItemDefinitionIndex = GetNetVarOffset("DT_BaseCombatWeapon", "m_iItemDefinitionIndex", clientClass);

    offset.m_iEntityQuality = GetNetVarOffset("DT_BaseAttributableItem", "m_iEntityQuality", clientClass);
    


    offset.m_iHealth = GetNetVarOffset("DT_BasePlayer", "m_iHealth", clientClass);
    offset.m_fFlags = GetNetVarOffset("DT_CSPlayer", "m_fFlags", clientClass);
    offset.m_vecOrigin = GetNetVarOffset("DT_BasePlayer", "m_vecOrigin", clientClass);
    offset.m_vecViewOffset = GetNetVarOffset("DT_CSPlayer", "m_vecViewOffset[0]", clientClass);

    //------------------------------------------------------ FrameStageNotify Hook ----------------------------------------------------

    // memcpy(g_vars.frameStageNotifyBytes, BaseClientDllVMT[37], 9); // proof that all functions don't need to be filled out just to get function pointer
    memcpy(g_vars.frameStageNotifyBytes, (char*)BaseClientDllVMT[37] + 0xB, 6);
    g_vars.FSMRetAddr = (uintptr_t)BaseClientDllVMT[37] + 0x11;
    //g_vars.oFrameStageNotify = (tFrameStageNotify)TrampHook((BYTE*)BaseClientDllVMT[37], (BYTE*)hkFrameStageNotify, 9);
    Detour((BYTE*)BaseClientDllVMT[37] + 0xB, (BYTE*)midFSM, 6);

    //uintptr_t itemDefnIndex2 = GetNetVarOffset("DT_WeaponBaseItem", "m_iItemDefinitionIndex", clientClass); // both net tables work the same



    while (!(GetAsyncKeyState(VK_END) & 1))
    {
       

        g_vars.localEnt = (Entity*)updateLocalPlayer();

        if (GetAsyncKeyState(VK_F7) & 1)
        {
            updateModelIDs(modelIDs);
            *(int*)(*(uintptr_t*)offset.dwClientState + offset.clientstate_delta_ticks) = -1; // ForceUpdate
        }

        Sleep(20);
    }

    //Patch(g_vars.createMoveBytes, (BYTE*)createMoveAddr + 30, 6); // was 9 bytes before
    Patch(g_vars.frameStageNotifyBytes, (BYTE*)BaseClientDllVMT[37] + 0xB, 6); // was 9 bytes before

    Sleep(100);

    Patch(g_vars.EndSceneBytes, (BYTE*)g_vars.d3d9Device[42] + 26, 5);

    ImGui_ImplDX9_Shutdown(); // Shutdown's aren't even necessary. Will keep them for now.
    ImGui_ImplWin32_Shutdown(); // Maybe if you don't shutdown there's a memory leak, similar to release.v

    SetWindowLongPtr(windowHndl, GWL_WNDPROC, (LONG_PTR)oWndProc); // see if this works ig
    Sleep(100); // If you release right after then sometimes it still tries to call the DrawText function with the released Font.

    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
        CloseHandle(CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, NULL));
    return TRUE;
}

