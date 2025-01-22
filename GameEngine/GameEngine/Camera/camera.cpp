#include "camera.h"

// Constructor with position only
Camera::Camera(glm::vec3 cameraPosition)
{
	this->cameraPosition = cameraPosition;
	this->cameraViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraRight = glm::cross(cameraViewDirection, cameraUp);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}

// Default constructor
Camera::Camera()
{
	this->cameraPosition = glm::vec3(0.0f, 0.0f, 100.0f);
	this->cameraViewDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	this->cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	this->cameraRight = glm::cross(cameraViewDirection, cameraUp);
	this->rotationOx = 0.0f;
	this->rotationOy = -90.0f;
}

// Constructor with custom view direction and up vector
Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraViewDirection, glm::vec3 cameraUp)
{
	this->cameraPosition = cameraPosition;
	this->cameraViewDirection = glm::normalize(cameraViewDirection);
	this->cameraUp = glm::normalize(cameraUp);
	this->cameraRight = glm::normalize(glm::cross(cameraViewDirection, cameraUp));
}

// Destructor
Camera::~Camera() {}

// Move forward
void Camera::keyboardMoveFront(float cameraSpeed)
{
	cameraPosition += cameraViewDirection * cameraSpeed;
}

// Move backward
void Camera::keyboardMoveBack(float cameraSpeed)
{
	cameraPosition -= cameraViewDirection * cameraSpeed;
}

// Move left
void Camera::keyboardMoveLeft(float cameraSpeed)
{
	cameraPosition -= cameraRight * cameraSpeed;
}

// Move right
void Camera::keyboardMoveRight(float cameraSpeed)
{
	cameraPosition += cameraRight * cameraSpeed;
}

// Move up
void Camera::keyboardMoveUp(float cameraSpeed)
{
	cameraPosition += cameraUp * cameraSpeed;
}

// Move down
void Camera::keyboardMoveDown(float cameraSpeed)
{
	cameraPosition -= cameraUp * cameraSpeed;
}

// Rotate around the X-axis (up/down)
void Camera::rotateOx(float angle)
{
	cameraViewDirection = glm::normalize(glm::vec3((glm::rotate(glm::mat4(1.0f), angle, cameraRight) * glm::vec4(cameraViewDirection, 1))));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraViewDirection));
	cameraRight = glm::cross(cameraViewDirection, cameraUp);
}

// Rotate around the Y-axis (left/right)
void Camera::rotateOy(float angle)
{
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, cameraUp);
	cameraViewDirection = glm::normalize(glm::vec3(rotation * glm::vec4(cameraViewDirection, 1.0f)));
	cameraRight = glm::normalize(glm::cross(cameraViewDirection, cameraUp));
}

// Return the camera's view matrix
glm::mat4 Camera::getViewMatrix()
{
	return glm::lookAt(cameraPosition, cameraPosition + cameraViewDirection, cameraUp);
}

// Return camera position
glm::vec3 Camera::getCameraPosition()
{
	return cameraPosition;
}

// Return camera direction
glm::vec3 Camera::getCameraViewDirection()
{
	return cameraViewDirection;
}

// Return camera up vector
glm::vec3 Camera::getCameraUp()
{
	return cameraUp;
}

glm::vec3 Camera::getCameraRight() {
	return cameraRight;
}
// Set camera position
// Set camera position and update the view direction vectors
void Camera::setCameraPosition(const glm::vec3& newPosition)
{
	cameraPosition = newPosition;
	// Recalculate the right, up, and view direction vectors
	cameraViewDirection = glm::normalize(cameraViewDirection); // Keep view direction normalized
	cameraRight = glm::normalize(glm::cross(cameraViewDirection, cameraUp)); // Recalculate right vector
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraViewDirection)); // Recalculate up vector
}


