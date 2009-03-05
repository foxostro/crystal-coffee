/**
 * @file app.cpp
 * @brief The application entry point, glut code, and application state
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
#include "treelib.h"
#include "devil_wrapper.h"
#include "graphicsdevice.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

/* the number of array elements per color */
#define COLOR_SIZE 4

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
    state.scene->camera.aspect = real_t(state.width)/real_t(state.height);
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

/**
 * Toggles between scenes 0 and 1.
 */
void app_toggle_scene()
{
    state.scene_index = !state.scene_index;
    delete state.scene;
    state.scene = new Scene();
    load_scene(state.scene, state.scene_index);
    update_camera_aspect();
    prj_initialize(state.scene, true);
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
    // clean up resources
    delete state.scene;

    // this is the only way to terminate GLUT without extensions.
    exit(0);
}

/**
 * The glut window resize callback.
 */
static void window_resize_callback(int width, int height)
{
    // update window information
    state.width = width;
    state.height = height;
    update_camera_aspect();
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
	g_graphicsdevice = new GraphicsDevice(ivec2(width, height), false, title);

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

    prj_initialize(state.scene, true);

    // TODO: initialize the gui

    // set the frame rate (fixed time-step)
    period = static_cast<int>(1000.0/fps); // frame period in milliseconds
    state.period = 1.0/fps;

	while(true) {
		assert(state.scene);

		if (state.scene_state == SCENE_PLAYING) {
			// invoke user scene update function
			prj_update(state.scene, state.period);
		}

		// invoke user render function
		prj_render(state.scene);

		// flush and swap the buffer
		glFlush();
		SDL_GL_SwapBuffers();

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

