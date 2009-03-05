/**
* @file treelib.h
* @brief Interface to TreeLib to generate tree geometry
* @see <http://www.delta-works.org/download/?&details=TreeLib>
*
* @author Andrew Fox (arfox)
*/

#ifndef _TREELIB_H_
#define _TREELIB_H_

#include "rendermethod.h"

namespace TreeLib
{
	typedef void * Tree;
}

/** This must be called before anything else here! */
void treelib_init();

/** Renders the current treelib composition. */
void treelib_render();

/** Generates a tree.
 *  @return Handle to the tree object.
 */
TreeLib::Tree gen_tree(void);

class RenderMethod_TreeLib : public RenderMethod
{
public:
	RenderMethod_TreeLib(TreeLib::Tree tree);

	virtual void draw(const Mat4 &transform) const;

private:
	const TreeLib::Tree tree;
};

#endif
