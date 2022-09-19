/*
 *  This file is part of OpenGLWrapper.
 *  Copyright (C) 2021 Marek Zalewski aka Drwalin
 *
 *  ICon3 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ICon3 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef VBO_H
#define VBO_H

#include <GL/glew.h>

#include <vector>
#include <tuple>

#include "OpenGL.h"

namespace gl {
	enum BufferTarget : GLenum {
		Array = GL_ARRAY_BUFFER,
		AtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
		CopyRead = GL_COPY_READ_BUFFER,
		CopyWrite = GL_COPY_WRITE_BUFFER,
		DispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
		ElementArray = GL_ELEMENT_ARRAY_BUFFER,
		PixelPack = GL_PIXEL_PACK_BUFFER,
		PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
		Query = GL_QUERY_BUFFER,
		ShaderStorage = GL_SHADER_STORAGE_BUFFER,
		Texture = GL_TEXTURE_BUFFER,
		TransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
		Uniform = GL_UNIFORM_BUFFER
	};
	
	enum BufferUsage {
		StaticDraw = GL_STATIC_DRAW,
		DynamicDraw = GL_DYNAMIC_DRAW,
		StreamDraw = GL_STREAM_DRAW
	};
}

template<typename T, unsigned C>
class Atr {
public:
	using type = T;
	const static unsigned elements = C;
	const static unsigned size = C*sizeof(T);
};

/*
	usage:
	
	VBO vbo(...);
	auto buf = vbo.Buffer<Atr<float, 3>, Atr<float, 2>, Atr<unsigned short, 2>>();
	buf.At<0>(i, 0) = value;
	
*/

class VBO {
public:
	
	template<unsigned id, typename tuple>
	class GetOffset {
	public:
		const static unsigned offset =
			GetOffset<id-1, tuple>::offset +
			std::tuple_element<id-1, tuple>::type::size;
	};
	
	template<typename tuple>
	class GetOffset<0, tuple> {
	public:
		const static unsigned offset = 0;
	};
	
	template<typename Tuple>
	class BufferRef {
	public:
		
		BufferRef() : vbo(NULL) {}
		BufferRef(VBO* vbo) : vbo(vbo) {}
		BufferRef(BufferRef&& r) : vbo(r.vbo) {}
		BufferRef(const BufferRef& r) : vbo(r.vbo) {}
		
		template<unsigned id>
		using TupleElement = typename std::tuple_element<id, Tuple>::type;
		
		template<unsigned attributeID>
		inline typename TupleElement<attributeID>::type& At(unsigned vertId,
				unsigned vectorId=0) {
			static float _S[16];
			if(vbo) {
				vbo->ReserveResizeVertices(vertId+1);
				size_t offset =  vertId*vbo->VertexSize() +
					GetOffset<attributeID, Tuple>::offset;
				using elem_type = typename TupleElement<attributeID>::type;
				elem_type* elements =
					reinterpret_cast<elem_type*>(&(vbo->buffer[offset]));
				return elements[vectorId];
			}
			return *((typename TupleElement<attributeID>::type*)&(_S));
		}
		
	private:
	
		VBO* vbo;
	};
	
	template<typename... Args>
	inline BufferRef<std::tuple<Args...>> Buffer() {
		return BufferRef<std::tuple<Args...>>(this);
	}
	
	
	
	friend class VAO;
	
	inline void ReserveResizeVertices(unsigned verts) {
		if(buffer.size()/vertexSize < verts) {
			buffer.resize(verts*vertexSize);
		}
	}
	
	void SetType(unsigned vertexSize, gl::BufferTarget target,
			gl::BufferUsage usage);
	
	// target: GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER
	// usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
	VBO(unsigned vertexSize, gl::BufferTarget target,
			gl::BufferUsage usage);
	~VBO();
	
	void Generate();
	void Update(unsigned beg, unsigned end);
	void ClearHostBuffer();
	void FetchAllDataToHostFromGPU();
	
	inline std::vector<unsigned char>& Buffer() { return buffer; }
	inline const std::vector<unsigned char>& Buffer() const { return buffer; }
	
	inline unsigned VertexSize() const { return vertexSize; }
	
private:
	
	gl::BufferTarget target;
	gl::BufferUsage usage;
	unsigned vboID;
	unsigned vertexSize, vertices;
	std::vector<unsigned char> buffer;
};

#endif
