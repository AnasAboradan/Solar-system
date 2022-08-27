#ifndef _Camera_H_
#define _Camera_H_


#include "Vec3.h"
#include <iostream>
#include "Ray.h"
#include "glm/mat4x4.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Camera {

public:
    glm::vec3 position;
    glm::vec3 looking_at;
    glm::vec3 vUp;
    glm::vec3 V, U, W;
    float _fov, focal_length;
    glm::mat4x4 view;


    Camera(glm::vec3 position, glm::vec3 look_at, glm::vec3 vUp, float fov, float focal_length) :
        position{ position }, looking_at{ look_at }, vUp{ vUp }, _fov{ fov }, focal_length{ focal_length }
    {
        calculateCameravector();
        calculateViewMatrix();
    }



    Ray generate_primary_ray(int i, int j, float imageWidth, float imageHight);
    void calculateCameravector();
    void calculateViewMatrix();



};

#endif