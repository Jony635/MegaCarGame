#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	CreateCar(SPORT);

	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	movecam = false;

	if (vehicle != nullptr) {

		turn = acceleration = brake = 0.0f;

		if (SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > 0) {
			if (vehicle->GetKmh() < 0)
				brake = SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) * BRAKE_POWER / MAX_AXIS;
			else if (vehicle->GetKmh() < max_speed)
			acceleration = SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) * MAX_ACCELERATION / MAX_AXIS;
		}

		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() < max_speed)
			acceleration = MAX_ACCELERATION;
		}
		if (App->input->GetKey(SDL_SCANCODE_P) == KEY_REPEAT || App->input->controller_key[SDL_CONTROLLER_BUTTON_A] == KEY_REPEAT)
		{
			if (vehicle->GetKmh() < max_speed + 25)
			acceleration = 2 * MAX_ACCELERATION;
		}



		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES) {
				turn += TURN_DEGREES;
				movecam = true;
			}
		}

	
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES) {
				turn -= TURN_DEGREES;
				movecam = true;
			}
		}

		int controller_turn = SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_LEFTX);

		if (controller_turn > AXIS_MARGIN) {
			if (turn < TURN_DEGREES) {
				turn += -TURN_DEGREES * (controller_turn - AXIS_MARGIN) / (MAX_AXIS - AXIS_MARGIN);
				movecam = true;
			}
		}
		else if (controller_turn < -AXIS_MARGIN) {

			if (turn < TURN_DEGREES) {
				turn += -TURN_DEGREES * (controller_turn - AXIS_MARGIN) / (MAX_AXIS - AXIS_MARGIN);
				movecam = true;
			}
		}
		

		if (SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) > 0) {
			if (vehicle->GetKmh() > 0)
				brake = SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) * BRAKE_POWER / MAX_AXIS;
			else if (vehicle->GetKmh() > - 30)
				acceleration = -(SDL_GameControllerGetAxis(App->input->controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT) * MAX_ACCELERATION / MAX_AXIS) / 2;
		}

		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			if (vehicle->GetKmh() > 0)
				brake = BRAKE_POWER;
			else if (vehicle->GetKmh() > - 30)
				acceleration = -MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT || App->input->controller_key[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] == KEY_REPEAT)
		{
			brake = 1.5 * BRAKE_POWER;
		}

		vehicle->ApplyEngineForce(acceleration);
		vehicle->Turn(turn);
		vehicle->Brake(brake);

		vehicle->Render();


		vehicle->GetTransform(matrix);


		new_pos = vec3(matrix[12], matrix[13], matrix[14]);
		App->camera->LookAt(new_pos);

		vehicle->vehicle->m_wheelInfo[2].m_worldTransform.getOpenGLMatrix(matrix);
		new_pos2 = vec3(matrix[12], matrix[13], matrix[14]);
	//	App->camera->LookAt(new_pos2);

		vehicle->vehicle->m_wheelInfo[0].m_worldTransform.getOpenGLMatrix(matrix);
		new_pos1 = vec3(matrix[12], matrix[13], matrix[14]);


		vec3 move = new_pos1 - new_pos2;

		App->camera->Position = new_pos2 + move * -2 + vec3{0, 4, 0};


		vehicle->GetTransform(matrix);
		new_pos = vec3(matrix[12], /*matrix[13],*/2, matrix[14]);
		App->camera->LookAt(new_pos + vec3{ 0, 2, 0 });





		char title[80];
		sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
		App->window->SetTitle(title);
	}

	return UPDATE_CONTINUE;
}

update_status ModulePlayer::PostUpdate(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->scene_intro->ReStart();

		CreateCar(SPORT);
		vehicle->GetTransform(original_matrix);

		for (int i(0); i < 20; i++) {
			LOG("%f", original_matrix[i]);
		}
	}


	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		vehicle->GetTransform(original_matrix);

		for (int i(0); i < 20; i++) {
			LOG("%f", original_matrix[i]);
		}

	}


	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		CreateCar(TRACTOR);
	}
	return UPDATE_CONTINUE;
}

