
#ifndef VAO_H
#define VAO_H

#include <GL/glew.h>

#include <vector>

#include "VBO.h"

class VAO {
public:
	
	VAO(GLenum mode);
	~VAO();
	
	void SetAttribPointer(VBO& vbo, int location, unsigned count, GLenum type, bool normalized, unsigned offset);		// offset in bytes
	void SetAttribDivider(VBO& vbo, int location, unsigned divisor);
	
	void SetSize(unsigned count);
	
	void Draw();
	void Draw(unsigned start, unsigned count);
	void DrawArrays(unsigned start, unsigned count);
	void DrawElements(unsigned start, unsigned count);
	
private:
	
	GLenum typeElements;
	unsigned sizeA, sizeI;
	unsigned vaoID;
	GLenum mode;
	bool drawArrays;
};

#endif

