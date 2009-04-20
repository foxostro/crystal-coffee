#ifndef _MATERIAL_H_
#define _MATERIAL_H_

/** Represents a material property for a geometry or part of a geometry. */
class Material
{
public:
	// the diffuse color
	Vec3 diffuse;

	// the ambient color
	Vec3 ambient;

	// the specular reflection color
	Vec3 specular;

	// the phong shininess
	real_t shininess;

public:
	Material()
	: diffuse(Vec3::Ones),
	  ambient(Vec3::Ones),
	  specular(Vec3::Ones),
	  shininess(0) {}

	Material(const Material &o)
	: diffuse(o.diffuse),
	  ambient(o.ambient),
	  specular(o.specular),
	  shininess(o.shininess) {}

	void bind() const;

private:
	Material& operator=(const Material&);
};

#endif
