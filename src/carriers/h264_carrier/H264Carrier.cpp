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

#include <cstdio>


#include "H264Carrier.h"
#include "H264Stream.h"
#include "yarp/os/Contact.h"
#include "yarp/os/impl/FakeFace.h"


using namespace yarp::os;
using namespace yarp::sig;

#define dbg_printf if (0) printf


ConstString H264Carrier::getName()
{
    return "h264";
}

bool H264Carrier::isConnectionless()
{
    return true;
}

bool H264Carrier::canAccept()
{
    return true;
}

bool H264Carrier::canOffer()
{
    return true;
}

bool H264Carrier::isTextMode()
{
    return false;
}

bool H264Carrier::canEscape()
{
    return false;
}

void H264Carrier::handleEnvelope(const yarp::os::ConstString& envelope)
{
    this->envelope = envelope;
}

bool H264Carrier::requireAck()
{
    return false;
}

bool H264Carrier::supportReply()
{
    return false;
}

bool H264Carrier::isLocal()
{
    return false;
}

// this is important - flips expected flow of messages
bool H264Carrier::isPush()
{
    return false;
}

ConstString H264Carrier::toString()
{
    return "h264_carrier";
}

void H264Carrier::getHeader(const Bytes& header)
{
}

bool H264Carrier::checkHeader(const Bytes& header)
{
    return true;
}

void H264Carrier::setParameters(const Bytes& header)
{
    // no parameters - no carrier variants
}


// Now, the initial hand-shaking

bool H264Carrier::prepareSend(ConnectionState& proto)
{
    // nothing special to do
    return true;
}

bool H264Carrier::sendHeader(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::expectSenderSpecifier(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::expectExtraHeader(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::respondToHeader(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::expectReplyToHeader(ConnectionState& proto)
{
    // sono il receiver...credo

    H264Stream *stream = new H264Stream(/*sender*/false,
                                          autoCompression());
    if (stream==NULL) { return false; }

    yarp::os::Contact remote = proto.getStreams().getRemoteAddress();
    bool ok = stream->open(remote);
    if (!ok)
    {
        delete stream;
        return false;
    }
    stream->start();

    proto.takeStreams(stream);
    return true;
}

bool H264Carrier::isActive()
{
    return true;
}

bool H264Carrier::write(ConnectionState& proto, SizedWriter& writer)
{
    printf("i'm in h264Carrier:write.... should not be here!! \n");
    return true;
}

bool H264Carrier::reply(ConnectionState& proto, SizedWriter& writer)
{
    return false;
}

bool H264Carrier::autoCompression() const
{
    return true;
}

bool H264Carrier::sendIndex(ConnectionState& proto, SizedWriter& writer)
{
    return true;
}

bool H264Carrier::expectIndex(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::sendAck(ConnectionState& proto)
{
    return true;
}

bool H264Carrier::expectAck(ConnectionState& proto)
{
    return true;
}

ConstString H264Carrier::getBootstrapCarrierName()
{
    return "";
}

yarp::os::Face* H264Carrier::createFace(void)
{
    return new yarp::os::impl::FakeFace();
}

