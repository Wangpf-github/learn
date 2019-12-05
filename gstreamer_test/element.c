/*
 * element.c
 * 一个自动创建视频文件的element，在管道中传给视频接受element并播放
 * gcc element.c -o element-Wall $(pkg-config --cflags --libs gstreamer-1.0)
 */

#include <gst/gst.h>

int main(int argc, char *argv[])
{
	GstElement *pipeline, *source, *sink;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	gst_init(&argc, &argv);

	/* 创建element，第一个参数是元件类型，第二个参数是元件名称 */
	source = gst_element_factory_make("videotestsrc", "source");
	sink = gst_element_factory_make("autovideosink", "sink");

	pipeline = gst_pipeline_new("test-pipeline");

	if(!pipeline || !source || !sink)
	{
		g_printerr("Not all elements could be created.\n");
		return -1;
	}

	/* 创建pipeline,把source和sink加入到pipeline中 */
	gst_bin_add_many(GST_BIN(pipeline), source, sink, NULL);
	if(gst_element_link(source, sink) != TRUE)
	{
		g_printerr("Element could not be linked.\n");
		gst_object_unref(pipeline);   //解除对pipeline的引用，释放资源
		return -1;
	}
	
	/* 修改source的属性:修改pattern模式，可以查找videotestsrc属性查看 */
	g_object_set(source, "pattern", 18,"motion", 0,"animation-mode", 2, NULL);
//	g_object_set(source, "motion", 0, NULL);
//	g_object_set(source, "animation-mode", 2, NULL);

	ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
	if(ret == GST_STATE_CHANGE_FAILURE)
	{
		g_printerr("Unable to set the pipeline to the palying state.\n");
		gst_object_unref(pipeline);
		return -1;
	}

	/* 等待出现错误或者播放结束 */
	bus = gst_element_get_bus(pipeline);
	/* 本函数的等待时间取决于第二个参数 */
	msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	/* 解析发生的错误 */
	if(msg != NULL)
	{
		GError *err;
		gchar *debug_info;

		switch(GST_MESSAGE_TYPE(msg))
		{
			case GST_MESSAGE_ERROR:
				gst_message_parse_error(msg, &err, &debug_info);
				g_printerr("...");
				g_printerr("...");
				g_clear_error(&err);
				g_free(debug_info);
				break;
			case GST_MESSAGE_EOS:
				g_print("End-Of-Stream reached.\n");
				break;
			default:
				g_printerr("Unexpected message received.\n");
				break;
		}
		gst_message_unref(msg);
	}

	/* 释放资源 */
	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}
