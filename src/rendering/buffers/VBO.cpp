#include"VBO.h"

VBO::VBO() {

}

VBO::VBO(GLfloat* vertices, GLsizeiptr size) {
	glGenBuffers(1, &ID);

	//Make the VBO the current vertex buffer object by binding it 
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	//Introduce the vertices into the vbo
	glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}

void VBO::Bind() {

	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete() {
	glDeleteBuffers(1, &ID);
}