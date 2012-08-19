# add custom variables to this file

# OF_ROOT allows to move projects outside apps/* just set this variable to the
# absoulte path to the OF root folder

OF_ROOT = ../OpenFrameworks/


# USER_CFLAGS allows to pass custom flags to the compiler
# for example search paths like:
# USER_CFLAGS = -I src/objects

USER_CFLAGS = -I/usr/include/gstreamer-0.10/ -I/usr/include/libxml2/ -I/usr/lib/ \
-I /usr/lib/jack/ \
-I ../OpenFrameworks/libs/ \
-I ../OpenFrameworks/libs/openFrameworks/
#-I../OpenFrameworks/libs/openFrameworksCompiled/lib/linux/ \
#-I/home/akzwar/Downloads/of_preRelease_v007_linux/export/linux/libs \
#-I/home/akzwar/Downloads/of_preRelease_v007_linux/


# USER_LDFLAGS allows to pass custom flags to the linker
# for example libraries like:
# USER_LD_FLAGS = libs/libawesomelib.a
USER_LDFLAGS = /usr/lib/libz.so ../OpenFrameworks/libs/openFrameworksCompiled/lib/linux/libopenFrameworks.a \
               ../OpenFrameworks/libs/openFrameworksCompiled/lib/linux/libopenFrameworksDebug.a


# use this to add system libraries for example:
# USER_LIBS = -lpango

USER_LIBS =


# change this to add different compiler optimizations to your project

USER_COMPILER_OPTIMIZATION = -march=native -mtune=native -Os


EXCLUDE_FROM_SOURCE="bin,.xcodeproj,obj,.git"
