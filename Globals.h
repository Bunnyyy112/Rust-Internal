#pragma once

#include <Windows.h>
#include <vector.h>

extern uintptr_t gBase;
extern uintptr_t base;
extern Matrix viewMatrix;
extern bool shouldExit;
extern uintptr_t LocalPlayer;
extern uintptr_t baseNet;
extern uintptr_t closestPlayer;
extern uintptr_t closestObject;
extern uintptr_t closestHeli;
extern uintptr_t closestHeliObj;

// Toggles
extern int playerESP;
extern int sleeperESP;
extern int silentAim;
extern int noRecoil;
extern int noSpread;
extern int fastReload;
extern int spiderMan;
extern int crosshair;
extern int instaEoka;
extern int automatic;
extern int fakeAdmin;
extern int inventoryESP;
extern int heldWeapon;
extern int jumpAim;
extern int sprintAim;
extern int fovCircle;
extern float fovSlider;
extern int tracers;
extern int bulletThickness;
extern int aimbot;
extern int skeletonESP;
extern int boxESP;
extern int scientistESP;
extern int rustbergMode;
extern int aimPrediction;
extern int helicopterESP;
extern int heliSilent;
extern int neverSink;

#define M_PI 3.14159265358979323846f
#define M_RADPI	57.295779513082f
#define M_PI_F ((float)(M_PI))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))