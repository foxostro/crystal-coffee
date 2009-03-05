/**
 * @file gui.cpp
 * @brief The code that creates a gui using glut to modify the application
 *  state. Processes all input events for the application.
 *
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Zeyang Li (zeyangl)
 * @author Andrew Fox (arfox)
 */

#include "glheaders.h"
#include "devil_wrapper.h"
#include "project.h"
#include "scene.h"
#include "searchfile.h"
#include "string_helper.h"
#include <cassert>
#include <iostream>

#define CAMERA_TRANSLATION_SCALE_FACTOR 0.01
#define CAMERA_ROTATION_SCALE_FACTOR 0.01

/* glut values for menu items */
enum
{
    MENU_QUIT,
    MENU_PAUSE,
    MENU_SCREENSHOT,
    MENU_TOGGLE_SCENE,
    MENU_TOGGLE_GLSL,
};

static const char *MENU_QUIT_MSG = "Quit";
static const char *MENU_PAUSE_MSG = "Play/Pause";
static const char *MENU_SCREENSHOT_MSG = "Screenshot";
static const char *MENU_TOGGLE_SCENE_MSG = "Toggle Scene";
#ifdef GUI_ENABLE_GLSL_TOGGLE
static const char *MENU_TOGGLE_GLSL_MSG = "Toggle GLSL";
#endif /* GUI_ENABLE_GLSL_TOGGLE */

static const char *gui_screenshot_filename = 0;

/* different camera operations performed by gui */
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

/* modifier keys */
enum {
    MODIFIER_NONE = 0,
    MODIFIER_CTRL = 1,
    MODIFIER_SHIFT = 2
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
    int x,y;
    /* button state of the left, middle, and right buttons. true means
     * down, false means up
     */
    bool button_state[3];

    /* The current camera control state of the x and y screen axes. */
    CameraControl camera_control[2];

    MouseState():
        x(0), y(0)
    {
        button_state[GLUT_LEFT_BUTTON] = false;
        button_state[GLUT_MIDDLE_BUTTON] = false;
        button_state[GLUT_RIGHT_BUTTON] = false;
        camera_control[0] = CC_NONEE;
        camera_control[1] = CC_NONEE;
    }
};

/* the glut id of the menu */
static int menu_id;
/* the mouse state */
static MouseState mouse;

static std::string gen_screenshot_filename(const std::string &prefix) {
	int highestNumber = 0;

	const std::string ext = std::string(".png");

	std::vector<std::string> files = SearchFile("./", ext);

	for(std::vector<std::string>::const_iterator iter=files.begin();
		iter!=files.end();
		++iter) {
			const std::string &fileName = *iter;
			const std::string strNum = fileName.substr(prefix.length(), fileName.length() - prefix.length() - ext.length());
			const int number = stoi(strNum);

			if (number>highestNumber) {
				highestNumber = number;
			}
	}

	std::string numberPart = justify_string_in_field(itos(++highestNumber), '0', 5, JUSTIFY_RIGHT);

	return prefix + numberPart + ext;
}

/**
 * Menu callback for glut.
 */
