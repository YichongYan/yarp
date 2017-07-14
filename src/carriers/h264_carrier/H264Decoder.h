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

#ifndef H264DECODER_INC
#define H264DECODER_INC

#include <yarp/sig/Image.h>

namespace yarp {
    namespace os {
        class H264Decoder;
    }
}

class yarp::os::H264Decoder
{
private:

    void *sysResource;

public:
    H264Decoder();
    ~H264Decoder();
    bool init(void);
    bool start();
    bool stop();
    yarp::sig::ImageOf<yarp::sig::PixelRgb> & getLastFrame(void);
    int getLastFrameSize(void);
};

#endif
