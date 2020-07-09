#include <tests/renderPhongFrame.hpp>

#include <student/phongMethod.hpp>
#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>

std::vector<uint8_t>renderPhongFrame(uint32_t width,uint32_t height){
  auto phong = PhongMethod();
  phong.gpu.createFramebuffer(width,height);

  auto orbitCamera = basicCamera::OrbitCamera();
  auto perspectiveCamera = basicCamera::PerspectiveCamera();
  orbitCamera.addDistance(1.f);
  perspectiveCamera.setNear(0.1f);
  auto const aspect = static_cast<float>(width) / static_cast<float>(height);
  perspectiveCamera.setAspect(aspect);

  auto const proj = perspectiveCamera.getProjection();
  auto const view = orbitCamera      .getView      ();
  auto const camera = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));
  glm::vec3  light         = glm::vec3(10.f,10.f,10.f);

  phong.onDraw(proj,view,light,camera);

  auto&gpu = phong.gpu;
  auto frame = gpu.getFramebufferColor();
  auto const w = gpu.getFramebufferWidth();
  auto const h = gpu.getFramebufferHeight(); 

  std::vector<uint8_t>res;

  if(frame == nullptr)
    return std::vector<uint8_t>(4*width*height);

  for(uint32_t i=0;i<w*h*4;++i)
    res.push_back(frame[i]);

  return res;
}
