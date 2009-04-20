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
	boost::shared_ptr<const RenderTarget2D> rendertarget;

public:
	StandardPass(void) {}
	virtual ~StandardPass() {}

	virtual void render(const Scene * scene);
};

class CubeMapUpdatePass : public Pass
{
public:
	boost::shared_ptr<const CubeMapTarget> cubemaptarget;

public:
	CubeMapUpdatePass();

	virtual ~CubeMapUpdatePass() { /* Do nothing */ }

	virtual void render(const Scene * scene);

private:
	void set_camera(const Vec3 &eye, const Quat &orientation);
	boost::shared_ptr<RenderTarget2D> rt;
	ivec2 dimensions;
};

extern Quat face_orientation[6];

#endif /* _PASSES_H_ */

