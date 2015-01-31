all: tcp_iopwm tcp_pwm
#webcam_server webcam_shower
#CC=gcc
#LD=ld
CC=arm-linux-gcc
#LD=arm-linux-ld
#CXXFLAGS=	-lm -Llib -lnmea 
#CXXFLAGS=	-c -g -O0 -fPIC -I/opt/webcam/ffmpeg/include -I/usr/include -L/opt/webcam/ffmpeg/lib
OBJS_SERVER=	tcp_iopwm.c
OBJS_CLIENT= 	tcp_pwm.c
LIBS_SERVER=	-lpthread 
LIBS_CLIENT=    -lpthread 

INCS_CLIENT = -I./nmealib/include -L./nmealib/lib
#.c.o:
#	$(CC)  $<

tcp_iopwm: $(OBJS_SERVER)
	$(CC)  $^   $(LIBS_SERVER)  -o $@

tcp_pwm: $(OBJS_CLIENT)
#	$(CC)  $^    $(LIBS_CLIENT)  -o $@

clean:
	rm -f *.o
	rm -f tcp_iopwm
	rm -f tcp_pwm
