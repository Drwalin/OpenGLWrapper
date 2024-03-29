/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2023 Marek Zalewski aka Drwalin
 *
 *  OpenGLWrapper is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenGLWrapper is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef OGLW_BASIC_MESH_LOADER_ANIMATION_HPP
#define OGLW_BASIC_MESH_LOADER_ANIMATION_HPP

#include <cinttypes>
#include <cmath>

#include <vector>
#include <unordered_map>
#include <type_traits>
#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Mesh.hpp"
#include "LoaderFlags.hpp"

class aiAnimation;
class aiScene;
class aiMesh;
class aiNode;

namespace gl {
namespace BasicMeshLoader {
	class Animation {
	public:
		
		std::string name;
		
		float duration;
		float framesPerSecond;
		
		LoaderFlagsBitfield flags;
		class AssimpLoader* loader;
		std::shared_ptr<Skeleton> skeleton;
		aiAnimation const* aiAnim;
		aiScene const* scene;
		
		
		uint32_t CountBones();
		
		void LoadAnimation(class AssimpLoader* loader,
				const aiAnimation* animation, std::shared_ptr<Mesh> mesh,
				LoaderFlagsBitfield flags);
		
		void GetModelBoneMatrices(glm::mat4* matrices, float time, bool loop);
		void GetModelBoneMatrices(std::vector<glm::mat4>& matrices, float time,
				bool loop);
		
		void ReadNodeHierarchy(glm::mat4* matrices, float time,
				const aiNode* pNode, glm::mat4 parentTransform);
	};
	
} // namespace BasicMeshLoader
} // namespace gl

#endif

