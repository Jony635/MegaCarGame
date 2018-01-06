#pragma once


#include "Module.h"
#include "p2DynArray.h"
#include "Globals.h"
#include "Primitive.h"
#include "Application.h"
#include "p2List.h"

#include "Application.h"
#include "PugiXml\src\pugixml.hpp"

#define MAX_SNAKE 2

#define RELEASE( x ) \
    {                        \
    if( x != NULL )        \
	    {                      \
      delete x;            \
	  x = NULL;              \
	    }                      \
    }

struct PhysBody3D;
struct PhysMotor3D;

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

struct MapData
{
	int					width;
	int					height;
	int					tile_width;
	int					tile_height;

	p2List<MapLayer*>	layers;
};



class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool Load(char* path);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	
	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	void CreateMap();
	void Draw();

	void ReStart();


public:
	Cube s;

	PhysBody3D* enter_track;
	bool entered = false;
	p2List<Cube> track_door;

	p2List<Cube> finish_line;
	p2List<PhysBody3D*> finish_line_p;

	pugi::xml_document	map_file;
	MapData data;
	p2List<Cube> track;

	p2List<PhysBody3D*> sensors;
	p2List<p2List_item<PhysBody3D*>*> sensors_to_delete;
	bool* sensors_passed = nullptr;
	int num_sensors = 0;

	bool first_win = true;

	bool finish_line_bool = false;
	PhysBody3D* line_sensor = nullptr;

};
