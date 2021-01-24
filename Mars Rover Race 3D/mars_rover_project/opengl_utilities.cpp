#include "opengl_utilities.h"

/* OpenGL Utility Structs */

VAO::VAO(
    const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec3>& normals,
    const std::vector<glm::vec2>& uvs,
    const std::vector<GLuint>& indices
)
{
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    vertex_count = GLsizei(positions.size());

    glGenBuffers(1, &position_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec3), positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &normals_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &uvs_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_buffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, static_cast<void *>(0));
    glEnableVertexAttribArray(2);


    element_array_count = GLsizei(indices.size());

    glGenBuffers(1, &element_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
};

/* OpenGL Utility Functions */
GLuint CreateShaderFromSource(const GLenum& shader_type, const GLchar * source)
{
	GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		std::cout << "Error: Shader Compilation failed" << std::endl;

		char info_log[512];
		glGetShaderInfoLog(shader, 512, NULL, info_log);
		std::cout << info_log << std::endl;

		glDeleteShader(shader);
		return NULL;
	}

	return shader;
}

GLuint CreateProgramFromSources(const GLchar * vertex_shader_source, const GLchar * fragment_shader_source)
{
    GLuint program = glCreateProgram();

    GLuint vertex_shader = CreateShaderFromSource(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = CreateShaderFromSource(GL_FRAGMENT_SHADER, fragment_shader_source);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    char info_log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::cout << "Error: Program Linking failed" << std::endl;
        glGetProgramInfoLog(program, 512, NULL, info_log);
        std::cout << info_log << std::endl;

        glDeleteProgram(program);
        return NULL;
    }

    return program;
}
