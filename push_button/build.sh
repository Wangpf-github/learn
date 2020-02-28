# !/bin/sh

export PATH=/home/wang/hi3559av_buitroot/buildroot-env/output/host/bin:$PATH
export PKG_CONFIG_PATH=/home/linux/git/buildroot-env/output/host/lib/pkgconfig/:$PATH
aarch64-buildroot-linux-gnu-gcc push_button.c button_test.c -o button_test -lpthread `pkg-config --cflags --libs glib-2.0 gobject-2.0 libgpiod` -I/home/wang/hi3559av_buitroot/buildroot-env/output/build/libgpiod-1.4.1/include -I/home/wang/hi3559av_buitroot/buildroot-env/output/build/libglib2-2.60.7/glib
