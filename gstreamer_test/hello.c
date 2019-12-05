/*
 * gcc -Wall $(pkg-config --cflags --libs gstreamer-1.0) hello.c -o helloworld
 */

#include <gst/gst.h>
#include <stdio.h>
/*
 * Global objects are usually a bad thing. For the purpose of this
 * example, we will use them, however.
 */

GstElement *pipeline, *source, *parser, *decoder, *conv, *sink;

static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
	GMainLoop *loop = data;

	switch(GST_MESSAGE_TYPE(msg))
	{
		case GST_MESSAGE_EOS:
			g_print("End-of-stream\n");
			g_main_loop_quit(loop);
			break;
		case GST_MESSAGE_ERROR:
			{
				gchar *debug;
				GError *err;

				gst_message_parse_error(msg, &err, &debug);
				g_free(debug);

				g_print("Error:%s\n",err->message);
				g_error_free(err);

				g_main_loop_quit(loop);
				break;
			}
		default:
			break;
	}
	return TRUE;
}
//    {{{1
static void new_pad(GstElement *element, GstPad *pad, gpointer data)
{
	GstPad *sinkpad;
	/* We can now link this pad with the audio decoder */
	g_print ("Dynamic pad created, linking parser/decoder\n");
	
	sinkpad = gst_element_add_pad(decoder, "sink");
	gst_pad_link(pad, sinkpad);

	gst_object_unref(sinkpad);
}
//   {{{1
int main(int argc, char *argv[])
{
	GMainLoop *loop;
	GstBus *bus;

	gst_init(&argc, &argv);
	loop = g_main_loop_new(NULL, FALSE);

	if(argc != 2)
	{
		g_print("Usage:%s<Ogg/Vorbis filename>\n", argv[0]);
		return -1;
	}

	/* create elements */
	pipeline = gst_pipeline_new("audio-player");
	source = gst_element_factory_make("filesrc", "file-source");
	parser = gst_element_factory_make("oggdemux", "ogg-parser"); //解析器
	decoder = gst_element_factory_make("vorbisdec", "vorbis-decoder"); //解码器
	conv = gst_element_factory_make("audioconvert", "converter"); //音频转换器
	sink = gst_element_factory_make("alsasink", "aqlsa-output"); //播放器
	if(!pipeline || !source || !parser || !decoder || !conv || !sink)
	{
		g_print("One element can not be created\n");
		return -1;
	}

	/* set filename property on the file source. Also add a message
	* handler. */
	g_object_set(G_OBJECT(source), "location", argv[1], NULL);

	bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
	gst_bus_add_watch(bus, bus_call, loop);
	gst_object_unref(bus);

	/* put all elements in a bin */
	gst_bin_add_many(GST_BIN(pipeline), source, parser, decoder, conv, sink, NULL);

	/* link together - note that we cannot link the parser and
	 * decoder yet, becuse the parser uses dynamic pads. For that,
	 * we set a pad-added signal handler. */
	gst_element_link(source, parser);
	gst_element_link_many(decoder, conv, sink, NULL);
	g_signal_connect(parser, "pad-added", G_CALLBACK(new_pad), NULL);

	/* Now set to playing and iterate. */
	g_print("Setting to PLAYING\n");
	gst_element_set_state(pipeline, GST_STATE_PLAYING);
	g_print("Running\n");
	g_main_loop_run(loop);

	/* clean up nicely */
	g_print ("Returned, stopping playback\n");
	gst_element_set_state (pipeline, GST_STATE_NULL);
	g_print ("Deleting pipeline\n");
	gst_object_unref (GST_OBJECT (pipeline));

	return 0;
}










