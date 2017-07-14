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



extern "C" {
#include <gst/gst.h>
#include <glib.h> //????
}


/*
  work around ends.
 */


#include "H264Carrier.h"

#include <yarp/sig/Image.h>
#include <yarp/sig/ImageNetworkHeader.h>
#include <yarp/os/Name.h>
#include <yarp/os/Bytes.h>



using namespace yarp::os;
using namespace yarp::sig;

#define dbg_printf if (0) printf



bool H264Carrier::write(ConnectionState& proto, SizedWriter& writer)
{
    printf("i'm in h264Carrier:write.... should not be here!! \n");
    /* WireImage rep;
    FlexImage *img = rep.checkForImage(writer);

    if (img==NULL) return false;
    int w = img->width();
    int h = img->height();
    int row_stride = img->getRowSize();
    JOCTET *data = (JOCTET*)img->getRawImage();

    JSAMPROW row_pointer[1];

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    cinfo.client_data = &proto;
    jpeg_create_compress(&cinfo);
    jpeg_net_dest(&cinfo);
    cinfo.image_width = w;
    cinfo.image_height = h;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    //jpeg_set_quality(&cinfo, 85, TRUE);
    dbg_printf("Starting to compress...\n");
    jpeg_start_compress(&cinfo, TRUE);
    if(!envelope.empty()) {
        jpeg_write_marker(&cinfo, JPEG_COM, reinterpret_cast<const JOCTET*>(envelope.c_str()), envelope.length() + 1);
        envelope.clear();
    }
    dbg_printf("Done compressing (height %d)\n", cinfo.image_height);
    while (cinfo.next_scanline < cinfo.image_height) {
        dbg_printf("Writing row %d...\n", cinfo.next_scanline);
        row_pointer[0] = data + cinfo.next_scanline * row_stride;
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
*/
    return true;
}

bool H264Carrier::reply(ConnectionState& proto, SizedWriter& writer) {
    return false;
}


bool H264Carrier::sendHeader(ConnectionState& proto)
{
/*    Name n(proto.getRoute().getCarrierName() + "://test");
    ConstString pathValue = n.getCarrierModifier("path");
    ConstString target = "GET /?action=stream\n\n";
    if (pathValue!="") {
        target = "GET /";
        target += pathValue;
    }
    target += " HTTP/1.1\n";
    Contact host = proto.getRoute().getToContact();
    if (host.getHost()!="") {
        target += "Host: ";
        target += host.getHost();
        target += "\r\n";
    }
    target += "\n";
    Bytes b((char*)target.c_str(),target.length());
    proto.os().write(b);
  */
  return true;
}

bool H264Carrier::autoCompression() const
{
    return true;

    /*#ifdef MJPEG_AUTOCOMPRESS
    return true;
    #else
        return false;
    #endif*/
}

