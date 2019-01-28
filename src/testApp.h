#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxBox2d.h"
#include "player.h"
#include "checkPoint.h"

class testApp : public ofBaseApp
{
	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void contactStart(ofxBox2dContactArgs &e);
        void contactEnd(ofxBox2dContactArgs &e);

        ofVideoGrabber 		    vidGrabber;
        ofxCvColorImage		    colorImg, hsbImg;
        ofxCvGrayscaleImage     grayImage, hue, sat, bri, filtered;
        ofxCvContourFinder 	    contourFinder;

        int                     findHue;
		int 				    threshold;
		int                     minSize;
		int                     maxSize;
		int                     raceMin;
		int                     raceSec;
		int                     elapsedSec;

		bool                    reloadBlobs;
		bool                    drawBlobs;
		bool                    showInfo;
		bool                    showCam;
		bool                    startRace;
		bool                    racing;
		bool                    setStartLine;
		bool                    rotatingStart;

		player                  p1;
		player                  p2;

		ofxBox2d			    box2d;
		vector<ofxBox2dPolygon> triangles;
		vector<checkPoint>      checkPoints;

		ofImage                 tarmac;
		vector<ofImage>         raceLights;

		ofSoundPlayer           sndLoop;
		ofSoundPlayer           sndBeep;
};

