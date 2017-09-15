/* Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
* Author: Valentina Gaggero
* email:   valentina.gaggero@iit.it
* website: www.robotcub.org
* Permission is granted to copy, distribute, and/or modify this program
* under the terms of the GNU General Public License, version 2 or any
* later version published by the Free Software Foundation.
*
* A copy of the license can be found at
* http://www.robotcub.org/icub/license/gpl.txt
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
* Public License for more details
*/



#include <yarp/os/Log.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include <cstdio>
#include <cstring>

#include "H264Stream.h"

//#define debug_time 1

#ifdef debug_time
    #include <yarp/os/Time.h>
    #define DBG_TIME_PERIOD_PRINTS 10 //10 sec
#endif


using namespace yarp::os;
using namespace yarp::sig;
using namespace std;


bool H264Stream::setStream(yarp::os::impl::DgramTwoWayStream *stream)
{
    delegate = stream;
    if(NULL == delegate)
    {
        return false;
    }
    return true;
}

void H264Stream::start (void)
{
    decoder = new H264Decoder(this->remotePort);
    decoder->init();
    decoder->start();
}

InputStream& H264Stream::getInputStream()
{
    return *this;
}

OutputStream& H264Stream::getOutputStream()
{
    return *this;
}

//using yarp::os::OutputStream::write;


//using yarp::os::InputStream::read;

bool H264Stream::setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data)
{
    return true;
}

YARP_SSIZE_T H264Stream::read(const Bytes& b)
{

#ifdef debug_time
    static bool isFirst = true;
    double start_time = Time::now();
    double start_timeCopy;
    double end_time=0;
    static double last_call;
    static double sumOf_timeBetweenCalls=0;

    static double sumOf_timeOnMutex = 0;
    static double sumOf_timeOfCopyPerPahse[5] ={0};
    static uint32_t count=0;
    static uint32_t countPerPhase[5]={0};
    #define MAX_COUNT  100


    if(isFirst)
    {
        last_call = start_time;
        isFirst = false;
    }
    else
    {
        sumOf_timeBetweenCalls+=(start_time -last_call);
        last_call = start_time;
    }


#endif

    bool debug = false;
    if (remaining==0)
    {
        if (phase==1)
        {
            phase = 2;
            cursor = (char*)(img.getRawImage());
            remaining = img.getRawImageSize();
        } else if (phase==3)
        {
            phase = 4;
            cursor = NULL;
            remaining = blobHeader.blobLen;
        } else
        {
            phase = 0;
        }
    }
    while (phase==0)
    {
        decoder->mutex.lock();
        int len = 0;
        if(decoder->newFrameIsAvailable())
        {
            ImageOf<PixelRgb> & img_dec = decoder->getLastFrame();
            img.copy(img_dec);
            len = decoder->getLastFrameSize();
            decoder->mutex.unlock();
            #ifdef debug_time
            end_time = Time::now();
            sumOf_timeOnMutex +=(end_time - start_time);
            count++;
            if(count>=MAX_COUNT)
            {
                printf("STREAM On %d times: timeOnMutex is long %.6f sec\n",
                        MAX_COUNT, (sumOf_timeOnMutex/MAX_COUNT) );
                for(int x=0; x<5; x++)
                {
                    printf("STREAM: phase:%d, count=%d, time=%.6f sec\n", x, countPerPhase[x], ((countPerPhase[x]==0) ? 0: sumOf_timeOfCopyPerPahse[x]/countPerPhase[x]) );
                    countPerPhase[x] = 0;
                    sumOf_timeOfCopyPerPahse[x] = 0;
                }
                printf("H264: sleep=%.6f\n\n", sumOf_timeBetweenCalls/count);
                count = 0;
                isFirst = true;
                sumOf_timeOnMutex = 0;
                sumOf_timeBetweenCalls = 0;
            }
            #endif

        }
        else
        {
            //printf("h264Stream::read has been called but no frame is available!!\n");
            phase = 0;
            remaining = 0;
            cursor = NULL;
            decoder->setReq();
            decoder->mutex.unlock();
            decoder->semaphore.waitWithTimeout(1);
            return 0;
        }


        if (debug)
        {
            printf("Length is \"%d\"\n", len);
        }
        imgHeader.setFromImage(img);
        phase = 1;
        cursor = (char*)(&imgHeader);
        remaining = sizeof(imgHeader);
    }

    if (remaining>0)
    {
        int allow = remaining;
        if ((int)b.length()<allow)
        {
            allow = b.length();
        }
        if (cursor!=NULL)
        {
            #ifdef debug_time
            start_timeCopy = Time::now();
            #endif
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            if (debug) printf("returning %d bytes\n", allow);
            #ifdef debug_time
                end_time = Time::now();
                sumOf_timeOfCopyPerPahse[phase] +=(end_time - start_timeCopy);
                countPerPhase[phase]++;
            #endif
            return allow;
        } else
        {
            printf("non devo mai essere qui!!@@@\n");
            int result = delegate->getInputStream().read(b);
            if (debug) printf("Read %d bytes\n", result);
            if (result>0)
            {
                remaining-=result;
                if (debug) printf("%d bytes of meat\n", result);
                return result;
            }
        }
    }
    return -1;
}

