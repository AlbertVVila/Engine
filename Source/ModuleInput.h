#ifndef __MODULEINPUT_H__
#define __MODULEINPUT_H__

#include "Module.h"
#include "Point.h"
#include "SDL/include/SDL_scancode.h"

#define NUM_MOUSE_BUTTONS 5

enum EventWindow
{
	WE_QUIT = 0,
	WE_HIDE = 1,
	WE_SHOW = 2,
	WE_COUNT
};

enum KeyState
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{

public:

	ModuleInput();
	~ModuleInput();

	bool Init(JSON * config);
	update_status PreUpdate();
	bool CleanUp();

	// Check key states (includes mouse and joy buttons)
	KeyState GetKey(int id) const
	{
		return keyboard[id];
	}

	bool IsKeyPressed(int id) const
	{
		return keyboard[id] == KEY_DOWN || keyboard[id] == KEY_REPEAT;
	}

	KeyState GetMouseButtonDown(int id) const
	{
		return mouse_buttons[id - 1];
	}

	// Check for window events last frame
	bool GetWindowEvent(EventWindow code) const;

	// Get mouse / axis position
	const void ModuleInput::GetMouseMotion(float &x, float &y) const;
	const fPoint& GetMousePosition() const;
	const float GetMouseWheel() const;

	void DrawGUI();
	void DropFile(char * dropped_file) const;

	private:
	bool windowEvents[WE_COUNT];
	KeyState*	keyboard = NULL;
	KeyState	mouse_buttons[NUM_MOUSE_BUTTONS];
	fPoint mouse_motion = { 0,0 };
	int mouse_wheel = 0;
	fPoint mouse = { 0,0 };

};

#endif // __MODULEINPUT_H__