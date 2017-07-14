/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef H264CARRIER_INC
#define H264CARRIER_INC

#include <yarp/os/Carrier.h>
#include <yarp/os/NetType.h>
#include "H264Stream.h"

#include <cstring>

namespace yarp {
    namespace os {
        class H264Carrier;
        class h264CarrierRaw;
    }
}

/**
 *
 * A carrier for sending/receiving images via h264 over http.
 *
 * .................. to complete
 *
 */
class yarp::os::H264Carrier : public Carrier
{
private:
    //h264Decoder *decoder;
    bool decoderIsRunning;
    yarp::os::ConstString envelope;
public:
    H264Carrier()
    {
        //decoder = new H264Decoder();
        //decoderIsRunning = decoder.init();
    }

    virtual Carrier *create() override
    {

        return new H264Carrier();

    }

    virtual ConstString getName() override
    {
        return "h264";
    }

    virtual bool isConnectionless() override
    {
        return true;
    }

    virtual bool canAccept() override
    {
        return true;
    }

    virtual bool canOffer() override
    {
        return true;
    }

    virtual bool isTextMode() override
    {
        return false;
    }

    virtual bool canEscape() override
    {
        return false;
    }

    virtual void handleEnvelope(const yarp::os::ConstString& envelope) override
    {
        this->envelope = envelope;
    }

    virtual bool requireAck() override
    {
        return false;
    }

    virtual bool supportReply() override
    {
        return false;
    }

    virtual bool isLocal() override
    {
        return false;
    }

    // this is important - flips expected flow of messages
    virtual bool isPush() override
    {
        return false;
    }

    virtual ConstString toString() override
    {
        return "h264_carrier";
    }

    virtual void getHeader(const Bytes& header) override
    {
       printf("sono dentro la getHeader con size = %d\n", header.length());
        /*
        // GET /?action=stream HTTP/1.1
        const char *target = "GET /?ac";
        for (size_t i=0; i<8 && i<header.length(); i++) {
            header.get()[i] = target[i];
        }*/
    }

    virtual bool checkHeader(const Bytes& header) override
    {
        printf("sono dentro la checkHeader\n");
        /*if (header.length()!=8) {
            return false;
        }
        const char *target = "GET /?ac";
        for (int i=0; i<8; i++) {
            if (header.get()[i] != target[i]) {
                return false;
            }
        }
        //printf("Got header\n");
        */
        return true;
    }

    virtual void setParameters(const Bytes& header) override
    {
        // no parameters - no carrier variants
    }


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override
    {
        // nothing special to do
        return true;
    }

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override
    {
        return true;
    }

    virtual bool expectExtraHeader(ConnectionState& proto) override
    {
        /*ConstString txt;
        do {
            txt = proto.is().readLine();
        } while (txt!="");*/
        return true;
    }

    bool respondToHeader(ConnectionState& proto) override
    {
        /*ConstString target = "HTTP/1.0 200 OK\r\n\
        Connection: close\r\n\
        Server: yarp/mjpeg_carrier/0.1\r\n\
        Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n\
        Pragma: no-cache\r\n\
        Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n\
        Content-Type: multipart/x-mixed-replace;boundary=boundarydonotcross\r\n\
        \r\n\
        --boundarydonotcross\r\n";
        Bytes b((char*)target.c_str(),strlen(target.c_str()));
        proto.os().write(b);
        sender = true; // this is a pull connection, not a push
        //MjpegStream *stream = new MjpegStream(proto.giveStreams(),sender);
        //if (stream==NULL) { return false; }
        //proto.takeStreams(stream);*/
        return true;
    }

    virtual bool expectReplyToHeader(ConnectionState& proto) override
    {
        /*ConstString txt;
        do {
            txt = proto.is().readLine();
        } while (txt!="");*/


        H264Stream *stream = new H264Stream(proto.giveStreams(),/*sender*/false,
                                              autoCompression());
        if (stream==NULL) { return false; }
        proto.takeStreams(stream);
        return true;
    }

    virtual bool isActive() override
    {
        return true;
    }


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer)
    {
        return true;
    }

    virtual bool expectIndex(ConnectionState& proto) override
    {
        return true;
    }

    virtual bool sendAck(ConnectionState& proto) override
    {
        return true;
    }

    virtual bool expectAck(ConnectionState& proto) override
    {
        return true;
    }

    virtual ConstString getBootstrapCarrierName() override { return ""; }

    virtual bool autoCompression() const;
};

#endif
