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

