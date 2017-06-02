//Copyright (C) Rayonix, LLC
#include <stdio.h>
#include <unistd.h>

#include "RayonixHsCamera.h"

int main() {
   int desiredFrameCount = 10;
   
   lima::RayonixHs::Camera camera;

   printf("Setting exposure time...\n");
   camera.setExpTime(0);

   printf("Setting num frames...\n");
   camera.setNbFrames(desiredFrameCount);
   
   printf("Starting acq...\n");
   camera.startAcq();
   while(!camera.acquiring()) {}
   printf("Acquisition started.\n");
   
   printf("Waiting on completion...\n");
   while(camera.acquiring()) {
      usleep(100 * 1000);
      //std::cout << "Still acquring..." << std::endl;
   }
   
   printf("Done.\n");

   return 0;
}
