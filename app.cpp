/**
 * @file app.cpp
 * @brief The application entry point, glut code, and application state
 *  management code.
 *
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Zeyang Li (zeyangl)
 */

/*
    YOU DO NOT NEED TO EDIT THIS FILE, though you may do so if you wish.
 */

#include "glheaders.h"
#include "imageio.h"
#include "project.h"
#include "scene.h"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>

/* the number of array elements per color */
#define COLOR_SIZE 4

/**
 * All the global state needed to run the application.
 */
struct GlutState
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
    float period;

    /* buffer used for the raytraced scene. */
    unsigned char *raytrace_buffer;
    /* size of allocated buffer. may differ from window size if window was
     * resized.
     */
    int buffer_size;

    SceneState scene_state;
    RenderState render_state;

    /* true if a raytrace is currently being executed. this should only
     * ever happen when the scene is paused
     */
    bool raytracing;
    /* boolean that is passed as new_trace to raytrace */
    bool new_trace;

    /* boolean to control GLSL state */
    bool glsl_enabled;
};

/**
 * The application state.
 */
static GlutState state;

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
    // scenes [0, MAX_STAFF_SCENE_NUM) are reserved for staff.
    if (num >= 0 && num < MAX_STAFF_SCENE_NUM)
        return ldr_load_staff_scene(scene, num);
    else
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
 * Start a new ray trace. Leaves render state unaffected. May fail if there
 * is insufficient memory.
 */
void app_start_raytrace()
{
    // can't start a raytrace if playing
    if (state.scene_state == SCENE_PLAYING)
        return;

    state.raytracing = false;
    state.new_trace = true;
    /* if buffer is null or wrong size, recreate it */
    if (!state.raytrace_buffer || state.width*state.height!=state.buffer_size) {
        delete [] state.raytrace_buffer; // will have no effect if null
        state.buffer_size = state.width*state.height;
        state.raytrace_buffer = new unsigned char[
                state.buffer_size * COLOR_SIZE];
    }
    // only preceed if the allocation was successful
    if (state.raytrace_buffer)
        state.raytracing = true;
}

/**
 * Stops a current raytrace.
 */
void app_abort_raytrace()
{
    state.raytracing = false;
}

/**
 * Returns the current scene state.
 */
SceneState app_get_scene_state()
{
    return state.scene_state;
}

/**
 * Sets the current scene state. If the scene is set to play, it may cause a
 * current raytrace to be terminated.
 */
