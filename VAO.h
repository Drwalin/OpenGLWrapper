
#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

#include <vector>

class VAO {
public:
	
	class VertexRef {
	public:
		
		template<typename T>
		class Value {
		public:
			Value() : vao(NULL), stride(0) {}
			Value(VAO* vao, unsigned stride) : vao(vao), stride(stride) {}
			Value(Value&& r) : vao(r.vao), stride(r.stride) {}
			Value(const Value& r) : vao(r.vao), stride(r.stride) {}
			
			inline T& operator[](unsigned id) {
				return *(T*)&(vao->AccessVertices()[stride + id*sizeof(T)]);
			}
			
			unsigned stride;
			VAO* vao;
		};
		
		VertexRef();
		VertexRef(VAO* vao, unsigned id);
		VertexRef(VertexRef&& r);
		VertexRef(const VertexRef& r);
		
		template<typename T, unsigned strideBytes>
		T& At(unsigned i) {
			return At<T>(i, strideBytes);
		}
		template<typename T>
		T& At(unsigned i, unsigned strideBytes) {
			static double R[16];
			if(vao) {
				return *(T*)&(vao->AccessVertices()[id*vao->GetVertexSize() + strideBytes]);
			} else {
				return *(T*)&R;
			}
		}
		
		template<typename T>
		inline Value<T> Ref(unsigned stride) {
			return Value(vao, id*vao->GetVertexSize() + stride);
		}
		
		unsigned id;
		VAO* vao;
	};
	
	inline VertexRef operator[](unsigned id) {
		ReserveVertices(id);
		return VertexRef(this, id);
	}
	
	void ReserveVertices(unsigned num);
	inline std::vector<unsigned char>& AccessVertices() {
		return vertices;
	}
	void SetAttribPointer(const int location, const unsigned int count, const GLenum type, const bool normalized, const unsigned int offset);		// offset in bytes
	void AddVertex(const void * bytes);
	void SetVertexSize(const unsigned int size);
	inline unsigned GetVertexSize() const {
		return vertexSize;
	}
	
	bool IsGenerated();
	void Generate(const GLenum type);
	void ClearVertices();
	
	void Draw(unsigned int beg, unsigned int end) const;
	void Draw() const;
	
	void Destroy();
	
	VAO();
	~VAO();
	
private:
	
	int vertexSize;
	bool generated;
	unsigned int vaoID, vboID;
	std::vector<unsigned char> vertices;
	GLenum type;
	int verticesCount;
};

#endif

