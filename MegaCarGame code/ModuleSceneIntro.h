#pragma once


#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Application.h"
#include "p2List.h"

#define MAX_SNAKE 2

struct PhysBody3D;
struct PhysMotor3D;

struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;

	p2List<MapLayer*>	layers;
};


struct MapLayer {

	const char* name = nullptr;
	int width, height;
	unsigned int* data;
	uint size_data = 0;
	int property = 0;
	float speed = 1;
	bool Navigation = false;

	~MapLayer() {
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Load(char* path);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	Cube s;
	PhysBody3D* sensor;

	pugi::xml_document	map_file;
	MapData data;
};
