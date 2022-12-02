#pragma once
#include "GL/glew.h"
#include "Application.h"

#include <iostream>
#include <sstream>
#include <fstream>

class GameOfLife :
    public Application
{
public:
    GameOfLife(const std::string& name, uint32_t width, uint32_t height);

    void Init();
    void Shutdown();

    void Update(float deltatime);
    void Draw(float deltatime);

private:
	struct ShaderSource
	{
		uint32_t type;
		std::string source;
	};
	ShaderSource ParseShader(const std::string& filepath)
	{
		std::ifstream stream(filepath);

		std::string line;
		std::stringstream ss;
		uint32_t type = GL_INVALID_ENUM;

		while (std::getline(stream, line))
		{
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = GL_VERTEX_SHADER;
				else if (line.find("fragment") != std::string::npos)
					type = GL_FRAGMENT_SHADER;
			}
			else
			{
				ss << line << '\n';
			}
		}

		return { type, ss.str() };
	};
	GLuint CompileShader(const ShaderSource& shaderSource)
	{
		unsigned int id = glCreateShader(shaderSource.type);
		const char* src = shaderSource.source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		//error handling
		int result;
		glGetShaderiv(id, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE)
		{
			int length;
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = (char*)alloca(length * sizeof(char));
			glGetShaderInfoLog(id, length, &length, message);
			std::cout << "Failed to compile " << (shaderSource.type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!" << std::endl;
			std::cout << message << std::endl;
			glDeleteShader(id);
			return 0;
		}

		return id;
	};
	GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		unsigned int program = glCreateProgram();
		unsigned int vs = CompileShader(ParseShader(vertexShader));
		unsigned int fs = CompileShader(ParseShader(fragmentShader));

		glAttachShader(program, vs);
		glAttachShader(program, fs);

		glLinkProgram(program);
		glValidateProgram(program);

		glDeleteShader(vs);
		glDeleteShader(fs);

		return program;
	}
};


