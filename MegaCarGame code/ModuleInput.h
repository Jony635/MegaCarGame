#pragma once
#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;

public:


	SDL_GameController* controller = nullptr;

	
	KEY_STATE controller_key[15];
	SDL_GameControllerButton button[15] = {
		SDL_CONTROLLER_BUTTON_A,
		SDL_CONTROLLER_BUTTON_B,
		SDL_CONTROLLER_BUTTON_X,
		SDL_CONTROLLER_BUTTON_Y,
		SDL_CONTROLLER_BUTTON_BACK,
		SDL_CONTROLLER_BUTTON_GUIDE,
		SDL_CONTROLLER_BUTTON_START,
		SDL_CONTROLLER_BUTTON_LEFTSTICK,
		SDL_CONTROLLER_BUTTON_RIGHTSTICK,
		SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
		SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
		SDL_CONTROLLER_BUTTON_DPAD_UP,
		SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		SDL_CONTROLLER_BUTTON_DPAD_RIGHT
	};


};