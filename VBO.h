
#ifndef VBO_H
#define VBO_H

#include <GL/glew.h>

#include <vector>
#include <tuple>

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
		const static unsigned offset = GetOffset<id-1, tuple>::offset + std::tuple_element<id-1, tuple>::type::size;
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
		inline typename TupleElement<attributeID>::type& At(unsigned vertId, unsigned vectorId=0) {
			static float _S[16];
			if(vbo) {
				vbo->ReserveResizeVertices(vertId+1);
				return ((typename TupleElement<attributeID>::type*)&(vbo->buffer[vertId*vbo->VertexSize() + GetOffset<attributeID, Tuple>::offset]))[vectorId];
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
	
	inline void SetType(unsigned vertexSize, GLenum target, GLenum usage) {
		this->vertexSize = vertexSize;
		this->target = target;
		this->usage = usage;
	}
	
	// target: GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER
	// usage: GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW
	VBO(unsigned vertexSize, GLenum target=GL_ARRAY_BUFFER, GLenum usage=GL_STATIC_DRAW);
	~VBO();
	
	void Generate();
	
	inline std::vector<unsigned char>& Buffer() {
		return buffer;
	}
	
	inline unsigned VertexSize() const {
		return vertexSize;
	}
	
private:
	
	GLenum target, usage;
	unsigned vboID;
	unsigned vertexSize, vertices;
	std::vector<unsigned char> buffer;
};

#endif
