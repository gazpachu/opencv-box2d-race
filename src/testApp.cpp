#include "testApp.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define RACE_SECONDS 180

//--------------------------------------------------------------
void testApp::setup()
{
    // App Setup
    //ofSetVerticalSync(true);
    //ofSetFrameRate(120);
    reloadBlobs = true;
    showCam = true;
    drawBlobs = false;
    showInfo = false;
    startRace = false;
    racing = false;

	threshold = 60;
	minSize = 50;
	maxSize = 15000;
	elapsedSec = 0;
	findHue = 84;

    // Load graphics
	tarmac.loadImage("tarmac_bg.png");

	for( int i = 0; i < 3; i++ )
    {
        ofImage img;
        img.loadImage("race_light_" + ofToString(i) + ".png");
        raceLights.push_back(img);
    }

    // Load sounds
    sndLoop.loadSound("loop.wav");
    sndLoop.setVolume(0.2f);
    sndLoop.setLoop(true);
    sndBeep.loadSound("beep.wav");

    // OpenCV Setup (allocate memory)
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(SCREEN_WIDTH, SCREEN_HEIGHT);
    colorImg.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
    grayImage.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Colour detection
    /*hsbImg.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
	hue.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
	sat.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
	bri.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);
	filtered.allocate(SCREEN_WIDTH, SCREEN_HEIGHT);*/

	// Box2D World Setup
	box2d.init();
	box2d.setGravity(0,0);
	box2d.createBounds();
	//box2d.createGround();
	box2d.setFPS(30.0);
	//box2d.registerGrabbing();

    // Collision listeners
	ofAddListener(box2d.contactStartEvents, this, &testApp::contactStart);
	ofAddListener(box2d.contactEndEvents, this, &testApp::contactEnd);

    // Players setup
	p1.init( box2d.getWorld(), 1, 200, 200 );
	p2.init( box2d.getWorld(), 2, 200, 200 );
}

//--------------------------------------------------------------
void testApp::update()
{
    bool bNewFrame = false;

    vidGrabber.grabFrame();
    bNewFrame = vidGrabber.isFrameNew();

    // Update geometry
	if( bNewFrame )
	{
        colorImg.setFromPixels(vidGrabber.getPixels(), SCREEN_WIDTH, SCREEN_HEIGHT);

        if( reloadBlobs )
        {
            reloadBlobs = false;

            grayImage = colorImg;
            grayImage.threshold(threshold);

            //////////////////////////////////////////////////////////////////////
            // Find track contours based on simple image difference
            //////////////////////////////////////////////////////////////////////
            contourFinder.findContours(grayImage, minSize, maxSize, 200, true);	// find holes

            // Destroy the polygons from the world
            for( int i = 0; i < triangles.size(); i++ )
            {
                triangles[i].destroy();
            }
            triangles.clear();

            // Transform the blobs into polygons
            for (int i = 0; i < contourFinder.nBlobs; i++)
            {
                vector<ofPoint> pts = contourFinder.blobs[i].pts;
                ofxBox2dPolygon poly;

                for( int j = 0; j < pts.size(); j++ )
                {
                    poly.addVertex( pts[j] );
                }

                //poly.setAsEdge(true);
                poly.triangulate(15);
                poly.setPhysics(0,0,0);
                poly.setFixedRotation(true);
                poly.create(box2d.getWorld());
                poly.bodyDef.type = b2_staticBody;
                triangles.push_back(poly);
            }

            //////////////////////////////////////////////////////////////////////
            // Find checkpoints contours based on colour
            //////////////////////////////////////////////////////////////////////

            //convert to hsb and store the three channels as grayscale images
            /*hsbImg = colorImg;
            hsbImg.convertRgbToHsv();
            hsbImg.convertToGrayscalePlanarImages(hue, sat, bri);

            if( findHue != 0 )
            {
                //filter image based on the hue value were looking for
                for( int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++ )
                {
                    filtered.getPixels()[i] = ofInRange( hue.getPixels()[i], findHue - 10, findHue + 10 ) ? 255 : 0;
                }
                filtered.flagImageChanged();

                //run the contour finder on the filtered image to find blobs with a certain hue
                contourFinder.findContours(filtered, minSize, maxSize, 20, false);

                // Destroy the current checkpoints from the world
                for( int i = 0; i < checkpoints.size(); i++ )
                {
                    checkpoints[i].destroy();
                }
                checkpoints.clear();

                // Transform the blobs into circles
                for (int i = 0; i < contourFinder.nBlobs; i++)
                {
                    ofxBox2dCircle circle;

                    circle.setup( box2d.getWorld(), contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y, 50 );
                    circle.bodyDef.type = b2_dynamicBody;
                    circle.bodyDef.active = false;
                    checkpoints.push_back(circle);
                }
            }*/
        }
	}

    // Gameplay logic
    if( startRace )
    {
        if( ((int)ofGetElapsedTimeMillis() / 1000) > elapsedSec )
        {
            if( elapsedSec == 2 )
            {
                startRace = false;
                racing = true;
                elapsedSec = 0;
                raceMin = RACE_SECONDS / 60;
                raceSec = RACE_SECONDS - (raceMin * 60);
                ofResetElapsedTimeCounter();
            }
            else
                elapsedSec++;
        }
    }

    // Update players and world
    p1.update();
    p2.update();

    box2d.update();
}

