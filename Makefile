IDIR =-I /usr/include/opencv \
	-I /home/dmitry/VTK-6.2.0/Common/Core \
	-I /home/dmitry/VTK-6.2.0/IO/Image \
	-I /home/dmitry/VTK-6.2.0/Common/ExecutionModel \
	-I /home/dmitry/VTK-6.2.0/Common/DataModel \
	-I /usr/include/boost

CC=gcc
CFLAGS=$(IDIR) -std=c++11 -O3 -march=corei7

ODIR=/bin
LDIR =-L/usr/lib/x86_64-linux-gnu \
	 -L/usr/lib/ \
	 -L/home/dmitry/VTK-6.2.0/lib \
	 -L/lib/x86_64-linux-gnu/

target=segmentation

LIBS=-Bstatic -lboost_system -lboost_date_time -lboost_filesystem \
	-lofstd -ldcmdata\
	-lopencv_core \
	-lopencv_imgproc \
	-lvtkCommonExecutionModel-6.2 \
	-lvtkCommonDataModel-6.2 \
	-lvtkCommonCore-6.2 \
	-lvtkIOImage-6.2 \
	-lvtkCommonMisc-6.2 \
	-lvtkCommonMath-6.2 \
	-lvtkCommonSystem-6.2 \
	-lvtkCommonTransforms-6.2 \
	-lvtksys-6.2 \
	-lvtkjpeg-6.2 \
	-lvtkpng-6.2 \
	-lvtktiff-6.2 \
	-lvtkmetaio-6.2 \
	-lvtkDICOMParser-6.2 \
	-lvtkzlib-6.2 \
	-lpthread \
	-lm -lstdc++ \

_DEPS =Ensemble.h \
	Ensemble3dExtension.h \
	DICOMin.h \
	point_3d.h \
	RegionGrow.h

DEPS =$(patsubst %,%,$(_DEPS))

_OBJ = Ensemble.o \
	Ensemble3dExtension.o \
	DICOMin.o \
	point_3d.o \
	RegionGrow.o \
	main.o

OBJ=$(patsubst %,%,$(_OBJ))

all: $(target) 
	

$(target): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDIR) $(LIBS)
	

.PHONY: clean

Ensemble.o:
	$(CC) $(CFLAGS) -c -o Ensemble.o Ensemble.cpp
	

Ensemble3dExtension.o:
	$(CC) $(CFLAGS) -c -o Ensemble3dExtension.o Ensemble3dExtension.cpp

DICOMin.o:
	$(CC) $(CFLAGS) -c -o DICOMin.o DICOMin.cpp

point_3d.o:
	$(CC) $(CFLAGS) -c -o point_3d.o point_3d.cpp

RegionGrow.o:
	$(CC) $(CFLAGS) -c -o RegionGrow.o RegionGrow.cpp

main.o:
	$(CC) $(CFLAGS) -c -o main.o main.cpp

#%.o: %.cpp $(DEPS)
#	$(CC) -c -o $@ $< $(IDIR)

remove:
	rm -f *.o *~ core *~ $(target)

clean: FORCE
	rm -f *.o *~ core *~ 

FORCE:
#rm -f *.o *~ core $(INCDIR)/*~ 