//YARP_SSIZE_T H264Stream::read(const Bytes& b)
//{

//#ifdef debug_time
//    static bool isFirst = true;
//    double start_time = Time::now();
//    double end_time=0;

//    static double last_call;
//    static double sumOf_timeBetweenCalls = 0;
//    static double sumOf_timeOfNewSampleFunc = 0;
//    static uint32_t count=0;
//    #define MAX_COUNT  100


//    if(!isFirst)
//        sumOf_timeBetweenCalls+=(start_time -last_call);

//    last_call = start_time;


//#endif

//    bool debug = false;
//    if (remaining==0)
//    {
//        if (phase==1)
//        {
//            phase = 2;
//            cursor = (char*)(img.getRawImage());
//            remaining = img.getRawImageSize();
//        } else if (phase==3)
//        {
//            phase = 4;
//            cursor = NULL;
//            remaining = blobHeader.blobLen;
//        } else
//        {
//            phase = 0;
//        }
//    }
//    while (phase==0)
//    {
//        decoder->mutex.lock();
//        int len = 0;
//        if(decoder->newFrameIsAvailable())
//        {
//            ImageOf<PixelRgb> & img_dec = decoder->getLastFrame();
//            img.copy(img_dec);
//            len = decoder->getLastFrameSize();
//            decoder->mutex.unlock();
//        }
//        else
//        {
//            phase = 0;
//            remaining = 0;
//            cursor = NULL;
//            decoder->mutex.unlock();
//            return 0;
//        }


//        if (debug)
//        {
//            printf("Length is \"%d\"\n", len);
//        }
//        imgHeader.setFromImage(img);
//        phase = 1;
//        cursor = (char*)(&imgHeader);
//        remaining = sizeof(imgHeader);
//    }

//    if (remaining>0)
//    {
//        int allow = remaining;
//        if ((int)b.length()<allow)
//        {
//            allow = b.length();
//        }
//        if (cursor!=NULL)
//        {
//            memcpy(b.get(),cursor,allow);
//            cursor+=allow;
//            remaining-=allow;
//            if (debug) printf("returning %d bytes\n", allow);
//            #ifdef debug_time
//                end_time = Time::now();
//                sumOf_timeOfNewSampleFunc += (end_time-start_time);
//                count++;
//                isFirst=false;

//                if(count>=MAX_COUNT)
//                {
//                    printf("STREAM On %d times: NewSampleFunc is long %.6f sec and sleeps %.6f sec\n",
//                            MAX_COUNT, (sumOf_timeOfNewSampleFunc/MAX_COUNT), (sumOf_timeBetweenCalls/MAX_COUNT) );
//                    count = 0;
//                    isFirst = true;
//                    sumOf_timeBetweenCalls = 0;
//                    sumOf_timeOfNewSampleFunc = 0;
//                }


//            #endif
//            return allow;
//        } else
//        {
//            printf("non devo mai essere qui!!@@@\n");
//            int result = delegate->getInputStream().read(b);
//            if (debug) printf("Read %d bytes\n", result);
//            if (result>0)
//            {
//                remaining-=result;
//                if (debug) printf("%d bytes of meat\n", result);
//                return result;
//            }
//        }
//    }
//    return -1;
//}


void H264Stream::write(const Bytes& b)
{
    delegate->getOutputStream().write(b);
}

