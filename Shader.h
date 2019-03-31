#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>

class Shader
{
public:
	GLuint Program;

	Shader() {}

	// Constructor generates the shader on the fly
	Shader(std::string paths[], int flags[])
	{
		// Shader Program
		this->Program = glCreateProgram();
		std::vector<GLuint> shaders;

		GLint success;
		GLchar infoLog[512];

		for (int i = 0; i < 5; i++)
		{
			if (flags[i] != 0)
			{
				std::string shaderCode;
				std::ifstream shaderFile;

				shaderFile.exceptions(std::ifstream::badbit);
				try
				{
					// Open files
					shaderFile.open(paths[i]);
					std::stringstream shaderStream;
					// Read file's buffer contents into streams
					shaderStream << shaderFile.rdbuf();
					// close file handlers
					shaderFile.close();
					// Convert stream into string
					shaderCode = shaderStream.str();
				}
				catch (std::ifstream::failure e)
				{
					std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
				}
				const GLchar *myShaderCode = shaderCode.c_str();
				// 2. Compile shaders
				GLuint myShader;
				
				// Vertex Shader
				if (i == 0)
					myShader = glCreateShader(GL_VERTEX_SHADER);
				else if (i == 1)
					myShader = glCreateShader(GL_TESS_CONTROL_SHADER);
				else if (i == 2)
					myShader = glCreateShader(GL_TESS_EVALUATION_SHADER);
				else if (i == 3)
					myShader = glCreateShader(GL_GEOMETRY_SHADER);
				else
					myShader = glCreateShader(GL_FRAGMENT_SHADER);

				glShaderSource(myShader, 1, &myShaderCode, NULL);
				glCompileShader(myShader);
				// Print compile errors if any
				glGetShaderiv(myShader, GL_COMPILE_STATUS, &success);
				if (!success)
				{
					glGetShaderInfoLog(myShader, 512, NULL, infoLog);
					std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
				}

				glAttachShader(this->Program, myShader);
				shaders.push_back(myShader);
			}
			else
			{
				continue;
			}
		}

		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}

		// Delete the shaders as they're linked into our program now and no longer necessery
		for (int i = 0; i < shaders.size(); i++)
		{
			glDeleteShader(shaders[i]);
		}
		shaders.clear();
	}
	// Uses the current shader
	void Use()
	{
		glUseProgram(this->Program);
	}
};

#endif