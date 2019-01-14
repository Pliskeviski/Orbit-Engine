#include "obtpch.h"
#include "glRenderer.h"

#include <vector>
#include "Orbit/Core.h"

#include "Orbit/Components/Camera.h"

namespace Orbit {
	glRenderer::glRenderer() {
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* renderer = glGetString(GL_RENDERER);

		std::string vertexPath   = __FILE__ "\\..\\Shader\\Shaders\\TransformVertexShader.vertexshader";
		std::string fragmentPath = __FILE__ "\\..\\Shader\\Shaders\\ColorFragmentShader.fragmentshader";
		this->m_Shader = new Shader(vertexPath, fragmentPath);

		glViewport(0, 0, 1280, 768);

		ORBIT_CORE_INFO("OpenGL {0} - {1} {2}", version, renderer, vendor);
	}

	void glRenderer::Draw() {
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->m_Shader->Use();
	}

	uint32_t glRenderer::CreateBuffer(subMesh* submesh) {

		glGenVertexArrays(1, &submesh->m_VAO);
		glBindVertexArray(submesh->m_VAO);

		//// Add buffers 
		submesh->m_VBOs.push_back(genVBO(submesh, submesh->m_VerticesCount * sizeof(glm::vec3), 3, &submesh->m_Vertices[0], GL_STATIC_DRAW));

		////if (submesh->hasUVs)
		////	s_mesh->VBOs.push_back(genVBO(s_mesh, s_mesh->vertSize * sizeof(glm::vec2), 2, &s_mesh->UVs[0], GL_STATIC_DRAW));

		////if (s_mesh->hasNormals)
		////	s_mesh->VBOs.push_back(genVBO(s_mesh, s_mesh->vertSize * sizeof(glm::vec3), 3, &s_mesh->Normals[0], GL_STATIC_DRAW));

		////if (!s_mesh->Tangents.empty())
		////	s_mesh->VBOs.push_back(genVBO(s_mesh, s_mesh->Tangents.size() * sizeof(glm::vec3), 3, &s_mesh->Tangents[0], GL_STATIC_DRAW));

		////if (!s_mesh->BiTangents.empty())
		////	s_mesh->VBOs.push_back(genVBO(s_mesh, s_mesh->BiTangents.size() * sizeof(glm::vec3), 3, &s_mesh->BiTangents[0], GL_STATIC_DRAW));

		glGenBuffers(1, &submesh->m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, submesh->m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, submesh->m_IndicesCount * sizeof(uint32_t), &submesh->m_Indices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		return 1;
	}
	int glRenderer::drawSubMesh(subMesh* submesh, Camera* scene_camera) {
		/*glBindVertexArray(submesh->m_VAO);
		for (unsigned int j = 0; j < submesh->m_VBOs.size(); j++) {
			glEnableVertexAttribArray(j);
		}

		glDrawElements(GL_TRIANGLES, submesh->m_IndicesCount, GL_UNSIGNED_SHORT, (void*)submesh->m_Indices[0]);
		for (int j = 0; j < submesh->m_VBOs.size(); j++) {
			glDisableVertexAttribArray(j);
		}*/

		GLuint ViewID       = glGetUniformLocation(this->m_Shader->getID(), "view");
		GLuint ModelID      = glGetUniformLocation(this->m_Shader->getID(), "model");
		GLuint ProjectionID = glGetUniformLocation(this->m_Shader->getID(), "projection");

		Math::mat4 Projection	= glm::perspective(scene_camera->getFov(), (GLfloat)scene_camera->m_Width / (GLfloat)scene_camera->m_Height, 0.1f, 1000.0f);
		Math::mat4 View			= scene_camera->getViewMatrix();
		Math::mat4 Model		= glm::translate(glm::mat4(1.f), submesh->m_Transform->getPosition()  + submesh->m_Parent->getPosition());
		Math::mat4 ModelRX		= glm::rotate	(glm::mat4(1.f), submesh->m_Transform->getRotationX() + submesh->m_Parent->getRotationX(), glm::vec3(1.0f, .0f, .0f));
		Math::mat4 ModelRY		= glm::rotate	(glm::mat4(1.f), submesh->m_Transform->getRotationY() + submesh->m_Parent->getRotationY(), glm::vec3(.0f, 1.0f, .0f));
		Math::mat4 ModelRZ		= glm::rotate	(glm::mat4(1.f), submesh->m_Transform->getRotationZ() + submesh->m_Parent->getRotationZ(), glm::vec3(.0f, .0f, 1.0f));
		Math::mat4 ModelScale	= glm::scale	(glm::mat4(1.f), submesh->m_Parent->getScale()		  * submesh->m_Transform->getScale());
		Math::mat4 ModelRotation = ModelRX * ModelRY * ModelRZ;

		Model *= ModelRotation * ModelScale;

		glUniformMatrix4fv(ModelID, 1, GL_FALSE, glm::value_ptr(Model));
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(View));
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(Projection));

		glBindVertexArray(submesh->m_VAO);
		glDrawElements(GL_TRIANGLES, submesh->m_IndicesCount, GL_UNSIGNED_INT, 0);

		return 1;
	}
	GLuint glRenderer::genVBO(subMesh* submesh, GLsizeiptr size, GLint dataSize, const void* data, GLenum usage) {
		glBindVertexArray(submesh->m_VAO);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, size, data, usage);

		glEnableVertexAttribArray(submesh->m_VBOs.size());
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(submesh->m_VBOs.size(), dataSize, GL_FLOAT, GL_FALSE, 0, 0);

		return buffer;
	}
}