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
    decoder = new H264Decoder();
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
        ImageOf<PixelRgb> & img_dec = decoder->getLastFrame();
        img.copy(img_dec);
        int len = decoder->getLastFrameSize();
        decoder->mutex.unlock();

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
            memcpy(b.get(),cursor,allow);
            cursor+=allow;
            remaining-=allow;
            if (debug) printf("returning %d bytes\n", allow);
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


void H264Stream::write(const Bytes& b)
{
    delegate->getOutputStream().write(b);
}

