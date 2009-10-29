#~sekiatu/work/SR4000/trunk/src/Makefile

OPTION = -Wall -g -Wextra -pedantic -O2
FLYOPTION = -Wall -Wextra -pedantic -O2
CC = g++
LIB = -lcxcore -lcv -lhighgui -lcvaux -lmesasr  -lpthread -lBiclops -lPMD -lUtils
OBJS = cameraImages.o funcPointing.o faceDetector.o imageClassifier.o regionDetector.o regionTracker.o pointProjector.o sharedMemory.o connection.o projectorConnection.o coordinateShifter.o line.o panTiltUnit.o templateMatching.o
INCD = ../include/
BIND = ../bin/
SRCS = $(OBJS:.o=.c)

Biclops_INCD = ../../../Biclops/Biclops/Software/libBiclops/include/
PMD_INCD = ../../../Biclops/motion/libPMD/include/
Utils_INCD = ../../../Biclops/motion/libUtils/include/

all : $(BIND)pointEstimator $(BIND)calibDataCollector $(BIND)faceTrack

clean:
	- @ rm -rf $(OBJS)


##################
# apps
##################

# pointEstimator
$(BIND)pointEstimator: pointEstimator.cpp $(OBJS)
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -o $@ pointEstimator.cpp $(OBJS)

# calibDataCollector
$(BIND)calibDataCollector: calibDataCollector.cpp $(OBJS)
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -o $@ calibDataCollector.cpp $(OBJS)

# faceTrack
$(BIND)faceTrack: faceTrack.cpp $(OBJS)
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -I$(Biclops_INCD) -I$(PMD_INCD) -o $@ faceTrack.cpp $(OBJS)

##################
# object files
##################

# cameraImages.o
cameraImages.o: cameraImages2.cpp $(INCD)pointing2.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c cameraImages2.cpp
	mv cameraImages2.o cameraImage.o

# funcPointing.o
funcPointing.o: funcPointing.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c funcPointing.cpp

# faceDetector.o
faceDetector.o: faceDetector.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c faceDetector.cpp

# imageClassifier.o
imageClassifier.o: imageClassifier.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c imageClassifier.cpp

# regionDetector.o
regionDetector.o: regionDetector.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c regionDetector.cpp 

# regionTracker.o
regionTracker.o: regionTracker.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c regionTracker.cpp

# pointProjector.o
pointProjector.o: pointProjector.cpp $(INCD)pointProjector.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c pointProjector.cpp

# sharedMemory.o
sharedMemory.o: sharedMemory.cpp $(INCD)sharedMemory.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c sharedMemory.cpp

# connection.o
connection.o: connection.cpp $(INCD)sharedMemory.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c connection.cpp

# projectorConnection.o
projectorConnection.o: projectorConnection.cpp $(INCD)sharedMemory.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c projectorConnection.cpp

# coordinateShifter.o
coordinateShifter.o: coordinateShifter.cpp $(INCD)coordinateShifter.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c coordinateShifter.cpp

# line.o
line.o: line.cpp $(INCD)pointing.h
	$(CC)  $(OPTION) -I$(INCD) $(LIB) -c line.cpp

# panTiltUnit.o
panTiltUnit.o: panTiltUnit.cpp $(INCD)panTiltUnit.h
	$(CC) $(OPTION) -I$(PMD_INCD) -I$(Utils_INCD) -I$(INCD) $(LIB) -c panTiltUnit.cpp

# templateMatching.o
templateMatching.o: templateMatching.cpp $(INCD)templateMatching.h
	$(CC) $(OPTION) -I$(INCD) $(LIB) -c templateMatching.cpp

##################
# for flymake
##################
check-syntax:
	$(CC) $(OPTION) -fsyntax-only 