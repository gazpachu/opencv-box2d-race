#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

enum direction { P_UP = 0, P_DOWN, P_LEFT, P_RIGHT };

class player : public ofxBox2dRect
{
	public:

		void init( b2World* b2world, int id, int _x, int _y );
		void update();
		void paint();

		void setStatus( int st );
		void setCrash();

		void keyPressed(direction dir);
		void keyReleased(direction dir);

		vector<ofImage> sprite;

        int id;
		int status;
		int height;
		int width;
		int laps;
		int elapsedTime;
		int recoveryTime;
		int lastCheckPoint;

		ofPoint pos;

        float vel_x;
        float vel_y;
        float angle;
        float speed_now;

        bool accelerating;
        bool backwards;
        bool turningLeft;
        bool turningRight;
        bool crashed;

        b2World* b2world;

        ofSoundPlayer sndEngine;
};