void app_set_scene_state(SceneState s)
{
    // if switching to playing, have to kill the current raytrace.
    if (s == SCENE_PLAYING)
        app_abort_raytrace();
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
 * Returns the current state of the glsl toggle flag.
 */
bool app_is_glsl_enabled()
{
    return state.glsl_enabled;
}

/**
 * Set the current state of the glsl toggle flag.
 */
void app_set_glsl_enabled(bool enabled)
{
    state.glsl_enabled = enabled;
}

/**
 * Finalizes the scene and terminates the application. User code should prefer
 * app_exit over a different method of terminating the application.
 */
void app_exit()
{
    // clean up resources
    delete [] state.raytrace_buffer; // will have no effect if null
    delete state.scene;

    // this is the only way to terminate GLUT without extensions.
    exit(0);
}

/**
 * The glut timer callback.
 */
static void timer_callback(int millis)
{
    assert(state.scene);

    // set the next timer callback to occur after the period has elapsed
    glutTimerFunc(millis, &timer_callback, millis);

    if (state.scene_state == SCENE_PLAYING) {
        assert(!state.raytracing); // we should never be doing both
        // invoke user scene update function
        prj_update(state.scene, state.period);
        glutPostRedisplay();
    }
    else if (state.raytracing) {
        // if window size changed, abort raytrace
        if (state.width * state.height != state.buffer_size) {
            state.raytracing = false;
        } else {
            // invoke user raytrace update function until finished.
            state.raytracing =
                !rt_raytrace(state.scene, state.width, state.height,
                             state.raytrace_buffer, state.new_trace);
            state.new_trace = false;
            glutPostRedisplay();
        }
    }
}

/**
 * The glut dispaly callback.
 */
static void display_callback()
{
    assert(state.scene);

    // only draw RT if the buffer exists and if window size didn't change
    if (state.render_state == RENDER_RT && state.raytrace_buffer &&
            state.width * state.height == state.buffer_size) {
        // back up all state, set everything to identity
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        // disable anything and everything remotely likely to be enabled
        // that would mess up RasterPos or DrawPixels.
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_SCISSOR_TEST);
        // reset the color
        glColor4d(1.0, 1.0, 1.0, 1.0);

        glRasterPos2d(-1, -1);
        glDrawPixels(state.width, state.height, GL_RGBA,
                     GL_UNSIGNED_BYTE, state.raytrace_buffer);

        // pop back state
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
        glPopAttrib();
    } else {
        // invoke user render function
        prj_render(state.scene);
    }

    // flush and swap the buffer
    glFlush();
    glutSwapBuffers();
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
        "\t-r / --raytrace\n" \
        "\t\tRaytraces the scene and saves to the output file without\n" \
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
const char* OPT_RT[] = { "-r", "--raytrace" };
#define OPTLEN_RT 2
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

    // search for raytrace argument which would preclude loading glut.
    // if found, raytrace one scene and output the result to a file.
    if ((index = getarg(argc, argv, OPTLEN_RT, OPT_RT)) != -1) {
        FILE* file;
        Scene scene;
        bool new_trace = true;
        unsigned char* buffer = 0;

        // load the scene
        if (!load_scene(&scene, state.scene_index)) {
            std::cerr << "Error: scene load failed.\n";
            return;
        }
        prj_initialize(&scene, false);
        scene.camera.aspect = real_t(width)/real_t(height);

        // create the buffer
        buffer = new unsigned char[COLOR_SIZE * width * height];
        if (!buffer) {
            std::cerr << "Error: could not create buffer.\n";
            return;
        }

        // check file writiability before we waste time with the render
        file = fopen(screenshot_filename, "ab");
        if (!file || fclose(file) != 0) {
            std::cerr << "Error: cannot open file for writing\n";
            return;
        }

        // raytrace the scene
        std::cout << "Starting trace of scene....\n";
        // invoke user raytrace update function until finished.
        while (!rt_raytrace(&scene, width, height, buffer, new_trace)) {
            new_trace = false;
        }

        // write to file
        std::cout << "Trace completed, writing to file...\n";
        if (!imageio_save_image(screenshot_filename, buffer, width, height)) {
            std::cerr << "Error: wrting to file failed.\n";
            return;
        }

        // clean up and return
        std::cout << "Trace written to '" << screenshot_filename << "'.\n";
        delete [] buffer;
        exit(0);
    }

    // init glut and parse out other args
    glutInit(&argc, argv);

    // initialize the application state
    state.width = width;
    state.height = height;
    // raytrace buffer is RGBA 32-bit format
    state.raytrace_buffer = 0;
    state.scene_state = SCENE_PLAYING;
    state.render_state = RENDER_GL;
    state.raytracing = false;
    state.glsl_enabled = true;

    // create a window with double buffering and depth testing
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow(title);

#if (defined USE_GLSL) && (defined WIN32)
    // use glew for windows to load extension libraries
    glewInit();
#endif

    // load first scene
    state.scene = new Scene();
    if (!load_scene(state.scene, state.scene_index)) {
        std::cerr << "Error: scene load failed.\n";
        return;
    }
    update_camera_aspect();
    prj_initialize(state.scene, true);

    // change the pixel alignment so our buffer is the correct size
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    // initialize the gui
    gui_initialize(screenshot_filename);

    // callback for rendering
    glutDisplayFunc(display_callback);
    // callback for window size changes
    glutReshapeFunc(window_resize_callback);
    // start the "update loop" by hacking the timer callback
    period = static_cast<int>(1000.0/fps); // frame period in milliseconds
    state.period = 1.0/fps;
    glutTimerFunc(period, &timer_callback, period);

    // start main loop, this will never return
    glutMainLoop();

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

