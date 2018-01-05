#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

enum CarType {
	NONE,

	SPORT,
	TRACTOR
};

#define MAX_ACCELERATION 10000.0f
#define TURN_DEGREES 30.0f * DEGTORAD
#define BRAKE_POWER 200.0f

#define MAX_AXIS 32767
#define AXIS_MARGIN 5000

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool CreateCar(CarType type);

public:

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

	bool movecam = false;

	vec3 last_pos{ 0,0,0 };
	vec3 new_pos;
	vec3 new_pos1;
	vec3 new_pos2;


	float* matrix = new float[20];
};