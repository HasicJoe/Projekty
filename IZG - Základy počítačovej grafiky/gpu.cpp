/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 */
#include <student/gpu.hpp>

/// \addtogroup gpu_init
/// @{

/**
 * @brief Constructor of GPU
 */
GPU::GPU()
{
  /// \todo Zde můžete alokovat/inicializovat potřebné proměnné grafické karty
}

/**
 * @brief Destructor of GPU
 */
GPU::~GPU()
{
  /// \todo Zde můžete dealokovat/deinicializovat grafickou kartu
}

/// @}

/** \addtogroup buffer_tasks 01. Implementace obslužných funkcí pro buffery
 * @{
 */

/**
 * @brief This function allocates buffer on GPU.
 * @param size size in bytes of new buffer on GPU.
 * @return unique identificator of the buffer
 */
BufferID GPU::createBuffer(uint64_t size)
{
	BufferCounter++;
	BufferStruct initBuffer;
	initBuffer.BufferID = BufferCounter;
	initBuffer.data = std::vector<uint8_t>(size_t(size));
	buffs.push_back(initBuffer);
	return BufferCounter;
}

/**
 * @brief This function frees allocated buffer on GPU.
 * @param buffer buffer identificator
 */
void GPU::deleteBuffer(BufferID buffer)
{
	for (size_t i = 0; i < buffs.size(); i++)
	{
		if (buffs[i].BufferID = buffer)
		{
			buffs.erase(buffs.begin() + i);
			return;
		}
	}
}

/**
 * @brief This function uploads data to selected buffer on the GPU
 *
 * @param buffer buffer identificator
 * @param offset specifies the offset into the buffer's data
 * @param size specifies the size of buffer that will be uploaded
 * @param data specifies a pointer to new data
 */
void GPU::setBufferData(BufferID buffer, uint64_t offset, uint64_t size, void const *data)
{

	if (isBuffer(buffer))
	{
		for (size_t i = 0; i < buffs.size(); i++)
		{
			if (buffs[i].BufferID == buffer)
			{
				uint8_t *dest = &buffs[i].data[offset];
				memcpy(dest, (uint8_t *)data, size);
				return;
			}
		}
	}
}

/**
 * @brief This function downloads data from GPU.
 *
 * @param buffer specfies buffer
 * @param offset specifies the offset into the buffer from which data will be returned, measured in bytes. 
 * @param size specifies data size that will be copied
 * @param data specifies a pointer to the location where buffer data is returned. 
 */
void GPU::getBufferData(BufferID buffer,
                        uint64_t offset,
                        uint64_t size,
                        void *data)
{
	if (isBuffer(buffer))
	{
		for (size_t i = 0; i < buffs.size(); i++)
		{
			if (buffs[i].BufferID == buffer)
			{
				uint8_t *dest = &buffs[i].data[offset];
				memcpy((uint8_t *)data, dest, size);
				return;
			}
		}
	}
}

/**
 * @brief This function tests if buffer exists
 *
 * @param buffer selected buffer id
 *
 * @return true if buffer points to existing buffer on the GPU.
 */
bool GPU::isBuffer(BufferID buffer)
{
	for (size_t i = 0; i < buffs.size(); i++)
	{
		if (buffs[i].BufferID == buffer)
		{
			return true;
		}
	}
	return false;
}
/// @}

/**
 * \addtogroup vertexpuller_tasks 02. Implementace obslužných funkcí pro vertex puller
 * @{
 */

/**
 * @brief This function creates new vertex puller settings on the GPU,
 *
 * @return unique vertex puller identificator
 */
ObjectID GPU::createVertexPuller()
{
	VertexPullCounter++;
	VertexPullerTable initTable;
	initTable.VertexID = VertexPullCounter;
	VertexPull.push_back(initTable);
	return VertexPullCounter;
}

/**
 * @brief This function deletes vertex puller settings
 *
 * @param vao vertex puller identificator
 */
void GPU::deleteVertexPuller(VertexPullerID vao)
{
	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			VertexPull.erase(VertexPull.begin() + i);
		}
	}
}

/**
 * @brief This function sets one vertex puller reading head.
 *
 * @param vao identificator of vertex puller
 * @param head id of vertex puller head
 * @param type type of attribute
 * @param stride stride in bytes
 * @param offset offset in bytes
 * @param buffer id of buffer
 */
void GPU::setVertexPullerHead(VertexPullerID vao, uint32_t head, AttributeType type, uint64_t stride, uint64_t offset, BufferID buffer)
{

	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			VertexPull[i].headArray[head].Atype = type;
			VertexPull[i].headArray[head].stride = stride;
			VertexPull[i].headArray[head].offset = offset;
			VertexPull[i].headArray[head].buffID = buffer;
			return;
		}
	}
}

/**
 * @brief This function sets vertex puller indexing.
 *
 * @param vao vertex puller id
 * @param type type of index
 * @param buffer buffer with indices
 */
void GPU::setVertexPullerIndexing(VertexPullerID vao, IndexType type, BufferID buffer)
{
	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			VertexPull[i].Indexing = type;
			VertexPull[i].BuffID = buffer;
			return;
		}
	}
}

/**
 * @brief This function enables vertex puller's head.
 *
 * @param vao vertex puller 
 * @param head head id
 */
