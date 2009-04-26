/**
 * @file project.h
 * @brief The common header for app.cpp, gui.cpp, ldr.cpp, staffldr.cpp,
 *  project.cpp, and raytrace.cpp.
 *
 * @author Eric Butler (edbutler)
 * @author Frank Palermo (fpalermo)
 * @author Andrew Fox (arfox)
 */

#ifndef _PROJECT_H_
#define _PROJECT_H_

/* forward declarations */

class Scene;


/* app.cpp defines and prototypes */

// default window dimensions
#define DEFAULT_WINDOW_WIDTH (800)
#define DEFAULT_WINDOW_HEIGHT (600)
// default framerate
#define DEFAULT_WINDOW_FPS (30.0)
// default window title
#define DEFAULT_WINDOW_TITLE "15-462"
// default scene number to load on program start
#define DEFAULT_INITIAL_SCENE (5)
#define NUM_SCENES (9)
// default filename to save screenshots in
#define DEFAULT_SCREENSHOT_FILENAME "sshot"

/**
 * Enumeration to distinguish which render we are currently performing.
 */
enum RenderState { RENDER_GL, RENDER_RT };
/**
 * Enumeration specifying the pause/play state of the scene.
 */
enum SceneState { SCENE_PLAYING, SCENE_PAUSED };

// app functions. documentation for each function is in app.cpp
Scene* app_get_scene();
void app_toggle_scene();
int app_get_screen_width();
int app_get_screen_height();
void app_start_raytrace();
void app_abort_raytrace();
SceneState app_get_scene_state();
void app_set_scene_state(SceneState s);
RenderState app_get_render_state();
void app_set_render_state(RenderState s);
void app_set_glsl_enabled(bool enabled);
void app_exit();


/* gui.cpp defines and prototypes */

// uncomment for P2 to enable menu item to toggle glsl features
//#define GUI_ENABLE_GLSL_TOGGLE

void gui_initialize(const char* screenshot_filename);


/* ldr.cpp and staffldr.cpp defines and prototypes */

bool ldr_load_scene(Scene* scene, int num);


/* project.cpp defines and prototypes */

void prj_initialize(Scene* scene);
void prj_update(Scene* scene, double time);


#endif /* _PROJECT_H_ */
