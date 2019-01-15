#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include <map>

#include "shader.h"
#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>


// Initialize shader from file. 
// Shader still needs to be attached and linked after this!
GLuint ShaderProgram::shader(const int shaderType, const std::string filename, bool &success)
{
	// Read Shader from file
	int shaderID = glCreateShader(shaderType);
	std::string shaderCode;
	std::ifstream shaderStream(filename, std::ios::in);
	if (shaderStream.is_open()) {
		std::stringstream sstr;
		sstr << shaderStream.rdbuf();
		shaderCode = sstr.str();
		shaderStream.close();
	} else {
		std::cout << "Impossible to open " << filename << ". \n";
		getchar();
		success = false;
		return -1;
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile shader
	char const* sourcePointer = shaderCode.c_str();
	glShaderSource(shaderID, 1, &sourcePointer, NULL);
	glCompileShader(shaderID);

	// Check shader
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (result != GL_TRUE) {
		std::vector<char> shaderErrorMessage(infoLogLength+1);
        glGetShaderInfoLog(shaderID, infoLogLength, NULL, &shaderErrorMessage[0]);

		std::string shaderTypeString;
        switch(shaderType) {
            case GL_VERTEX_SHADER:
                shaderTypeString = "vertex"; break;
            case GL_FRAGMENT_SHADER:
                shaderTypeString = "fragment"; break;
            default:
                shaderTypeString = "other";
        }

		std::cout << "Failed to compile " << shaderTypeString << ", " << &shaderErrorMessage[0];

		success = false;
		return -1;
	}
	success = true;

	return shaderID;
}

GLuint ShaderProgram::uniformID(std::string name) { 
    try {
        return uniformVariable.at(name).first; 
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid uniform: " + name);
    }
}

GLenum ShaderProgram::uniformType(std::string name) { 
    try {
        return uniformVariable.at(name).second; 
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid uniform: " + name);
    }
}

GLuint ShaderProgram::attributeID(std::string name) { 
    try {
        return attributeVariable.at(name).first; 
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid attribute: " + name);
    }
}


GLenum ShaderProgram::attributeType(std::string name) { 
    try {
        return attributeVariable.at(name).second; 
    } catch (const std::out_of_range& e) {
        throw std::invalid_argument("Invalid attribute: " + name);
    }
}

// Initialize for later use of setUniform and setAttribute.
// After linking.
void ShaderProgram::initAttributesUniforms(bool printVariables) {
	uniformVariable.clear();
	attributeVariable.clear();

	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 128; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length


	glGetProgramiv(ID, GL_ACTIVE_ATTRIBUTES, &count);

	for (i = 0; i < count; i++)
	{
		glGetActiveAttrib(ID, (GLuint)i, bufSize, &length, &size, &type, name);
		int id = glGetAttribLocation(ID, name);
		//glEnableVertexAttribArray(id);
		if (id != -1)
			attributeVariable[name] = VariableData(id, type);
		if (printVariables && (id != -1))
			printf("#%d. Attribute #%d Type: %u Name: %s\n", i, id, type, name);
	}


	glGetProgramiv(ID, GL_ACTIVE_UNIFORMS, &count);

	for (i = 0; i < count; i++)
	{
		glGetActiveUniform(ID, (GLuint)i, bufSize, &length, &size, &type, name);
		int id = glGetUniformLocation(ID, name);
		if (id != -1)
			uniformVariable[name] = VariableData(id, type);

		if (printVariables && (id != -1))
			printf("#%d. Uniform #%d Type: %u Name: %s\n", i, id, type, name);
	}

}

void ShaderProgram::setUniform(std::string name, const GLint& value) {
	GLuint id = uniformID(name);
	GLenum type = uniformType(name);
    switch(type) {
        case GL_INT:
        case GL_BOOL:
            glUniform1i(id, value);
            break;

        default:
    		throw std::invalid_argument( "Invalid variable type to set with int or not implemented yet!" );
    }
}

void ShaderProgram::setUniform(std::string name, const GLuint& value) {
	GLuint id = uniformID(name);
	GLenum type = uniformType(name);
    switch(type) {
        case GL_UNSIGNED_INT:
        case GL_BOOL:
            glUniform1ui(id, value);
            break;

        default:
    		throw std::invalid_argument( "Invalid variable type to set with unsigned int or not implemented yet!" );
    }
}


void ShaderProgram::setUniform(std::string name, const GLfloat& value) {
	GLuint id = uniformID(name);
	GLenum type = uniformType(name);
	if(type !=  GL_FLOAT) {
		throw std::invalid_argument( "Invalid variable type to set with float or not implemented yet!" );
		return;
	}
	glUniform1f(id, value);
}


void ShaderProgram::setUniform(std::string name, const GLfloat* values, GLsizei count /*=1*/, GLboolean transpose/*=GL_FALSE*/) {
	GLuint id = uniformID(name);
	GLenum type = uniformType(name);

	switch (type) {
		case GL_FLOAT_VEC2:
			glUniform2fv(id, count, values);
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv(id, count, values);
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv(id, count, values);
			break;
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv(id, count, transpose, values);
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(id, count, transpose, values);
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(id, count, transpose, values);
			break;
		default:
    		throw std::invalid_argument( "Invalid variable type to set with an float* or not implemented yet!" + std::to_string(type));
			return;
	}
}



ShaderProgram::ShaderProgram(bool& success, \
	const std::string vertexShaderPath, \
	const std::string fragmentShaderPath, \
	const std::string geometryShaderPath /*=""*/,
	bool printVariables /*=true*/) {
   	ID = glCreateProgram();

    int vertexShader = shader(GL_VERTEX_SHADER, vertexShaderPath, success);
	if (!success) return;
    glAttachShader( ID, vertexShader );
    int fragmentShader = shader(GL_FRAGMENT_SHADER, fragmentShaderPath, success);
    if (!success) return;
	glAttachShader( ID, fragmentShader );

	int geometryShader = -1;
    if (geometryShaderPath != "") {
        geometryShader = shader(GL_GEOMETRY_SHADER, geometryShaderPath, success);
    	glAttachShader( ID, geometryShader );
        if (!success) return;
    }


    glLinkProgram( ID );

	glDetachShader( ID, vertexShader );
	glDeleteShader( vertexShader );
	glDetachShader( ID, fragmentShader );
	glDeleteShader( fragmentShader );
	if (geometryShader >= 0) {
		glDetachShader( ID, geometryShader );
		glDeleteShader( geometryShader );
	}
	

	//Check for errors
	GLint programSuccess = GL_TRUE;
	glGetProgramiv( ID, GL_LINK_STATUS, &programSuccess );
	if( programSuccess != GL_TRUE )
	{
		printf( "Error linking program %d!\n", ID );
		printLog();
		return;
	}

    initAttributesUniforms(printVariables);
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram( ID );
}

void ShaderProgram::use() {
    glUseProgram( ID );
}

void ShaderProgram::printLog() {
{
	//Make sure program is valid
	if( glIsProgram( ID ) )
	{
        // Get info log
		int infoLogLength = 50;
		int maxLength = infoLogLength;
		glGetProgramiv( ID, GL_INFO_LOG_LENGTH, &maxLength );
		char* infoLog = new char[ maxLength ];
		glGetProgramInfoLog( ID, maxLength, &infoLogLength, infoLog );

        // Print log
		if( infoLogLength > 0 )
		{
			printf( "%s\n", infoLog );
		}
		
		//Deallocate string
		delete[] infoLog;
	}
	else
	{
		printf( "Program %d not properly initialized.", ID );
	}
}

}