void GPU::enableVertexPullerHead(VertexPullerID vao, uint32_t head)
{
	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			VertexPull[i].headArray[head].enabled = true;
			return;
		}
	}
}

/**
 * @brief This function disables vertex puller's head
 *
 * @param vao vertex puller id
 * @param head head id
 */
void GPU::disableVertexPullerHead(VertexPullerID vao, uint32_t head)
{
	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			VertexPull[i].headArray[head].enabled = false;
			return;
		}
	}
}

/**
 * @brief This function selects active vertex puller.
 *
 * @param vao id of vertex puller
 */
void GPU::bindVertexPuller(VertexPullerID vao)
{
	activeVertexPuller = &VertexPull[vao];
	ActiveVertexPuller = vao;
	return;
}

/**
 * @brief This function deactivates vertex puller.
 */
void GPU::unbindVertexPuller()
{
	activeVertexPuller = nullptr;
	ActiveVertexPuller = emptyID;
	return;
}

/**
 * @brief This function tests if vertex puller exists.
 *
 * @param vao vertex puller
 *
 * @return true, if vertex puller "vao" exists
 */
bool GPU::isVertexPuller(VertexPullerID vao)
{
	for (size_t i = 0; i < VertexPull.size(); i++)
	{
		if (VertexPull[i].VertexID == vao)
		{
			return true;
		}
	}
	return false;
}

/// @}

/** \addtogroup program_tasks 03. Implementace obslužných funkcí pro shader programy
 * @{
 */

/**
 * @brief This function creates new shader program.
 *
 * @return shader program id
 */
ProgramID GPU::createProgram()
{
	ProgramsCounter++;
	ProgramSettings initPrograms;
	initPrograms.ProgramID = ProgramsCounter;
	Programs.push_back(initPrograms);
	return ProgramsCounter;
}

/**
 * @brief This function deletes shader program
 *
 * @param prg shader program id
 */
void GPU::deleteProgram(ProgramID prg)
{
	for (size_t i = 0; i < Programs.size(); i++)
	{
		if (Programs[i].ProgramID == prg)
		{
			Programs.erase(Programs.begin() + i);
			return;
		}
	}
}

/**
 * @brief This function attaches vertex and frament shader to shader program.
 *
 * @param prg shader program
 * @param vs vertex shader 
 * @param fs fragment shader
 */
void GPU::attachShaders(ProgramID prg, VertexShader vs, FragmentShader fs)
{
	for (size_t i = 0; i < Programs.size(); i++)
	{
		if (Programs[i].ProgramID == prg)
		{
			Programs[i].vertexShader = vs;
			Programs[i].fragmentShader = fs;
			return;
		}
	}
}

/**
 * @brief This function selects which vertex attributes should be interpolated during rasterization into fragment attributes.
 *
 * @param prg shader program
 * @param attrib id of attribute
 * @param type type of attribute
 */
void GPU::setVS2FSType(ProgramID prg, uint32_t attrib, AttributeType type)
{
	Programs[prg - 1].vs2fs[attrib] = type;
	Programs[prg - 1].vs2fs_set = true;
	return;
}

/**
 * @brief This function actives selected shader program
 *
 * @param prg shader program id
 */
void GPU::useProgram(ProgramID prg)
{
	activeProgram = &Programs[prg];
	ActiveProgram = prg;
	return;
}

/**
 * @brief This function tests if selected shader program exists.
 *
 * @param prg shader program
 *
 * @return true, if shader program "prg" exists.
 */
bool GPU::isProgram(ProgramID prg)
{
	for (size_t i = 0; i < Programs.size(); i++)
	{
		if (Programs[i].ProgramID == prg)
		{
			return true;
		}
	}
	return false;
}

/**
 * @brief This function sets uniform value (1 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform1f(ProgramID prg, uint32_t uniformId, float const &d)
{
	Programs[prg - 1].uniforms.uniform[uniformId].v1 = d;
	return;
}

/**
 * @brief This function sets uniform value (2 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform2f(ProgramID prg, uint32_t uniformId, glm::vec2 const &d)
{
	Programs[prg - 1].uniforms.uniform[uniformId].v2 = glm::vec2(d);
	return;
}

/**
 * @brief This function sets uniform value (3 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform3f(ProgramID prg, uint32_t uniformId, glm::vec3 const &d)
{
	Programs[prg - 1].uniforms.uniform[uniformId].v3 = glm::vec3(d);
	return;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniform4f(ProgramID prg, uint32_t uniformId, glm::vec4 const &d)
{
	Programs[prg - 1].uniforms.uniform[uniformId].v4 = glm::vec4(d);
	return;
}

/**
 * @brief This function sets uniform value (4 float).
 *
 * @param prg shader program
 * @param uniformId id of uniform value (number of uniform values is stored in maxUniforms variable)
 * @param d value of uniform variable
 */
void GPU::programUniformMatrix4f(ProgramID prg, uint32_t uniformId, glm::mat4 const &d)
{
	Programs[prg - 1].uniforms.uniform[uniformId].m4 = d;
	return;
}

/// @}

/** \addtogroup framebuffer_tasks 04. Implementace obslužných funkcí pro framebuffer
 * @{
 */

/**
 * @brief This function creates framebuffer on GPU.
 *
 * @param width width of framebuffer
 * @param height height of framebuffer
 */
