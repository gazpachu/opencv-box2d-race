#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

class checkPoint : public ofxBox2dCircle
{
	public:

		void init( int _id, int _x, int _y );
		void create( b2World* b2world );
		void update();
		void paint();

        bool created;
        int id, radius;
        ofPoint pos;

        b2World* b2world;
};

