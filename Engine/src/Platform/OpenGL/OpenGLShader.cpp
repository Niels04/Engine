#include "Engpch.hpp"
#include "glCore.hpp"
#include "OpenGLShader.hpp"

namespace Engine
{
    GLshader::GLshader(const char* name)
        : m_filepath("res/OpenGLshaders/"), m_renderer_id(0)
    {
        m_filepath.append(name);
        ShaderProgramSource source = parseShader(m_filepath);
        m_renderer_id = createShader(source.VertexSource, source.FragmentSource);
    }

        /*Cshader::Cshader(std::string_view name, vec3_vt<bool, GLint, GLenum> params)
            : m_filepath("res/shaders/precompiled/"), m_renderer_id(0)
        {
            m_filepath.append(name);
            loadBinary(name, params.v1, params.v2);
        }*/

    GLshader::~GLshader()
    {
        GLCALL(glDeleteProgram(m_renderer_id));
    }

    ShaderProgramSource GLshader::parseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);

        ENG_CORE_ASSERT(stream.is_open(), "Ifstream wasn't open when trying to parse shader.");

        enum class ShaderType
        {
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        while (getline(stream, line))//if getline() is true there are still more lines to read in the file
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = ShaderType::VERTEX;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = ShaderType::FRAGMENT;
                }
            }
            else
            {
                ss[(int)type] << line << "\n";//we use "type" as our index
            }
        }

        return { ss[0].str(), ss[1].str() }; //we return our struct
    }

    uint32_t GLshader::compileShader(unsigned int type, const std::string& source) //don't want to always use the OpenGl types, so we write unsigned int insted of
    {
        GLCALL(unsigned int id = glCreateShader(type));
        const char* src = source.c_str();
        GLCALL(glShaderSource(id, 1, &src, nullptr));
        GLCALL(glCompileShader(id));
        //ERROR Catching
        int result;
        GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
        if (result == GL_FALSE)
        {
            int length;
            GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* message = (char*)alloca(length * sizeof(char));//allocates enough memory on the stack for our char-array
            GLCALL(glGetShaderInfoLog(id, length, NULL, message));//third param set to NULL. It would return the length of the returned string
            std::cout << "Failed to compile ";
            switch (type)
            {
            case GL_VERTEX_SHADER:
            {
                std::cout << "vertex";
            }break;
            case GL_FRAGMENT_SHADER:
            {
                std::cout << "fragment";
            }break;
            default:
            {
                std::cout << "\"TypeERROR\"";
            }break;
            }
            std::cout << "-shader:" << std::endl;
            std::cout << message << std::endl;

            GLCALL(glDeleteShader(id));

            return 0;
        }

        return id;
    }

    uint32_t GLshader::createShader(const std::string vertexShader, const std::string fragmentShader)
    {
        GLCALL(unsigned int program = glCreateProgram());
        unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);

        GLCALL(glAttachShader(program, vs));
        GLCALL(glAttachShader(program, fs));
        GLCALL(glLinkProgram(program));
        GLCALL(glValidateProgram(program));
        //allways detach shaders after successful linking
        glDetachShader(program, vs);
        glDetachShader(program, fs);
        //then delete them
        GLCALL(glDeleteShader(vs));
        GLCALL(glDeleteShader(fs));

        return program;
    }

    void GLshader::bind() const
    {
        GLCALL(glUseProgram(m_renderer_id));
    }

    void GLshader::unbind() const
    {
            GLCALL(glUseProgram(0));
    }

        /*vec3_vt<bool, GLint, GLenum> Cshader::saveBinary(std::string_view name) const//WE WILL HAVE TO ADD A LOT OF STUFF TO THIS FUCTION
        {
            GLint buffersize;
            void* buffer = NULL;
            GLenum binaryFormat;
            GLCALL(glGetProgramiv(m_renderer_id, GL_PROGRAM_BINARY_LENGTH, &buffersize));
            if (buffersize != NULL)
            {
                buffer = malloc(buffersize);
                GLCALL(glGetProgramBinary(m_renderer_id, buffersize, NULL, &binaryFormat, buffer));
            }
            else
            {
                std::cout << "Failed to save shader-binary. \"GL_PROGRAM_BINARY_LENGT\" was 0.\n";
                ASSERT(false);
            }
            //write the binary to file:
            std::ofstream fileOutput(std::string("res/shaders/precompiled/").append(name));//creates a file with name of the shader in the dir for precompiled shaders
            if (fileOutput.is_open())
            {
                fileOutput.write((char*)buffer, buffersize);//write buffer to the file
            }
            else
            {
                std::cout << "Error saving compiled shader-binary! Can't create file!\n";
                ASSERT(false);
            }
            free(buffer);
            fileOutput.close();

            return vec3_vt<bool, GLint, GLenum>(true, buffersize, binaryFormat);
        }*/

        /*void Cshader::loadBinary(std::string_view name, GLint size, GLenum format)
        {
            //load the data into an array:
            std::ifstream fileInput(std::string("res/shaders/precompiled/").append(name));
            if (fileInput.is_open())
            {
                void* buffer = malloc(size);
                fileInput.read((char*)buffer, size);
                GLCALL(unsigned int program = glCreateProgram());
                GLCALL(glProgramBinary(program, format, buffer, size));
                free(buffer);
                GLint status;
                //test if program got linked successfully:
                GLCALL(glGetProgramiv(program, GL_LINK_STATUS, &status));
                if (GL_FALSE == status)
                {
                    std::cout << "Error linking precompiled Program \"" << name << "\".\n";
                    ASSERT(false);
                }
                GLCALL(glValidateProgram(program));
                GLCALL(glGetProgramiv(program, GL_VALIDATE_STATUS, &status));
                if (GL_FALSE == status)
                {
                    std::cout << "Error validating precompiled Program \"" << name << "\".\n";
                    ASSERT(false);
                }
            }
            else
            {
                std::cout << "Error loading precompiled shader with name \"" << name << "\"!\n";
                ASSERT(false);
            }
        }*/

        /*void Cshader::setprojData(float zFar, float zNear, vec2<float> fov)
        {
            setUniform2f("u_tanfov", tanf(fov.v0 * 0.5f * (3.14159f / 180.0f)), tanf(fov.v1 * 0.5f * (3.14159f / 180.0f)));
            setUniform1f("u_near", zNear);
            setUniform1f("u_far", zFar);
        }*/
        /*void Cshader::setprojMat(mat44f& projMat)
        {
            setUniformMat4f("u_mat", projMat);
        }*/

        /*void Cshader::setOffset(vec3<float> model, vec3<float> cam)
        {
            setUniform3f("u_offset", model.v0 - cam.v0, model.v1 - cam.v1, model.v2 - cam.v2);
            setUniform3f("u_modelPos", model);
        }*/
        /*void Cshader::setRot(vec2<float> camRot)
        {
            setUniform2f("u_rot", camRot.v0, camRot.v1);
        }*/

        //setuniforms:
    void GLshader::setUniform1f(const std::string& name, float fValue)
    {
        GLCALL(glUniform1f(getUniformLocation(name), fValue));
    }
    void GLshader::setUniform2f(const std::string& name, float v0, float v1)
    {
        GLCALL(glUniform2f(getUniformLocation(name), v0, v1));
    }
    void GLshader::setUniform3f(const std::string& name, float v0, float v1, float v2)
    {
        GLCALL(glUniform3f(getUniformLocation(name), v0, v1, v2));
    }
    /*void Cshader::setUniform3f(const std::string& name, vec3<float> values)
    {
        GLCALL(glUniform3f(getUniformLocation(name), values.v0, values.v1, values.v2));
    }*/
    void GLshader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        GLCALL(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
    }
    void GLshader::setUniform1i(const std::string& name, int value)
    {
        GLCALL(glUniform1i(getUniformLocation(name), value));
    }
    /*void Cshader::setUniformMat4f(const std::string& name, const mat44f& mat)
    {
        GLCALL(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat.m_mat[0][0]));//last param specifies a pointer to our array of numbers
        //third param set to true when our matrix needs to get transposed, so that openGl can read it properly, but glm automatically does that for us
    }*/
    //setuniforms end

    int GLshader::getUniformLocation(const std::string& name)//returns an int or an unsigned int?
    {
        if (m_uniformLocation_cache.find(name) != m_uniformLocation_cache.end())//if uniform is already in cache we don't need to search for its location again
            return m_uniformLocation_cache[name];

        GLCALL(int location = glGetUniformLocation(m_renderer_id, name.c_str()));
        if (location == -1)//if location is -1, openGl couldn't find it
            ENG_CORE_WARN("Uniform {0} not found.", name);

        m_uniformLocation_cache[name] = location;//creates a new entry every time because at this point were sure there is no entry m_uniformLocation_cache[name]
        //if a location is -1 we store it anyways so we already know it doesn't exist the next time we search for it

        return location;
    }
}