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

#include "H264Decoder.h"

#include <gst/gst.h>
#include <glib.h>

#include <gst/app/gstappsink.h>
#include <stdio.h>
#include <string.h>

#include <yarp/os/Log.h>

using namespace yarp::sig;
using namespace yarp::os;



//-------------------------------------------------------------------
//---------------  CALLBACK FUNCTIONS -------------------------------
//-------------------------------------------------------------------

static GstBusSyncReply bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
    GstElement *pipeline = (GstElement *) data;

    switch (GST_MESSAGE_TYPE (msg))
    {

        case GST_MESSAGE_EOS:
        {
            g_print ("End of stream\n");
            gst_element_set_state (pipeline, GST_STATE_NULL);
            // g_main_loop_quit (loop);
            break;
        }

        case GST_MESSAGE_ERROR:
        {
            gchar  *debug;
            GError *error;

            gst_message_parse_error (msg, &error, &debug);
            g_free (debug);

            g_printerr ("Error: %s\n", error->message);
            g_error_free (error);

            gst_element_set_state (pipeline, GST_STATE_NULL);
            break;
        }
        default:
        break;
    }

  return GST_BUS_PASS;
}


static gboolean link_videosrc2rtpdepay(GstElement *e1, GstElement *e2)
{
    gboolean link_ok;
    GstCaps *caps;


// "application/x-rtp, media=(string)video, clock-rate=(int)90000, encoding-name=(string)H264, payload=(int)96, a-framerate=(string)30"
    caps = gst_caps_new_simple("application/x-rtp",
                                "media", G_TYPE_STRING, "video",
                                "clock-rate", G_TYPE_INT, 90000,
                                "encoding-name", G_TYPE_STRING, "H264",
                                "payload", G_TYPE_INT, 96,
                                "a-framerate", G_TYPE_STRING, "30",
                                NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);
    if(!link_ok)
    {
        g_print("failed link videosrc2convert with caps!!\n");
    }
    else
    {
        g_print("link videosrc2convert with caps OK!!!!!!\n");
    }

    return (link_ok);
}



static gboolean link_convert2next(GstElement *e1, GstElement *e2)
{
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple("video/x-raw",
                               "format", G_TYPE_STRING, "RGB",
                              NULL);


    link_ok = gst_element_link_filtered(e1, e2, caps);
    if(!link_ok)
    {
        g_print("failed  link_convert2next with caps!!\n");
    }
    else
    {
        g_print("link_convert2next with caps OK!!!!!!\n");
    }

    return (link_ok);
}


GstFlowReturn new_sample(GstAppSink *appsink, gpointer user_data)
{
    ImageOf<PixelRgb> *new_frame = (ImageOf<PixelRgb>*)user_data;

    GstSample *sample = NULL;
    g_signal_emit_by_name (appsink, "pull-sample", &sample, NULL);
    if(!sample)
    {
        g_print("Could not take a sample!\n");
        return GST_FLOW_OK;
    }

    GstCaps *caps = gst_sample_get_caps (sample);
    if(!caps)
    {
        g_print("Could not get caps of sample!\n");
        return GST_FLOW_ERROR;
    }
    GstStructure *struc = gst_caps_get_structure(caps, 0);
    if(!struc)
    {
        g_print("Could not get struct of caps!\n");
        return GST_FLOW_ERROR;
    }
    gint width, height;
    gboolean res;
    res = gst_structure_get_int(struc, "width", &width);
    if(!res)
    {
        g_print("Could not get width!\n");
        return GST_FLOW_ERROR;
    }

    res = gst_structure_get_int(struc, "height", &height);
    if(!res)
    {
        g_print("Could not get height!\n");
        return GST_FLOW_ERROR;
    }
    //g_print("Image has size %d x %d", width, height);

    GstBuffer *buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if(!gst_buffer_map(buffer, &map, GST_MAP_READ))
    {
        g_print("Could not get map!\n");
        return GST_FLOW_ERROR;
    }
    //HERE I GET MY IMAGE!!!!
    //DO SOMETHING...
    //ImageOf<PixelRgb> &yframebuff = yarp_stuff_ptr->yport_ptr->prepare();
    new_frame->resize(width, height);

    unsigned char *ydata_ptr = new_frame->getRawImage();
    memcpy(ydata_ptr, map.data, width*height*3);

    gst_buffer_unmap(buffer, &map);

    gst_sample_unref(sample);

    return GST_FLOW_OK;

}







//----------------------------------------------------------------------








class H264DecoderHelper
{
public:
    GMainLoop *loop;

