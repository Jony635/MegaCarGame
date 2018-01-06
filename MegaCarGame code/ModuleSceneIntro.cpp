#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"
#include "Color.h"

enum MAP_ENTITIES
{
	CUBE = 1,
	PICK_UPS = 2,
	ENTER_TRACK = 998
};

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

	Load("data/maps/map3.tmx");
	CreateMap();

	App->audio->PlayMusic("data/audio/Tobu  Wholm - Motion (Copyright Free Gaming Music).ogg");

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
	Plane p(0, 0, 0, 0);
	p.wire = false;
	p.axis = false;

	p.color = { 0,1,0,1 };
		
	p.Render();

	sensor->GetTransform(&s.transform);
	s.Render();

	Sphere sens(13);
	sens.color = { 1,1,0,0.5 };

	for (p2List_item<PhysBody3D*>* iterator = sensors.getFirst(); iterator != nullptr; iterator = iterator->next)
	{
		iterator->data->GetTransform(&sens.transform);
		sens.Render();
	}

	bool win = false;
	for (int i = 0; i < num_sensors; i++)
	{
		if (sensors_passed[i])
		{
			win = true;
		}
		else
		{
			win = false;
			break;
		}
	}

	Draw();

	return UPDATE_CONTINUE;
}
update_status ModuleSceneIntro::PostUpdate(float dt)
{
	p2List_item<p2List_item<PhysBody3D*>*>* iterator = sensors_to_delete.getFirst();
	while (iterator != nullptr)
	{
		p2List_item<p2List_item<PhysBody3D*>*>* next = iterator->next;
		sensors.del(iterator->data);
		sensors_to_delete.del(iterator);
		iterator = next;
	}
	sensors_to_delete.clear();
	return UPDATE_CONTINUE;
}
void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body2 == (PhysBody3D*)App->player->vehicle)
	{
		int i = 0;
		for (p2List_item<PhysBody3D*>* iterator = sensors.getFirst(); iterator != nullptr; iterator = iterator->next)
		{
			if (body1 == iterator->data)
			{
				sensors_passed[i] = true;
				sensors_to_delete.add(iterator);
				App->audio->PlayFx(App->audio->collectible_fx);
				break;
			}
			i++;
		}
	}
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
		int size_x = 4, size_y = 8, size_z = 4;
		int x, y, z, w, h;
		x = y = z = w = h = 0;
		y = size_y / 2;
		for (int id = 0; id < layer->data->size_data; id++)
		{
			if (layer->data->data[id] == MAP_ENTITIES::CUBE)
			{
				Cube new_cube(size_x, size_y, size_z);
				new_cube.SetPos(x, y, z);
				new_cube.color = Grey;
				App->physics->AddBody(new_cube, 0);
				track.add(new_cube);

			}

			else if (layer->data->data[id] == MAP_ENTITIES::PICK_UPS)
			{
				Sphere new_sphere(13);
				new_sphere.SetPos(x, y, z);
				new_sphere.color = { 1,1,0,0.5 };

				PhysBody3D* new_sensor = App->physics->AddBody(new_sphere, 0);
				new_sensor->SetAsSensor(true);
				new_sensor->collision_listeners.add(this);

				sensors.add(new_sensor);
				
			}
			
			else if (layer->data->data[id] == MAP_ENTITIES::ENTER_TRACK)
			{
				Sphere new_sphere(13);
				new_sphere.SetPos(x, y, z);
				new_sphere.color = Red;
				App->physics->AddBody(new_sphere, 0);
				enter_track = new_sphere;
				
			}
			w++;

			if (w == layer->data->width)
			{
				w = 0;
				h++;
			}

			x = w * size_x;
			z = h * size_z;
		}
	}

	num_sensors = sensors.count();
	sensors_passed = new bool[num_sensors];
	for (int i = 0; i < num_sensors; i++)
	{
		sensors_passed[i] = false;
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

	enter_track.Render();

}
