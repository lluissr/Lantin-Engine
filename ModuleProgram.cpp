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

	//vertex
	char* dataVertex = nullptr;
	FILE* filevertex = nullptr;
	int sizeVertex;
	fopen_s(&filevertex, "../Default.vs", "rb");
	if (filevertex)
	{
		fseek(filevertex, 0, SEEK_END);
		sizeVertex = ftell(filevertex);
		rewind(filevertex);
		dataVertex = (char*)malloc(sizeVertex + 1);
		fread(dataVertex, 1, sizeVertex, filevertex);
		dataVertex[sizeVertex] = 0;
		fclose(filevertex);
	}

	//fragment
	char* dataFragment = nullptr;
	FILE* file = nullptr;
	int sizeFragment;
	fopen_s(&file, "../default.fs", "rb");
	if (file)
	{
		fseek(file, 0, SEEK_END);
		sizeFragment = ftell(file);
		rewind(file);
		dataFragment = (char*)malloc(sizeFragment + 1);
		fread(dataFragment, 1, sizeFragment, file);
		dataFragment[sizeFragment] = 0;
		fclose(file);
	}

	GLuint vertexShader, fragmentShader;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &dataVertex, &sizeVertex);
	glCompileShader(vertexShader);
	
	glShaderSource(fragmentShader, 1, &dataFragment, &sizeFragment);
	glCompileShader(fragmentShader);

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

	//Program
	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	////------------------ TODO: Check if linked is correct
	//GLint linked;
	//char infoLog[512];
	//glGetProgramiv(program, GL_LINK_STATUS, &linked);
	//if (!linked) {
	//	glGetProgramInfoLog(program, 512, NULL, infoLog);
	//}
	//glDeleteShader(linked);
	////-------------------------------------

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
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
	return true;
}