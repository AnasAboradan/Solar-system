#define _CRT_SECURE_NO_WARNINGS

#include <glew.h>
#include <freeglut.h>

#include <vector>
#include <iostream>
#include <stdio.h>
using namespace std;
#include <chrono>
#include <glut.h>
#include "Vec3.h"
#include "Image.h"
#include "Ray.h"
#include "Sphere.h"
#include "Camera.h"
#include <string.h>
#include <stdio.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/glm.hpp"
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <ctime>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>

int Sun = 0;
int Mercury = 1;
int Venus = 2;
int Earth = 3;
int Earth_moon = 4;
int Mars = 5;
int Mars_moon1 = 6;
int Mars_moon2 = 7;
int Mars_moon3 = 8;
int Jupiter = 9;
int Staurn = 10;
int Uranus = 11;
int Neptune = 12;



double pi = 3.14159265359;


Camera cam = Camera({ 0.0, 0.0, 20.0 }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, 120.0f, 50.0f);
float deltaTime = 0.0f;	
float lastFrame = 0.0f; 

GLuint texture = 0;
void background()
{
	glBindTexture(GL_TEXTURE_2D, texture);
	const int iw = 2048;
	const int ih = 1024;

	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex2i(0, 0);
	glTexCoord2i(1, 0); glVertex2i(iw, 0);
	glTexCoord2i(1, 1); glVertex2i(iw, ih);
	glTexCoord2i(0, 1); glVertex2i(0, ih);
	glEnd();
	glPopMatrix();
}

class Scene {
public:
	vector<Sphere> spheres;

	Scene(void) {

	}
	void add(const Sphere& s) {
		spheres.push_back(s);
	}

	void load(char* fileName) {
		// load a file with spheres for your scene here ...
	}

};
float g = 20.0f;
void glSetPixel(int x, int y, const Vec3f& c) {
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glColor3f(c.r, c.g, c.b);
	glEnd();
}

class SimpleRayTracer {
private:


	Ray getEyeRayDirection(int x, int y) {

		return cam.generate_primary_ray(x, y, (float)image->getWidth(), (float)image->getHeight());
	}

public:
	Scene* scene;
	Image* image;
	SimpleRayTracer(Scene* scene, Image* image) {
		this->scene = scene;
		this->image = image;
	}

	void searchClosestHit(const Ray& ray, HitRec& hitRec) {
		for (int i = 0; i < scene->spheres.size(); i++) {
			scene->spheres[i].intersectPoints(ray, hitRec);

		}
	}

	bool inShadow(const Vec3f& lightVec, const Vec3f& offsetPoint, float lightDist) {
		float interceptDist = INFINITY;
		Vec3f intersect;
		Ray r;
		HitRec hitRec;
		r.d = lightVec;
		r.o = offsetPoint;
			for (int i = 1; i < scene->spheres.size(); i++) {
				hitRec.anyHit = false;
			if (scene->spheres[i].isHit(lightVec, offsetPoint)) {
				scene->spheres[i].intersectPoints(r, hitRec);
				float tempInterceptDist = (hitRec.p -offsetPoint).len();
				if (tempInterceptDist < interceptDist) {
					interceptDist = tempInterceptDist;
					intersect = hitRec.p;
				}
			}
		}

			if (lightDist > interceptDist)
				return true;
			else return false;
	}


	Vec3f shade(Vec3f& point, Vec3f& normal, int i)
	{
		Vec3f lightVec;
		Vec3f offsetPoint = point + normal * 0.1f;
		bool visible = false;
		float lightDist;
		Vec3f color;
		lightVec = scene->spheres[Sun].c - offsetPoint;
		lightDist = lightVec.len();
		lightVec.normalize();

		if (!inShadow(lightVec, offsetPoint, lightDist))
		{
			color = scene->spheres[i].GetColor(normal);
			return color;
		}

		return scene->spheres[i].GetColor(normal) * 0.5f;
	}

