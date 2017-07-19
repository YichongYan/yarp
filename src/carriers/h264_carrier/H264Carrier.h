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


namespace yarp {
    namespace os {
        class H264Carrier;
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
    bool decoderIsRunning;
    yarp::os::ConstString envelope;
public:
    H264Carrier()
    {;}

    virtual Carrier *create() override
    {
        return new H264Carrier();
    }

    virtual ConstString getName() override;

    virtual bool isConnectionless() override;

    virtual bool canAccept() override;

    virtual bool canOffer() override;

    virtual bool isTextMode() override;

    virtual bool canEscape() override;

    virtual void handleEnvelope(const yarp::os::ConstString& envelope) override;

    virtual bool requireAck() override;

    virtual bool supportReply() override;

    virtual bool isLocal() override;

    // this is important - flips expected flow of messages
    virtual bool isPush() override;

    virtual ConstString toString() override;

    virtual void getHeader(const Bytes& header) override;

    virtual bool checkHeader(const Bytes& header) override;

    virtual void setParameters(const Bytes& header) override;


    // Now, the initial hand-shaking

    virtual bool prepareSend(ConnectionState& proto) override;

    virtual bool sendHeader(ConnectionState& proto) override;

    virtual bool expectSenderSpecifier(ConnectionState& proto) override;

    virtual bool expectExtraHeader(ConnectionState& proto) override;

    bool respondToHeader(ConnectionState& proto) override;

    virtual bool expectReplyToHeader(ConnectionState& proto) override;

    virtual bool isActive() override;


    // Payload time!

    virtual bool write(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool reply(ConnectionState& proto, SizedWriter& writer) override;

    virtual bool sendIndex(ConnectionState& proto, SizedWriter& writer);

    virtual bool expectIndex(ConnectionState& proto) override;

    virtual bool sendAck(ConnectionState& proto) override;

    virtual bool expectAck(ConnectionState& proto) override;

    virtual ConstString getBootstrapCarrierName() override;

    virtual bool autoCompression() const;
};

#endif
