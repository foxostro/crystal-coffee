/**
* @file treelib.cpp
* @brief Interface to TreeLib to generate tree geometry
* @see <http://www.delta-works.org/download/?&details=TreeLib>
*
* @author Andrew Fox (arfox)
*/

#include <iostream>
#include "treelib.h"



/***************************************************************************/
/***************************************************************************/
/***************************************************************************/



#define TL_RENDER_STRATEGY              0x0100
#define TL_IMPOSTER_RESOLUTION          0x0101
#define TL_MAX_IMPOSTERS_PER_FRAME      0x0102
#define TL_USE_BARK_SHADERS             0x0103
#define TL_EXPORTED_FLOATS_PER_VERTEX   0x0104
#define TL_EXPORTED_FLOATS_PER_LEAF     0x0105
#define TL_SUPPRESS_WARNINGS            0x0106
#define TL_SHADER_ALPHA_TEST            0x0107

#define TL_ERROR                        0x0300
#define TL_VERSION                      0x0301

#define TL_WIND                         0x0400

#define TL_ANISOTROPY                   0x0200
#define TL_IMPOSTER_RANGE               0x0201
#define TL_MAX_VIEWING_DISTANCE         0x0202
#define TL_SCALE                        0x0203
#define TL_SNOW_DENSITY                 0x0204

#define TL_FACES                        0x1100
#define TL_NODES                        0x1101
#define TL_BRANCHES                     0x1102
#define TL_LEAVES                       0x1103
#define TL_VERTEX_PATH_LENGTH           0x1104
#define TL_INDEX_PATH_LENGTH            0x1105


#define TL_BOX_LOWER_CORNER             0x1400
#define TL_BOX_UPPER_CORNER             0x1401
#define TL_CENTER                       0x1402

#define TL_RADIUS                       0x1200

#define TL_SEED                         0x2100
#define TL_SHADOW                       0x2101
#define TL_AUTO_BALANCE                 0x2102
#define TL_ROTATE_LEAVES                0x2103
#define TL_BARK_TEXTURE                 0x2104
#define TL_BARK_BUMP_MAP                0x2105
#define TL_LEAF_TEXTURE                 0x2106
#define TL_BOTTOM_CAP                   0x2107
#define TL_LOWER_BRANCH_MIN_GENERATION  0x2108
#define TL_UPPER_BRANCH_MIN_GENERATION  0x2109
#define TL_MAX_RECURSION_LEVEL          0x210A
#define TL_MIN_GENERATION_LEAP          0x210B
#define TL_MAX_GENERATION_LEAP          0x210C
#define TL_MIN_BRANCH_LENGTH            0x210D
#define TL_MAX_BRANCH_LENGTH            0x210E
#define TL_MAX_GENERATION               0x210F

#define TL_AGE                          0x2200
#define TL_LOD                          0x2201
#define TL_LOD_FALLOFF                  0x2202
#define TL_LEAF_SIZE                    0x2203
#define TL_SEGMENT_LENGTH               0x2204
#define TL_GENERATION_RADIUS            0x2205
#define TL_VARIANCE                     0x2206
#define TL_LOWER_MIN_DEVIATION          0x2207
#define TL_UPPER_MIN_DEVIATION          0x2208
#define TL_IDEAL_CHILD_DEVIATION        0x2209
#define TL_CHILD_DEVIATION_TOLERANCE    0x220A
#define TL_VERTICAL_ORIENTATION         0x220B
#define TL_FLEETING_ORIENTATION         0x220C
#define TL_MIN_BRANCH_AGE               0x220D
#define TL_MAX_BRANCH_AGE               0x220E
#define TL_LEAF_IDEAL_BRANCH_AGE        0x220F
#define TL_LEAF_BRANCH_AGE_TOLERANCE    0x2210
#define TL_SHADOW_CONE_STEEPNESS        0x2211
#define TL_BOTTOM_CAP_STEEPNESS         0x2212


