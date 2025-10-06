#include"Camera.h"

Camera::Camera(int width, int height, glm::vec3 position) {
	
	Camera::width = width;
	Camera::height = height;
	Position = position;
}


void Camera::UpdateMatrix(float fovDeg, float nearPlane, float farPlane) 
{
	viewMatrix = glm::lookAt(Position, Position + Orientation, Up);
	projectionMatrix = glm::perspective(glm::radians(fovDeg),
		(float)(width / height),
		nearPlane, farPlane);

	cameraMatrix = projectionMatrix * viewMatrix;
}


void Camera::Matrix(Shader& shader, const char* uniform) 
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform),
					  1,
					  GL_FALSE,
		              glm::value_ptr(cameraMatrix));
}


void Camera::Inputs(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS ) {

		Position += speed * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {

		Position += speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {

		Position += speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {

		Position += speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

		Position += speed * Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {

		Position += speed * -Up;
	}

	/*
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick) {

			glfwSetCursorPos(window, (width / 2), (height/2));
			firstClick = false;
		}

		double mouseX;
		double mouseY;

		glfwGetCursorPos(window, &mouseX, &mouseY);

		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (height / 2)) / height;

		// rotating the camera by an rotX amount through the right vector 
		glm::vec3 newOrientation = glm::rotate(Orientation, 
									           glm::radians(-rotX),
									           glm::cross(Orientation, Up));

		//preventing gimbal lock and undesirable effects when the axis are near paralell
		if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(-5.0f))))
		{
			Orientation = newOrientation;

		}

		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		glfwSetCursorPos(window, (width / 2), (height / 2));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
	}
	*/
}
