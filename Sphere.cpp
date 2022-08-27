
#pragma once
#include "Sphere.h"

#include "stdio.h"
#include <iostream>
using namespace std;

#include <math.h>
#include <algorithm>
#define M_PI   3.14159265358979323846264338327950288

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



bool Sphere::intersectPoints(const Ray& r, HitRec& rec)
{
	float t0, t1;
	Vec3f line = c - r.o;
	float tca = line.dot(r.d);
	float d2 = line.dot(line) - tca * tca;
	if (d2 > this->r * this->r) return false;
	float thc = sqrt(this->r * this->r - d2);
	t0 = tca - thc;
	t1 = tca + thc;
	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1;
		if (t0 < 0) return false;
	}
	rec.tHit = t0;
	computeSurfaceHitFields(r, rec);
	rec.anyHit = true;

	return true;
}

bool Sphere::isHit(Vec3f rayVector, Vec3f rayOrigin)
{
	float t0, t1;
	Vec3f line = c - rayOrigin;
	float tca = line.dot(rayVector);
	float d2 = line.dot(line) - tca * tca;
	if (d2 > this->r * this->r) return false;
	float thc = sqrt(this->r * this->r - d2);
	t0 = tca - thc;
	t1 = tca + thc;
	if (t0 > t1) std::swap(t0, t1);

	if (t0 < 0) {
		t0 = t1;
		if (t0 < 0) return false;
	}
	else return true;
}


void Sphere::computeSurfaceHitFields(const Ray & r, HitRec & rec) const {
	rec.p = r.o + r.d * rec.tHit;
	rec.n = (rec.p - c).normalize();
}




Vec3f Sphere::GetColor(Vec3f p)
{
	if (textrure == NULL)
		cout << "errorr loading texture";

	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(planet_angel), glm::vec3(0.0, 1.0, 0.0));
	glm::vec4 s = trans * glm::vec4(p.x,p.y,p.z, 0);


	auto phi = atan2(s.z, s.x);
	auto theta = asin(s.y);
	double u = 1 - (phi + M_PI) / (2 * M_PI);
	double v = (theta + M_PI / 2) / M_PI;



	int i = int((u)*txtW);
	int j = int((1 - v) * txtH - 0.001f);
	if (i < 0) i = 0;
	if (j < 0) j = 0;
	if (i > txtW - 1) i = txtW - 1;
	if (j > txtH - 1) j = txtH - 1;
	float r = int(textrure[3 * i + 3 * txtW * j]) / 255.0f;
	float g = int(textrure[3 * i + 3 * txtW * j + 1]) / 255.0f;
	float b = int(textrure[3 * i + 3 * txtW * j + 2]) / 255.0f;
	return Vec3f(r, g, b);
}