void GPU::createFramebuffer(uint32_t width, uint32_t height)
{
	FrameBufferWidth = width;
	FrameBufferHeight = height;
	uint64_t size = width * height;
	uint64_t colorSize = 4 * size;
	ColorBuff.resize(colorSize);
	DepthBuff.resize(size);
	return;
}

/**
 * @brief This function deletes framebuffer.
 */
void GPU::deleteFramebuffer()
{
	ColorBuff.clear();
	DepthBuff.clear();
	FrameBufferWidth = 0;
	FrameBufferHeight = 0;
	return;
}

/**
 * @brief This function resizes framebuffer.
 *
 * @param width new width of framebuffer
 * @param height new heght of framebuffer
 */
void GPU::resizeFramebuffer(uint32_t width, uint32_t height)
{
	FrameBufferWidth = width;
	FrameBufferHeight = height;
	uint64_t size = width * height;
	uint64_t colorSize = 4 * size;
	ColorBuff.resize(colorSize);
	DepthBuff.resize(size);
	return;
}

/**
 * @brief This function returns pointer to color buffer.
 *
 * @return pointer to color buffer
 */
uint8_t *GPU::getFramebufferColor()
{
	return &ColorBuff[0];
}

/**
 * @brief This function returns pointer to depth buffer.
 *
 * @return pointer to dept buffer.
 */
float *GPU::getFramebufferDepth()
{
	return &DepthBuff[0];
}

/**
 * @brief This function returns width of framebuffer
 *
 * @return width of framebuffer
 */
uint32_t GPU::getFramebufferWidth()
{
	return FrameBufferWidth;
}

/**
 * @brief This function returns height of framebuffer.
 *
 * @return height of framebuffer
 */
uint32_t GPU::getFramebufferHeight()
{
	return FrameBufferHeight;
}

/// @}

/** \addtogroup draw_tasks 05. Implementace vykreslovacích funkcí
 * Bližší informace jsou uvedeny na hlavní stránce dokumentace.
 * @{
 */

