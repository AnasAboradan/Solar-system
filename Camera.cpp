#include "Camera.h"
#include "Vec3.h"
#include "Ray.h"


Ray Camera::generate_primary_ray(int i, int j, float imageWidth, float imageHight)
{
	Ray R;
	double pi = 3.14159265359;
	float radian = _fov * (pi / 180);

	auto half_height = tan((radian) / 2.0) * focal_length;
	auto half_width = (imageWidth / imageHight) * half_height;

	float l, r, t, b;
	l = (float) -half_width;
	r = (float)half_width;
	b = (float)-half_height;
	t = (float)half_height;


	float u = l + ((r - l) * ((float)i + 0.5f)) / imageWidth;
	float v = b + ((t - b) * ((float)j + 0.5)) / imageHight;

	glm::vec4 d = glm::vec4(glm::normalize(W * (-focal_length) + U * (u)+V * (v)),0.0f);
	d = view * d;

	R.d = Vec3f(d.x,d.y,d.z);
	R.o = Vec3f(position.x,position.y,position.z);

	return R;
}

void Camera::calculateCameravector()
{
	W = glm::normalize(looking_at - position);
	W = W * -1.0f;
	U = glm::normalize(glm::cross(vUp, W));
	V = glm::normalize(glm::cross(W, U));
}

void Camera::calculateViewMatrix()
{
	glm::vec3 p = { position.x,position.y,position.z };
	glm::vec3 lookAt = { looking_at.x ,looking_at.y,looking_at.z };
	glm::vec3 up = { vUp.x,vUp.y,vUp.z };
	view = glm::lookAt(p, p + lookAt, up);
}
