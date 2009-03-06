/** @file passes.h
 *   @brief Define the interface to custom passes
 *
 *  @author Andrew Fox (arfox)
 */

#ifndef _PASSES_H_
#define _PASSES_H_

#include "glheaders.h"
#include "vec/vec.h"
#include "vec/mat.h"

class StandardPass : public Pass
{
public:
	StandardPass(void) { /* Do nothing */ }
	virtual ~StandardPass() { /* Do nothing */ }

	virtual void render(const Scene * scene);
};

#endif /* _PASSES_H_ */

