# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall
LFLAGS  = -lws2_32 -lwinmm -lNIDAQmx
#LFLAGS  = -lws2_32 -lwinmm -lphidget21 -lNIDAQmx
IDIR 	= -I"C:\Program Files\Phidgets" -I"C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\include"
LDIR 	= -L"C:\Program Files\Phidgets\x86" -L"C:\Program Files (x86)\National Instruments\Shared\ExternalCompilerSupport\C\lib32\msvc"

# the build target executable:
# TARGET = IMPACTS
# SOURCES = main.cpp qbAPI\src\qbmove_communications.cpp

TARGET2 = MAIN_IMPACTS
SOURCES2 = Main_Impacts_acquisition.cpp qbAPI\src\qbmove_communications.cpp qbAPI\src\cp_communications.cpp

all: $(TARGET) $(TARGET2)

$(TARGET):
	$(CC) $(CFLAGS) $(IDIR) -o $(TARGET) $(SOURCES) $(LDIR) $(LFLAGS)

$(TARGET2):
	$(CC) $(CFLAGS) $(IDIR) -o $(TARGET2) $(SOURCES2) $(LDIR) $(LFLAGS)

clean: $(RM) $(TARGET) $(TARGET2)

