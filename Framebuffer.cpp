/*
*  RaspGFX V0.1  
*  file: Framebuffer.cpp
*  
*  This file is part of the RaspiGFX project, a graphic library
*  for the use of small TFT LCD display modules (like the ili9340)
*  on Raspberry. This software contains portions of code produced
*  by Adafruit Industries. (For the original code and related license
*  see https://github.com/adafruit/Adafruit-GFX-Library)
*
*  Copyright (C) 2016 Vince Damiani
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "Framebuffer.h"
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/utsname.h>

#define DEBUG_MODE false
#define DEBUG(fmt, ...) if(DEBUG_MODE) printf(fmt, ##__VA_ARGS__)

#define FBTFT_MOD_PATH "/lib/modules/%s/kernel/drivers/staging/fbtft/fbtft_device.ko"
#define FB_DEVICE "/dev/fb1"


Framebuffer::Framebuffer(FBTftParam* prm) : fbTftParam(prm) {
   fbp = nullptr;
   deleteModule();
   initModule();
   getFramebuffer();
}

/*
*  Opens framebuffer device, fills variable and
*  fixed info structures and finally provides
*  the pointer to the mapped framebuffer into
*  memory.
*/
int8_t Framebuffer::getFramebuffer(void) {

   // Open device file
   int fbfd = open(FB_DEVICE, O_RDWR);
   if(fbfd == -1) {
      throw std::system_error {
         errno,
         std::system_category(), 
         "cannot open framebuffer device " + std::string(FB_DEVICE)
      };
      return -1;
   }

   // Get variable screen information
   if(ioctl(fbfd, FBIOGET_VSCREENINFO, &(var_info)) == -1) {
      throw std::system_error {
         errno,
         std::system_category(), 
         "reading variable framebuffer information"
      };
      return -1;
   }

   // Get fixed screen information
   if(ioctl(fbfd, FBIOGET_FSCREENINFO, &(fix_info)) == -1) {
      throw std::system_error {
         errno,
         std::system_category(), 
         "reading fixed framebuffer information"
      };
      return -1;
   }

   width = var_info.xres;
   height = var_info.yres;
   line_length = fix_info.line_length;
   bpp = var_info.bits_per_pixel;
   rotate = var_info.rotate;

   DEBUG("Opened framebuffer device");
   DEBUG("\n\tdevice: %s\n\tresolution: %dx%d\n\tbpp: \
%d\n\trotate: %d\n\tline length: %d\n",
      FB_DEVICE, width, height, bpp, rotate, line_length);

   // map fb to user mem
   fbp = (char*)mmap(nullptr,
                     fix_info.smem_len,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fbfd,
                     0);

   if(fbp == MAP_FAILED) {
      throw std::system_error(
         errno,
         std::system_category(), 
         "mapping framebuffer device to memory"
      );
      return -1;
   }
   close(fbfd);
   return 0;
}

/*
*  Loads fbtft_device module into kernel.
*  See https://github.com/notro/fbtft/wiki
*  for more info.
*/
int8_t Framebuffer::initModule(void) {
    
   // Get current kernel version
   struct utsname si;
   uname(&si);
   DEBUG("System release: %s\n", si.release);

   // Get fbtft_device kernel module
   char mod_path_buff[128];
   snprintf(mod_path_buff, 128, FBTFT_MOD_PATH, si.release);

   int fd = open(mod_path_buff, O_RDONLY);
   if(fd == -1) {
      perror("Opening module file");
      return -1;
   }

   struct stat st;
   fstat(fd, &st);
   void *image = malloc(st.st_size);
   read(fd, image, st.st_size);

   // Prepare kernel module parameters
   snprintf(mod_path_buff, 128, "name=%s rotate=%d speed=%d000000", 
      fbTftParam->device_name.c_str(), fbTftParam->rotate, fbTftParam->speed);

   // Load fbtft_device module
   if(syscall(__NR_init_module, image, st.st_size, mod_path_buff) != 0) {
      perror("Module initializing");
   }

   close(fd);
   free(image);

   return 0;
}

/*
*  Unloads fbtft_device module.   
*/
int8_t Framebuffer::deleteModule(void) {
   if(fbp != nullptr) {
      if(munmap(fbp, fix_info.smem_len) == -1) {
         perror("unmapping");
         return -1;
      }   
   }
   if(syscall(__NR_delete_module, "fbtft_device", O_NONBLOCK) != 0) {
      perror("Removing module");
      return -1;
   }
   fbp = nullptr;
   return 0;
}

Framebuffer::~Framebuffer() {
   
}