	Vec3f raytace(Ray ray, bool &background)
	{

		float traceClose = INFINITY;
		Vec3f tempPoint, hitPoint, hitNormal;
		float tempDist;
		int j;

		Vec3f color;
		Vec3f c;
		Vec3f bgColor = Vec3f(1.0f, 0.0f, 0.0f);
		HitRec hitRec;


		for (int i = 0; i < scene->spheres.size(); i++) {
			hitRec.anyHit = false;
			scene->spheres[i].intersectPoints(ray, hitRec);
			if (hitRec.anyHit) {
				tempPoint = hitRec.p;
				tempDist = (tempPoint - ray.o).len();
				if (tempDist < traceClose) {
					hitPoint = tempPoint;
					hitNormal = hitRec.n;
					traceClose = tempDist;
					j = i;
				}
			}
		}

		if (traceClose < INFINITY)
		{
			Ray r;
			if (j == 0)
			{
				background = false;
				return scene->spheres[j].GetColor(hitNormal);
			}
			background = false;
			return shade(hitPoint, hitNormal, j);
			
		}
		else
		{
			background = true;
			return bgColor;
		}
		
	}

	void fireRays(void) {
		bool bg;
		Ray ray;
		Vec3f Color;
		Vec3f bgc = Vec3f(1.0f, 0.0f, 0.0f);
		Vec3f white = Vec3f(0.0f, 0.0f, 0.0f);
		for (int y = 0; y < image->getHeight(); y++)
		{
			for (int x = 0; x < image->getWidth(); x++)
			{
				ray = getEyeRayDirection(x, y);
				Color = raytace(ray,bg);
				if (!bg)
				{
					image->setPixel(x, y, Color);
					glSetPixel(x, y, Color);
				}

			}
		}


	}
};

SimpleRayTracer* rayTracer;

GLuint LoadTexture()
{
	int txtW, txtH, nrChannels;
	unsigned char* data = stbi_load("../img/stars.jpg", &txtW, &txtH, &nrChannels, 0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, txtW, txtH, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return texture; 
}

void display()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glTranslatef(cam.position.x, cam.position.y, cam.position.z);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	background();
	glDisable(GL_TEXTURE_2D);
	rayTracer->fireRays();

	float currentFrame = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glFlush();
}


void changeSize(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glViewport(0, 0, w, h);
}

void keypress(unsigned char key, int x, int y)
{
	const float cameraSpeed = 100.5f * deltaTime;
	switch (key) {
	case 'i':
		cam.vUp.x += 0.2f;
		cam.calculateViewMatrix();
		break;
	case 'j':
		cam.vUp.y += 0.2f;
		cam.calculateViewMatrix();
		break;
	case 'k':
		cam.vUp.z += 0.5f;
		cam.calculateViewMatrix();
		break;
	case 'I':
		cam.vUp.x -= 0.2f;
		cam.calculateViewMatrix();
		break;
	case 'J':
		cam.vUp.y -= 0.2f;
		cam.calculateViewMatrix();
		break;
	case 'K':
		cam.vUp.z -= 0.5f;
		cam.calculateViewMatrix();
		break;
	case'w':
		cam.position += cameraSpeed * cam.looking_at;
		cam.calculateViewMatrix();
		break;
	case 'd':
		cam.position += glm::normalize(glm::cross(cam.looking_at, cam.vUp)) * cameraSpeed;
		cam.calculateViewMatrix();
		break;
	case 's':
		cam.position -= cameraSpeed * cam.looking_at;
		cam.calculateViewMatrix();
		break;
	case 'a':
		cam.position -= glm::normalize(glm::cross(cam.looking_at, cam.vUp)) * cameraSpeed;
		cam.calculateViewMatrix();
		break;
	case 'Q':
	case 'q':
		glutLeaveMainLoop();
		break;

	}

	glutPostRedisplay();
}


