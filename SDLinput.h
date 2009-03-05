#ifndef _SDL_INPUT_H_
#define _SDL_INPUT_H_

#include <vector>
#include "vec/vec.h"

#define XPAD_AXIS_X     (0)
#define XPAD_AXIS_Y     (1)
#define XPAD_AXIS_Z     (2)
#define XPAD_AXIS_YROT  (3)
#define XPAD_AXIS_XROT  (4)
#define XPAD_BTN_A      (0)
#define XPAD_BTN_B      (1)
#define XPAD_BTN_X      (2)
#define XPAD_BTN_Y      (3)
#define XPAD_BTN_L      (4)
#define XPAD_BTN_R      (5)
#define XPAD_BTN_BACK   (6)
#define XPAD_BTN_START  (7)
#define XPAD_BTN_XY     (8)
#define XPAD_BTN_XY_ROT (9)

#define MAX_JOYSTICK_BUTTONS (64)
#define MAX_JOYSTICK_AXII (64)
#define MAX_JOYSTICKS (64)

/**
Handles SDL input events and passes them onto registered input handlers.

Only one SDLinput object should be in action at a time, as it empties out the
input event queue. However, a separate SDLinput object should be used for each
separate game state as event handlers are usually only applicable to some
specific context.

For example, the player movement callbacks should not be handled while in the
game menu, and the menu navigation callbacks should not be called when outside
the game menu.
*/
class SDLinput
{
private:
	/** Array of all keys where true indicates that the key is down */
	bool keyState[SDLK_LAST];

	/** Joystick threshold / dead-zone */
	int threshold;

	/** State of a single joystick */
	class Joystick
	{
	public:
		SDL_Joystick * handle;
		int which; /**< Joystick index */
		bool btnState[MAX_JOYSTICK_BUTTONS];

		Joystick(SDL_Joystick *h, int w) : handle(h), which(w)
		{
			for (size_t i=0; i<sizeof(btnState); ++i)
			{
				btnState[i] = false;
			}
		}
	};

	typedef std::vector<Joystick> JoystickList;

	/** Open joysticks */
	JoystickList joysticks;

public:
	void (*callback_video_resize)(const ivec2 &dimensions);
	void (*callback_key_down)(SDLKey key);
	void (*callback_key_up)(SDLKey key);
	void (*callback_key_press)(SDLKey key);
	void (*callback_mouse_motion)(int x, int y, int xrel, int yrel);
	void (*callback_mouse_button_left_down)(int x, int y);
	void (*callback_mouse_button_left_up)(int x, int y);
	void (*callback_mouse_button_right_down)(int x, int y);
	void (*callback_mouse_button_right_up)(int x, int y);
	void (*callback_joy_axis_motion)(int joystick, int axis, int value);
	void (*callback_joy_button_down)(int joystick, int button);
	void (*callback_joy_button_up)(int joystick, int button);
	void (*callback_joy_button_press)(int joystick, int button);

public:
	SDLinput(bool grab_mouse);
	         
	/** Polls for new events in input */
	void poll();
	
private:
	/** Do not call the assignment operator */
	SDLinput operator=(const SDLinput &rh);
	
	/** Do not call the copy constructor */
	SDLinput(const SDLinput &o);
	
	void broadcastKeyDownEvents(SDLKey key);
	void broadcastKeyUpEvents(SDLKey key);
	void broadcastKeyPressEvents(SDLKey key);
	void broadcastMouseMoveEvents(int x, int y, int xrel, int yrel);
	void broadcastMouseDownLeftEvents(int x, int y);
	void broadcastMouseDownRightEvents(int x, int y);
	void broadcastMouseUpLeftEvents(int x, int y);
	void broadcastMouseUpRightEvents(int x, int y);
	void broadcastJoyButtonDownEvents(int joystick, int button);
	void broadcastJoyButtonUpEvents(int joystick, int button);
	void broadcastJoyButtonPressEvents(int joystick, int button);
	void broadcastJoyAxisMotionEvents(int joystick, int axis, int value);
	void broadcastVideoResizeEvents(const ivec2 &dimensions);

	void closeJoysticks();
	void initializeJoystickDevices();
};

#endif
