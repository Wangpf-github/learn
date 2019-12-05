/*
 * demuxer.c
 * 执行过程中创建一个pipeline
 * 
 */

#include <gst/gst.h>

typedef struct _CustomData
{
	GstElement *pipeline;
	GstElement *source;
	GstElement *convert;
	GstElement *sink;
}CustomData;

static void pad_added_handler(GstElement *src, GstPad *pad, CustomData *data);

int main(int argc, char *argv[])
{
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	gts_init(&argc, &argv);

	/* 创建元件 */
	data.source = gst_element_factory_make("uridecodebin", "source");
	data.convert = gst_element_factory_make("audioconvert", "convert");
	data.sink = gst_element_factory_make("autoaudiosink", "sink");

	/* 创建新的pipeline */
	data.pipeline = gst_pipeline_new("test-pipeline");

	if (!data.pipeline || !data.source || !data.convert || !data.sink) 
	{
		g_printerr ("Not all elements could be created.\n");
		return -1;
	}
	/* 创建了pipeline,但是没有进行链接，稍后在执行过程中进行链接 */

	gst_bin_add_many(GST_BIN(data.pipeline), data.source, data.convert,data.sink, NULL);

	if(!gst_element_link(data.convert, data.sink))
	{
		g_printerr("Elements could not be linked.\n");
		gst_ibject_unref(data.pipeline);
		return -1;
	}

	g_object_set(data.source, "uri", "http://docs.gstreamer.com/media/sintel_trailer-480p.webm", NULL);

	g_signal_connect(data.source, "pad-added", G_CALLBACK(pad_added_handler), &data);

	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if(ret == GTS_CHANGE_FAILURE)
	{
		g_printerr("Unable to set pipeline to the playing state\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* 建立总线 */
	bus = gst_element_get_bus(data.pipeline);

	do
	{
		msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_EOS,
				GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

		/* 解析返回消息 */
		if(msg != NULL)
		{
			GError *err;
			gchar *debug_info;

			switch (GST_MESSAGE_TYPE (msg)) {
			case GST_MESSAGE_ERROR:
				gst_message_parse_error (msg, &err, &debug_info);
				g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
				g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
				g_clear_error (&err);
				g_free (debug_info);
				terminate = TRUE;
				break;
			case GST_MESSAGE_EOS:
				g_print ("End-Of-Stream reached.\n");
				terminate = TRUE;
				break;
			case GST_MESSAGE_STATE_CHANGED:
				/* We are only interested in state-changed messages from the pipeline */
				if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
					GstState old_state, new_state, pending_state;
					gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
					g_print ("Pipeline state changed from %s to %s:\n",
							gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
				}
				break;
			default:
				/* We should not reach here */
				g_printerr ("Unexpected message received.\n");
				break;
			}
			gst_message_unref(msg);
		}
	}while(!terminate);

	gst_object_unref (bus);
	gst_element_set_state (data.pipeline, GST_STATE_NULL);
	gst_object_unref (data.pipeline);
	return 0;
}

static void pad_added_handler(GstElement *src, GstPad *pad, CustomData *data)
{
	GstPad *sink_pad = gst_element_get_static_pad(data->convert, "sink");
	GstPadLinkReturn ret;
	GstCaps *new_pad_caps = NULL;
	GstStructure *new_pad_struct = NULL;
	const gchar *new_pad_type = NULL;

	g_print("Received new pad '%s' from '%s':\n", GST_PAD_NAME (new_pad), GST_ELEMENT_NAME (src));

	if(gst_pad_is_linked(sink_pad))
	{
		g_print("We are already linked, Ignoring.\n");
		goto exit;
	}

	new_pad_caps = gst_pad_get_caps(new_pad);
	new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
	new_pad_type = gst_structure_get_name(new_pad_struct);

}