#define TL_VERTEX_PATH                  0x1403        //{x,y,z,nx,ny,nz,tx,ty,tz,u,v,wstrength,wphase}
#define TL_INDEX_PATH                   0x1500
#define TL_LEAF_PATH                    0x1404        //{x,y,z,wstrength,wphase}


/* shadow constants */

#define TL_NO_SHADOW                    0
#define TL_FLAT_BLOB_SHADOW             1
#define TL_CONICAL_BLOB_SHADOW          2


/* bottom cap constants */

#define TL_NO_CAP                       0
#define TL_FLAT_CAP                     1
#define TL_POINTY_CAP                   2

#define TL_ROUNDED_POINTY_CAP           3    //not implemented yet
#define TL_SPHERIC_CAP                  4    //not implemented yet



/* render strategy */

#define TL_CPU_TRANSFORM                0
#define TL_COLLAPSED_QUADS              1
#define TL_POINTS                       2
#define TL_COLLAPSED_QUADS_SHADER       3
#define TL_POINTS_SHADER                4
#define TL_BEST_AVAILABLE               5




namespace TreeLib
{
	typedef int     Attribute;

	typedef bool        (*_explicitInit)(const char*);                      //_ZN7TreeLib4initEPKc				?init@TreeLib@@YA_NPBD@Z
	typedef bool        (*_implicitInit)();                                 //_ZN7TreeLib4initEv					?init@TreeLib@@YA_NXZ
	typedef Tree        (*_createTree)();                                   //_ZN7TreeLib10createTreeEv			?createTree@TreeLib@@YAPAXXZ
	typedef Tree        (*_createTreeFromSeed)(int);                        //_ZN7TreeLib18createTreeFromSeedEi		?createTreeFromSeed@TreeLib@@YAPAXH@Z
	typedef void        (*_discardTree)(Tree);                              //_ZN7TreeLib11discardTreeEPv			?discardTree@TreeLib@@YAXPAX@Z
	typedef void        (*_rebuildTree)(Tree);                              //_ZN7TreeLib11rebuildTreeEPv			?rebuildTree@TreeLib@@YAXPAX@Z
	typedef float       (*_getAttributef)(Tree,Attribute);                  //_ZN7TreeLib13getAttributefEPvi			?getAttributef@TreeLib@@YAMPAXH@Z
	typedef const float*(*_getAttributefv)(Tree,Attribute);                 //_ZN7TreeLib14getAttributefvEPvi		?getAttributefv@TreeLib@@YAPBMPAXH@Z
	typedef int         (*_getAttributei)(Tree,Attribute);                  //_ZN7TreeLib13getAttributeiEPvi			?getAttributei@TreeLib@@YAHPAXH@Z
	typedef const int*  (*_getAttributeiv)(Tree,Attribute);                 //_ZN7TreeLib14getAttributeivEPvi		?getAttributeiv@TreeLib@@YAPBHPAXH@Z
	typedef const char* (*_getAttributecv)(Tree,Attribute);                 //_ZN7TreeLib14getAttributecvEPvi		?getAttributecv@TreeLib@@YAPBDPAXH@Z

	typedef bool        (*_setAttributef)(Tree,Attribute,float);            //_ZN7TreeLib13setAttributefEPvif			?setAttributef@TreeLib@@YA_NPAXHM@Z
	typedef bool        (*_setAttributefv)(Tree,Attribute,const float*);    //_ZN7TreeLib14setAttributefvEPviPKf			?setAttributefv@TreeLib@@YA_NPAXHPBM@Z
	typedef bool        (*_setAttributei)(Tree,Attribute,int);              //_ZN7TreeLib13setAttributeiEPvii				?setAttributei@TreeLib@@YA_NPAXHH@Z
	typedef bool        (*_setAttributecv)(Tree,Attribute,const char*);     //_ZN7TreeLib14setAttributecvEPviPKc			?setAttributecv@TreeLib@@YA_NPAXHPBD@Z
	typedef bool        (*_pushAttributes)();                               //_ZN7TreeLib14pushAttributesEv				?pushAttributes@TreeLib@@YA_NXZ
	typedef bool        (*_popAttributes)();                                //_ZN7TreeLib13popAttributesEv				?popAttributes@TreeLib@@YA_NXZ
	typedef int         (*_saveAttributes)();                               //_ZN7TreeLib14saveAttributesEv				?saveAttributes@TreeLib@@YAHXZ
	typedef bool        (*_loadAttributes)(Tree,int configuration);         //_ZN7TreeLib14loadAttributesEPvi			?loadAttributes@TreeLib@@YA_NPAXH@Z
	typedef bool        (*_freeAttributes)(int configuration);              //_ZN7TreeLib14freeAttributesEi				?freeAttributes@TreeLib@@YA_NH@Z

