/**
 * @file app.cpp
 * @brief The application entry point
 *  management code.
 *
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Zeyang Li (zeyangl)
 * @author Andrew Fox (arfox)
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <SDL/SDL.h>
#include "glheaders.h"
#include "project.h"
#include "scene.h"
#include "timer.h"
#include "SDLinput.h"
#include "treelib.h"
#include "devil_wrapper.h"
#include "graphicsdevice.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

/* the number of array elements per color */
#define COLOR_SIZE 4

#define CAMERA_TRANSLATION_SCALE_FACTOR (0.01)
#define CAMERA_ROTATION_SCALE_FACTOR (0.01)

#define MOUSE_LEFT_BUTTON (0)
#define MOUSE_MIDDLE_BUTTON (1)
#define MOUSE_RIGHT_BUTTON (2)

Timer g_timer;
GraphicsDevice *g_graphicsdevice = 0;

/**
 * All the global state needed to run the application.
 */
struct AppState
{
    // the array of scenes
    Scene* scene;
    // current scene index
    int scene_index;

    /* actual window width */
    int width;
    /* actual window height */
    int height;
    /* period of updates, in seconds */
    real_t period;

    /* size of allocated buffer. may differ from window size if window was
     * resized.
     */
    int buffer_size;

    SceneState scene_state;
    RenderState render_state;
	SDLinput * input;
};

/**
 * The application state.
 */
static AppState state;

/**
 * Updates the scene's camera's aspect ratio. Should invoke this when
 * width/height changes or a new scene is loaded.
 */
static void update_camera_aspect()
{
	/*
	Camera * primary_camera = state.scene->primary_camera;
	assert(primary_camera);
	primary_camera->aspect = real_t(state.width)/real_t(state.height);
	*/
}

/**
 * Loads a scene.
 */
static bool load_scene(Scene* scene, int num)
{
    return ldr_load_scene(scene, num);
}

/**
 * Returns the current scene.
 */
Scene* app_get_scene()
{
    return state.scene;
}

/** Cycles between available scenes */
void app_toggle_scene()
{
    state.scene_index = (state.scene_index + 1) % NUM_SCENES;
    delete state.scene;
    state.scene = new Scene();
    load_scene(state.scene, state.scene_index);
    update_camera_aspect();
    prj_initialize(state.scene);
}

/** Reloads the current scene */
void app_reload_scene()
{
	delete state.scene;
	state.scene = new Scene();
	load_scene(state.scene, state.scene_index);
	update_camera_aspect();
	prj_initialize(state.scene);
}

/**
 * Returns the current screen width in pixels.
 */
int app_get_screen_width()
{
    return state.width;
}

/**
 * Returns the current screen height in pixels.
 */
int app_get_screen_height()
{
    return state.height;
}

/**
 * Returns the current scene state.
 */
SceneState app_get_scene_state()
{
    return state.scene_state;
}

/**
 * Sets the current scene state.
 */
void app_set_scene_state(SceneState s)
{
    state.scene_state = s;
}

/**
 * Returns the current render state.
 */
RenderState app_get_render_state()
{
    return state.render_state;
}

/**
 * Sets the current render state.
 */
void app_set_render_state(RenderState s)
{
    state.render_state = s;
}

/**
 * Finalizes the scene and terminates the application. User code should prefer
 * app_exit over a different method of terminating the application.
 */
void app_exit()
{
	delete state.input;
	state.input = NULL;

    delete state.scene;
	state.scene = NULL;

    exit(0);
}

static void window_resize_callback(const ivec2 &dimensions)
{
    // update window information
    state.width = dimensions.x;
    state.height = dimensions.y;
    update_camera_aspect();
}

/* different camera operations controlled with the mouse */
enum CameraControl
{
	CC_NONEE, // represents event of doing nothing
	CC_FOCUS_PITCH,
	CC_FOCUS_YAW,
	CC_FOCUS_ZOOM,
	CC_TRANSLATE_X,
	CC_TRANSLATE_Y,
	CC_TRANSLATE_Z,
	CC_ROLL,
	CC_PITCH,
	CC_YAW
};

/*
Camera controls:
left-click, x-axis yaws around focus
left-click, y-axis pitches around focus
middle-click, y-axis zooms toward/away from focus

ctrl + left-click, x-axis controls translation along local x-axis
ctrl + left-click, y-axis controls translation along local y-axis
ctrl + middle-click, y-axis controls translation along local z-axis

shift + left-click, x-axis controls yaw
shift + left-click, y-axis controls pitch
shift + middle-click, y-axis controls roll

array elements are indexed by
control_map[modifier key state][button][mouse axis]
*/
static const CameraControl control_map[3][2][2] = {
	{
		{ CC_FOCUS_YAW  , CC_FOCUS_PITCH },
		{ CC_NONEE      , CC_FOCUS_ZOOM  }
	},
	{
		{ CC_TRANSLATE_X, CC_TRANSLATE_Y },
		{ CC_NONEE      , CC_TRANSLATE_Z }
	},
	{
		{ CC_YAW        , CC_PITCH       },
		{ CC_NONEE      , CC_ROLL,       }
	}
};

