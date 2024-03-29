#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/scalar_multiplication.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.0f;
const float SENSITIVITY = 0.1f;
const float ORBITSENSITIVITY = 0.005f;
const float ZOOM = 60.0f;
const float RADIUS = 10.0f;

// orbit camera default values
const glm::vec3 PIVOT = glm::vec3(0.0f);

// abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera {
public:
	// camera attribues
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//orbit mode
	glm::vec3 Pivot;

	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	// camera mode
	bool IsOrbit;
	bool canOrbit = false;
	float Radius;
	float OrbitSensitivity;
	GLboolean isShiftPan = false;
	GLboolean isMousePan = false;

	// constructor with vectors
	// a colon after the parameter list indicates the start of an initialization list
	// following the colon, each member variable is initialiazed
	Camera(
		glm::vec3 position = glm::vec3(0.0f, 0.5f, 0.0f),
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
		float yaw = YAW,
		float pitch = PITCH,
		bool isOrbit = true) :
		Front(glm::vec3(0.0f, 0.0f, -1.0f)),
		MovementSpeed(SPEED),
		MouseSensitivity(SENSITIVITY),
		Zoom(ZOOM),
		Radius(RADIUS),
		OrbitSensitivity(ORBITSENSITIVITY) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		IsOrbit = isOrbit;
		updateCameraVectors();
		if (IsOrbit)
			ProcessMouseMovement(0.1f, 0.1f, true);
	}

	// constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, bool isOrbit) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		pitch = pitch;
		IsOrbit = isOrbit;
		updateCameraVectors();
	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		if (IsOrbit) {
			return glm::lookAt(Position, Pivot, Up);
		}
		else
			return glm::lookAt(Position, Position + Front, Up);
	}

	// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		if (!IsOrbit) {
			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			if (direction == UP)
				Position += Up * velocity;
			if (direction == DOWN)
				Position -= Up * velocity;
		}
		else {
			if (direction == DOWN) {
				Pivot = glm::vec3(0.0f);
				Radius = RADIUS;
				updateCameraVectors();
			}
		}
	}

	// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		if (IsOrbit) {
			if (canOrbit) {
				if (isShiftPan || isMousePan) {
					xoffset *= (float)(-OrbitSensitivity * glm::abs(glm::length(Position - Pivot)) * 0.1);
					yoffset *= (float)(-OrbitSensitivity * glm::abs(glm::length(Position - Pivot)) * 0.1);
					Position += Right * xoffset;
					Position += Up * yoffset;
					Pivot += Right * xoffset;
					Pivot += Up * yoffset;
				}
				else {
					xoffset *= -OrbitSensitivity;
					yoffset *= -OrbitSensitivity;
					Yaw += xoffset;
					Pitch += yoffset;
					if (constrainPitch)
					{
						if (glm::degrees(Pitch) > 89.0f)
							Pitch = glm::radians(89.0f);
						if (glm::degrees(Pitch) < -89.0f)
							Pitch = glm::radians(-89.0f);
					}
				}
			}
		}
		else {
			xoffset *= MouseSensitivity;
			yoffset *= MouseSensitivity;

			Yaw += xoffset;
			Pitch += yoffset;

			// make sure that when pitch is out of bounds, screen doesn't get flipped
			if (constrainPitch)
			{
				if (Pitch > 89.0f)
					Pitch = 89.0f;
				if (Pitch < -89.0f)
					Pitch = -89.0f;
			}
		}
		// update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}

	// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
	void ProcessMouseScroll(float yoffset)
	{
		if (IsOrbit) {
			Position += Front * yoffset * 0.1f * (Radius - 0.1);//(glm::abs(glm::length(Position - Pivot)));
			if (glm::distance(Position, Pivot) < 0.1f)
				Position -= Front * yoffset * 0.1f;
			//Radius = glm::length(glm::vec2(Position.x, Position.z) - glm::vec2(Pivot.x, Pivot.z));
			Radius = glm::length(Position - Pivot);
			updateCameraVectors();


		}
		else {
			Zoom -= (float)yoffset;
			if (Zoom < 1.0f)
				Zoom = 1.0f;
			if (Zoom > 60.0f)
				Zoom = 60.0f;
		}
	}
private:
	// calculates the front vector from the Camera's (updated) Euler Angles
	void updateCameraVectors()
	{
		if (IsOrbit) {
			// calculate the new Front vector, looking from cameras position to pivot point
			Front = glm::normalize(Pivot - Position);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
			float horizontalRadius = cos(Pitch) * Radius;
			float camX = (sin(Yaw) * horizontalRadius);
			float camY = sin(Pitch) * Radius;
			float camZ = (cos(Yaw) * horizontalRadius);
			Position.x = camX;
			Position.y = camY;
			Position.z = camZ;
			Position += Pivot;
		}
		else {
			// calculate the new Front vector
			glm::vec3 front;
			front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			front.y = sin(glm::radians(Pitch));
			front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
			Front = glm::normalize(front);
			// also re-calculate the Right and Up vector
			Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			Up = glm::normalize(glm::cross(Right, Front));
		}
	}
};
