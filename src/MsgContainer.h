/*
MsgContainer.h
FifoContainer library v0.1 - 2016-6-30
Copyright (c) 2016 Thales Services. All rights reserved.
*/
#ifndef MsgContainer_h
#define MsgContainer_h

#include <arduino.h>

#define CONTAINER_SIZE 		4 // container size in byte
#define CONTAINER_NUMBER 	5 // number of container in FIFO queue

/*
 Definition of a container
*/
typedef struct {
	byte content[CONTAINER_SIZE]; 	// Container size
	int usedNb; 					// number of byte in data that contain relevant data
	boolean free;					// is the container free <=> usedNb = 0
} T_S_Container;

/*
* circular container : act like a FIFO
*/
class MsgContainer
{
public:
	MsgContainer();
	
	boolean isFull();
	boolean isEmpty();
	
	int containerSize() {return CONTAINER_SIZE;};
	
	byte * currentContainerContent();
	int currentContainerSize(); // current container used size
	void freeCurrentContainer();
	
	//T_S_Container * getWorkMsg();
	//void releaseWorkMsg();

	boolean add(char * c, int size);
private:
	T_S_Container _container[CONTAINER_NUMBER];
	int _freeIdx;
	int _currentIdx;
	
	int getFreeIdx(); // index of the first free data
	int updateCurrentIdx(); // index of the working data
};

#endif