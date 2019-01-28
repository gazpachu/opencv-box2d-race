#include "player.h"

#define ANGLE_INCREMENT 0.065f
#define SPEED_INCREMENT 0.1f
#define TOP_SPEED 8.0f

//--------------------------------------------------------------
void player::init( b2World* world, int _id, int _x, int _y )
{
    id = _id;
    pos.x = _x;
    pos.y = _y;

    angle = 0.0f;
    speed_now = 0.0f;
    laps = 0;
    elapsedTime = 0;
    recoveryTime = 0;
    lastCheckPoint = 0;

    accelerating = false;
    backwards = false;
    turningLeft = false;
    turningRight = false;
    crashed = false;

    // Load graphics
    for( int i = 0; i < 1; i++ )
    {
        ofImage img;
        img.loadImage("player" + ofToString(id) + "-" + ofToString(i) + ".png");
        sprite.push_back(img);
    }

    height = sprite[0].getHeight();
    width = sprite[0].getWidth();

    // Load sounds
    sndEngine.loadSound("engine.wav");
    sndEngine.setLoop(true);
    sndEngine.play();

    b2world = world;

    setPhysics(1,0,0);
    setup(b2world, pos.x, pos.y, width/2, height/2);
    bodyDef.fixedRotation = true;
    bodyDef.type = b2_dynamicBody;
    bodyDef.bullet = true;
    bodyDef.linearDamping = 1;
    bodyDef.angularDamping = 1;
    body->SetUserData( this );

}

//--------------------------------------------------------------
void player::update()
{
    //Update acceleration
    if( accelerating && speed_now < TOP_SPEED )
        speed_now += SPEED_INCREMENT;
    else if( !accelerating && speed_now > 0 )
        speed_now -= SPEED_INCREMENT;

    if( !accelerating )
    {
        if( backwards && speed_now > -TOP_SPEED )
            speed_now -= SPEED_INCREMENT;
        else if( !backwards && speed_now < 0 )
            speed_now += SPEED_INCREMENT;
    }

    if( !crashed )
    {
        // Round velocity
        if( !accelerating && !backwards && speed_now <= 0.05f && speed_now >= -0.05f )
            speed_now = 0.0f;

        //Update direction
        if( turningLeft )
            angle -= ANGLE_INCREMENT;

        if( turningRight )
            angle += ANGLE_INCREMENT;

        //Update velocity
        vel_x = speed_now * cos(angle);
        vel_y = speed_now * sin(angle);

        body->SetLinearVelocity(b2Vec2(vel_x, vel_y));
        body->SetTransform( body->GetPosition(), angle );
    }
    else
    {
        vel_x = 0.0f;
        vel_y = 0.0f;
        speed_now = 0.0f;

        // Update the rotation of the car after the impact
        angle = body->GetAngle();

        if( elapsedTime < recoveryTime )
            elapsedTime++;
        else
        {
          elapsedTime = 0;
          recoveryTime = 0;
          crashed = false;
        }
    }

    sndEngine.setSpeed( 1.0f + speed_now );
}

//--------------------------------------------------------------
void player::paint()
{
    ofEnableAlphaBlending();

    ofPoint newPos = getPosition();

    ofPushMatrix();
        ofTranslate(newPos.x, newPos.y, 0);
        ofRotate( ofRadToDeg(angle) );
        sprite[status].setAnchorPercent(0.5, 0.5);
        sprite[status].draw(0, 0);
    ofPopMatrix();

    //draw(); // Box2D
}

//--------------------------------------------------------------
void player::setCrash()
{
    if( speed_now > 7.0f )
        recoveryTime = 50;
    else if( speed_now > 5.0f )
        recoveryTime = 30;
    else if( speed_now > 3.0f )
        recoveryTime = 15;
    else if( speed_now < 3.0f )
        recoveryTime = 3;

    crashed = true;
}

//--------------------------------------------------------------
void player::keyPressed( direction dir )
{
    switch(dir)
    {
		case P_UP: accelerating = true; break;
		case P_DOWN: backwards = true; break;
        case P_LEFT: turningLeft = true; break;
		case P_RIGHT: turningRight = true; break;
	}
}

//--------------------------------------------------------------
void player::keyReleased( direction dir )
{
    switch(dir)
    {
		case P_UP: accelerating = false; break;
		case P_DOWN: backwards = false; break;
        case P_LEFT: turningLeft = false; break;
		case P_RIGHT: turningRight = false; break;
	}
}
