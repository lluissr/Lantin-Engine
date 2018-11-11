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

	program = CreateProgram("Default.vs", "default.fs");
	axisProgram = CreateProgram("defaultColor.vs", "defaultColor.fs");

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

	assert(vertexShaderPath != NULL);
	assert(fragmentShaderPath != NULL);

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
	assert(path != NULL);

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