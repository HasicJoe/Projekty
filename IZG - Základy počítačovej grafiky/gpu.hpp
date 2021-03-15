/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <string.h>
#include <map>
#include <unordered_map>
#include <array>
#include <inttypes.h>
#include <limits>
#include <bits/stdc++.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
/**ls
 * @brief This class represent software GPU
 */
class GPU{
  public:
    GPU();
    virtual ~GPU();

    //buffer object commands
    BufferID  createBuffer           (uint64_t size);
    void      deleteBuffer           (BufferID buffer);
    void      setBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void const* data);
    void      getBufferData          (BufferID buffer,uint64_t offset,uint64_t size,void      * data);
    bool      isBuffer               (BufferID buffer);

    //vertex array object commands (vertex puller)
    ObjectID  createVertexPuller     ();
    void      deleteVertexPuller     (VertexPullerID vao);
    void      setVertexPullerHead    (VertexPullerID vao,uint32_t head,AttributeType type,uint64_t stride,uint64_t offset,BufferID buffer);
    void      setVertexPullerIndexing(VertexPullerID vao,IndexType type,BufferID buffer);
    void      enableVertexPullerHead (VertexPullerID vao,uint32_t head);
    void      disableVertexPullerHead(VertexPullerID vao,uint32_t head);
    void      bindVertexPuller       (VertexPullerID vao);
    void      unbindVertexPuller     ();
    bool      isVertexPuller         (VertexPullerID vao);

    //program object commands
    ProgramID createProgram          ();
    void      deleteProgram          (ProgramID prg);
    void      attachShaders          (ProgramID prg,VertexShader vs,FragmentShader fs);
    void      setVS2FSType           (ProgramID prg,uint32_t attrib,AttributeType type);
    void      useProgram             (ProgramID prg);
    bool      isProgram              (ProgramID prg);
    void      programUniform1f       (ProgramID prg,uint32_t uniformId,float     const&d);
    void      programUniform2f       (ProgramID prg,uint32_t uniformId,glm::vec2 const&d);
    void      programUniform3f       (ProgramID prg,uint32_t uniformId,glm::vec3 const&d);
    void      programUniform4f       (ProgramID prg,uint32_t uniformId,glm::vec4 const&d);
    void      programUniformMatrix4f (ProgramID prg,uint32_t uniformId,glm::mat4 const&d);

    //framebuffer functions
    void      createFramebuffer      (uint32_t width,uint32_t height);
    void      deleteFramebuffer      ();
    void      resizeFramebuffer      (uint32_t width,uint32_t height);
    uint8_t*  getFramebufferColor    ();
    float*    getFramebufferDepth    ();
    uint32_t  getFramebufferWidth    ();
    uint32_t  getFramebufferHeight   ();

    //execution commands
    void      clear                  (float r,float g,float b,float a);
    void      drawTriangles          (uint32_t  nofVertices);
    void      assemblePrimitives     ();
    void      clipping               ();
    void      perspectiveDivision    ();
    void      viewportTransformation ();
    void      rasterization          ();
    bool      depthTest              (float newDepth,float oldDepth);

    /// \addtogroup gpu_init 00. proměnné, inicializace / deinicializace grafické karty
    /// @{
  private:
    //this struct respesent one head of VertexPuller
    struct VertexHead
    {
      uint64_t buffID;
      uint64_t offset;
      uint64_t stride;
      AttributeType Atype;
      bool enabled = false ; //false at start
    };
    // struct represents VerteXPullerTable
    struct VertexPullerTable{
      uint64_t VertexID;
      uint64_t BuffID;
      IndexType Indexing;
      std::array<VertexHead,maxAttributes> headArray; //maxAttributes equal to 16
    };

    struct ProgramSettings
    {
      uint64_t ProgramID;
      VertexShader vertexShader = VertexShader();
      FragmentShader fragmentShader = FragmentShader();
      Uniforms uniforms;
      AttributeType vs2fs[maxAttributes]; //maxAttributes equal to 16
      bool vs2fs_set = false;
    };
    struct BufferStruct{
      uint64_t BufferID;
      std::vector<uint8_t> data;
    };

    struct Triangle
    {
      OutVertex A;
      OutVertex B;
      OutVertex C;
    };

    //vector to buffer functions
    std::vector<BufferStruct>buffs;
    //vector to VertexPuller functions
    std::vector<VertexPullerTable> VertexPull;
    // vector to programs functions
    std::vector<ProgramSettings> Programs;
    // vectors to framebuffer
    std::vector<uint8_t> ColorBuff;
    std::vector<float>DepthBuff;
    std::vector <OutVertex> setPrimitives;
    std::vector <Triangle> PrimitivesToTriangle;
    std::vector <Triangle> AfterClipping;
    std::vector <Triangle> Transformed;
    std::vector <float> MinX;
    std::vector <float> MinY;
    std::vector <float> MaxX;
    std::vector <float> MaxY;
    
    VertexPullerTable* activeVertexPuller = nullptr;
    ProgramSettings* activeProgram = nullptr;
    uint16_t IAcount = 0;
    uint64_t VertexPullCounter = 0;
    uint64_t ProgramsCounter = 0;
    uint64_t ActiveProgram = emptyID;
    uint64_t ActiveVertexPuller = emptyID;
    uint32_t FrameBufferWidth = 0;
    uint32_t FrameBufferHeight = 0;
    uint64_t BufferCounter = 0;


  template <typename IndexType> 
  void indexDraw()
	{
  auto& actProgram = Programs[ActiveProgram-1];
  auto pullerCounter = 0;
  InVertex invertex;
  std::vector<IndexType> indexArray = reinterpret_cast<std::vector <IndexType>&>(buffs[VertexPull[ActiveVertexPuller-1].BuffID - 1].data);
  std::vector<OutVertex> outvertex {indexArray.size()};
  for(auto&vertex : outvertex)
  {
    invertex.gl_VertexID =(uint32_t) indexArray[pullerCounter];
    for(size_t i = 0; i < maxAttributes;i++)
    {
      if(VertexPull[ActiveVertexPuller-1].headArray[i].enabled == true)
      {
        if(VertexPull[ActiveVertexPuller-1].headArray[i].Atype == AttributeType::FLOAT) // float
        {
          invertex.attributes[i].v1 = reinterpret_cast<float&>(buffs[VertexPull[ActiveVertexPuller-1].headArray[i].buffID-1].data[VertexPull[ActiveVertexPuller-1].headArray[i].stride * (uint64_t)indexArray[pullerCounter] + VertexPull[ActiveVertexPuller-1].headArray[i].offset]);
        }
        else if(VertexPull[ActiveVertexPuller-1].headArray[i].Atype == AttributeType::VEC2) // vec2
        {
          invertex.attributes[i].v2 = reinterpret_cast<glm::vec2&>(buffs[VertexPull[ActiveVertexPuller-1].headArray[i].buffID-1].data[VertexPull[ActiveVertexPuller-1].headArray[i].stride * (uint64_t)indexArray[pullerCounter] + VertexPull[ActiveVertexPuller-1].headArray[i].offset]);
        }
        else if(VertexPull[ActiveVertexPuller-1].headArray[i].Atype == AttributeType::VEC3) // vec3
        {
          invertex.attributes[i].v3 = reinterpret_cast<glm::vec3&>(buffs[VertexPull[ActiveVertexPuller-1].headArray[i].buffID-1].data[VertexPull[ActiveVertexPuller-1].headArray[i].stride * (uint64_t)indexArray[pullerCounter] + VertexPull[ActiveVertexPuller-1].headArray[i].offset]);
        }
        else if(VertexPull[ActiveVertexPuller-1].headArray[i].Atype == AttributeType::VEC4) // vec4
        {
          invertex.attributes[i].v4 =reinterpret_cast<glm::vec4&>(buffs[VertexPull[ActiveVertexPuller-1].headArray[i].buffID-1].data[VertexPull[ActiveVertexPuller-1].headArray[i].stride * (uint64_t)indexArray[pullerCounter] + VertexPull[ActiveVertexPuller-1].headArray[i].offset]);
        } 
      }
    }
    actProgram.vertexShader(vertex,invertex,actProgram.uniforms);
    pullerCounter++;
  }
  for(size_t i = 0; i < outvertex.size();i++)
  {
    setPrimitives.push_back(outvertex[i]);
  }
  outvertex.clear();
  assemblePrimitives();
}


   
    
    /// \todo zde si můžete vytvořit proměnné grafické karty (buffery, programy, ...)
};

