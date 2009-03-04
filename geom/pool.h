/**
 * @file pool.h
 * @brief Generates pool geometry for the water pool scene
 *
 * @author Eric Butler (edbutler)
 * @author Andrew Fox (arfox)
 */

#ifndef _POOL_H_
#define _POOL_H_

#include "scene.h"
#include "trianglesoup.h"

// pool boundaries
#define PIX (5)
#define POX (9)
#define PIZ (5)
#define POZ (9)
#define POY (-.5)
#define PIY (-4)
#define PBY (-5)

TriangleSoup gen_pool_geometry(Scene * scene);

#endif
