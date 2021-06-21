#include <Windows.h>
#include <vector.h>
#include <Memory.hpp>
#include <memory>
#include <mutex>
#include <vector>
#define M_PI 3.14159265358979323846f
#define M_RADPI	57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#ifndef _VARIABLES_H_
#define _VARIABLES_H_
namespace impl {
	// Important


	__declspec(selectany) std::unique_ptr<Memory> memory;

	// Storage

	__declspec(selectany) uintptr_t gBase = NULL;
	__declspec(selectany) uintptr_t base = NULL;
	__declspec(selectany) uintptr_t LocalPlayer = NULL;
	__declspec(selectany) Matrix viewMatrix;
	__declspec(selectany) uintptr_t baseNet = NULL;
	__declspec(selectany) bool shouldExit = false;
	__declspec(selectany) uintptr_t closestPlayer = NULL;
	__declspec(selectany) uintptr_t closestObject = NULL;
	__declspec(selectany) uintptr_t closestHeli = NULL;
	__declspec(selectany) uintptr_t closestHeliObj = NULL;
	__declspec(selectany) std::mutex player_list_mutex{};
	__declspec(selectany) std::vector<uintptr_t> players{};

	// Toggles

	__declspec(selectany) int lang = 0;
	__declspec(selectany) int playerESP = true;
	__declspec(selectany) int sleeperESP = true;
	__declspec(selectany) int silentAim = false;
	__declspec(selectany) int noRecoil = false;
	__declspec(selectany) int aimPrediction = false;
	__declspec(selectany) int helicopterESP = false;
	__declspec(selectany) int noSpread = false;
	__declspec(selectany) int fastReload = false;
	__declspec(selectany) int rapidFire = false;
	__declspec(selectany) int spiderMan = false;
	__declspec(selectany) int crosshair = false;
	__declspec(selectany) int instaEoka = false;
	__declspec(selectany) int automatic = false;
	__declspec(selectany) int fakeAdmin = false;
	__declspec(selectany) int inventoryESP = false;
	__declspec(selectany) int heldWeapon = true;
	__declspec(selectany) int jumpAim = false;
	__declspec(selectany) int sprintAim = false;
	__declspec(selectany) int fovCircle = true;
	__declspec(selectany) float fovSlider = 300.0f;
	__declspec(selectany) int bulletThickness = false;
	__declspec(selectany) int tracers = false;
	__declspec(selectany) int aimbot = false;
	__declspec(selectany) int skeletonESP = false;
	__declspec(selectany) int boxESP = false;
	__declspec(selectany) int scientistESP = false;
	__declspec(selectany) int heliSilent = false;
	__declspec(selectany) int neverSink = false;
	__declspec(selectany) int Runmeele = false;
	__declspec(selectany) int Antiaim = false;
	__declspec(selectany) int Omnisprint = false;
	__declspec(selectany) int fakelag = false;
	__declspec(selectany) int playerfov = false;
	__declspec(selectany) int Unlimitedjump = false;
	__declspec(selectany) int girrafe = false;
}
#endif