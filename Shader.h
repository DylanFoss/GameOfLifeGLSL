#pragma once
#include "GL/glew.h"

namespace Utils{

struct ShaderSource
{
	uint32_t type;
	std::string source;
};

class Shader
{
public:
	GLuint CompileShader(const ShaderSource& shaderSource);

private:

	GLuint m_ShaderID;

};

}
