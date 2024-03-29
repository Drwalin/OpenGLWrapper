//
//  Camera.h
//  opengl1
//
//  Created by Kacper Orzeszko on 25.01.2018.
//  Copyright © 2018 Kacper Orzeszko. All rights reserved.
//
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159f
#endif

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat YAW=-90.f;
const GLfloat PITCH=0.0f;
const GLfloat SPEED=6.0f;
const GLfloat SENSITIVITY=0.25f;
const GLfloat ZOOM=45.0f; //FIELD OF VIEW

class Camera {
public:
	
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;
	
	GLfloat yaw;
	GLfloat pitch;
	
	GLfloat movementSpeed;
	GLfloat mouseSensitivity;
	GLfloat zoom;
	
	inline void updateCameraVectors() {
		glm::mat4 rot(1.0f);
		rot=glm::rotate<float>(rot, -yaw*M_PI/180.0f, glm::vec3(0.0f,1.0f,0.0f));
		rot=glm::rotate<float>(rot, -pitch*M_PI/180.0f, glm::vec3(1.0f,0.0f,0.0f));
		
		this->front=rot * glm::vec4(0.0f,0.0f,1.0f,0.0f);
		
		this->right=glm::normalize(glm::cross(this->front, this->worldUp));
		this->up=glm::normalize(glm::cross(this->right, this->front));
		if(up.y < 0.0f)
			up *= -1.0f;
	}
	
public:
	
	inline Camera(
			glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f),
			glm::vec3 up=glm::vec3(0.0f, 1.0f, 0.0f),
			GLfloat yaw=YAW,
			GLfloat pitch=PITCH) : front(glm::vec3(0.0f, 0.0f, -1.0f)),
			movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		this->position=position;
		this->worldUp=up;
		this->yaw=yaw;
		this->pitch=pitch;
		this->updateCameraVectors();
	}
	
	inline Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,
			GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
			front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED),
			mouseSensitivity(SENSITIVITY), zoom(ZOOM) {
		
		this->position=glm::vec3(posX, posY, posZ);
		this->worldUp=glm::vec3(upX, upY, upZ);
		this->yaw=yaw;
		this->pitch=pitch;
		this->updateCameraVectors();
	}
	
	inline glm::mat4 getViewMatrix() {
	   return glm::lookAt(this->position, this->position + this->front, this->up);
	}
	
	inline void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime) {
		GLfloat velocity=this->movementSpeed*deltaTime;
		
		if(FORWARD == direction) {
			this->position += this->front * velocity;
		}
		if(BACKWARD == direction) {
			this->position -= this->front * velocity;
		}
		if(LEFT == direction) {
			this->position -= this->right * velocity;
		}
		if(RIGHT == direction) {
			this->position += this->right * velocity;
		}
	}
	
	inline void Up(float scale, GLfloat deltaTime) {
		position.y += deltaTime * scale * SPEED;
	}
	
	inline void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset,
			GLboolean constrainPitch=true) {
		xOffset *= this->mouseSensitivity;
		yOffset *= this->mouseSensitivity;
		this->yaw += xOffset;
		this->pitch -= yOffset;
		{
			if(this->pitch > 89.0f)
				this -> pitch= 89.0f;
			if(this->pitch < -89.0f)
				this -> pitch=-89.0f;
			this->updateCameraVectors();
		}
	}
	
	inline void ProcessMouseScroll(GLfloat yOffset) {
		if(this->zoom >= 1.0f && this-> zoom <= 45.0f) {
			this->zoom -= yOffset;
		}
		if(this->zoom <= 1.0f) {
			this->zoom=1.0f;
		}
		if(this->zoom >= 45.0f) {
			this->zoom=45.0f;
		}
	}
	
	inline GLfloat getZoom() {
		return this->zoom;
	}
};