bool ModulePlayer::CreateCar(CarType type) {

	VehicleInfo car;

	App->physics->ClearVehicle();

	switch (type)
	{
	case NONE:
		break;
	case SPORT:
	{
		// Car properties ----------------------------------------
		car.chassis_size.Set(2, 1, 4);
		car.chassis_offset.Set(0, 1, 0);
		car.mass = 2000.0f;
		car.suspensionStiffness = 15.88f;
		car.suspensionCompression = 0.83f;
		car.suspensionDamping = 0.88f;
		car.maxSuspensionTravelCm = 1000.0f;
		car.frictionSlip = 5;
		car.maxSuspensionForce = 10000.0f;

		// Wheel properties ---------------------------------------
		float connection_height = 1.2f;
		float wheel_radius = 0.6f;
		float wheel_width = 0.5f;
		float suspensionRestLength = 1.2f;

		// Don't change anything below this line ------------------

		float half_width = car.chassis_size.x*0.5f;
		float half_length = car.chassis_size.z*0.5f;

		vec3 direction(0, -1, 0);
		vec3 axis(-1, 0, 0);

		car.num_wheels = 4;
		car.wheels = new Wheel[4];

		// FRONT-LEFT ------------------------
		car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
		car.wheels[0].direction = direction;
		car.wheels[0].axis = axis;
		car.wheels[0].suspensionRestLength = suspensionRestLength;
		car.wheels[0].radius = wheel_radius;
		car.wheels[0].width = wheel_width;
		car.wheels[0].front = true;
		car.wheels[0].drive = false;
		car.wheels[0].brake = true;
		car.wheels[0].steering = true;

		// FRONT-RIGHT ------------------------
		car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
		car.wheels[1].direction = direction;
		car.wheels[1].axis = axis;
		car.wheels[1].suspensionRestLength = suspensionRestLength;
		car.wheels[1].radius = wheel_radius;
		car.wheels[1].width = wheel_width;
		car.wheels[1].front = true;
		car.wheels[1].drive = false;
		car.wheels[1].brake = true;
		car.wheels[1].steering = true;

		// REAR-LEFT ------------------------
		car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
		car.wheels[2].direction = direction;
		car.wheels[2].axis = axis;
		car.wheels[2].suspensionRestLength = suspensionRestLength;
		car.wheels[2].radius = wheel_radius;
		car.wheels[2].width = wheel_width;
		car.wheels[2].front = false;
		car.wheels[2].drive = true;
		car.wheels[2].brake = true;
		car.wheels[2].steering = false;

		// REAR-RIGHT ------------------------
		car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
		car.wheels[3].direction = direction;
		car.wheels[3].axis = axis;
		car.wheels[3].suspensionRestLength = suspensionRestLength;
		car.wheels[3].radius = wheel_radius;
		car.wheels[3].width = wheel_width;
		car.wheels[3].front = false;
		car.wheels[3].drive = true;
		car.wheels[3].brake = true;
		car.wheels[3].steering = false;
		
		vehicle = App->physics->AddVehicle(car);
		vehicle->SetPos(12.5, 1, 28);
	}
		break;
	case TRACTOR:
	{
		// Car properties ----------------------------------------
		car.chassis_size.Set(2.5, 2, 4);
		car.chassis_offset.Set(0, 1.5, 0);
		car.mass = 500.0f;
		car.suspensionStiffness = 15.88f;
		car.suspensionCompression = 0.83f;
		car.suspensionDamping = 0.88f;
		car.maxSuspensionTravelCm = 1000.0f;
		car.frictionSlip = 50.5;
		car.maxSuspensionForce = 6000.0f;

		// Wheel properties ---------------------------------------
		float connection_height = 1.2f;
		float wheel_radius = 0.6f;
		float wheel_width = 0.5f;
		float suspensionRestLength = 1.2f;

		// Don't change anything below this line ------------------

		float half_width = car.chassis_size.x*0.5f;
		float half_length = car.chassis_size.z*0.5f;

		vec3 direction(0, -1, 0);
		vec3 axis(-1, 0, 0);

		car.num_wheels = 3;
		car.wheels = new Wheel[car.num_wheels];

		// FRONT ------------------------
		car.wheels[0].connection.Set(half_width - 2.5*wheel_width, connection_height, half_length - wheel_radius);
		car.wheels[0].direction = direction;
		car.wheels[0].axis = axis;
		car.wheels[0].suspensionRestLength = suspensionRestLength;
		car.wheels[0].radius = wheel_radius;
		car.wheels[0].width = wheel_width;
		car.wheels[0].front = true;
		car.wheels[0].drive = true;
		car.wheels[0].brake = false;
		car.wheels[0].steering = true;

		// REAR-LEFT ------------------------
		car.wheels[1].connection.Set(half_width - 0.3f * wheel_width, 2 * connection_height, -half_length + wheel_radius);
		car.wheels[1].direction = direction;
		car.wheels[1].axis = axis;
		car.wheels[1].suspensionRestLength = suspensionRestLength;
		car.wheels[1].radius = 2 * wheel_radius + connection_height / 2;
		car.wheels[1].width = 2 * wheel_width;
		car.wheels[1].front = false;
		car.wheels[1].drive = false;
		car.wheels[1].brake = true;
		car.wheels[1].steering = false;

		// REAR-RIGHT ------------------------
		car.wheels[2].connection.Set(-half_width + 0.3f * wheel_width, 2 * connection_height, -half_length + wheel_radius);
		car.wheels[2].direction = direction;
		car.wheels[2].axis = axis;
		car.wheels[2].suspensionRestLength = suspensionRestLength;
		car.wheels[2].radius = 2 * wheel_radius + connection_height / 2;
		car.wheels[2].width = 2 * wheel_width;
		car.wheels[2].front = false;
		car.wheels[2].drive = false;
		car.wheels[2].brake = true;
		car.wheels[2].steering = false;
		vehicle = App->physics->AddVehicle(car);
		vehicle->SetPos(0, 2, 2);
	}
		break;
	default:
		break;
	}

	return vehicle != nullptr;
}


