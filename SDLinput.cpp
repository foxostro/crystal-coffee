#include <SDL/SDL.h>
#include <iostream>
#include "SDLinput.h"

void app_exit(); // app.cpp

// set to 1 to enable verbose logging of input events
#define VERBOSE_LOGGING (0)

SDLinput::SDLinput(bool grab_mouse)
: threshold(10000),
  callback_video_resize(NULL),
  callback_key_down(NULL),
  callback_key_up(NULL),
  callback_key_press(NULL),
  callback_mouse_motion(NULL),
  callback_mouse_button_left_down(NULL),
  callback_mouse_button_left_up(NULL),
  callback_mouse_button_right_down(NULL),
  callback_mouse_button_right_up(NULL),
  callback_joy_button_down(NULL),
  callback_joy_button_up(NULL),
  callback_joy_button_press(NULL)
{
	for (size_t s = 0; s < sizeof(keyState); ++s) {
		keyState[s] = false;
	}

	initializeJoystickDevices();
	
	SDL_WM_GrabInput(grab_mouse ? SDL_GRAB_ON : SDL_GRAB_OFF);
}

void SDLinput::broadcastKeyDownEvents(SDLKey key)
{
#if VERBOSE_LOGGING
	std::clog << "Key Down: " << key << std::endl;
#endif

	if(callback_key_down) {
		callback_key_down(key);
	}
}

void SDLinput::broadcastKeyUpEvents(SDLKey key)
{
#if VERBOSE_LOGGING
	std::clog << "Key Up: " << key << std::endl;
#endif

	if(callback_key_up) {
		callback_key_up(key);
	}
}

void SDLinput::broadcastKeyPressEvents(SDLKey key)
{
#if VERBOSE_LOGGING
	std::clog << "Key Press: " << key << std::endl;
#endif

	if(callback_key_press) {
		callback_key_press(key);
	}
}

void SDLinput::broadcastMouseMoveEvents(int x, int y, int xrel, int yrel)
{
#if VERBOSE_LOGGING
	std::clog << "Mouse Move: (" << x << ", " << y << ")"
	          << "with relative vector (" << xrel << ", " << yrel << ")"
			  << std::endl;
#endif

	if(callback_mouse_motion) {
		callback_mouse_motion(x, y, xrel, yrel);
	}
}

void SDLinput::broadcastMouseDownLeftEvents(int x, int y)
{
#if VERBOSE_LOGGING
	std::clog << "Mouse Down Left: (" << x << ", " << y << ")" << std::endl;
#endif

	if(callback_mouse_button_left_down) {
		callback_mouse_button_left_down(x, y);
	}
}

void SDLinput::broadcastMouseDownRightEvents(int x, int y)
{
#if VERBOSE_LOGGING
	std::clog << "Mouse Down Right: (" << x << ", " << y << ")" << std::endl;
#endif

	if(callback_mouse_button_right_down) {
		callback_mouse_button_right_down(x, y);
	}
}

void SDLinput::broadcastMouseUpLeftEvents(int x, int y)
{
#if VERBOSE_LOGGING
	std::clog << "Mouse Up Left: (" << x << ", " << y << ")" << std::endl;
#endif

	if(callback_mouse_button_left_up) {
		callback_mouse_button_left_up(x, y);
	}
}

void SDLinput::broadcastMouseUpRightEvents(int x, int y)
{
#if VERBOSE_LOGGING
	std::clog << "Mouse Up Right: (" << x << ", " << y << ")" << std::endl;
#endif

	if(callback_mouse_button_right_up) {
		callback_mouse_button_right_up(x, y);
	}
}

void SDLinput::broadcastJoyAxisMotionEvents(int joystick, int axis, int value)
{
#if VERBOSE_LOGGING
	std::clog << "Joy Axis Motion: joystick=" << joystick << std::endl
	          << "                 axis=" << axis << std::endl
			  << "                 value=" << value << std::endl;
#endif

	if(callback_joy_axis_motion) {
		callback_joy_axis_motion(joystick, axis, value);
	}
}

void SDLinput::broadcastJoyButtonDownEvents(int joystick, int button)
{
#if VERBOSE_LOGGING
	std::clog << "Joy Button Down: joystick=" << joystick << std::endl
	          << "                 button=" << button << std::endl;
#endif

	if(callback_joy_button_down) {
		callback_joy_button_down(joystick, button);
	}
}

void SDLinput::broadcastJoyButtonUpEvents(int joystick, int button)
{
#if VERBOSE_LOGGING
	std::clog << "Joy Button Up: joystick=" << joystick << std::endl
	          << "               button=" << button << std::endl;
#endif

	if(callback_joy_button_up) {
		callback_joy_button_up(joystick, button);
	}
}

