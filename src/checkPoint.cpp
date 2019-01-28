#include "checkPoint.h"

//--------------------------------------------------------------
void checkPoint::init( int _id, int _x, int _y )
{
    id = _id;
    pos.x = _x;
    pos.y = _y;
    radius = 0;
    created = false;
}

//--------------------------------------------------------------
void checkPoint::create( b2World* world )
{
    b2world = world;

    setPhysics(0,0,0);
    setup(b2world, pos.x, pos.y, radius);
    body->SetUserData( this );

    for( b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext() )
    {
        f->SetSensor(true);
    }

    created = true;
}

//--------------------------------------------------------------
void checkPoint::update()
{

}

//--------------------------------------------------------------
void checkPoint::paint()
{
    ofEnableAlphaBlending();

    if( id == 0 )
        ofSetColor( ofColor(0, 255, 0, 40) );
    else
        ofSetColor( ofColor(255, 0, 0, 40) );

    if( created )
        draw(); // Box2D
    else
        ofCircle( pos, radius );
}
