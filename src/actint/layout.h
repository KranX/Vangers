//
// Created by Nikita Prianichnikov on 22.06.2020.
//

#ifndef VANGERS_LAYOUT_H
#define VANGERS_LAYOUT_H
#include <iostream>

const unsigned int   WIDGET_ANCHOR_RIGHT = 0x1;
const unsigned int   WIDGET_ANCHOR_BOTTOM = 0x2;
const unsigned int   WIDGET_ANCHOR_INITIALIZED = 0x10;

template <class T> void layout(T* view, int width, int height){
	unsigned int anchor = view->anchor;

	if(anchor & WIDGET_ANCHOR_INITIALIZED){
		std::cerr<<"Layout for view "<<view<<" is already done"<<std::endl;
		return;
	}

	view->anchor |= WIDGET_ANCHOR_INITIALIZED;

	if(anchor & WIDGET_ANCHOR_RIGHT){
		view->PosX = width - view->PosX - view->SizeX;
	}

	if(anchor & WIDGET_ANCHOR_BOTTOM){
		view->PosY = height - view->PosY - view->SizeY;
	}
}

#endif //VANGERS_LAYOUT_H
