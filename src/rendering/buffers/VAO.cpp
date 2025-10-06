
#include"VAO.h"

VAO::VAO() {
	glGenVertexArrays(1, &ID);
}

void VAO::LinkAttrib(VBO VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset ) {
	VBO.Bind();
	//configure the Vertex attributes so that openGL knows how to read them
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride , offset);
	//Enable the vertex attribute so that openGl knows how to use it 
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}

void VAO::Bind() {
	//Make the VAO the current Vertex array object by binding it 
	glBindVertexArray(ID);
}

void VAO::Unbind() {
	glBindVertexArray(0);
}

void VAO::Delete() {
	glDeleteVertexArrays(1, &ID);
}