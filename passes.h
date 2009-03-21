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
	const RenderTarget2D * rendertarget;

public:
	StandardPass(void) : rendertarget(0) { /* Do nothing */ }
	virtual ~StandardPass() { /* Do nothing */ }

	virtual void render(const Scene * scene);
};

class CubeMapUpdatePass : public Pass
{
public:
	const CubeMapTarget * rendertarget;

public:
	CubeMapUpdatePass(void) : rendertarget(0) { /* Do nothing */ }
	virtual ~CubeMapUpdatePass() { /* Do nothing */ }

	virtual void render(const Scene * scene);
};

extern Quat face_orientation[6];

#endif /* _PASSES_H_ */

