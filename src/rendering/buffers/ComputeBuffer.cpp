#include "ComputeBuffer.h"
#include <glm/glm.hpp>

/*
template<typename T>
ComputeBuffer<T>::ComputeBuffer(size_t count, GLenum usage) :
	count(count),
	usage(usage)
{
	glGenBuffers(1, &bufferID);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count * sizeof(T), nullptr, usage);
}

template<typename T>
ComputeBuffer<T>::~ComputeBuffer()
{
	if (bufferID) {
		glDeleteBuffers(1, &bufferID);
	}
}

template<typename T>
ComputeBuffer<T>::ComputeBuffer(ComputeBuffer&& other) noexcept :
	bufferID(other.bufferID),
	count(other.count),
	usage(other.usage)
{
	other.bufferID = 0;
	other.count = 0;
}

template<typename T>
ComputeBuffer<T>& ComputeBuffer<T>::operator=(ComputeBuffer&& other) noexcept
{
	if (this != &other) {
		if (bufferID) {
			glDeleteBuffers(1, &bufferID);
		}

		bufferID = other.bufferID;
		count = other.count;
		usage = other.usage;

		other.bufferID = 0;
		other.count = 0;
	}
	return *this;
}

template<typename T>
void ComputeBuffer<T>::SetData(const std::vector<T>& data)
{
	if (data.size() != count) {
		return;
	}
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T), data.data());
}

template<typename T>
void ComputeBuffer<T>::GetData(std::vector<T>& output) const
{
	if (output.size() != count) {
		output.resize(count);
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, output.size() * sizeof(T), output.data());
}

template<typename T>
T* ComputeBuffer<T>::Map(GLenum access)
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, bufferID);
	return static_cast<T*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, access));
}

template<typename T>
void ComputeBuffer<T>::Unmap()
{
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

template<typename T>
void ComputeBuffer<T>::Bind(GLuint bindingIndex) const
{
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingIndex, bufferID);
}

template class ComputeBuffer<float>;
template class ComputeBuffer<glm::vec4>;
template class ComputeBuffer<int>;
template class ComputeBuffer<unsigned int>;
*/