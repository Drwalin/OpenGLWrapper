
#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

#include <vector>

#include "VBO.h"

class VAO {
public:
	
	VAO(GLenum mode);
	~VAO();
	
	// offset in bytes
	void SetAttribPointer(VBO& vbo, int location, unsigned count, GLenum type,
			bool normalized, unsigned offset, unsigned divisor=0);
	
	void SetSize(unsigned count);
	void SetInstances(unsigned instances);
	
	void Draw();
	void Draw(unsigned start, unsigned count);
	void DrawArrays(unsigned start, unsigned count);
	void DrawElements(unsigned start, unsigned count);
	
private:
	
	GLenum typeElements;
	unsigned sizeA, sizeI, instances;
	unsigned vaoID;
	GLenum mode;
	bool drawArrays;
};

#endif

