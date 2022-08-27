#ifndef _SPHERE_H_
#define _SPHERE_H_


#pragma once
#include "Vec3.h"
#include "Ray.h"
#include "Camera.h"
// #include "common.h"

typedef struct _Texture {
	unsigned char* textrure;
	int height, width;
}Texture;


class Sphere {
public:
	Vec3f c;
	float r;
	int txtW;
	int txtH;
	float planet_angel;
	float angle;
	float speed;
	unsigned char * textrure = NULL;
	Texture animation[100];

public:
	Sphere(const Vec3f & cen, float rad,float _angle,float _speed) 
		: c(cen), r(rad) {
		textrure = NULL; 
		planet_angel = 20.f;
		angle = _angle;
		speed = _speed;
	}

	bool intersectPoints(const Ray& r, HitRec& rec );
	bool isHit(Vec3f rayVector, Vec3f rayOrigin);
	void computeSurfaceHitFields(const Ray & r, HitRec & rec) const;
	Vec3f GetColor(Vec3f direction);
};

#endif