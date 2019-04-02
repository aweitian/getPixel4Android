#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

void usage() {
    fprintf(stderr,
            "usage: \n"
            "   getpixel info -- show width height bpp\n"
            "   getpixel x y\n"
            "   index starts 1\n"
            "   eg: getpixel 1 1\n"
    );
}

int main(int argc, char**argv)
{
    int x = 0, y = 0;
    const char* fbpath = "/dev/graphics/fb0";
    unsigned char * mapbase;
    int fb;
    int bpp = -1;
    int i;
    int m = 0;//xy mode
    if (argc != 3)
    {
        if (argc != 2)
        {
            usage(); 
            exit(1);
        }
        if (strcmp(argv[1],"info") != 0)
        {
            usage(); 
            exit(1);
        }
        m = 1;
    }

    fb = open(fbpath, O_RDONLY);

    if (fb <= 0) 
    {
        perror("Error: cannot open framebuffer device");
        return 1;
    }
    struct fb_var_screeninfo vinfo;
    // struct fb_fix_screeninfo finfo;
    // if (ioctl(fbFd, FBIOGET_FSCREENINFO, &finfo) == -1)
    // {
    //     perror("Error reading variable information");
    //     return 1;
    // } 

    if (ioctl(fb, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("Error reading variable information");
        return 1;
    }

    switch (vinfo.bits_per_pixel) {
        case 16:
            bpp = 2;
            break;
        case 24:
            bpp = 3;
            break;
        case 32:
            bpp = 4;
            break;
        default:
            perror("Error reading bits_per_pixel");
            return 1;
    }
    if(m == 1)
    {
        printf("%d,%d,%d",vinfo.xres,vinfo.yres,bpp);
        return 1;
    }
    int mapsize = vinfo.xres * vinfo.yres * bpp + 12;
    // printf("map size:%d\n", mapsize);
    int fd=open("/dev/awmem",O_CREAT|O_RDWR);//fd为文件描述符。/dev/mem是整个物理地址空间。
    mapbase=(unsigned char *)mmap(NULL, mapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);//通过文件描述符和物理地址得到虚拟地址
    if (mapbase == (void *)(-1)) 
    {
        fprintf(stderr, "mmap() failed\n");
        close(fb);
        close(fd);
        return 1;
    }
    system("/system/bin/screencap /dev/awmem");

    x = atoi(argv[1])-1;
    y = atoi(argv[2])-1;

    if (x < 0 || y < 0 || x > vinfo.xres || y > vinfo.yres)
    {
        printf("max info:%d,%d,%d",vinfo.xres,vinfo.yres,bpp);
        exit(1);
    }

    for(i=0;i<3;i++)
    {
        printf("%02x", *(mapbase+12+(x+y*vinfo.xres)*bpp+i));
    }   

    munmap((void *)mapbase, mapsize);
    close(fb);
    close(fd);
    // fprintf(stderr, "ok\n");
    return(0);
}    