//--------------------------------------------------------------
void testApp::draw()
{
    ofEnableAlphaBlending();
    ofSetColor( ofColor(255) );

    tarmac.draw(0,0);

    if( showCam )
        colorImg.draw(0,0);

    // Draw OpenCV blobs
    if( drawBlobs )
    {
        for( int i = 0; i < contourFinder.nBlobs; i++ )
        {
            contourFinder.blobs[i].draw(0,0);
        }
    }

    // Draw Track
    for( int i = 0; i < triangles.size(); i++ )
    {
        triangles[i].draw();
    }

    // Draw checkpoints
    /*for( int i = 0; i < checkpoints.size(); i++ )
    {
        checkpoints[i].draw();
    }*/

    // Draw strings
    ofSetHexColor(0xffffff);
    char reportStr[1024];

    if( showInfo )
    {
        sprintf(reportStr, "threshold %i (press: +/-)\nmin size: %i\nmax size: %i\nnum blobs found %i, fps: %f\nTriangles amount: %i\nPos: %f, %f\nSpeed: %f, %f\nAngle: %f", threshold, minSize, maxSize, contourFinder.nBlobs, ofGetFrameRate(), triangles.size(), p1.pos.x, p1.pos.y, p1.vel_x, p1.vel_y, ofRadToDeg(p1.angle));
        ofDrawBitmapString(reportStr, 20, 20);
    }
    else
    {
        if( racing )
        {
            if( ofGetElapsedTimeMillis() / 1000 > elapsedSec )
            {
                raceSec -= 1;
                elapsedSec++;
            }

            if( raceSec == 0 && raceMin > 0 )
            {
                raceMin--;
                raceSec = 59;
            }

            char raceSecTxt[2];
            sprintf(raceSecTxt, "%i", raceSec);

            if( raceSec < 10 )
                sprintf(raceSecTxt, "0%i", raceSec);

            // Check if the race is over
            if( raceSec == 0 && raceMin == 0 )
            {
                racing = false;
            }

            sprintf(reportStr, "Time: %i:%s\nP1 Laps: %i\nP2 Laps: %i\nP1 CheckPoint: %i\nP2 CheckPoint: %i", raceMin, raceSecTxt, p1.laps, p2.laps, p1.lastCheckPoint, p2.lastCheckPoint);
            ofDrawBitmapString(reportStr, 20, 20);
        }
        else
        {
            if( p1.laps != 0 && p1.laps != 0 && elapsedSec > 0 )
            {
                sndLoop.stop();

                if( p1.laps > p2.laps )
                    sprintf(reportStr, "The winner is PLAYER 1!");
                else if( p2.laps > p1.laps )
                    sprintf(reportStr, "The winner is PLAYER 2!");
                else
                    sprintf(reportStr, "There was a draw!");
            }
        }
    }

    // Draw start line and checkpoints
    for( int i = 0; i < checkPoints.size(); i++ )
    {
        checkPoints[i].paint();
    }

    // Reset colors
    ofSetColor( ofColor(255) );

    // Draw players
	p1.paint();
	p2.paint();

	/*hsbImg.draw(640,0);
    hue.draw(0,240);
    sat.draw(320,240);
    bri.draw(640,240);
    filtered.draw(0,480);*/

	if( startRace )
    {
        raceLights[elapsedSec].draw( ofGetWidth() / 2 - raceLights[0].width / 2, ofGetHeight() / 2 - raceLights[0].height / 2 );

        if( elapsedSec == 2 )
            sndLoop.play();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
		case ' ':
			reloadBlobs = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;

        case 'f':
			ofToggleFullscreen();
			break;

        case 'b':
			if( drawBlobs ) drawBlobs = false;
			else drawBlobs = true;
			break;

        case 'm':
			if( showCam ) showCam = false;
			else showCam = true;
			break;

        case 'o':
			maxSize += 100;
			break;
		case 'l':
			maxSize -= 100  ;
			break;

        case 'i':
			minSize++;
			break;
		case 'k':
			minSize --;
			break;

        case 'n':
			if( showInfo ) showInfo = false;
			else showInfo = true;
			break;

        case 'r':
            for( int i = 0; i < checkPoints.size(); i++ )
            {
                checkPoints[i].destroy();
            }
			checkPoints.clear();
			break;
	}

	if( racing )
    {
        switch (key)
        {
            // Player 1
            case OF_KEY_UP: p1.keyPressed(P_UP); break;
            case OF_KEY_DOWN: p1.keyPressed(P_DOWN); break;
            case OF_KEY_LEFT: p1.keyPressed(P_LEFT); break;
            case OF_KEY_RIGHT: p1.keyPressed(P_RIGHT); break;

            // Player 2
            case 'w': p2.keyPressed(P_UP); break;
            case 's': p2.keyPressed(P_DOWN); break;
            case 'a': p2.keyPressed(P_LEFT); break;
            case 'd': p2.keyPressed(P_RIGHT); break;
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
    switch( key )
    {
        // Player 1
        case OF_KEY_UP: p1.keyReleased(P_UP); break;
        case OF_KEY_DOWN: p1.keyReleased(P_DOWN); break;
        case OF_KEY_LEFT: p1.keyReleased(P_LEFT); break;
        case OF_KEY_RIGHT: p1.keyReleased(P_RIGHT); break;

        // Player 2
        case 'w': p2.keyReleased(P_UP); break;
        case 's': p2.keyReleased(P_DOWN); break;
        case 'a': p2.keyReleased(P_LEFT); break;
        case 'd': p2.keyReleased(P_RIGHT); break;
    }
}

//--------------------------------------------------------------
void testApp::contactStart(ofxBox2dContactArgs &e)
{
	if(e.a != NULL && e.b != NULL)
    {
		// if we collide with the ground we do not
		// want to play a sound. this is how you do that
		/*if(e.a->GetType() == b2Shape::e_circle && e.b->GetType() == b2Shape::e_circle)
        {
            e.a->GetBody()->GetUserData()

		}*/
	}
}

//--------------------------------------------------------------
void testApp::contactEnd(ofxBox2dContactArgs &e)
{
	if(e.a != NULL && e.b != NULL)
    {
        player* pa = static_cast<player*>( e.a->GetBody()->GetUserData() );
        player* pb = static_cast<player*>( e.b->GetBody()->GetUserData() );
        checkPoint* cp1 = static_cast<checkPoint*>(e.a->GetBody()->GetUserData());
        checkPoint* cp2 = static_cast<checkPoint*>(e.b->GetBody()->GetUserData());

        if( pa != NULL && !e.b->IsSensor() )
            switch(pa->id)
            {
                case 1: if( !p1.crashed ) p1.setCrash(); break;
                case 2: if( !p2.crashed ) p2.setCrash(); break;
            }

        if( pb != NULL && !e.a->IsSensor() )
            switch(pb->id)
            {
                case 1: if( !p1.crashed ) p1.setCrash(); break;
                case 2: if( !p2.crashed ) p2.setCrash(); break;
            }

        if( e.a->IsSensor() && pb != NULL && cp1 != NULL && (pb->id == 1 || pb->id == 2 ) )
        {
           if( pb->lastCheckPoint == checkPoints.size() - 1 && cp1->id == 0 )
           {
                pb->laps++; // if this is the start line, increase laps
                pb->lastCheckPoint = 0; // reset checkpoints
           }

           if( pb->lastCheckPoint == cp1->id - 1 )
                pb->lastCheckPoint = cp1->id; // update the checkpoint counter in the player
        }

        if( e.b->IsSensor() && pa != NULL && cp2 != NULL && (pa->id == 1 || pa->id == 2 ) )
        {
            if( pa->lastCheckPoint == checkPoints.size() - 1 && cp2->id == 0 )
            {
                pa->laps++; // if this is the start line, increase laps
                pa->lastCheckPoint = 0; // reset checkpoints
            }

            if( pa->lastCheckPoint == cp2->id - 1 )
                pa->lastCheckPoint = cp2->id; // update the checkpoint counter in the player
        }
	}
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
    // Set start line and checkpoints
    if( button == 0 )
    {
        checkPoint cp;
        cp.init( checkPoints.size(), x, y );
        checkPoints.push_back(cp);
    }

    //calculate local mouse x,y in image
    /*int mx = x % SCREEN_WIDTH;
    int my = y % SCREEN_HEIGHT;

    //get hue value on mouse position
    findHue = hue.getPixels()[ my * SCREEN_WIDTH + mx ];

    cout << findHue;*/
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
    checkPoints[checkPoints.size()-1].radius = abs(checkPoints[checkPoints.size()-1].pos.y - y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
    // Set the checkpoints
    if( button == 0 )
    {
        checkPoints[checkPoints.size()-1].create(box2d.getWorld());
    }

    // Start the race!
    if( button == 2 )
    {
        sndLoop.stop();
        sndBeep.play();

        // Move and reset players
        p1.setPosition(x, y);
        p2.setPosition(x, y);
        p1.laps = 0;
        p2.laps = 0;

        ofResetElapsedTimeCounter();
        elapsedSec = 0;
        startRace = true;
        racing = false;
    }
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}