	typedef bool        (*_renderCompositionLeaves)();                      //_ZN7TreeLib23renderCompositionLeavesEv		?renderCompositionLeaves@TreeLib@@YA_NXZ
	typedef bool        (*_renderCompositionShadows)();                     //_ZN7TreeLib24renderCompositionShadowsEv		?renderCompositionShadows@TreeLib@@YA_NXZ
	typedef bool        (*_renderCompositionBranches)();                    //_ZN7TreeLib25renderCompositionBranchesEv		?renderCompositionBranches@TreeLib@@YA_NXZ
	typedef bool        (*_renderCompositionImposters)();                   //_ZN7TreeLib26renderCompositionImpostersEv	?renderCompositionImposters@TreeLib@@YA_NXZ
	typedef void        (*_flushComposition)();                             //_ZN7TreeLib16flushCompositionEv			?flushComposition@TreeLib@@YAXXZ
	typedef bool        (*_renderComposition)();                            //_ZN7TreeLib17renderCompositionEv			?renderComposition@TreeLib@@YA_NXZ
	typedef bool        (*_putTree)(Tree, const float position[3], const float up[3], const float right[3]);    //_ZN7TreeLib7putTreeEPvPKfS2_S2_		?putTree@TreeLib@@YA_NPAXQBM11@Z


	typedef bool        (*_preloadGlobalTextures)();                            //_ZN7TreeLib21preloadGlobalTexturesEv	?preloadGlobalTextures@TreeLib@@YA_NXZ
	typedef int         (*_preloadTexture)(const char*filename, bool equalize); //_ZN7TreeLib14preloadTextureEPKcb		?preloadTexture@TreeLib@@YAHPBD_N@Z
	typedef int         (*_preloadNormalMap)(const char*filename);              //_ZN7TreeLib16preloadNormalMapEPKc		?preloadNormalMap@TreeLib@@YAHPBD@Z

	extern _explicitInit          init;
	extern _createTree            createTree;
	extern _createTreeFromSeed    createTreeFromSeed;
	extern _discardTree           discardTree;
	extern _rebuildTree           rebuildTree;
	extern _getAttributef         getAttributef;
	extern _getAttributefv        getAttributefv;
	extern _getAttributei         getAttributei;
	extern _getAttributeiv        getAttributeiv;
	extern _getAttributecv        getAttributecv;
	extern _setAttributef         setAttributef;
	extern _setAttributefv        setAttributefv;
	extern _setAttributei         setAttributei;
	extern _setAttributecv        setAttributecv;
	extern _pushAttributes        pushAttributes;
	extern _popAttributes         popAttributes;

	extern _renderCompositionLeaves		renderCompositionLeaves;
	extern _renderCompositionShadows	renderCompositionShadows;
	extern _renderCompositionBranches	renderCompositionBranches;
	extern _renderCompositionImposters	renderCompositionImposters;
	extern _flushComposition			flushComposition;
	extern _renderComposition			renderComposition;
	extern _putTree						putTree;

	extern _preloadGlobalTextures		preloadGlobalTextures;
	extern _preloadTexture				preloadTexture;
	extern _preloadNormalMap			preloadNormalMap;

	extern _saveAttributes				saveAttributes;
	extern _loadAttributes				loadAttributes;
	extern _freeAttributes				freeAttributes;


	bool        load(const char*filename);
	void        unload();
	const char* error();
}