static void menu_callback(int menu_item)
{
    RenderState rstate = app_get_render_state();
    SceneState sstate = app_get_scene_state();

    switch (menu_item)
    {
    case MENU_QUIT:
        app_exit();
        break;
    case MENU_PAUSE:
        if (sstate == SCENE_PAUSED)
            app_set_scene_state(SCENE_PLAYING);
        else
            app_set_scene_state(SCENE_PAUSED);
        break;
    case MENU_SCREENSHOT:
        // write a screenshot out to a file
        if (gui_screenshot_filename) {
			std::string filename = gen_screenshot_filename(gui_screenshot_filename);
#if 0
            if (imageio_save_screenshot(filename.c_str()))
                std::cout << "Saved screenshot to '";
            else
				std::cerr << "Error: could not save screenshot to '";
			std::cout << filename << "'.\n";
#else
			ILuint handle=0;
			ilGenImages(1, &handle);
			ilBindImage(handle);
			ilutGLScreen();
			ilSaveImage(const_cast<char*>(filename.c_str()));
			ilDeleteImages(1, &handle);
			std::cout << "Saved screenshot to '" << filename << "'.\n";
#endif
        }
        break;
    case MENU_TOGGLE_SCENE:
        app_toggle_scene();
        break;
    case MENU_TOGGLE_GLSL:
        app_set_glsl_enabled(!app_is_glsl_enabled());
        break;
    default:
        break;
    }
}

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
    Camera& cam = app_get_scene()->camera;
    real_t angle = CAMERA_ROTATION_SCALE_FACTOR * delta;
    real_t distance = CAMERA_TRANSLATION_SCALE_FACTOR * delta;
    real_t new_focus_dist;

    switch (cc)
    {
    case CC_FOCUS_YAW:
        cam.yaw_about_focus(-angle);
        break;
    case CC_FOCUS_PITCH:
        cam.pitch_about_focus(-angle);
        break;
    case CC_FOCUS_ZOOM:
        // change focus based on an exponential scale
        new_focus_dist = pow(2, distance) * cam.focus_dist;
        cam.translate(Vec3::UnitZ * (new_focus_dist - cam.focus_dist));
        cam.focus_dist = new_focus_dist;
        break;
    case CC_TRANSLATE_X:
        cam.translate(-Vec3::UnitX * distance);
        break;
    case CC_TRANSLATE_Y:
        cam.translate(Vec3::UnitY * distance);
        break;
    case CC_TRANSLATE_Z:
        cam.translate(Vec3::UnitZ * distance);
        break;
    case CC_ROLL:
        cam.roll(angle);
        break;
    case CC_PITCH:
        cam.pitch(angle);
        break;
    case CC_YAW:
        cam.yaw(angle);
        break;
    default:
        break;
    }

    // must redraw since the camera moved
    glutPostRedisplay();
}

/**
 * Mouse button press callback for glut.
 */
static void mouse_button_callback(int button, int state, int x, int y)
{
    mouse.button_state[button] = (state == GLUT_DOWN);

    // left and middle button control camera
    if (button!=GLUT_RIGHT_BUTTON) {
        // if either is released, cancel controls
        if (state==GLUT_UP) {
            mouse.camera_control[0] = CC_NONEE;
            mouse.camera_control[1] = CC_NONEE;
        } else {
            int modifiers = glutGetModifiers();
            int modifier_index;
            if (modifiers & GLUT_ACTIVE_CTRL)
                modifier_index = MODIFIER_CTRL;
            else if (modifiers & GLUT_ACTIVE_SHIFT)
                modifier_index = MODIFIER_SHIFT;
            else
                modifier_index = MODIFIER_NONE;

            mouse.camera_control[0] = control_map[modifier_index][button][0];
            mouse.camera_control[1] = control_map[modifier_index][button][1];
        }
    }
}

/**
 * Mouse motion callback for glut.
 */
static void mouse_motion_callback(int x, int y)
{
    int oldx = mouse.x;
    int oldy = mouse.y;

    mouse.x = x;
    mouse.y = y;

    if (x != oldx)
        apply_control(mouse.camera_control[0], x - oldx);
    if (y != oldy)
        apply_control(mouse.camera_control[1], y - oldy);
}

/**
 * Initializes the gui module.
 * @param screenshot_filename The filename to use for saving screenshots via
 *  the right click menu.
 * @pre glut must have been initialized.
 */
void gui_initialize(const char* screenshot_filename)
{
    gui_screenshot_filename = screenshot_filename;

    // build a right-click menu containing our default items, user may add
    // more during his initialization
    menu_id = glutCreateMenu(menu_callback);
    glutSetMenu(menu_id);

    glutAddMenuEntry(MENU_PAUSE_MSG, MENU_PAUSE);
    glutAddMenuEntry(MENU_SCREENSHOT_MSG, MENU_SCREENSHOT);
#ifdef GUI_ENABLE_GLSL_TOGGLE
    glutAddMenuEntry(MENU_TOGGLE_GLSL_MSG, MENU_TOGGLE_GLSL);
#endif /* GUI_ENABLE_GLSL_TOGGLE */
    glutAddMenuEntry(MENU_TOGGLE_SCENE_MSG, MENU_TOGGLE_SCENE);
    glutAddMenuEntry(MENU_QUIT_MSG, MENU_QUIT);

    glutAttachMenu(GLUT_RIGHT_BUTTON);

    // callback for mouse drags
    glutMotionFunc(mouse_motion_callback);
    // callback for mouse drags with no button pressed
    glutPassiveMotionFunc(mouse_motion_callback);
    // callback for mouse button events
    glutMouseFunc(mouse_button_callback);
}