/**
 * @brief This functino clears framebuffer.
 *
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void GPU::clear(float r, float g, float b, float a)
{
	uint8_t red = (uint8_t)r * 255;
	uint8_t green = (uint8_t)g * 255;
	uint8_t blue = (uint8_t)b * 255;
	uint8_t alpha = (uint8_t)a * 255;

	for (size_t i = 0; i < FrameBufferWidth; i++)
	{
		for (size_t j = 0; j < FrameBufferHeight; j++)
		{
			ColorBuff[(i * FrameBufferWidth + j) * 4] = red;
			ColorBuff[(i * FrameBufferWidth + j) * 4 + 1] = green;
			ColorBuff[(i * FrameBufferWidth + j) * 4 + 2] = blue;
			ColorBuff[(i * FrameBufferWidth + j) * 4 + 3] = alpha;
			DepthBuff[(i * FrameBufferWidth + j)] = 1024.f;
		}
	}
}

void GPU::drawTriangles(uint32_t nofVertices)
{
	if((nofVertices % 3) != 0)
		return;

	if(VertexPull[ActiveVertexPuller - 1].Indexing == IndexType::UINT8)
	{
		indexDraw<uint8_t>();
		return;
	}
	else if(VertexPull[ActiveVertexPuller - 1].Indexing == IndexType::UINT16)
	{
		indexDraw<uint16_t>();
		return;
	}
	else if(VertexPull[ActiveVertexPuller - 1].Indexing == IndexType::UINT32)
	{
		indexDraw<uint32_t>();
		return;
	}
	auto &actProgram = Programs[ActiveProgram - 1];
	auto pullerCounter = 0;
	InVertex invertex;
	std::vector<OutVertex> outvertex{size_t(nofVertices)};

	for (auto&vertex : outvertex)
	{
		invertex.gl_VertexID = (uint32_t)pullerCounter;
		for (size_t i = 0; i < maxAttributes; i++)
		{
			if (VertexPull[ActiveVertexPuller - 1].headArray[i].enabled == true)
			{
				if (VertexPull[ActiveVertexPuller - 1].headArray[i].Atype == AttributeType::FLOAT) // float
				{
					invertex.attributes[i].v1 = reinterpret_cast<float &>(buffs[VertexPull[ActiveVertexPuller - 1].headArray[i].buffID - 1].data[VertexPull[ActiveVertexPuller - 1].headArray[i].stride * pullerCounter + VertexPull[ActiveVertexPuller - 1].headArray[i].offset]);
				}
				else if (VertexPull[ActiveVertexPuller - 1].headArray[i].Atype == AttributeType::VEC2) // vec2
				{
					invertex.attributes[i].v2 = reinterpret_cast<glm::vec2 &>(buffs[VertexPull[ActiveVertexPuller - 1].headArray[i].buffID - 1].data[VertexPull[ActiveVertexPuller - 1].headArray[i].stride * pullerCounter + VertexPull[ActiveVertexPuller - 1].headArray[i].offset]);
				}
				else if (VertexPull[ActiveVertexPuller - 1].headArray[i].Atype == AttributeType::VEC3) // vec3
				{
					invertex.attributes[i].v3 = reinterpret_cast<glm::vec3 &>(buffs[VertexPull[ActiveVertexPuller - 1].headArray[i].buffID - 1].data[VertexPull[ActiveVertexPuller - 1].headArray[i].stride * pullerCounter + VertexPull[ActiveVertexPuller - 1].headArray[i].offset]);
				}
				else if (VertexPull[ActiveVertexPuller - 1].headArray[i].Atype == AttributeType::VEC4) // vec4
				{
					invertex.attributes[i].v4 = reinterpret_cast<glm::vec4 &>(buffs[VertexPull[ActiveVertexPuller - 1].headArray[i].buffID - 1].data[VertexPull[ActiveVertexPuller - 1].headArray[i].stride * pullerCounter + VertexPull[ActiveVertexPuller - 1].headArray[i].offset]);
				}
			}
		}
		actProgram.vertexShader(vertex, invertex, actProgram.uniforms);
		pullerCounter++;
	}
	pullerCounter = 0;
	for(size_t i = 0; i < outvertex.size();i++)
		setPrimitives.push_back(outvertex[i]);

	outvertex.clear();
	assemblePrimitives();
}

void GPU::assemblePrimitives()
{
	size_t PrimitivesCount = setPrimitives.size();
	if(PrimitivesCount % 3 != 0)
		return;
	
	size_t TriangleToSet = PrimitivesCount / 3;
	auto a = 0;
	auto b = 1;
	auto c = 2;
	for(size_t i=0; i < TriangleToSet; i++)
	{
		Triangle initTriangle;
		initTriangle.A = setPrimitives[a];
		initTriangle.B = setPrimitives[b];
		initTriangle.C = setPrimitives[c];
		PrimitivesToTriangle.push_back(initTriangle);
		a=a+3;
		b=b+3;
		c=c+3;
	}
	setPrimitives.clear();
	clipping();
}

void GPU::clipping()
{
	for(size_t i = 0 ; i < PrimitivesToTriangle.size();i++)
	{
		glm::vec4 a = PrimitivesToTriangle[i].A.gl_Position;
		glm::vec4 b = PrimitivesToTriangle[i].B.gl_Position;
		glm::vec4 c = PrimitivesToTriangle[i].C.gl_Position;

		// vsetky tri body splnuju podmienku - nemusime rezat
		if((a.z>=-a.w) && (b.z >= -b.w) && (c.z >= -c.w))
		{
			AfterClipping.push_back(PrimitivesToTriangle[i]);
		}
		// jeden z bodov nesplnuje podmienku - vzniknu dva mensie trojuholniky
		else if( ((a.z >= -a.w) && (b.z >=-b.w) && (c.z < -c.w)) || ((a.z >= -a.w) && (b.z < -b.w) && (c.z >= -c.w)) || ((a.z < -a.w) && (b.z >= -b.w) && (c.z >= -c.w)))
		{
			if(((a.z >= -a.w) && (b.z >= -b.w) && (c.z < -c.w)))
			{
				// C nesplnuje podmienku musime vypocitat 2 parametre
				float t1 = (-c.w - c.z) / (a.w - c.w + a.z - c.z);
				float t2 = (-c.w - c.z) / (b.w - c.w + b.z - c.z);

				float t11 = 1.0 - t1;
				float t21 = 1.0 - t2;

				Triangle actual;
				Triangle after1;
				Triangle after2;

				actual = PrimitivesToTriangle[i];

				after1.A = PrimitivesToTriangle[i].A;
				after1.B = PrimitivesToTriangle[i].B;
				after1.C.gl_Position = after1.A.gl_Position * t1 + actual.C.gl_Position * t11;
				for(size_t j = 0 ; j < maxAttributes;j++)
				{
					after1.C.attributes[j].v1 = after1.A.attributes[j].v1 * t1 + actual.C.attributes[j].v1 * t11;
					after1.C.attributes[j].v2 = after1.A.attributes[j].v2 * t1 + actual.C.attributes[j].v2 * t11;
					after1.C.attributes[j].v3 = after1.A.attributes[j].v3 * t1 + actual.C.attributes[j].v3 * t11;
					after1.C.attributes[j].v4 = after1.A.attributes[j].v4 * t1 + actual.C.attributes[j].v4 * t11;
				}
				after2.B = PrimitivesToTriangle[i].B;
				after2.A = after1.C;

				after2.C.gl_Position = after2.B.gl_Position * t2 + actual.C.gl_Position * t21;

				for(size_t j = 0 ; j < maxAttributes;j++)
				{
					after2.C.attributes[j].v1 = after2.B.attributes[j].v1 * t2 + actual.C.attributes[j].v1 * t21;
					after2.C.attributes[j].v2 = after2.B.attributes[j].v2 * t2 + actual.C.attributes[j].v2 * t21;
					after2.C.attributes[j].v3 = after2.B.attributes[j].v3 * t2 + actual.C.attributes[j].v3 * t21;
					after2.C.attributes[j].v4 = after2.B.attributes[j].v4 * t2 + actual.C.attributes[j].v4 * t21;
				}
				AfterClipping.push_back(after1); // triangle AB & AC(t)
				AfterClipping.push_back(after2); // triangle B AC(t) && BC(t)
			}
			else if(((a.z >= -a.w) && (b.z < -b.w) && (c.z >= -c.w)))
			{
				//B nesplnuje podmienku
				float t1 = (-b.w - b.z) / (a.w - b.w + a.z - b.z);
				float t2 = (-b.w - b.z) / (c.w - b.w + c.z - b.z);

				float t11 = 1.0 - t1;
				float t21 = 1.0 - t2;

				Triangle actual;
				Triangle after1;
				Triangle after2;

				actual = PrimitivesToTriangle[i];
				after1.A = PrimitivesToTriangle[i].A;
				after1.C = PrimitivesToTriangle[i].C;

				after1.B.gl_Position = after1.A.gl_Position * t1 + actual.B.gl_Position *t11;
				for(size_t j = 0 ; j < maxAttributes;j++)
				{
					after1.B.attributes[j].v1 = after1.A.attributes[j].v1 * t1 + actual.B.attributes[j].v1 * t11;
					after1.B.attributes[j].v2 = after1.A.attributes[j].v2 * t1 + actual.B.attributes[j].v2 * t11;
					after1.B.attributes[j].v3 = after1.A.attributes[j].v3 * t1 + actual.B.attributes[j].v3 * t11;
					after1.B.attributes[j].v4 = after1.A.attributes[j].v4 * t1 + actual.B.attributes[j].v4 * t11;
				}
				after2.C = PrimitivesToTriangle[i].C;
				after2.A = after1.B;


				after2.B.gl_Position = after2.C.gl_Position *t2 + actual.B.gl_Position * t21;
				for(size_t j = 0 ; j < maxAttributes;j++)
				{
					after2.B.attributes[j].v1 = after2.C.attributes[j].v1 * t2 + actual.B.attributes[j].v1 * t21;
					after2.B.attributes[j].v2 = after2.C.attributes[j].v2 * t2 + actual.B.attributes[j].v2 * t21;
					after2.B.attributes[j].v3 = after2.C.attributes[j].v3 * t2 + actual.B.attributes[j].v3 * t21;
					after2.B.attributes[j].v4 = after2.C.attributes[j].v4 * t2 + actual.B.attributes[j].v4 * t21;
				}
				AfterClipping.push_back(after1); // A B1 C
				AfterClipping.push_back(after2); // B1 B2 C
			}
			else if(((a.z < -a.w) && (b.z >= -b.w) && (c.z >= -c.w)))
			{
				// A nesplnuje podmienku
				float t1 = (-a.w - a.z) / (b.w - a.w + b.z - a.z);
				float t2 = (-a.w - a.z) / (c.w - a.w + c.z - a.z);

				float t11 = 1.0 - t1;
				float t21 = 1.0 - t2;

				Triangle actual;
				Triangle after1;
				Triangle after2;

				actual = PrimitivesToTriangle[i];

				after1.B = PrimitivesToTriangle[i].B;
				after1.C = PrimitivesToTriangle[i].C;

				after1.A.gl_Position = after1.B.gl_Position * t1 + actual.A.gl_Position * t11;
				for(size_t j = 0 ; j < maxAttributes; j++)
				{
					after1.A.attributes[j].v1 = after1.B.attributes[j].v1 * t1 + actual.A.attributes[j].v1 * t11;
					after1.A.attributes[j].v2 = after1.B.attributes[j].v2 * t1 + actual.A.attributes[j].v2 * t11;
					after1.A.attributes[j].v3 = after1.B.attributes[j].v3 * t1 + actual.A.attributes[j].v3 * t11;
					after1.A.attributes[j].v4 = after1.B.attributes[j].v4 * t1 + actual.A.attributes[j].v4 * t11;
				}
				after2.A = after1.A;
				after2.B = after1.C;
				after2.C.gl_Position = after2.B.gl_Position *t2 + actual.A.gl_Position * t21;
				for(size_t j = 0 ; j < maxAttributes;j++)
				{
					after2.C.attributes[j].v1 = after2.B.attributes[j].v1 * t2 + actual.A.attributes[j].v1 * t21;
					after2.C.attributes[j].v2 = after2.B.attributes[j].v2 * t2 + actual.A.attributes[j].v2 * t21;
					after2.C.attributes[j].v3 = after2.B.attributes[j].v3 * t2 + actual.A.attributes[j].v3 * t21;
					after2.C.attributes[j].v4 = after2.B.attributes[j].v4 * t2 + actual.A.attributes[j].v4 * t21;
				}
				AfterClipping.push_back(after1); // A(t) B C
				AfterClipping.push_back(after2); // A(t) C A(t)2
			}
		}
		// dva outvertexy nesplnuju podmienku
		else if(((a.z < -a.w) && (b.z < -b.w) && (c.z >= -c.w)) || ((a.z < -a.w) && (b.z >= -b.w) && (c.z < -c.w)) || ((a.z >= -a.w) && (b.z < -b.w) && (c.z < -c.w)) )
		{
			if(((a.z < -a.w) && (b.z < -b.w) && (c.z >= -c.w)))
			{
				// A a B nesplnaju podmienku pre near planu, musime vypocitat dva parametre BC a AC
				float t1 = (-a.w - a.z) / (c.w - a.w + c.z - a.z);
				float t2 = (-b.w - b.z) / (c.w - b.w + c.z - b.z);

				float t11 = 1.0 - t1;
				float t21 = 1.0 - t2;

				Triangle after;
				Triangle actual;

				actual = PrimitivesToTriangle[i];
				after.C = PrimitivesToTriangle[i].C;

				after.A.gl_Position = PrimitivesToTriangle[i].C.gl_Position * t1  + PrimitivesToTriangle[i].B.gl_Position * t11;
				for(size_t j = 0; j < maxAttributes;j++)
				{
					after.A.attributes[j].v1 = actual.C.attributes[j].v1 * t1 + actual.B.attributes[j].v1 * t11;
					after.A.attributes[j].v2 = actual.C.attributes[j].v2 * t1 + actual.B.attributes[j].v2 * t11;
					after.A.attributes[j].v3 = actual.C.attributes[j].v3 * t1 + actual.B.attributes[j].v3 * t11;
					after.A.attributes[j].v4 = actual.C.attributes[j].v4 * t1 + actual.B.attributes[j].v4 * t11;
				}

				after.B.gl_Position = PrimitivesToTriangle[i].C.gl_Position * t2  + PrimitivesToTriangle[i].A.gl_Position * t21;
				for(size_t j = 0; j < maxAttributes;j++)
				{
					after.B.attributes[j].v1 = actual.C.attributes[j].v1 * t2 + actual.A.attributes[j].v1 * t21;
					after.B.attributes[j].v2 = actual.C.attributes[j].v2 * t2 + actual.A.attributes[j].v2 * t21;
					after.B.attributes[j].v3 = actual.C.attributes[j].v3 * t2 + actual.A.attributes[j].v3 * t21;
					after.B.attributes[j].v4 = actual.C.attributes[j].v4 * t2 + actual.A.attributes[j].v4 * t21;
				}
				AfterClipping.push_back(after);
			}
			else if(((a.z < -a.w) && (b.z >= -b.w) && (c.z < -c.w)))
			{
				// pozicie bodov A a C nesplnaju podmienku
				// parameter pre C
				float t1 = (-c.w - c.z) / (b.w - c.w + b.z - c.z);
				float t2 = (-a.w - a.z) / (b.w - a.w + b.z - a.z);

				float t11 = 1.0 - t1;
				float t21 = 1.0 - t2;

				Triangle after;
				Triangle actual;
				actual = PrimitivesToTriangle[i];
				after.B = PrimitivesToTriangle[i].B;

				after.C.gl_Position = PrimitivesToTriangle[i].B.gl_Position * t1  + PrimitivesToTriangle[i].A.gl_Position * t11;
				for(size_t j = 0; j < maxAttributes;j++)
				{
					after.C.attributes[j].v1 = actual.B.attributes[j].v1 * t1 + actual.A.attributes[j].v1 * t11;
					after.C.attributes[j].v2 = actual.B.attributes[j].v2 * t1 + actual.A.attributes[j].v2 * t11;
					after.C.attributes[j].v3 = actual.B.attributes[j].v3 * t1 + actual.A.attributes[j].v3 * t11;
					after.C.attributes[j].v4 = actual.B.attributes[j].v4 * t1 + actual.A.attributes[j].v4 * t11;
				}
				after.A.gl_Position = PrimitivesToTriangle[i].B.gl_Position * t2  + PrimitivesToTriangle[i].C.gl_Position * t21;
				for(size_t j = 0; j < maxAttributes;j++)
				{
					after.A.attributes[j].v1 = actual.B.attributes[j].v1 * t2 + actual.C.attributes[j].v1 * t21;
					after.A.attributes[j].v2 = actual.B.attributes[j].v2 * t2 + actual.C.attributes[j].v2 * t21;
					after.A.attributes[j].v3 = actual.B.attributes[j].v3 * t2 + actual.C.attributes[j].v3 * t21;
					after.A.attributes[j].v4 = actual.B.attributes[j].v4 * t2 + actual.C.attributes[j].v4 * t21;
				}
				AfterClipping.push_back(after);
			}
			else if( ((a.z >= -a.w) && (b.z < -b.w) && (c.z < -c.w)))
			{
				// pozicie bodov B a C nesplnaju podmienku pre near planu, musime vypocitat dva parametre pre AC a AB
				//parameter pre C
				float t1 = (-c.w - c.z) / (a.w - c.w + a.z - c.z);
				//parameter pre B
				float t2 = (-b.w - b.z) / (a.w - b.w + a.z - b.z);

				float t11 = 1.0-t1;
				float t21 = 1.0-t2;

				Triangle after;
				Triangle actual;
				actual = PrimitivesToTriangle[i];
				after.A = actual.A;
				after.C.gl_Position = PrimitivesToTriangle[i].A.gl_Position * t1 + PrimitivesToTriangle[i].B.gl_Position * t11;
				
				for(size_t j = 0 ; j < maxAttributes; j++)
				{
					after.C.attributes[j].v1 = actual.A.attributes[j].v1 * t1 + actual.B.attributes[j].v1 * t11;
					after.C.attributes[j].v2 = actual.A.attributes[j].v2 * t1 + actual.B.attributes[j].v2 * t11;
					after.C.attributes[j].v3 = actual.A.attributes[j].v3 * t1 + actual.B.attributes[j].v3 * t11;
					after.C.attributes[j].v4 = actual.A.attributes[j].v4 * t1 + actual.B.attributes[j].v4 * t11;
				}

				after.B.gl_Position = actual.A.gl_Position * t2 + actual.C.gl_Position * t21;
				for(size_t j = 0 ; j < maxAttributes; j++)
				{
					after.B.attributes[j].v1 = actual.A.attributes[j].v1 * t1 + actual.C.attributes[j].v1 * t11;
					after.B.attributes[j].v2 = actual.A.attributes[j].v2 * t1 + actual.C.attributes[j].v2 * t11;
					after.B.attributes[j].v3 = actual.A.attributes[j].v3 * t1 + actual.C.attributes[j].v3 * t11;
					after.B.attributes[j].v4 = actual.A.attributes[j].v4 * t1 + actual.C.attributes[j].v4 * t11;
				}
				AfterClipping.push_back(after);
			}
		}
		// mozme zahodit
		else
		{
			continue;
		}
	}
	PrimitivesToTriangle.clear();
	perspectiveDivision();
}


void GPU::perspectiveDivision()
{
	for(size_t i = 0 ; i < AfterClipping.size();i++)
	{
		AfterClipping[i].A.gl_Position.x = AfterClipping[i].A.gl_Position.x / AfterClipping[i].A.gl_Position.w;
		AfterClipping[i].A.gl_Position.y = AfterClipping[i].A.gl_Position.y / AfterClipping[i].A.gl_Position.w;
		AfterClipping[i].A.gl_Position.z = AfterClipping[i].A.gl_Position.z / AfterClipping[i].A.gl_Position.w;

		AfterClipping[i].B.gl_Position.x = AfterClipping[i].B.gl_Position.x / AfterClipping[i].B.gl_Position.w;
		AfterClipping[i].B.gl_Position.y = AfterClipping[i].B.gl_Position.y / AfterClipping[i].B.gl_Position.w;
		AfterClipping[i].B.gl_Position.z = AfterClipping[i].B.gl_Position.z / AfterClipping[i].B.gl_Position.w;

		AfterClipping[i].C.gl_Position.x = AfterClipping[i].C.gl_Position.x / AfterClipping[i].C.gl_Position.w;
		AfterClipping[i].C.gl_Position.y = AfterClipping[i].C.gl_Position.y / AfterClipping[i].C.gl_Position.w;
		AfterClipping[i].C.gl_Position.z = AfterClipping[i].C.gl_Position.z / AfterClipping[i].C.gl_Position.w;
	}
	viewportTransformation();
}

void GPU:: viewportTransformation()
{
	std::vector<float>sortwidth;
	std::vector<float>sortheight;

	//inspiration from http://web.cse.ohio-state.edu/~shen.94/5542/Site/Slides_files/coordinates5542.pdf
	for(size_t i = 0;i < AfterClipping.size();i++)
	{
		AfterClipping[i].A.gl_Position.x = ((AfterClipping[i].A.gl_Position.x -(-1)) / 2.0) * FrameBufferWidth;
		AfterClipping[i].B.gl_Position.x = ((AfterClipping[i].B.gl_Position.x -(-1)) / 2.0) * FrameBufferWidth;
		AfterClipping[i].C.gl_Position.x = ((AfterClipping[i].C.gl_Position.x -(-1)) / 2.0) * FrameBufferWidth;

		sortwidth.push_back(AfterClipping[i].A.gl_Position.x);
		sortwidth.push_back(AfterClipping[i].B.gl_Position.x);
		sortwidth.push_back(AfterClipping[i].C.gl_Position.x);
		sort(sortwidth.begin(),sortwidth.end());
		MinX.push_back(sortwidth[0]);
		MaxX.push_back(sortwidth[2]);
		sortwidth.clear();

		AfterClipping[i].A.gl_Position.y = ((AfterClipping[i].A.gl_Position.y -(-1)) / 2.0) * FrameBufferHeight;
		AfterClipping[i].B.gl_Position.y = ((AfterClipping[i].B.gl_Position.y -(-1)) / 2.0) * FrameBufferHeight;
		AfterClipping[i].C.gl_Position.y = ((AfterClipping[i].C.gl_Position.y -(-1)) / 2.0) * FrameBufferHeight;

		sortheight.push_back(AfterClipping[i].A.gl_Position.y);
		sortheight.push_back(AfterClipping[i].B.gl_Position.y);
		sortheight.push_back(AfterClipping[i].C.gl_Position.y);
		sort(sortheight.begin(),sortheight.end());
		MinY.push_back(sortheight[0]);
		MaxY.push_back(sortheight[2]);
		sortheight.clear();

	}
	rasterization();
}

// inspiration from https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage
// interpolation insp from https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
void GPU::rasterization()
{
	auto &program = Programs[ActiveProgram - 1];
	for(size_t i = 0; i < AfterClipping.size();i++)
	{
		for(uint32_t y =(uint32_t) MinY[i]; y < MaxY[i];y++)
		{
			for(uint32_t x =(uint32_t)MinX[i];x < MaxX[i];x++)
			{
				glm::vec4 A = AfterClipping[i].A.gl_Position;
				glm::vec4 B = AfterClipping[i].B.gl_Position;
				glm::vec4 C = AfterClipping[i].C.gl_Position;
				glm::vec4 currPoint;
				currPoint.x = x+0.5;
				currPoint.y = y +0.5;

				float E_AB = (currPoint.x - A.x)*(B.y - A.y) - (currPoint.y - A.y)*(B.x - A.x);
				float E_BC = (currPoint.x - B.x)*(C.y - B.y) - (currPoint.y - B.y)*(C.x - B.x);
				float E_CA = (currPoint.x - C.x)*(A.y - C.y) - (currPoint.y - C.y)*(A.x - C.x);
	
				if((E_AB >=0 && E_CA>=0 && E_BC>=0) || (E_AB<=0 && E_CA <=0 && E_BC<=0))
				{
					InFragment infragment;
					OutFragment outfragment;
					infragment.gl_FragCoord.x = currPoint.x;
					infragment.gl_FragCoord.y = currPoint.y;
					//if(program.vs2fs_set == true) optimalizacia nevysla 
					//{
						// interpolacia vektorov v0 = b-a; v1 = c-a; v2 = currPoint - a; kedze z suradnicu z currPoint nepozname pouzijeme 0
						glm::vec3 v0,v1,v2;
						v0.x = B.x - A.x;
						v0.y = B.y - A.y;
						v0.z = B.z - A.z;
						v1.x = C.x-A.x;
						v1.y = C.y-A.y;
						v1.z = C.z - A.y;
						v2.x = currPoint.x - A.x;
						v2.y = currPoint.y - A.y;
						v2.z = 0.f - A.z;
						float d00 = glm::dot(v0,v0);
						float d01 = glm::dot(v0,v1);
						float d11 = glm::dot(v1,v1);
						float d20 = glm::dot(v2,v0);
						float d21 = glm::dot(v2,v1);
						float invD = 1.f / (d00 * d11 - d01 * d01);
						float lambda1 = (d11 * d20 - d01 * d21) * invD;
						float lambda2 = (d00 * d21 - d01 * d20) * invD;
						float lambda0 = 1.f - lambda1 - lambda2;

						float upSide = ((A.z * lambda0) / A.w) + ((B.z * lambda1) / B.w) + ((C.z * lambda2) / C.w);
						float downSide = (lambda0 / A.w) + (lambda1 / B.w) + (lambda2/ C.w);
						float calculate_Z = upSide / downSide;

						//printf("%f %f %f\n",upSide, downSide, calculate_Z);
						infragment.gl_FragCoord.z = calculate_Z;
						for(size_t j = 0; j < maxAttributes;j++)
						{
							if(program.vs2fs[j] == AttributeType::FLOAT)
							{
							infragment.attributes[j].v1 = (((AfterClipping[i].A.attributes[j].v1 * lambda0) / A.w ) + ((AfterClipping[i].B.attributes[j].v1 * lambda2) / B.w) + ((AfterClipping[i].C.attributes[j].v1 * lambda1) / C.w)) / ((lambda0 / A.w)+(lambda2 / B.w)+(lambda1 / C.w));
							}
							else if(program.vs2fs[j] == AttributeType::VEC2)
							{
							infragment.attributes[j].v2 = (((AfterClipping[i].A.attributes[j].v2 * lambda0) / A.w ) + ((AfterClipping[i].B.attributes[j].v2 * lambda2) / B.w) + ((AfterClipping[i].C.attributes[j].v2 * lambda1) / C.w)) / ((lambda0 / A.w)+(lambda2 / B.w)+(lambda1 / C.w));
							}
							else if(program.vs2fs[j] == AttributeType::VEC3)
							{
							 //printf("x%f y:%f  		%f %f %f\n",currPoint.x,currPoint.y,getlambda[0],getlambda[1],getlambda[2]);
							infragment.attributes[j].v3 = (((AfterClipping[i].A.attributes[j].v3 * lambda0) / A.w ) + ((AfterClipping[i].B.attributes[j].v3 * lambda1) / B.w) + ((AfterClipping[i].C.attributes[j].v3 * lambda2) / C.w)) / ((lambda0 / A.w)+(lambda1 / B.w)+(lambda2 / C.w));
							}
							else if(program.vs2fs[j] == AttributeType::VEC4)
							{
								//printf("x%f y:%f  		%f %f %f\n",currPoint.x,currPoint.y,getlambda[0],getlambda[1],getlambda[2]);
							infragment.attributes[j].v4 = (((AfterClipping[i].A.attributes[j].v4 * lambda0) / A.w ) + ((AfterClipping[i].B.attributes[j].v4 * lambda1) / B.w) + ((AfterClipping[i].C.attributes[j].v4 * lambda2) / C.w)) / ((lambda0 / A.w)+(lambda1 / B.w)+(lambda2 / C.w));
							}
						}
					//}
					program.fragmentShader(outfragment,infragment,program.uniforms);
					float storedDepth = DepthBuff[y * FrameBufferWidth + x]; // ziskavame aktualne zapisanu hlbku z framebuffera
					float newDepth = infragment.gl_FragCoord.z;

					bool reWrite = depthTest(newDepth,storedDepth);

					//printf("NEW:%f STORED %f\n",newDepth,storedDepth);
					if(reWrite)
					{
					ColorBuff[(y * FrameBufferWidth + x) * 4] = outfragment.gl_FragColor.r*255;
					ColorBuff[(y * FrameBufferWidth + x) * 4 + 1] = outfragment.gl_FragColor.g*255;
					ColorBuff[(y * FrameBufferWidth + x) * 4 + 2] = outfragment.gl_FragColor.b*255;
					ColorBuff[(y * FrameBufferWidth + x) * 4 + 3] = outfragment.gl_FragColor.a*255;
					DepthBuff[y * FrameBufferWidth + x] = infragment.gl_FragCoord.z;
					}
				}
			}
		}
	}
	AfterClipping.clear();
	return;
}

bool GPU::depthTest(float newDepth,float oldDepth)
{
	if(newDepth < oldDepth)
	{
		return true;
	}
	else
	{
		return false;
	}
}