/**
* The current state of the mouse.
*/
struct MouseState
{
	/* mouse position as of last callback */
	int x, y;

	bool button_state[3];

	/* The current camera control state of the x and y screen axes. */
	CameraControl camera_control[2];

	MouseState() : x(0), y(0)
	{
		button_state[0] = false; // left
		button_state[1] = false; // middle
		button_state[2] = false; // right
		camera_control[0] = CC_NONEE;
		camera_control[1] = CC_NONEE;
	}
};

/* the mouse state */
static MouseState mouse;

/**
* Applies the given control to the current scene's camera.
* @param cc The control to apply.
* @param delta The distance of mouse movement.
*/
static void apply_control(CameraControl cc, int delta)
{
	// if nothing, just return
	if (cc == CC_NONEE)
		return;

	assert(app_get_scene());
	assert(app_get_scene()->primary_camera);

	Camera * cam = app_get_scene()->primary_camera;
	assert(cam);

	real_t angle = CAMERA_ROTATION_SCALE_FACTOR * delta;
	real_t distance = CAMERA_TRANSLATION_SCALE_FACTOR * delta;
	real_t new_focus_dist;

	switch(cc)
	{
	case CC_FOCUS_YAW:
		cam->yaw_about_focus(-angle);
		break;
	case CC_FOCUS_PITCH:
		cam->pitch_about_focus(-angle);
		break;
	case CC_FOCUS_ZOOM:
		// change focus based on an exponential scale
		new_focus_dist = pow(2, distance) * cam->focus_dist;
		cam->translate(Vec3::UnitZ * (new_focus_dist - cam->focus_dist));
		cam->focus_dist = new_focus_dist;
		break;
	case CC_TRANSLATE_X:
		cam->translate(-Vec3::UnitX * distance);
		break;
	case CC_TRANSLATE_Y:
		cam->translate(Vec3::UnitY * distance);
		break;
	case CC_TRANSLATE_Z:
		cam->translate(Vec3::UnitZ * distance);
		break;
	case CC_ROLL:
		cam->roll(angle);
		break;
	case CC_PITCH:
		cam->pitch(angle);
		break;
	case CC_YAW:
		cam->yaw(angle);
		break;
	default:
		break;
	}
}

static void mouse_button_handler(int button, bool down, int x, int y)
{
	mouse.button_state[button] = down;

	// left and middle button control camera
	if (button!=MOUSE_RIGHT_BUTTON) {
		// if either is released, cancel controls
		if(!down) {
			mouse.camera_control[0] = CC_NONEE;
			mouse.camera_control[1] = CC_NONEE;
		} else {
			/*
			int modifiers = glutGetModifiers();
			int modifier_index;
			if (modifiers & GLUT_ACTIVE_CTRL)
				modifier_index = MODIFIER_CTRL;
			else if (modifiers & GLUT_ACTIVE_SHIFT)
				modifier_index = MODIFIER_SHIFT;
			else
				modifier_index = MODIFIER_NONE;
			*/
			int modifier_index = 0;

			mouse.camera_control[0] = control_map[modifier_index][button][0];
			mouse.camera_control[1] = control_map[modifier_index][button][1];
		}
	}
}

static void mouse_button_left_down(int x, int y)  { mouse_button_handler(MOUSE_LEFT_BUTTON, true, x, y);   }
static void mouse_button_left_up(int x, int y)    { mouse_button_handler(MOUSE_LEFT_BUTTON, false, x, y);  }
static void mouse_button_right_down(int x, int y) { mouse_button_handler(MOUSE_RIGHT_BUTTON, true, x, y);  }
static void mouse_button_right_up(int x, int y)   { mouse_button_handler(MOUSE_RIGHT_BUTTON, false, x, y); }

static void mouse_motion_callback(int x, int y, int xrel, int yrel)
{
	mouse.x = x;
	mouse.y = y;

	if(xrel != 0) { apply_control(mouse.camera_control[0], xrel); }
	if(yrel != 0) { apply_control(mouse.camera_control[1], yrel); }
}

static void key_press(SDLKey key)
{
	switch(key)
	{
	case SDLK_F1:
		app_toggle_scene();
		break;

	case SDLK_SPACE:
		app_reload_scene();
		break;
	}
}

/**
 * Prints program usage.
 */
