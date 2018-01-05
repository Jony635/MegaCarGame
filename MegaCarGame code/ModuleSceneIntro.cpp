#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	s.size = vec3(5, 3, 1);
	s.SetPos(0, 2.5f, 20);

	sensor = App->physics->AddBody(s, 0.0f);
	sensor->SetAsSensor(true);
	sensor->collision_listeners.add(this);

	Load("data/maps/map2.tmx");
	CreateMap();


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	sensor->GetTransform(&s.transform);
	s.Render();
	Draw();
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if(body2 == (PhysBody3D*)App->player->vehicle)
	LOG("Hit!");
}



bool ModuleSceneIntro::Load(char* path) {
	bool ret = true;
	pugi::xml_parse_result result = map_file.load_file(path);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", path, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		pugi::xml_node map = map_file.child("map");

		if (map == NULL)
		{
			LOG("Error parsing map xml file: Cannot find 'map' tag.");
			ret = false;
		}
		else
		{
			data.width = map.attribute("width").as_int();
			data.height = map.attribute("height").as_int();
			data.tile_width = map.attribute("tilewidth").as_int();
			data.tile_height = map.attribute("tileheight").as_int();
		}
	}

	// Iterate all layers and load each of them
	// Load layer info ----------------------------------------------

	pugi::xml_node layers;
	for (layers = map_file.child("map").child("layer"); layers && ret; layers = layers.next_sibling("layer")) {

		MapLayer* layer = new MapLayer;

		if (ret == true) {

			ret = LoadLayer(layers, layer);

		}
		data.layers.add(layer);
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", path);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);


		p2List_item<MapLayer*>* item_layer = data.layers.getFirst();
		while (item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name);
			LOG("tile width: %d tile height: %d", l->width, l->height);
			item_layer = item_layer->next;
		}
	}

	return ret;
}



bool ModuleSceneIntro::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;
	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();


	for (pugi::xml_node iterator = node.child("data").child("tile"); iterator != nullptr; iterator = iterator.next_sibling())
	{
		layer->size_data++;

	}


	layer->data = new uint[layer->size_data];

	memset(layer->data, 0, layer->size_data);

	int i = 0;
	for (pugi::xml_node iterator = node.child("data").child("tile"); iterator != nullptr; iterator = iterator.next_sibling())
	{
		layer->data[i] = iterator.attribute("gid").as_uint();

		i++;
	}

	return ret;
}

void ModuleSceneIntro::CreateMap() {

	for (p2List_item<MapLayer*>* layer = this->data.layers.getFirst(); layer != nullptr; layer = layer->next)
	{
		int size_x = 4, size_y = 4, size_z = 4;
		int x, y, z, w, h;
		x = y = z = w = h = 0;
		y = size_y / 2;
		for (int id = 0; id < layer->data->size_data; id++)
		{
			if (layer->data->data[id] != 0)
			{
				Cube new_cube(size_x, size_y, size_z);
				new_cube.SetPos(x, y, z);
				new_cube.color = White;
				App->physics->AddBody(new_cube, 0);
				track.add(new_cube);
			}
			w++;

			if (w == layer->data->width)
			{
				w = 0;
				h++;
			}

			x = w * size_x;
			z = h * size_y;
		}
	}
}


void ModuleSceneIntro::Draw() {

	int size_x = 4, size_y = 4, size_z = 4;
	int x, y, z, w, h;
	x = y = z = w = h = 0;

	for (p2List_item<Cube>* box = track.getFirst(); box != nullptr; box = box->next)
	{
		box->data.Render();		
	}
}
