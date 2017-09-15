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

#ifndef H264STREAM_INC
#define H264STREAM_INC

#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include "BlobNetworkHeader.h"
#include "H264Decoder.h"
#include <yarp/os/InputStream.h>


namespace yarp {
    namespace os {
        class H264Stream;
    }
}

class yarp::os::H264Stream : public yarp::os::impl::DgramTwoWayStream
{
private:

    DgramTwoWayStream *delegate;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    int phase;
    char *cursor;
    int remaining;
    H264Decoder *decoder;
    int remotePort;
public:
    H264Stream( int remotePort) :
            phase(0),
            cursor(NULL),
            remaining(0),
            remotePort(remotePort)
    {;}

    virtual ~H264Stream()
    {
        if (decoder!=NULL)
        {
            delete decoder;
            decoder = NULL;
        }

        if (delegate!=NULL)
        {
            delete delegate;
            delegate = NULL;
        }
    }

    bool setStream(yarp::os::impl::DgramTwoWayStream *stream);

    void start (void);

    virtual InputStream& getInputStream() override;
    virtual OutputStream& getOutputStream() override;

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override;

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override;

};

#endif