#if defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(_Windows)
#define SYSTEM_WINDOWS
#elif defined(__linux) || defined(linux) || defined(unix) || defined(__unix__) || defined(__unix)
#define SYSTEM_UNIX
#else
#define SYSTEM_UNKNOWN
#endif


#include <stdlib.h>

#ifdef SYSTEM_WINDOWS
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#elif defined(SYSTEM_UNIX)
#include <unistd.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <string.h>
#include <fcntl.h>
#include <stdarg.h>


#ifndef SEM_R
#define SEM_A        0200    /* alter permission */
#define SEM_R       0400    /* read permission */
#endif
#endif



namespace TreeLib
{
	_explicitInit          init=NULL;
	_createTree            createTree=NULL;
	_createTreeFromSeed    createTreeFromSeed=NULL;
	_discardTree           discardTree=NULL;
	_rebuildTree           rebuildTree=NULL;
	_getAttributef         getAttributef=NULL;
	_getAttributefv        getAttributefv=NULL;
	_getAttributei         getAttributei=NULL;
	_getAttributeiv        getAttributeiv=NULL;
	_getAttributecv        getAttributecv=NULL;
	_setAttributef         setAttributef=NULL;
	_setAttributefv        setAttributefv=NULL;
	_setAttributei         setAttributei=NULL;
	_setAttributecv        setAttributecv=NULL;
	_pushAttributes        pushAttributes=NULL;
	_popAttributes         popAttributes=NULL;

	_renderCompositionLeaves	renderCompositionLeaves=NULL;
	_renderCompositionShadows	renderCompositionShadows=NULL;
	_renderCompositionBranches	renderCompositionBranches=NULL;
	_renderCompositionImposters	renderCompositionImposters=NULL;
	_flushComposition			flushComposition=NULL;
	_renderComposition			renderComposition=NULL;
	_putTree					putTree=NULL;

	_preloadGlobalTextures		preloadGlobalTextures=NULL;
	_preloadTexture				preloadTexture=NULL;
	_preloadNormalMap			preloadNormalMap=NULL;

	_saveAttributes				saveAttributes=NULL;
	_loadAttributes				loadAttributes=NULL;
	_freeAttributes				freeAttributes=NULL;

}

#ifdef SYSTEM_WINDOWS
static HMODULE             module_handle=NULL;
#else
static void               *module_handle=NULL;
#endif
static bool         dll_error=false;
static const char*  local_error="No Error";
static bool			priorize_main=true;
static char         error_buffer[512];
static const char*	failed_symbol0,*failed_symbol1;


template <class T>
static bool locate(const char*name,T&target)
{

#ifdef SYSTEM_WINDOWS
	target = (T)GetProcAddress(module_handle,name);
#else
	target = (T)dlsym(module_handle,name);
#endif
	return target != NULL;
}

template <class T>
static bool locate(const char*name,const char*alt_name,T&target)
{
	if (priorize_main)
	{
		if (locate(name,target))
			return true;
		if (locate(alt_name,target))
		{
			priorize_main = false;
			return true;
		}
	}
	else
	{
		if (locate(alt_name,target))
			return true;
		if (locate(name,target))
		{
			priorize_main = true;
			return true;
		}
	}
	failed_symbol0 = name;
	failed_symbol1 = alt_name;
	return false;
}

