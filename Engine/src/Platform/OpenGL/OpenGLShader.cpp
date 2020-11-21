#include "Engpch.hpp"
#include "glCore.hpp"
#include "OpenGLShader.hpp"

namespace Engine
{
    static GLenum shaderTypeFromString(const std::string& type)
    {
        if (type == "vertex")                   return GL_VERTEX_SHADER;
        if (type == "fragment")                 return GL_FRAGMENT_SHADER;
        if (type == "pixel")                    return GL_FRAGMENT_SHADER;
        if (type == "tesselation-control")      return GL_TESS_CONTROL_SHADER;
        if (type == "tesselation-evaluation")   return GL_TESS_EVALUATION_SHADER;
        if (type == "geometry")                 return GL_GEOMETRY_SHADER;
        if (type == "compute")                  return GL_COMPUTE_SHADER;

        ENG_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }


    GLshader::GLshader(const char* name)
        : m_filepath("res/OpenGLshaders/"), m_renderer_id(0)
    {
        m_filepath.append(name);
        std::unordered_map<GLenum, std::string> sources = parseShader(m_filepath);//first is shaderType, second is source
        m_renderer_id = createShader(sources);
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

    std::unordered_map<GLenum, std::string> GLshader::parseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath, std::ios::in, std::ios::binary);//read in binary data, because the shader is already in the format we want it to be
        ENG_CORE_ASSERT(stream.is_open(), "Ifstream wasn't open when trying to parse shader '{0}'.", filepath);
        //if no errors occurr proceed to load the shader into a string
        std::string source;
        stream.seekg(0, std::ios::end);
        source.resize(stream.tellg());
        stream.seekg(0, std::ios::beg);
        stream.read(&source[0], source.size());
        stream.close();

        std::unordered_map<GLenum, std::string> shaderSources;//shader sources get parsed in this map with their type as a key

        const char* typeToken = "#type";
        size_t typeTokenLength = strlen(typeToken);
        size_t pos = source.find(typeToken, 0);
        ENG_CORE_ASSERT(pos != std::string::npos, "Synthax error when trying to parse shader.");
        while (pos != std::string::npos)//parses all shaders into the map
        {
            size_t temp = source.find_first_of("\r\n", pos);//look for a newline or a carrigage return -> this will be the end of the type-string
            ENG_CORE_ASSERT(temp != std::string::npos, "Synthax error when trying to parse shader.");
            size_t begin = pos + typeTokenLength + 1;
            std::string sType = source.substr(begin, temp - begin);//read the type from the end of the type-token to the next newline(so there needs to be a newline after the "#type <shader>"-statement
            GLenum type = shaderTypeFromString(sType);
            //reset the whole thing and safe the current shader into the map
            size_t beginShaderCode = source.find_first_not_of("\r\n", temp);//find the position where our shader code starts(the first thing that isn't a newline after our type-statement)
            pos = source.find(typeToken, beginShaderCode);//find the end of the shader by searching for the next type-statement
            shaderSources[type] = source.substr(beginShaderCode, pos - (beginShaderCode == std::string::npos ? source.size() : beginShaderCode));
        }

        return shaderSources;
    }

    uint32_t GLshader::createShader(const std::unordered_map<GLenum, std::string>& shaderSource)
    {
        GLCALL(unsigned int program = glCreateProgram());
        unsigned int shaderIDs[6];//there should never be more than 6 shaders in one program
        ENG_CORE_ASSERT(shaderSource.size() <= 6U, "There were more than 6 shaders defined, which should not be possible(glsl only defines 6 shaderTypes).");
        uint8_t count = 0;//how many times has the loop been executed?
        for (const std::pair<GLenum, std::string>& element : shaderSource)//iterate over all shaders
        {
            shaderIDs[count] = compileShader(element.first, element.second);
            GLCALL(glAttachShader(program, shaderIDs[count]));
            count++;
        }
        //link and validate the shader-program
        GLCALL(glLinkProgram(program));
        GLCALL(glValidateProgram(program));

        //allways detach shaders after successful linking and also delete them:
        for (count = 0; count < shaderSource.size(); count++)
        {
            glDetachShader(program, shaderIDs[count]);
            GLCALL(glDeleteShader(shaderIDs[count]));
        }
        
        return program;
    }

    uint32_t GLshader::compileShader(const GLenum type, const std::string& source)
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
            case GL_TESS_CONTROL_SHADER:
            {
                std::cout << "tesselation-control";
            }break;
            case GL_TESS_EVALUATION_SHADER:
            {
                std::cout << "tesselation-evaluation";
            }break;
            case GL_GEOMETRY_SHADER:
            {
                std::cout << "geometry";
            }break;
            case GL_COMPUTE_SHADER:
            {
                std::cout << "compute";
            }
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

    //setuniforms:
    void GLshader::setUniform1b(const std::string& name, bool bValue)//just for comfort, gets converted to int anyways
    {
        GLCALL(glUniform1i(getUniformLocation(name), (int)bValue));
    }
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
    void GLshader::setUniform3f(const std::string& name, const vec3& vec)
    {
        GLCALL(glUniform3f(getUniformLocation(name), vec.x, vec.y, vec.z));
    }
    void GLshader::setUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
    {
        GLCALL(glUniform4f(getUniformLocation(name), v0, v1, v2, v3));
    }
    void GLshader::setUniform4f(const std::string& name, const vec4& vec)
    {
        GLCALL(glUniform4f(getUniformLocation(name), vec.x, vec.y, vec.z, vec.w));
    }
    void GLshader::setUniform1i(const std::string& name, int value)
    {
        GLCALL(glUniform1i(getUniformLocation(name), value));
    }
    void GLshader::setUniformMat4(const std::string& name, const mat4& mat)
    {
        GLCALL(glUniformMatrix4fv(getUniformLocation(name), 1, GL_TRUE, &mat.mat[0][0]));//last param specifies a pointer to our array of numbers
        //third param set to true when our matrix needs to get transposed, gets set to true here, because I use my own library, that doesn't do that automatically
    }
    //setuniforms end
    void GLshader::bindUniformBlock(const std::string& name, uint32_t bindingPoint)
    {
        GLCALL(uint32_t index = glGetUniformBlockIndex(m_renderer_id, name.c_str()));
        ENG_CORE_ASSERT(index != GL_INVALID_INDEX, "Error when trying to retrieve uniform block location \"{0}\". Could not find the block in the shader.", name.c_str());
        GLCALL(glUniformBlockBinding(m_renderer_id, index, bindingPoint));
    }

    int GLshader::getUniformLocation(const std::string& name)
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