void rotatePlanet(int planetIndex) {

	float distance = (rayTracer->scene->spheres[Sun].c - rayTracer->scene->spheres[planetIndex].c).len();
	rayTracer->scene->spheres[planetIndex].angle += rayTracer->scene->spheres[planetIndex].speed;
	float X = rayTracer->scene->spheres[Sun].c.x + cos(rayTracer->scene->spheres[planetIndex].angle) * distance;
	float Z = rayTracer->scene->spheres[Sun].c.z + sin(rayTracer->scene->spheres[planetIndex].angle) * distance;
	rayTracer->scene->spheres[planetIndex].c.x = X;
	rayTracer->scene->spheres[planetIndex].c.y = rayTracer->scene->spheres[planetIndex].c.y;
	rayTracer->scene->spheres[planetIndex].c.z = Z;

}


void rotatemoon(int CenterPlanetIndex, int planetIndex, float lenght) {

	float distance = (rayTracer->scene->spheres[Sun].c - rayTracer->scene->spheres[planetIndex].c).len();
	rayTracer->scene->spheres[planetIndex].angle += rayTracer->scene->spheres[planetIndex].speed;
	float X = rayTracer->scene->spheres[CenterPlanetIndex].c.x + cos(rayTracer->scene->spheres[planetIndex].angle) * lenght;
	float Z = rayTracer->scene->spheres[CenterPlanetIndex].c.z + sin(rayTracer->scene->spheres[planetIndex].angle) * lenght;
	rayTracer->scene->spheres[planetIndex].c.x = X;
	rayTracer->scene->spheres[planetIndex].c.y = rayTracer->scene->spheres[planetIndex].c.y;
	rayTracer->scene->spheres[planetIndex].c.z = Z;


}
int i = 0;
int j = 0;
void a()
{
	if (i == 24)
		i = 0;


	rayTracer->scene->spheres[Sun].txtH = rayTracer->scene->spheres[Sun].animation[i].height;
	rayTracer->scene->spheres[Sun].txtW = rayTracer->scene->spheres[Sun].animation[i].width;
	rayTracer->scene->spheres[Sun].textrure = rayTracer->scene->spheres[Sun].animation[i].textrure;
	i++;

	rotatePlanet(Mercury);
	rotatePlanet(Venus);
	rotatePlanet(Earth);
	rotatePlanet(Mars);
	rotatePlanet(Mars);
	rotatePlanet(Jupiter);
	rotatePlanet(Staurn);
	rotatePlanet(Uranus);
	rotatePlanet(Neptune);

	rotatemoon(Earth, Earth_moon, 70.0f);
	rotatemoon(Mars, Mars_moon1, 70.0f);
	rotatemoon(Mars, Mars_moon2, 75.0f);
	rotatemoon(Mars, Mars_moon3, 80.0f);


	for (int i = 1; i < rayTracer->scene->spheres.size(); i++)
		rayTracer->scene->spheres[i].planet_angel -= 5.0f;
	glutPostRedisplay();
}