bool    TreeLib::load(const char*filename)
{
#ifdef SYSTEM_WINDOWS
	module_handle = LoadLibrary(filename);
#elif defined(SYSTEM_UNIX)
	module_handle = dlopen(filename,RTLD_LAZY);
#endif
	if (!module_handle)
	{
		dll_error = true;
		local_error = NULL;
		return false;
	}
	bool success=true;

	success &= locate("?init@TreeLib@@YA_NPBD@Z",				"_ZN7TreeLib4initEPKc",init);
	success &= locate("?createTree@TreeLib@@YAPAXXZ",			"_ZN7TreeLib10createTreeEv",createTree);
	success &= locate("?createTreeFromSeed@TreeLib@@YAPAXH@Z",	"_ZN7TreeLib18createTreeFromSeedEi",createTreeFromSeed);
	success &= locate("?discardTree@TreeLib@@YAXPAX@Z",			"_ZN7TreeLib11discardTreeEPv",discardTree);
	success &= locate("?rebuildTree@TreeLib@@YAXPAX@Z",			"_ZN7TreeLib11rebuildTreeEPv",rebuildTree);
	success &= locate("?getAttributef@TreeLib@@YAMPAXH@Z",		"_ZN7TreeLib13getAttributefEPvi",getAttributef);
	success &= locate("?getAttributefv@TreeLib@@YAPBMPAXH@Z",	"_ZN7TreeLib14getAttributefvEPvi",getAttributefv);
	success &= locate("?getAttributei@TreeLib@@YAHPAXH@Z",		"_ZN7TreeLib13getAttributeiEPvi",getAttributei);
	success &= locate("?getAttributeiv@TreeLib@@YAPBHPAXH@Z",	"_ZN7TreeLib14getAttributeivEPvi",getAttributeiv);
	success &= locate("?getAttributecv@TreeLib@@YAPBDPAXH@Z",	"_ZN7TreeLib14getAttributecvEPvi",getAttributecv);
	success &= locate("?setAttributef@TreeLib@@YA_NPAXHM@Z",		"_ZN7TreeLib13setAttributefEPvif",setAttributef);
	success &= locate("?setAttributei@TreeLib@@YA_NPAXHH@Z",		"_ZN7TreeLib13setAttributeiEPvii",setAttributei);
	success &= locate("?setAttributefv@TreeLib@@YA_NPAXHPBM@Z",	"_ZN7TreeLib14setAttributefvEPviPKf",setAttributefv);
	success &= locate("?setAttributecv@TreeLib@@YA_NPAXHPBD@Z",	"_ZN7TreeLib14setAttributecvEPviPKc",setAttributecv);
	success &= locate("?pushAttributes@TreeLib@@YA_NXZ",			"_ZN7TreeLib14pushAttributesEv",pushAttributes);
	success &= locate("?popAttributes@TreeLib@@YA_NXZ",			"_ZN7TreeLib13popAttributesEv",popAttributes);
	success &= locate("?saveAttributes@TreeLib@@YAHXZ",			"_ZN7TreeLib14saveAttributesEv",saveAttributes);
	success &= locate("?loadAttributes@TreeLib@@YA_NPAXH@Z",		"_ZN7TreeLib14loadAttributesEPvi",loadAttributes);
	success &= locate("?freeAttributes@TreeLib@@YA_NH@Z",		"_ZN7TreeLib14freeAttributesEi",freeAttributes);

	success &= locate("?renderCompositionLeaves@TreeLib@@YA_NXZ",	"_ZN7TreeLib23renderCompositionLeavesEv",renderCompositionLeaves);
	success &= locate("?renderCompositionShadows@TreeLib@@YA_NXZ",	"_ZN7TreeLib24renderCompositionShadowsEv",renderCompositionShadows);
	success &= locate("?renderCompositionBranches@TreeLib@@YA_NXZ",	"_ZN7TreeLib25renderCompositionBranchesEv",renderCompositionBranches);
	success &= locate("?renderCompositionImposters@TreeLib@@YA_NXZ",	"_ZN7TreeLib26renderCompositionImpostersEv",renderCompositionImposters);
	success &= locate("?flushComposition@TreeLib@@YAXXZ",			"_ZN7TreeLib16flushCompositionEv",flushComposition);
	success &= locate("?renderComposition@TreeLib@@YA_NXZ",			"_ZN7TreeLib17renderCompositionEv",renderComposition);
	success &= locate("?putTree@TreeLib@@YA_NPAXQBM11@Z",			"_ZN7TreeLib7putTreeEPvPKfS2_S2_",putTree);

	success &= locate("?preloadGlobalTextures@TreeLib@@YA_NXZ",	"_ZN7TreeLib21preloadGlobalTexturesEv",preloadGlobalTextures);
	success &= locate("?preloadTexture@TreeLib@@YAHPBD_N@Z",		"_ZN7TreeLib14preloadTextureEPKcb",preloadTexture);
	success &= locate("?preloadNormalMap@TreeLib@@YAHPBD@Z",		"_ZN7TreeLib16preloadNormalMapEPKc",preloadNormalMap);



	if (!success)
	{
		dll_error = true;
		sprintf_s(error_buffer, "Failed to link one or more symbols (last reported failed symbol: %s/%s).",failed_symbol0,failed_symbol1);
		local_error = error_buffer;
		unload();
		return false;
	}

	dll_error = false;
	return init(filename);
}

