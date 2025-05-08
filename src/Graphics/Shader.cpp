#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    std::string vertexCode; // Vertex shader code
    std::string fragmentCode; // Fragment shader code
    std::ifstream vShaderFile; // Vertex shader file
    std::ifstream fShaderFile; // Fragment shader file
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Set exceptions for file operations
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit); // Set exceptions for file operations

    try
    {
        vShaderFile.open(vertexPath); // Open vertex shader file
        fShaderFile.open(fragmentPath); // Open fragment shader file
        std::stringstream vShaderStream, fShaderStream; // String streams for shader code

        vShaderStream << vShaderFile.rdbuf(); // Read vertex shader file into string stream
        fShaderStream << fShaderFile.rdbuf(); // Read fragment shader file into string stream

        vShaderFile.close(); // Close vertex shader file
        fShaderFile.close(); // Close fragment shader file

        vertexCode = vShaderStream.str(); // Convert vertex shader string stream to string
        fragmentCode = fShaderStream.str(); // Convert fragment shader string stream to string
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }

    const char* vShaderCode = vertexCode.c_str(); // Convert vertex shader string to C-style string
    const char* fShaderCode = fragmentCode.c_str(); // Convert fragment shader string to C-style string

    unsigned int vertex, fragment; // Shader IDs

    vertex = glCreateShader(GL_VERTEX_SHADER); // Create vertex shader
    glShaderSource(vertex, 1, &vShaderCode, NULL); // Set vertex shader source code
    glCompileShader(vertex); // Compile vertex shader
    checkCompileErrors(vertex, "VERTEX"); // Check for compilation errors in vertex shader

    fragment = glCreateShader(GL_FRAGMENT_SHADER); // Create fragment shader
    glShaderSource(fragment, 1, &fShaderCode, NULL); // Set fragment shader source code
    glCompileShader(fragment); // Compile fragment shader
    checkCompileErrors(fragment, "FRAGMENT"); // Check for compilation errors in fragment shader

    ID = glCreateProgram(); // Create shader program
    glAttachShader(ID, vertex); // Attach vertex shader to program
    glAttachShader(ID, fragment); // Attach fragment shader to program
    glLinkProgram(ID); // Link shader program
    checkCompileErrors(ID, "PROGRAM"); // Check for linking errors in shader program

    glDeleteShader(vertex); // Delete vertex shader
    glDeleteShader(fragment); // Delete fragment shader
}

Shader::~Shader()
{
    glDeleteProgram(ID); // Delete shader program
}

void Shader::use()
{
    glUseProgram(ID); // Use shader program
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); // Set boolean uniform
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value); // Set integer uniform
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); // Set float uniform
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)); // Set matrix uniform
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)); // Set matrix uniform
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat)); // Set matrix uniform
}

void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success; // Compilation status
    GLchar infoLog[1024]; // Error message buffer
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // Get compilation status
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog); // Get error message
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success); // Get linking status
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog); // Get error message
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}



/*
 Explanation:
  Constructor (Shader::Shader):
      Takes paths to vertex and fragment shader files.
      Reads the content of these files into strings.
      Creates a vertex shader object (glCreateShader(GL_VERTEX_SHADER)), sets its source (glShaderSource), and compiles it (glCompileShader).
      Does the same for the fragment shader.
      Creates a shader program (glCreateProgram()), attaches the compiled shaders (glAttachShader), and links them (glLinkProgram).
      Calls checkCompileErrors to report any issues during compilation or linking.
      Deletes the individual shader objects (glDeleteShader) after linking, as they are now part of the program.

      Destructor (Shader::~Shader):
          Cleans up by deleting the shader program (glDeleteProgram(ID)).

      use():
          Activates the shader program for subsequent rendering calls (glUseProgram(ID)).

      setBool, setInt, setFloat:
          These are helper functions to set uniform variables in the shader. glGetUniformLocation finds the location of a uniform by its name, and glUniform* functions set its value. We'll add more for vectors and matrices when we need GLM.

      checkCompileErrors():
          A private helper to check for errors after compiling a shader or linking a program. It prints error messages to std::cerr.
*/