void SDLinput::broadcastJoyButtonPressEvents(int joystick, int button)
{
#if VERBOSE_LOGGING
	std::clog << "Joy Button Press: joystick=" << joystick << std::endl
	          << "                  button=" << button << std::endl;
#endif

	if(callback_joy_button_press) {
		callback_joy_button_press(joystick, button);
	}
}

void SDLinput::broadcastVideoResizeEvents(const ivec2 &dimensions)
{
#if VERBOSE_LOGGING
	std::clog << "Window Resize: (" << dimensions.x << ", " << dimensions.y << ")" << std::endl;
#endif

	if(callback_video_resize) {
		callback_video_resize(dimensions);
	}
}

void SDLinput::poll() {
	SDL_Event event;
	event.type = SDL_NOEVENT;
	
	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			keyState[event.key.keysym.sym] = event.key.state == SDL_PRESSED;
			broadcastKeyPressEvents(event.key.keysym.sym);
			break;
			
		case SDL_KEYUP:
			keyState[event.key.keysym.sym] = event.key.state == SDL_PRESSED;
			broadcastKeyUpEvents(event.key.keysym.sym);
			break;
			
		case SDL_MOUSEMOTION:
			broadcastMouseMoveEvents(event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
			break;
			
		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				broadcastMouseDownLeftEvents(event.motion.x, event.motion.y);
				break;
				
			case SDL_BUTTON_RIGHT:
				broadcastMouseDownRightEvents(event.motion.x, event.motion.y);
				break;
			}
			break;
			
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				broadcastMouseUpLeftEvents(event.motion.x, event.motion.y);
				break;
				
			case SDL_BUTTON_RIGHT:
				broadcastMouseUpRightEvents(event.motion.x, event.motion.y);
				break;
			}
			break;
			
		case SDL_JOYBUTTONDOWN:
			joysticks[event.jbutton.which].btnState[event.jbutton.button] = event.jbutton.state==SDL_PRESSED;
			broadcastJoyButtonPressEvents(event.jbutton.which, event.jbutton.button);
			break;
			
		case SDL_JOYBUTTONUP:
			joysticks[event.jbutton.which].btnState[event.jbutton.button] = event.jbutton.state==SDL_PRESSED;
			broadcastJoyButtonUpEvents(event.jbutton.which, event.jbutton.button);
			break;

		case SDL_VIDEORESIZE:			
			broadcastVideoResizeEvents(ivec2(event.resize.w, event.resize.h));
			break;
			
		case SDL_QUIT:
			app_exit();			
			break;
		}
	}
	
	// Poll for joystick axis events
	for(JoystickList::iterator i=joysticks.begin();
	     i!=joysticks.end(); ++i)
	{
		const Joystick &joystick = *i;
		
		if (joystick.handle) {
			const int numAxes = SDL_JoystickNumAxes(joystick.handle);
			for (int axis=0; axis<numAxes; ++axis)
			{
				Sint16 value = SDL_JoystickGetAxis(joystick.handle, axis);
				
				if (abs(value) > threshold) {
					broadcastJoyAxisMotionEvents(joystick.which, axis, value);
				}
			}
		}
	}
	
	// Fire off events for key holds
	for (size_t key=0; key<sizeof(keyState); ++key)
	{
		if (keyState[key]) {
			broadcastKeyDownEvents((SDLKey)key);
		}
	}
	
	// Fire off events for joystick button holds
	for (JoystickList::iterator i=joysticks.begin();
	     i!=joysticks.end(); ++i)
	{
		const Joystick &joystick = *i;
		
		for (int i=0; i<MAX_JOYSTICK_BUTTONS; ++i)
		{
			if (joystick.btnState[i]) {
				broadcastJoyButtonDownEvents(joystick.which, i);
			}
		}
	}
}

void SDLinput::closeJoysticks()
{
	for(JoystickList::iterator i=joysticks.begin(); i!=joysticks.end(); ++i)
	{
		SDL_Joystick *joystick = i->handle;

		if (joystick) {
			SDL_JoystickClose(joystick);
		}
	}

	joysticks.clear();
}

void SDLinput::initializeJoystickDevices()
{
	closeJoysticks();

	const int numJoysticks = SDL_NumJoysticks();

	for(int which = 0; which < numJoysticks; ++which)
	{
		SDL_Joystick *handle = SDL_JoystickOpen(which);

		if (handle) {
			std::clog << "Joystick " << SDL_JoystickName(which) << " Successfully opened." << std::endl;
		}

		joysticks.push_back(Joystick(handle, which));
	}
}