void    TreeLib::unload()
{
	if (!module_handle)
		return;
#ifdef SYSTEM_WINDOWS
	FreeLibrary(module_handle);
#elif defined(SYSTEM_UNIX)
	dlclose(module_handle);
#endif
	module_handle = NULL;
}

const char* TreeLib::error()
{
	if (dll_error)
	{
		if (local_error)
			return local_error;
#ifdef SYSTEM_WINDOWS
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
			);
		size_t len = strlen((char*)lpMsgBuf);
		if (len >= sizeof(error_buffer))
			len = sizeof(error_buffer)-1;
		memcpy(error_buffer,lpMsgBuf,len);
		error_buffer[len] = 0;
		LocalFree( lpMsgBuf );
		return error_buffer;
#elif defined(SYSTEM_UNIX)
		return dlerror();
#endif
	}
	if (!getAttributecv)
		return "Library not loaded";
	return getAttributecv(NULL,TL_ERROR);
}



/***************************************************************************/
/***************************************************************************/
/***************************************************************************/




/*
Here's how we get tree geometry:

const float * vertices; 
const int * indices;
int num_of_vertices, num_of_indices;

vertices = TreeLib::getAttributefv(tree, TL_VERTEX_PATH); // strided data
num_of_vertices = TreeLib::getAttributei(tree, TL_VERTEX_PATH_LENGTH);

indices = TreeLib::getAttributeiv(tree, TL_INDEX_PATH);
num_of_indices = TreeLib::getAttributei(tree, TL_INDEX_PATH_LENGTH);

if(!vertices || !num_of_indices) {
const char * error = TreeLib::getAttributecv(NULL, TL_ERROR);
std::cerr << error << std::endl;
} else {
std::cout << "Everything is good." << std::endl;
std::cout << "Num vertices: " << num_of_vertices << std::endl;
std::cout << "Num indices: " << num_of_indices << std::endl;
}
*/



void treelib_init()
{
	if(!TreeLib::load("treelib.dll"))
	{
		std::cerr << "The TreeLib library failed to load!" << std::endl;
	}

	// Set some default tree materials
	TreeLib::setAttributecv(NULL, TL_BARK_TEXTURE, "images/bark.png");
	TreeLib::setAttributecv(NULL, TL_BARK_BUMP_MAP, "images/bark_bump.png");
	TreeLib::setAttributecv(NULL, TL_LEAF_TEXTURE, "images/leaves.png");
	TreeLib::preloadGlobalTextures();
}

void treelib_render()
{
	TreeLib::renderComposition();
}

TreeLib::Tree gen_tree(void)
{
	return TreeLib::createTree();
}

RenderMethod_TreeLib::
RenderMethod_TreeLib(TreeLib::Tree _tree) : tree(_tree) { /* Do Nothing */ }

void RenderMethod_TreeLib::draw(const Mat4 &transform) const
{
	float right[3], up[3], position[3];

	right[0] = transform._m[0][0];
	right[1] = transform._m[0][1];
	right[2] = transform._m[0][2];

	up[0] = transform._m[1][0];
	up[1] = transform._m[1][1];
	up[2] = transform._m[1][2];

	position[0] = transform._m[3][0];
	position[1] = transform._m[3][1];
	position[2] = transform._m[3][2];

	TreeLib::putTree(tree, position, up, right);
}
