#include"shaderClass.h"

#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

std::string get_file_contents(const char* fileName) {
	std::ifstream in(fileName, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	throw std::runtime_error(std::string("Failed to open file: ") + fileName);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile) 
{
	CreateFromFiles({ {GL_VERTEX_SHADER, vertexFile }, 
					  {GL_FRAGMENT_SHADER, fragmentFile} 
		           });
}

Shader::Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile)
{
	CreateFromFiles({ {GL_VERTEX_SHADER, vertexFile },
					  {GL_FRAGMENT_SHADER, fragmentFile},
		              {GL_GEOMETRY_SHADER, geometryFile}
		});
}

Shader::Shader(const char* vertexFile, const char* fragmentFile, const char* tessControlFile, const char* tessEvalFile)
{
	CreateFromFiles({ {GL_VERTEX_SHADER, vertexFile },
					  {GL_FRAGMENT_SHADER, fragmentFile},
					  {GL_TESS_CONTROL_SHADER, tessControlFile },
		              {GL_TESS_EVALUATION_SHADER, tessEvalFile}
		});
}

Shader::Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile, const char* tessControlFile, const char* tessEvalFile)
{
	CreateFromFiles({ {GL_VERTEX_SHADER, vertexFile },
					  {GL_FRAGMENT_SHADER, fragmentFile},
					  {GL_TESS_CONTROL_SHADER, tessControlFile },
					  {GL_TESS_EVALUATION_SHADER, tessEvalFile},
		              {GL_GEOMETRY_SHADER, geometryFile}
		});
}

Shader::Shader(const std::vector<std::pair<GLenum, const char*>>& shaders)
{
	CreateFromFiles(shaders);
}

void Shader::Activate(){
	glUseProgram(ID);
}

void Shader::Delete() {
	glDeleteProgram(ID);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::SetUInt(const std::string& name, uint32_t value) const {
	glUniform1ui(glGetUniformLocation(ID, name.c_str()), value);
}

std::string Shader::PreProcessShader(const std::string& filePath, 
	std::unordered_set<std::string>& includedFiles, const std::string& baseDir)
{
	// checking first for circular dependencies
	if (includedFiles.find(filePath) != includedFiles.end()) {
		std::cerr << "WARNING : Circular Dependency include detected for file:" << filePath << std::endl;
		return "";
	}
	includedFiles.insert(filePath);

	std::string sourceCode = get_file_contents(filePath.c_str());
	std::stringstream inputStream(sourceCode);
	std::stringstream outputStream;
	std::string line;

	std::string currentBaseDir = baseDir.empty() ? GetDirectoryFromPath(filePath) : baseDir;

	std::string fullCode;

	int lineNumber = 1;
	while (std::getline(inputStream, line)) {
		if (line.find("#include") == 0) {
			std::size_t firstQuote = line.find('"');
			std::size_t secondQuote = line.find('"', firstQuote + 1);
			if (firstQuote != std::string::npos && secondQuote != std::string::npos) {
				std::string includeFilename = line.substr(firstQuote + 1, secondQuote - firstQuote - 1);
				//std::string includePath = currentBaseDir + "/" + includeFilename;
				std::string includePath = includeFilename;
				try {
					std::string includedCode = PreProcessShader(includePath, includedFiles, currentBaseDir);
					outputStream << includedCode << "\n";
					//outputStream << "#line " << lineNumber + 1 << " \"" << filePath << "\"\n";
				}
				catch (const std::exception& e) {
					std::cerr << "ERROR: Failed to include file: " << includePath << " - " << e.what() << "\n";
					outputStream << "// ERROR: Failed to include: " << includeFilename << "\n";
				}
			}
			else {
				std::cerr << "WARNING: Malformed #include directive in file: " << filePath << " - " << line << "\n";
				outputStream << line << "\n";
			}
		}
		else {
			outputStream << line << "\n";
		}
			lineNumber++;
	}
	
	std::cout << outputStream.str();
	return outputStream.str();
}

std::string Shader::GetDirectoryFromPath(const std::string& filepath)
{
	std::size_t found = filepath.find_last_of("/\\");
	if (found != std::string::npos) {
		return filepath.substr(0, found);
	}
	return "";
}

GLuint Shader::CompileShader(GLenum type, const char* source, const char* shaderType)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << infoLog << std::endl;
		glDeleteShader(shader);
		return 0; 
	}

	return shader;
}

void Shader::LinkProgram(const std::vector<GLuint>& shaders)
{
	ID = glCreateProgram();

	for (GLuint shader : shaders) {
		glAttachShader(ID, shader);
	}

	glLinkProgram(ID);

	GLint success;
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}


	for (GLuint shader : shaders) {
		glDeleteShader(shader);
	}
}

void Shader::CreateFromFiles(const std::vector<std::pair<GLenum, const char*>>& shaders)
{
	std::vector<GLuint> compiledShaders;
	std::unordered_set<std::string> includedFiles;

	for (const auto& shaderInfo : shaders) {
		GLenum type = shaderInfo.first;
		const char* filePath = shaderInfo.second;

		std::string baseDir = GetDirectoryFromPath(filePath);
		std::string processedCode = PreProcessShader(filePath, includedFiles, baseDir);

		const char* source = processedCode.c_str();
		const char* typeName = "";
		switch (type) {
			case GL_VERTEX_SHADER: typeName = "VERTEX"; break;
			case GL_FRAGMENT_SHADER: typeName = "FRAGMENT"; break;
			case GL_GEOMETRY_SHADER: typeName = "GEOMETRY"; break;
			case GL_TESS_CONTROL_SHADER: typeName = "TESS_CONTROL"; break;
			case GL_TESS_EVALUATION_SHADER: typeName = "TESS_EVALUATION"; break;
		}

		GLuint shader = CompileShader(type, source, typeName);
		if (shader != 0) {
			compiledShaders.push_back(shader);
		}

		includedFiles.clear();
	}

	if (!compiledShaders.empty()) {
		LinkProgram(compiledShaders);
	}
	else {
		std::cerr << "ERROR: No shaders were successfully compiled!" << std::endl;
		ID = 0;
	}
}
