#include <tests/takeScreenShot.hpp>
#include <tests/renderPhongFrame.hpp>
#include <student/application.hpp>
#include <SDL.h>
#include <string>

void takeScreenShot(std::string const&groundTruthFile){
  uint32_t width = 500;
  uint32_t height = 500;

  auto frame = renderPhongFrame(width,height);

  auto surface = SDL_CreateRGBSurface(0, width, height, 24,0,0,0,0);

  copyToSDLSurface(surface,frame.data(),width,height);

  std::cerr << "storing screenshot to: \"" << groundTruthFile << "\"" << std::endl;

  SDL_Surface* rgb = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
  SDL_SaveBMP(rgb, groundTruthFile.c_str());
  SDL_FreeSurface(rgb);
  SDL_FreeSurface(surface);

}