void init(void)
{
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Solar system");
	glutDisplayFunc(display);
	glutIdleFunc(a);
	glutReshapeFunc(changeSize);
	glutKeyboardFunc(keypress);

	cam.position = glm::vec3(0.0f, 0.0f, 0.0f);



	Scene* scene = new Scene;
	int  nrChannels;
	Sphere sun1 = Sphere(Vec3f(0.0f, 0.0f, -800.0f), 250.0f, 0.0f, 0.0f);
	sun1.textrure = stbi_load("../img/sun.bmp", &sun1.txtW, &sun1.txtH, &nrChannels, 0);

	string textureName;

	for (int i = 0; i <24; i++)
	{
		textureName = "../img/sunTexture/sun (" + to_string(i+1) + ").jpg";
		sun1.animation[i].textrure = stbi_load(textureName.c_str() ,&sun1.animation[i].width, &sun1.animation[i].height, &nrChannels, 0);
	}





	Sphere mercury1 = Sphere(Vec3f(200.0f * 2.0f, 0.0f, -800.0f), 20.0f, 0.0f, 0.01f);
	mercury1.textrure = stbi_load("../img/mercury.jpg", &mercury1.txtW, &mercury1.txtH, &nrChannels, 0);

	Sphere venus = Sphere(Vec3f(280.0f * 2.0f, 0.0f, -800.0f), 40.0f, 0.0f, 0.003f);
	venus.textrure = stbi_load("../img/venus.jpg", &venus.txtW, &venus.txtH, &nrChannels, 0);

	Sphere earth = Sphere(Vec3f(400.0f * 2.0f, 0.0f, -800.0f), 50.0f, 0.0f, 0.001f);
	earth.textrure = stbi_load("../img/earth.jpg", &earth.txtW, &earth.txtH, &nrChannels, 0);

	Sphere mars = Sphere(Vec3f(550.0f * 1.7f, 0.0f, -800.0f), 30.0f, 0.0f, 0.002f);
	mars.textrure = stbi_load("../img/mars.jpg", &mars.txtW, &mars.txtH, &nrChannels, 0);

	Sphere jupiter = Sphere(Vec3f(710.0f * 1.6f, 0.0f, -800.0f), 60.0f, 0.0f, 0.005f);
	jupiter.textrure = stbi_load("../img/jupiter.jpg", &jupiter.txtW, &jupiter.txtH, &nrChannels, 0);

	Sphere saturn = Sphere(Vec3f(900.0f * 1.5f, 0.0f, -800.0f), 50.0f, 0.0f, 0.002f);
	saturn.textrure = stbi_load("../img/saturn.jpg", &saturn.txtW, &saturn.txtH, &nrChannels, 0);

	Sphere uranus = Sphere(Vec3f(1050.0f * 1.4f, 0.0f, -800.0f), 45.0f, 0.0f, 0.002f);
	uranus.textrure = stbi_load("../img/uranus.jpg", &uranus.txtW, &uranus.txtH, &nrChannels, 0);

	Sphere neptune = Sphere(Vec3f(1250.0f * 1.3f, 0.0f, -800.0f), 40.0f, 0.0f, 0.001f);
	neptune.textrure = stbi_load("../img/neptune.jpg", &neptune.txtW, &neptune.txtH, &nrChannels, 0);



	Sphere earth_moon = Sphere(Vec3f(60.0f * 2.0f, 0.0f, -800.0f), 5.0f, 0.0f, 0.1f);
	earth_moon.textrure = stbi_load("../img/moon.bmp", &earth_moon.txtW, &earth_moon.txtH, &nrChannels, 0);

	Sphere marsMoon = Sphere(Vec3f(590.0f * 2.0f, 0.0f, -800.0f), 5.0f, 90.0f, 0.1f);
	marsMoon.textrure = stbi_load("../img/moon.bmp", &marsMoon.txtW, &marsMoon.txtH, &nrChannels, 0);

	Sphere marsMoon1 = Sphere(Vec3f(590.0f * 2.0f, 0.0f, -800.0f), 5.0f, 80.0f, 0.3f);
	marsMoon1.textrure = stbi_load("../img/moon.bmp", &marsMoon1.txtW, &marsMoon1.txtH, &nrChannels, 0);

	Sphere marsMoon2 = Sphere(Vec3f(590.0f * 2.0f, 0.0f, -800.0f), 5.0f, -60.0f, 0.1f);
	marsMoon2.textrure = stbi_load("../img/moon.bmp", &marsMoon2.txtW, &marsMoon2.txtH, &nrChannels, 0);


	scene->add(sun1);
	scene->add(mercury1);
	scene->add(venus);
	scene->add(earth);
	scene->add(earth_moon);
	scene->add(mars);
	scene->add(marsMoon);
	scene->add(marsMoon1);
	scene->add(marsMoon2);
	scene->add(jupiter);
	scene->add(saturn);
	scene->add(uranus);
	scene->add(neptune);


	Image* image = new Image(1280, 720);

	rayTracer = new SimpleRayTracer(scene, image);

	texture = LoadTexture();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	init();
	glutMainLoop();
	return 0;
}