    GstElement *pipeline;
    GstElement *source;
    GstElement *sink;
    GstElement *rtpDepay;
    GstElement *parser;
    GstElement *convert;
    GstElement *decoder;

    GstBus *bus; //maybe can be moved in function where i use it
    guint bus_watch_id;

    ImageOf<PixelRgb> myframe;

    H264DecoderHelper(){;}
    ~H264DecoderHelper(){;}

    bool istantiateElements(void)
    {
        gst_init(NULL, NULL);
        pipeline = gst_pipeline_new ("video-player");
        source   = gst_element_factory_make ("udpsrc",       "video-source");
        rtpDepay = gst_element_factory_make ("rtph264depay", "rtp-depay");
        parser   = gst_element_factory_make ("h264parse",    "parser");
        decoder  = gst_element_factory_make ("avdec_h264",   "decoder");
        convert  = gst_element_factory_make ("videoconvert", "convert"); //because use RGB space
        sink     = gst_element_factory_make ("appsink",      "video-output");

        if (!pipeline || !source || !rtpDepay || !parser || !decoder || !convert || !sink)
        {
            g_printerr ("One element could not be created. Exiting.\n");
            return false;
        }
        return true;
    }

    bool configureElements(gint portnumber) //maybe i can make callbak configurable in the future.....
    {
        // 1) configure source port
        g_print("try configure source port with %d.... \n", portnumber);
        g_object_set(source, "port", portnumber/*3111*/, NULL);

        // 2) configure callback on new frame
        g_print("try configure appsink.... \n");
        //I decided to use callbaxk mechanism because it should have less overhead
        g_object_set( sink, "emit-signals", false, NULL );

        GstAppSinkCallbacks cbs; // Does this need to be kept alive?

        // Set Video Sink callback methods
        cbs.eos = NULL;
        cbs.new_preroll = NULL;
        cbs.new_sample = &new_sample;
        gst_app_sink_set_callbacks( GST_APP_SINK( sink ), &cbs, &myframe, NULL );

  /*      //3) add watch ( a message handler)
        bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
        //bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
        gst_object_unref (bus);

        gst_bus_set_sync_handler(bus, bus_call, pipeline, NULL);
        gst_object_unref (bus);
    */
        return true;

    }

    bool linkElements(void)
    {

        g_print("try to add elements to pipeline..... \n");
        /* we add all elements into the pipeline */
        gst_bin_add_many (GST_BIN (pipeline),
                        source, rtpDepay, parser, decoder, convert, sink, NULL);

        g_print("Elements were been added to pipeline!");


        /* autovideosrc ! "video/x-raw, width=640, height=480, format=(string)I420" ! videoconvert ! 'video/x-raw, format=(string)RGB'  ! yarpdevice ! glimagesink */
        g_print("try to link_videosrc2convert..... \n");
        gboolean result = link_videosrc2rtpdepay(source, rtpDepay);
        if(!result)
        {
            return false;
        }

        g_print("try to link_convert2next..... \n");
        result = link_convert2next(convert, sink);
        if(!result)
            {
                return false;
            }
        g_print("try to link all other elements..... \n");
        gst_element_link_many(rtpDepay, parser, decoder, convert, NULL);

        return true;
    }


};



#define GET_HELPER(x) (*((H264DecoderHelper*)(x)))

H264Decoder::H264Decoder()
{
    sysResource = new H264DecoderHelper();
    yAssert(sysResource!=NULL);

}

bool H264Decoder::init(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    if(!helper.istantiateElements())
    {
        printf("Error in istantiateElements\n");
        return false;
    }

    if(!helper.configureElements(3111))
    {
        printf("Error in configureElements\n");
        return false;
    }

    if(!helper.linkElements())
    {
        printf("Error in linkElements\n");
        return false;
    }

    printf("gstreamer init ok");
    return true;

}


bool H264Decoder::start()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_PLAYING);
    return true;

}

bool H264Decoder::stop()
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    gst_element_set_state (helper.pipeline, GST_STATE_NULL);
    gst_bus_set_sync_handler(gst_pipeline_get_bus (GST_PIPELINE (helper.pipeline)), nullptr, nullptr, nullptr);
    g_print ("Deleting pipeline\n");
    gst_object_unref (GST_OBJECT (helper.pipeline));
    return true;
}

H264Decoder::~H264Decoder()
{
    stop();
    delete &GET_HELPER(sysResource);


}

ImageOf<PixelRgb> & H264Decoder::getLastFrame(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return helper.myframe;
}

int H264Decoder::getLastFrameSize(void)
{
    H264DecoderHelper &helper = GET_HELPER(sysResource);
    return (helper.myframe.width() * helper.myframe.height() * 3);
}
