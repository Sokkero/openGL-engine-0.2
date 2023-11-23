#pragma once

#include "../../nodeComponents/GeometryComponent.h"
#include "RenderManager.h"
#include "UboBlock.h"
#include <utility>

namespace Engine
{
    inline const unsigned int GLOBAL_VERTEX_POSITION = 0;
    inline const unsigned int GLOBAL_VERTEX_COLOR = 1;
    inline const unsigned int GLOBAL_VERTEX_NORMAL = 2;

    class Shader
    {
        public:
            Shader() = default;
            ~Shader() = default;

            void registerShader(RenderManager& renderManager, const std::string& shaderPath, const std::string& shaderName)
            {
                m_shaderIdentifier = renderManager.registerShader(shaderPath, shaderName);
            };

            virtual void renderVertices(GeometryComponent* object, const glm::mat4& mvp) = 0;

            std::pair<std::string, GLuint> getShaderIdentifier() { return m_shaderIdentifier; }

            std::map<std::string, GLint> getAllActiveUniforms() { return m_activeUniforms; }

            void addActiveUniform(const char* uniform)
            {
                const GLuint index = glGetUniformLocation(m_shaderIdentifier.second, uniform);

                if(index == GL_INVALID_VALUE)
                {
                    fprintf(stderr, "Uniform index not found! Shader invalid");
                    return;
                }
                else if(index == GL_INVALID_OPERATION)
                {
                    fprintf(stderr, "Uniform index not found! Linking failed");
                    return;
                }

                m_activeUniforms.emplace(uniform, index);
            }

            GLint getActiveUniform(const std::string& uniform)
            {
                const auto& uniformPosition = m_activeUniforms[uniform];

                if(!uniformPosition)
                {
                    return -1;
                }

                return uniformPosition;
            }

            void removeActiveUniform(const std::string& uniform) { m_activeUniforms.erase(uniform); }

            std::vector<std::shared_ptr<UboBlock>> getBoundUbos() { return m_boundUbos; }

            void bindUbo(const std::shared_ptr<UboBlock>& ubo)
            {
                unsigned int index =
                        glGetUniformBlockIndex(m_shaderIdentifier.second, ubo->getBindingPoint().first);

                if(index == GL_INVALID_INDEX)
                {
                    fprintf(stderr, "Ubo index not found!");
                    return;
                }

                glUniformBlockBinding(m_shaderIdentifier.second, index, ubo->getBindingPoint().second);

                m_boundUbos.push_back(ubo);
            }

            void removeBoundUbo(const std::shared_ptr<UboBlock>& ubo)
            {
                m_boundUbos.erase(std::remove(m_boundUbos.begin(), m_boundUbos.end(), ubo), m_boundUbos.end());
            }

            void bindTexture(
                    GLuint attribId,
                    GLenum targetType,
                    GLuint bufferId,
                    int size,
                    GLenum dataType,
                    bool normalized,
                    int stride,
                    GLenum textureType,
                    GLenum textureTargetType,
                    GLuint textureBufferId,
                    GLint textureSamplerUniformId
            )
            {
                glActiveTexture(textureType);
                glBindTexture(textureTargetType, textureBufferId);
                glUniform1i(textureSamplerUniformId, 0);

                bindVertexData(attribId, targetType, bufferId, size, dataType, normalized, stride);
            }

            void bindVertexData(
                    GLuint attribId,
                    GLenum targetType,
                    GLuint bufferId,
                    int size,
                    GLenum dataType,
                    bool normalized,
                    int stride
            )
            {
                glEnableVertexAttribArray(attribId);
                glBindBuffer(targetType, bufferId);
                glVertexAttribPointer(attribId, size, dataType, normalized, stride, (void*)nullptr);
            }

        private:
            std::pair<std::string, GLuint> m_shaderIdentifier;
            std::map<std::string, GLint> m_activeUniforms;
            std::vector<std::shared_ptr<UboBlock>> m_boundUbos;
    };
} // namespace Engine