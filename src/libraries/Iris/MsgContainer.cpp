#include "MsgContainer.h"

//******************************************************************************
//* Constructors
//******************************************************************************
MsgContainer::MsgContainer ()
{
	_freeIdx = 0;
	_currentIdx = 0;
}
//******************************************************************************
//* Public Methods
//******************************************************************************
boolean MsgContainer::isEmpty()
{
	return ( updateCurrentIdx() < 0); // if no valid working index, the container is empty
}

boolean MsgContainer::isFull()
{
	boolean ret = true;
	if ( this->getFreeIdx() >= 0 ) {
		ret = false; // at least one element is free
	}
	return ret;
}

int MsgContainer::getFreeIdx()
{
	int idx = 0;
	for (int i=0; i < CONTAINER_NUMBER; i++) {
		idx = (_freeIdx + i) % CONTAINER_NUMBER;
		if ( _container[idx].free ) {
			_freeIdx = idx;
			return idx; // at least one element is free
		}
	}

	return -1; // no element free found, the container is full
}

int MsgContainer::updateCurrentIdx()
{
	//for debug : print how many used elts
	/*int nbUsed = 0;
	for (int j=0; j < CONTAINER_NUMBER; j++) {
		if ( ! _container[j].free) {
			nbUsed++;
		}
	}
	
	if (nbUsed > 0) {
		Serial.print(" nbElt used : ");
		Serial.println(nbUsed);
	}*/
	// end debug
	
	int idx = 0;
	for (int i=0; i < CONTAINER_NUMBER; i++) {
		idx = (_currentIdx + i) % CONTAINER_NUMBER;
		if ( ! _container[idx].free ) {
			_currentIdx = idx;
			return idx; // at least one element is NOT free
		}
	}
	
	return -1; // all element are free found
}

/*
	add content to the first free container
*/
boolean MsgContainer::add(char * c, int size) // TODO remove containerIdx
{
	boolean ret = false;
	if ( size <= CONTAINER_SIZE ) { // cannot add message whose size is over the CONTAINER_SIZE
				
		int containerIdx = getFreeIdx();
		if ( containerIdx >= 0 ) { // remains free space
			for (int i = 0; i < size; i++) {
				_container[containerIdx].content[i] = c[i];
			}
			_container[containerIdx].usedNb = size;
			_container[containerIdx].free = false;
			updateCurrentIdx(); // will update _currentIdx if necessary
			ret = true;
		}
	
	}
	return ret;
}
	
byte * MsgContainer::currentContainerContent()
{
	if ( _currentIdx >= 0 ) {
		return _container[_currentIdx].content;
	}
	else {
		return NULL;
	}
}

int MsgContainer::currentContainerSize()
{
	if ( _currentIdx >= 0 ) {
		return _container[_currentIdx].usedNb;
	}
	else {
		return -1;
	}
}


void MsgContainer::freeCurrentContainer()
{
	_container[_currentIdx].free = true;
	updateCurrentIdx(); // will update _currentIdx
}