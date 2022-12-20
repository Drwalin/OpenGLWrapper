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
		ARRAY_BUFFER = GL_ARRAY_BUFFER,
		ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER,
		COPY_READ_BUFFER = GL_COPY_READ_BUFFER,
		COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,
		DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,
		ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
		PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,
		PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,
		QUERY_BUFFER = GL_QUERY_BUFFER,
		SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,
		TEXTURE_BUFFER = GL_TEXTURE_BUFFER,
		TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,
		UNIFORM_BUFFER = GL_UNIFORM_BUFFER,
		DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER
	};
	
	enum BufferUsage {
		STATIC_DRAW = GL_STATIC_DRAW,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		STREAM_DRAW = GL_STREAM_DRAW
	};

	template<typename T, unsigned C>
	class Atr {
	public:
		using type = T;
		const static unsigned elements = C;
		const static unsigned size = C*sizeof(T);
	};
	
	template<typename T, unsigned C>
	using A = Atr<T, C>;

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
		
		inline unsigned GetIdGL() const { return vboID; }
		
		void BindBufferBase(gl::BufferTarget target, int location);
		
	private:
		
		gl::BufferTarget target;
		gl::BufferUsage usage;
		unsigned vboID;
		unsigned vertexSize, vertices;
		std::vector<unsigned char> buffer;
	};
}

#endif