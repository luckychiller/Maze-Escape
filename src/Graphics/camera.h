#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // Constructor with vectors
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
            glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
            float yaw = YAW, float pitch = PITCH);
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, 
            float upX, float upY, float upZ, 
            float yaw, float pitch);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

    // take screen width and height and return a projection matrix
    glm::mat4 GetProjectionMatrix(float screenWidth, float screenHeight);

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset);

private:
    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
};



/*
Camera_Movement enum: Defines directions for keyboard input abstraction.
Default values: Constants for initial yaw, pitch, speed, sensitivity, and zoom (FOV).
Camera Attributes:
Position: Where the camera is in world space.
Front: The direction the camera is looking.
Up: The camera's local up direction.
Right: The camera's local right direction.
WorldUp: A fixed up direction in the world (usually 0,1,0), used to calculate Right and Up.
Yaw, Pitch: Euler angles for orientation.
MovementSpeed, MouseSensitivity, Zoom: Control parameters.
Constructors: Initialize the camera.
GetViewMatrix(): Calculates and returns the view matrix using glm::lookAt. glm::lookAt takes the camera's position, the point it's looking at (Position + Front), and the world's up vector.
GetProjectionMatrix(): Calculates and returns the perspective projection matrix. This is similar to what we had in main.cpp but now encapsulated.
ProcessKeyboard(): Updates the camera's Position based on keyboard input and deltaTime (for frame-rate independent speed).
ProcessMouseMovement(): Updates Yaw and Pitch based on mouse movement offsets. It also constrains pitch to prevent flipping.
ProcessMouseScroll(): Adjusts Zoom (FOV) based on mouse scroll wheel input.
updateCameraVectors(): Recalculates Front, Right, and Up vectors whenever Yaw or Pitch changes. This is the core of the camera's orientation logic using trigonometry.
*/
