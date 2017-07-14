/*
 * Copyright (C) 2010 RobotCub Consortium
 * Authors: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#ifndef H264STREAM_INC
#define H264STREAM_INC

#include <yarp/os/InputStream.h>
#include <yarp/os/OutputStream.h>
//#include <yarp/os/TwoWayStream.h>
#include <yarp/os/impl/DgramTwoWayStream.h>
#include <yarp/os/StringInputStream.h>
#include <yarp/os/StringOutputStream.h>
#include <yarp/os/ManagedBytes.h>
#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>

#include "BlobNetworkHeader.h"

#include"H264Decoder.h"



namespace yarp {
    namespace os {
        class H264Stream;
    }
}

class yarp::os::H264Stream : public yarp::os::impl::DgramTwoWayStream//,
                             // public InputStream,
                              //public OutputStream
{
private:
    DgramTwoWayStream *delegate;
    StringInputStream sis;
    StringOutputStream sos;
    yarp::sig::ImageOf<yarp::sig::PixelRgb> img;
    yarp::sig::ImageNetworkHeader imgHeader;
    BlobNetworkHeader blobHeader;
    ManagedBytes cimg;
    int phase;
    char *cursor;
    int remaining;
    bool sender;
    bool firstRound;
    bool autocompress;
    yarp::os::Bytes envelope;
    readEnvelopeCallbackType readEnvelopeCallback;
    void* readEnvelopeCallbackData;
    H264Decoder *decoder;
public:
    H264Stream(/*DgramTwoWayStream *delegate,*/ bool sender, bool autocompress) :
            //delegate(delegate),
            phase(0),
            cursor(NULL),
            remaining(0),
            sender(sender),
            firstRound(true),
            autocompress(autocompress),
            readEnvelopeCallback(NULL),
            readEnvelopeCallbackData(NULL)
    {
       /* decoder = new H264Decoder();
        decoder->init();
        decoder->start();*/
    }

    virtual ~H264Stream()
    {
        if (delegate!=NULL)
        {
            delete delegate;
            delegate = NULL;
        }
     /*   if(decoder != NULL)
        {
            delete decoder;
            decoder = NULL;
        }*/
    }

    bool setStream(yarp::os::impl::DgramTwoWayStream *stream)
    {
        delegate = stream;
        if(NULL == delegate)
        {
            return false;
        }
        return true;
    }

    void start (void)
    {
        decoder = new H264Decoder();
        decoder->init();
        decoder->start();
    }

    virtual InputStream& getInputStream() override { return *this; }
    virtual OutputStream& getOutputStream() override { return *this; }


//    virtual const Contact& getLocalAddress() override {
//        return delegate->getLocalAddress();
//    }

//    virtual const Contact& getRemoteAddress() override {
//        return delegate->getRemoteAddress();
//    }

//    virtual bool isOk() override {
//        return delegate->isOk();
//    }

//    virtual void reset() override {
//        delegate->reset();
//    }

//    virtual void close() override {
//        delegate->close();
//    }

//    virtual void beginPacket() override {
//        delegate->beginPacket();
//    }

//    virtual void endPacket() override {
//        delegate->endPacket();
//    }

    using yarp::os::OutputStream::write;
    virtual void write(const Bytes& b) override;

    using yarp::os::InputStream::read;
    virtual YARP_SSIZE_T read(const Bytes& b) override;

//    virtual void interrupt() override {
//        delegate->getInputStream().interrupt();
//    }

    virtual bool setReadEnvelopeCallback(InputStream::readEnvelopeCallbackType callback, void* data) override {
      /*  if (!autocompress) {
            return false;
        }
        return decompression.setReadEnvelopeCallback(callback, data);*/
        return true;
    }
};

#endif
