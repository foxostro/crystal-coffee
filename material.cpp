#include <SDL/SDL.h>
#include "glheaders.h"
#include "vec/vec.h"
#include "material.h"

void Material::bind() const
{
	const GLfloat c_a[] = { (GLfloat)ambient.x, (GLfloat)ambient.y, (GLfloat)ambient.z, 1 };
	const GLfloat c_d[] = { (GLfloat)diffuse.x, (GLfloat)diffuse.y, (GLfloat)diffuse.z, 1 };
	const GLfloat c_s[] = { (GLfloat)specular.x, (GLfloat)specular.y, (GLfloat)specular.z, 1 };
	const GLfloat black[] = { 0, 0, 0, 1 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, c_a);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, c_d);
	glMaterialfv(GL_FRONT, GL_SPECULAR, c_s);
	glMaterialfv(GL_FRONT, GL_EMISSION, black);
	glMaterialf(GL_FRONT, GL_SHININESS, (GLfloat)shininess);
}
