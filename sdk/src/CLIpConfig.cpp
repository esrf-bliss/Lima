/*
| ==============================================================================
| Copyright (C) 2006-2007 Prosilica.  All Rights Reserved.
|
| Redistribution of this header file, in original or modified form, without
| prior written consent of Prosilica is prohibited.
|
|==============================================================================
|
| This sample code (Linux only) allow to change the IP Configuration of a given
| camera.
|
|==============================================================================
|
| THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
| WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
| NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
| DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
| LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
| OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED  AND ON ANY THEORY OF
| LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
| NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
| EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
|
|==============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>

#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define _LINUX

#ifdef __x86_64
	#define _x64
#else
	#define _x86
#endif


#include <PvApi.h>

// Maximum cameras in our list
#define MAX_CAMERA_LIST 20

void Sleep(unsigned int time)
{
    struct timespec t,r;
    
    t.tv_sec    = time / 1000;
    t.tv_nsec   = (time % 1000) * 1000000;    
    
    while(nanosleep(&t,&r)==-1)
        t = r;
}

void ShowUsage()
{
    printf("usage: CLIpConfig [-u <camera unique ID>|-l] [-g|-s] [-m|-i|-n|-w] <string>\n");
    printf("-l\tlist all the cameras visible\n");
    printf("-u\tcamera unique ID\n");
    printf("-g\tget configuration\n");
    printf("-s\tset configuration\n");
    printf("-m\tmode (DHCP,AUTOIP or FIXED)\n");
    printf("-i\tIP address\n");
    printf("-n\tSubnet mask\n");
    printf("-w\tGateway\n");
}

void ListCameras()
{
    tPvCameraInfo   cameraList[MAX_CAMERA_LIST];
    unsigned long   cameraNum = 0;
    unsigned long   cameraRle;

    // first, get list of reachable cameras.
    cameraNum = PvCameraList(cameraList,MAX_CAMERA_LIST,NULL);

   // keep how many cameras listed are reachable
    cameraRle = cameraNum;    

    // then we append the list of unreachable cameras.
    if (cameraNum < MAX_CAMERA_LIST)
    {
        cameraNum += PvCameraListUnreachable(&cameraList[cameraNum],
                                             MAX_CAMERA_LIST-cameraNum,
                                             NULL);
    }

    if(cameraNum)
    {
        // and display them
        for(unsigned int i=0;i<cameraNum;i++)
        {
            if(i < cameraRle)
                printf("%s - %7s - Unique ID = %lu\n",cameraList[i].SerialString,
                                                      cameraList[i].DisplayName,
                                                      cameraList[i].UniqueId);
            else
                printf("%s - %7s - Unique ID = %lu (*)\n",cameraList[i].SerialString,
                                                          cameraList[i].DisplayName,
                                                          cameraList[i].UniqueId);      
        }

        if(cameraNum != cameraRle)
            printf("(*) camera is not reachable\n");
    }
    else
        printf("sorry, no camera was detected. Is there any plugged?\n");
}

int main(int argc, char* argv[])
{
    int err = 0;

    // initialise the Prosilica API
    if(!PvInitialize())
    {
        int c;
        unsigned long uid = 0;
        bool bList = false;
        bool bGet  = false;
        bool bSet  = false;
        bool bMode = false;
        bool bAddr = false;
        bool bMask = false;
        bool bWay  = false;
        char* vMode = NULL;
        char* vAddr = NULL;
        char* vMask = NULL;
        char* vWay  = NULL;
    
        while ((c = getopt (argc, argv, "lu:gsm:i:n:pw:h?")) != -1)
        {
            switch(c)
            {
                case 'l':
                {
                    bList = true;
                    break;
                }
                case 'u':
                {
                    if(optarg)
                        uid = atol(optarg);
                    
                    break;    
                }
                case 'g':
                {
                    bGet = true;
                    break;
                }
                case 's':
                {
                    bSet = true;
                    break;
                }
                case 'm':
                {
                    bMode = true;
                    if(optarg)
                        vMode = optarg;
                    break;
                }
                case 'i':
                {
                    bAddr = true;
                    if(optarg)
                        vAddr = optarg;
                    break;
                }
                case 'n':
                {
                    bMask = true;
                    if(optarg)
                        vMask = optarg;
                    break;
                }
                case 'w':
                {
                    bWay = true;
                    if(optarg)
                        vWay = optarg;
                    break;
                }
                case '?':
                case 'h':
                {
                    ShowUsage();
                    break;    
                }
                default:
                    break;
            }
        }

        if(uid || bList)
        {

        
            if(bList)
            {
                printf("Searching for cameras ...\n");
                Sleep(2000);
            
                ListCameras();
            }
            else
            {
                printf("Looking for the camera ...\n");
                Sleep(1000);
            
                if(bGet)
                {
                    tPvCameraInfo  camInfo;
                    tPvIpSettings  camConf;
                    struct in_addr addr;

                    if(!PvCameraInfo(uid,&camInfo) &&
                        !PvCameraIpSettingsGet(uid,&camConf))
                    {
                        printf("\t\t\t%s - %s\n",camInfo.SerialString,camInfo.DisplayName);
                        printf("Mode supported:\t\t");
                        if(camConf.ConfigModeSupport & ePvIpConfigPersistent)
                            printf("FIXED ");
                        if(camConf.ConfigModeSupport & ePvIpConfigDhcp)
                            printf("DHCP ");
                        if(camConf.ConfigModeSupport & ePvIpConfigAutoIp)
                            printf("AutoIP");
                        printf("\n");
                        printf("Current mode:\t\t");
                        if(camConf.ConfigMode & ePvIpConfigPersistent)
                            printf("FIXED\n");
                        else
                        if(camConf.ConfigMode & ePvIpConfigDhcp)
                            printf("DHCP&AutoIP\n");
                        else
                        if(camConf.ConfigMode & ePvIpConfigAutoIp)
                            printf("AutoIP\n");

                        addr.s_addr = camConf.CurrentIpAddress;
                        printf("Current address:\t%s\n",inet_ntoa(addr));
                        addr.s_addr = camConf.CurrentIpSubnet;
                        printf("Current subnet:\t\t%s\n",inet_ntoa(addr));
                        addr.s_addr = camConf.CurrentIpGateway;
                        printf("Current gateway:\t%s\n",inet_ntoa(addr));
                    }
                    else
                        fprintf(stderr,"failed to talk to the camera!\n");
                }
                else
                if(bSet)
                {
                    tPvCameraInfo  camInfo;
                    tPvIpSettings  camConf;
                    bool           bApply = false;

                    if(!PvCameraInfo(uid,&camInfo) &&
                        !PvCameraIpSettingsGet(uid,&camConf))
                    {
                        if(bMode && vMode)
                        {
                            unsigned long Mode = 0;                          
                                                 
                            if(!strcasecmp(vMode,"fixed"))
                                Mode = ePvIpConfigPersistent;
                            else
                            if(!strcasecmp(vMode,"dhcp"))
                                Mode = ePvIpConfigDhcp;
                            else
                            if(!strcasecmp(vMode,"autoip"))
                                Mode = ePvIpConfigAutoIp;
                            else
                            {
                                fprintf(stderr,"%s isn't a valid mode\n",vMode);
                                err = 1;
                            }

                            if(Mode)
                            {
                                if(camConf.ConfigModeSupport & Mode)
                                {
                                    camConf.ConfigMode = (tPvIpConfig)Mode;
                                    bApply = true; 
                                }
                                else
                                {
                                    fprintf(stderr,"%s isn't supported by the camera\n",vMode);
                                    err = 1;
                                }
                            }
                        }
                    
                        if(bAddr && vAddr)
                        {
                            camConf.PersistentIpAddr = inet_addr(vAddr); 
                            bApply = true;     
                        }
                      
                        if(bMask && vMask)
                        {
                            camConf.PersistentIpSubnet = inet_addr(vMask);
                            bApply = true;     
                        }
                        
                        if(bWay && vWay)
                        {
                            camConf.PersistentIpGateway = inet_addr(vWay);
                            bApply = true;     
                        }                                                 

                        if(bApply)
                        {
                            if(PvCameraIpSettingsChange(uid,&camConf))
                            {
                                fprintf(stderr,"failed to set the configuration!\n");
                                err = 1;
                            }
                            else
                                printf("Settings changed for %s - %s\n",camInfo.SerialString,camInfo.DisplayName);
                        }
                    }
                    else
                        fprintf(stderr,"failed to talk to the camera!\n");
                }
                else
                {
                    ShowUsage();
                    err = 1;  
                }
            }
        }
        else
        {
            ShowUsage();
            err = 1;  
        }

        // uninitialise the API
        PvUnInitialize();
    }
    else
    {
        err = 1;
        fprintf(stderr,"failed to initialise the API\n");
    }
    
	return err;
}
