#pragma once

#include <glad/glad.h> 

#include "Utilities.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

struct displayVertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	vector<Vertex*> children;
	bool operator==(const displayVertex& other) const {
		return Position == other.Position;
	}
	void setPos(glm::vec3 newPos) {
		Position = newPos;
		for (int i = 0; i < children.size(); i++) {
			children[i]->Position = newPos;
			if (children[i]->mesh != nullptr)
				children[i]->mesh->updateVertexPos();
		}
	}
};

class Model {
public:
	// model data
	std::vector<Texture> textures_loaded;
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection; 

	std::vector<displayVertex> origVertices;

	Model() {
		gammaCorrection = false;
	}

	Model(std::string const &path, bool gamma = false) : gammaCorrection(gamma) {
		loadModel(path);
	}

	void replaceModel(std::string const &path, bool gamma = false) {
		textures_loaded.clear();
		meshes.clear();
		directory.clear();
		origVertices.clear();
		loadModel(path);
	}

	void Draw(Shader& shader, Shader* vertShader = NULL, bool textured = false) {
		shader.setInt("selectedIndex", selectedIndex);
		for (unsigned int i = 0; i < meshes.size(); i++)
			if(vertShader != NULL)
				meshes[i].Draw(shader, vertShader, textured);
			else
				meshes[i].Draw(shader, NULL, textured);
	}

	std::vector<displayVertex> getInterceptedVertices(glm::vec3 Ray, glm::vec3 cameraPos) {
		std::vector<displayVertex> interceptedVerts;
		for (int i = 0; i < origVertices.size(); i++) {
			if (Utilities::discriminant(Ray, cameraPos, origVertices[i].Position, 0.5f) >= 0) {
				interceptedVerts.push_back(origVertices[i]);
				std::cout << "Vertex " << i << " at pos " << 
					origVertices[i].Position.x << ", " <<
					origVertices[i].Position.y << ", " <<
					origVertices[i].Position.z << 
					std::endl;
			}
		}
		std::cout << std::endl;
		selectClosest(interceptedVerts, cameraPos);
		return interceptedVerts;
	}

	int selectedIndex = 0;
private:
	void selectClosest(std::vector<displayVertex> interceptedVerts, glm::vec3 cameraPos) {
		if (!interceptedVerts.size() == 0) {
			double closest = 100000.0f;
			int closestIndex = selectedIndex;
			for (int i = 0; i < interceptedVerts.size(); i++) {
				double distance = glm::distance(interceptedVerts[i].Position, cameraPos);
				if (distance < closest) {
					closest = distance;
					closestIndex = i;
				}
			}
			//selectedIndex = closestIndex;
			std::cout << "selected Index: " << selectedIndex << std::endl;
		}
		else {
			//std::cout << "No intercepted Vertices" << std::endl;
		}
	}

	void loadModel(std::string path) {
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals /* | aiProcess_FlipUVs */ | aiProcess_CalcTangentSpace);
		// check for errors
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));
		processOrigVertices(scene);
		processNode(scene->mRootNode, scene);
		processDisplayVertChildren();
	}

	void processDisplayVertChildren() {
		for (int i = 0; i < meshes.size(); i++) {
			for (int j = 0; j < meshes[i].vertices.size(); j++) {
				for (int k = 0; k < origVertices.size(); k++) {
					if (origVertices[k].Position == meshes[i].vertices[j].Position) {
						origVertices[k].children.push_back(&meshes[i].vertices[j]);
					}
				}
			}
		}
	}

	void processOrigVertices(const aiScene* scene) {
		for (int meshIndex = 0; meshIndex < scene->mNumMeshes; meshIndex++) {
			for (int vertexIndex = 0; vertexIndex < scene->mMeshes[meshIndex]->mNumVertices; vertexIndex++) {
				displayVertex vertex;
				vertex.Position.x = scene->mMeshes[meshIndex]->mVertices[vertexIndex].x;
				vertex.Position.y = scene->mMeshes[meshIndex]->mVertices[vertexIndex].y;
				vertex.Position.z = scene->mMeshes[meshIndex]->mVertices[vertexIndex].z;
				auto it = std::find(origVertices.begin(), origVertices.end(), vertex);
				if(it == origVertices.end())
					origVertices.push_back(vertex);
			}
		}
	}

	void processNode(aiNode* node, const aiScene* scene) {
		// process all the node's meshes (if any)
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
			for (int i = 0; i < meshes[meshes.size() - 1].vertices.size(); i++) {
				meshes[meshes.size() - 1].vertices[i].mesh = &meshes[meshes.size() - 1];
			}
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
		
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			// process vertex positoins, normals and texture coordinates
			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			//displayVertex dvertex;
			//dvertex.Position.x = vector.x;
			//dvertex.Position.y = vector.y;
			//dvertex.Position.z = vector.z;
			//for (int i = 0; i < origVertices.size(); i++) {
			//	if (origVertices[i].Position == dvertex.Position) {
			//		origVertices[i].children.push_back(&vertex);
			//	}
			//}

			//auto it = std::find(origVertices.begin(), origVertices.end(), dvertex);
			//if (it != origVertices.end()) {
			//	displayVertex parentVertex = *it;
			//	parentVertex.children.push_back(&vertex);
			//}


			if (mesh->HasNormals()) {
				vector.x = mesh->mNormals[i].x;
				vector.y = mesh->mNormals[i].y;
				vector.z = mesh->mNormals[i].z;
				vertex.Normal = vector;
			}

			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
				// tangent
				vector.x = mesh->mTangents[i].x;
				vector.y = mesh->mTangents[i].y;
				vector.z = mesh->mTangents[i].z;
				vertex.Tangent = vector;
				// bitangent
				vector.x = mesh->mBitangents[i].x;
				vector.y = mesh->mBitangents[i].y;
				vector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = vector;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f, 0.0f);
			

			vertices.push_back(vertex);
		}

		// process indices
		// walk through each of the mesh's faces (a face is a mesh its traingle) and 
		// retrieve the corresponding vertex indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		// process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

		}

		Mesh currMesh = Mesh(vertices, indices, textures);
		return currMesh;
	}

	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<Texture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++) {
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(textures_loaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				Texture texture;
				texture.id = TextureFromFile(str.C_Str(), this->directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture); 
			}
		}
		return textures;
	}
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		std::cout << "Texture failed to load at path: " << filename << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}