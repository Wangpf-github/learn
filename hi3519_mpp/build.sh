arm-hisiv500-linux-uclibcgnueabi-gcc -DG_LOG_DOMAIN=\"MPP\" -mcpu=cortex-a17.cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=softfp -c -fpic -std=gnu11 -Wall -Wextra -O2 -Wno-missing-field-initializers -o mpp2.o -isystem /home/linux/Hi3519V101_SDK_V1.0.3.0/mpp_big-little/include/ `pkg-config --cflags glib-2.0` mpp2.c
arm-hisiv500-linux-uclibcgnueabi-gcc -DG_LOG_DOMAIN=\"MPP\" -mcpu=cortex-a17.cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=softfp -c -fpic -std=gnu11 -Wall -Wextra -O2 -Wno-missing-field-initializers -o imx117.o -isystem /home/linux/Hi3519V101_SDK_V1.0.3.0/mpp_big-little/include/ imx117.c
arm-hisiv500-linux-uclibcgnueabi-gcc -DG_LOG_DOMAIN=\"MPP\" -shared -o libmpp2.so -L /home/linux/Hi3519V101_SDK_V1.0.3.0/mpp_big-little/lib mpp2.o imx117.o -Wl,-Bstatic -lmpi -live -lmd -lisp -l_hiae -l_hiawb -l_hiaf -l_hidefog -lVoiceEngine -ldnvqe -lupvqe -Wl,-Bdynamic -lm -lrt -ldl -lpthread