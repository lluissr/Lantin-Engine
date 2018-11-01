#include "Application.h"
#include "ModuleProgram.h"
#include "GL/glew.h"
#include "SDL.h"

ModuleProgram::ModuleProgram()
{
}

ModuleProgram::~ModuleProgram()
{
}

bool ModuleProgram::Init()
{

	program = CreateProgram("../Default.vs", "../default.fs");
	axisProgram = CreateProgram("../defaultColor.vs", "../defaultColor.fs");

	//------------------ TODO: Check if compile is correct
	//GLint compiledVertex;
	//GLint compiledFragment;

	//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiledVertex);
	//if (!compiledVertex)
	//{
	//	GLint blen = 0;
	//	GLsizei slen = 0;

	//	glGetShaderiv(compiledVertex, GL_INFO_LOG_LENGTH, &blen);
	//	if (blen > 1)
	//	{
	//		GLchar* compiler_log = (GLchar*)malloc(blen);
	//		glGetInfoLogARB(compiledVertex, blen, &slen, compiler_log);
	//		//cout << "compiler_log:\n", compiler_log);
	//		free(compiler_log);
	//	}
	//}

	//glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiledFragment);
	//if (!compiledFragment)
	//{

	//}
	//glDeleteShader(compiledVertex);
	//glDeleteShader(compiledFragment);
	//-------------------------------------

	////------------------ TODO: Check if linked is correct
	//GLint linked;
	//char infoLog[512];
	//glGetProgramiv(program, GL_LINK_STATUS, &linked);
	//if (!linked) {
	//	glGetProgramInfoLog(program, 512, NULL, infoLog);
	//}
	//glDeleteShader(linked);
	////-------------------------------------

	return true;
}

update_status ModuleProgram::PreUpdate()
{
	return UPDATE_CONTINUE;
}


update_status ModuleProgram::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleProgram::CleanUp()
{
	glDeleteProgram(program);
	glDeleteProgram(axisProgram);
	return true;
}

GLuint ModuleProgram::CreateProgram(const char* vertexShaderPath, const char* fragmentShaderPath) const {
	
	char* dataVertex = ReadShader(vertexShaderPath);
	char* dataFragment = ReadShader(fragmentShaderPath);

	GLuint vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &dataVertex, NULL);
	glCompileShader(vertexShader);

	glShaderSource(fragmentShader, 1, &dataFragment, NULL);
	glCompileShader(fragmentShader);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

char* ModuleProgram::ReadShader(const char* path) const
{
	char* dataVertex = nullptr;
	FILE* file = nullptr;
	int sizeFragment;
	fopen_s(&file, path, "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		sizeFragment = ftell(file);
		rewind(file);
		dataVertex = (char*)malloc(sizeFragment + 1);
		fread(dataVertex, 1, sizeFragment, file);
		dataVertex[sizeFragment] = 0;
		fclose(file);
	}

	return dataVertex;
}