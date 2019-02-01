

#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <map>

#include "GL/glew.h"
#include <SDL2/SDL_opengl.h>


typedef std::pair<GLuint, GLenum> VariableData;
typedef std::map<std::string, VariableData> ShaderProgramVariableMap;


class ShaderProgram {
    public: 
        GLuint ID;

		static GLuint shader(const int shaderType, const std::string &filename, bool &success);

        ShaderProgram() : ID(-1) {};
        ShaderProgram(bool &success, 
			const std::string &vertexShaderPath, 
			const std::string &fragmentShaderPath, 
			const std::string &geometryShaderPath ="",
			bool printVariables=false);
		~ShaderProgram();

        void use();
        void printLog();
		void initAttributesUniforms(bool printVariables);

		GLuint uniformID(std::string name);
		GLenum uniformType(std::string name);
        GLuint attributeID(std::string name);
		GLenum attributeType(std::string name);

		void setUniform(std::string name, const GLint& value);
		void setUniform(std::string name, const GLfloat& value);
		void setUniform(std::string name, const GLuint& value);
		void setUniform(std::string name, const GLfloat* values, GLsizei count=1, GLboolean transpose=false);

	private:
		ShaderProgramVariableMap uniformVariable;
		ShaderProgramVariableMap attributeVariable;
};


#endif // SHADER_HPP
