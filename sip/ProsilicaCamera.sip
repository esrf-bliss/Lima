namespace Prosilica
{
  class Camera
  {
%TypeHeaderCode
#include <ProsilicaCamera.h>
%End
  public:
    Camera(const char*);
    ~Camera();
      
    bool isMonochrome() const;
    void getMaxWidthHeight(unsigned long& width /Out/,unsigned long& height /Out/);
    int getNbAcquiredFrames() const;

    VideoMode getVideoMode() const;
    void 	setVideoMode(VideoMode);
      
    void 	startAcq();
    void	reset();
  };
};