static void print_usage(const char* progname)
{
    std::cout << "Usage: " << progname << " [OPTIONS]...\nOptions:\n" \
        "\t-h / --help\n" \
        "\t\tPrint usage information and exit.\n" \
        "\t-s / --scene [SCENE NUMBER]\n" \
        "\t\tLoads the given scene number as the initial scene.\n" \
        "\t-o / --output [FILENAME]\n" \
        "\t\tSets the filename used for screenshots.\n" \
        "\t\tloading a window. This is useful for tracing scenes in the\n" \
        "\t\tbackground or on machines without a display available.\n" \
        "\t-gldebug\n\t\tAfter processing callbacks and/or events, check if\n" \
        "\t\tthere are any OpenGL errors by calling  glGetError. If an error\n" \
        "\t\tis reported, print out a warning by looking up the error code\n" \
        "\t\twith gluErrorString. Using this option is helpful in detecting\n" \
        "\t\tOpenGL run-time errors.\n";
}

/**
 * If any of optv are contained in argv, returns the index into argv. Otherwise
 * returns -1.
 */
static int getarg(int argc, char *argv[], int optc, const char* optv[])
{
    for (int i=1; i < argc; i++)
        for (int j=0; j < optc; j++)
            if (strcmp(optv[j], argv[i]) == 0)
                return i;

    return -1;
}

// the command line options
const char* OPT_HP[] = { "-h", "--help" };
#define OPTLEN_HP 2
const char* OPT_SN[] = { "-s", "--scene" };
#define OPTLEN_SN 2
const char* OPT_OP[] = { "-o", "--output" };
#define OPTLEN_OP 2

/**
 * Initialize the application.
 */
static void app_initialize(int argc, char *argv[],
                           int width, int height, float fps,
                           const char *title)
{
    int index;
    int period;

    // serach for help arg
    if (getarg(argc, argv, OPTLEN_HP, OPT_HP) != -1) {
        print_usage(argv[0]);
        exit(0);
    }

    // search for output arg
    const char* screenshot_filename = DEFAULT_SCREENSHOT_FILENAME;
    if ((index = getarg(argc, argv, OPTLEN_OP, OPT_OP)) != -1) {
        // parse scene number
        if (index >= argc - 1) {
            std::cerr << "Error: cannot parse scene number.\n";
            goto FAIL;
        }
        screenshot_filename = argv[index+1];
    }

    // search for scene argument
    state.scene_index = DEFAULT_INITIAL_SCENE;
    if ((index = getarg(argc, argv, OPTLEN_SN, OPT_SN)) != -1) {
        // parse scene number
        if (index >= argc - 1 ||
                sscanf(argv[index+1], "%d", &state.scene_index) != 1) {
            std::cerr << "Error: cannot parse scene number.\n";
            goto FAIL;
        }
    }

    // initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);

    // initialize the application state
    state.width = width;
    state.height = height;
    state.scene_state = SCENE_PLAYING;
    state.render_state = RENDER_GL;

    // create a window and an OpenGL context
	g_graphicsdevice = new GraphicsDevice(ivec2(width, height),
	                                      false, // windowed
										  true,  // resizable window
										  true,  // show the mouse cursor
										  title);

	// initialize input devices
	state.input = new SDLinput(false); // do not grab the mouse
	state.input->callback_video_resize = &window_resize_callback;
	state.input->callback_mouse_motion = &mouse_motion_callback;
	state.input->callback_mouse_button_left_down = &mouse_button_left_down;
	state.input->callback_mouse_button_left_up = &mouse_button_left_up;
	state.input->callback_mouse_button_right_down = &mouse_button_right_down;
	state.input->callback_mouse_button_right_up = &mouse_button_right_up;
	state.input->callback_key_press = &key_press;

	// Initialize TreeLib once for the entire application
	treelib_init();

	// Initialize DevIL once for the entire application
	devil_init();

    // load first scene
    state.scene = new Scene();
    if (!load_scene(state.scene, state.scene_index)) {
        std::cerr << "Error: scene load failed.\n";
        return;
    }
    update_camera_aspect();

    prj_initialize(state.scene);

    // set the frame rate (fixed time-step)
    period = static_cast<int>(1000.0/fps); // frame period in milliseconds
    state.period = 1.0/fps;

	while(true) {
		assert(state.scene);
		assert(state.input);

		// poll for input events
		state.input->poll();

		if (state.scene_state == SCENE_PLAYING) {
			// invoke user scene update function
			prj_update(state.scene, state.period);
		}

		// render all passes and swap buffers
		state.scene->render();

		// Finish processing and possibly stall to maintain constant rate
		while (g_timer.getElapsedTimeMS() < state.period);

		g_timer.update();
	}

FAIL:
    print_usage(argv[0]);
    return;
}

/**
 * Application entry point.
 */
int main(int argc, char *argv[])
{
    app_initialize(argc, argv, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                   DEFAULT_WINDOW_FPS, DEFAULT_WINDOW_TITLE);
    // if we reach here it's because init failed, so return error value
    return -1;
}

