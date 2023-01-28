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

#include <cstdio>

#include <algorithm>

#include <openglwrapper/basic_mesh_loader/Skeleton.hpp>

#include <assimp/scene.h>

namespace gl {
namespace BasicMeshLoader {
	void Skeleton::LoadSkeleton(const aiSkeleton* skel) {
		bones.resize(skel->mNumBones);
		
		for(int i=0; i<skel->mNumBones; ++i) {
			const aiSkeletonBone* bone = skel->mBones[i];
			Bone& b = bones[i];
// 			b.inverseBindingPoseMatrix = bone->mLocalMatrix;
// 			b.relativeOffsetMatrix = bone->mOffsetMatrix;
			b.parentId = bone->mParent;
			b.id = i;
		}
		
		this->name = skel->mName.C_Str();
		printf(" skeleton name: %s\n", skel->mName.C_Str());
	}
} // namespace BasicMeshLoader
} // namespace gl


