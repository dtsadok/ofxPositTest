/*
 *  Species.h
 *  kitchenSink
 *
 *  Created by Daniel on 4/17/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

class SugarPacket
{	
public:
	SugarPacket(int energy, ofVec3f start);

	int energy; //measured in seconds - adds to ttl of Creature
	ofVec3f pos;
};

class Creature
{
public:
	Creature(int lifespan, ofVec3f startPos, ofVec3f startVel);
	//creates a composite creature out of two (maybe composite) creatures
	Creature(Creature creature1, Creature creature2);

	bool alive;
	int ttl; //time to live :-P
	const int lifespan; //max ttl
	ofVec3f vel;
	ofVec3f pos;
	int numCreatures; //creatures can team up as one

	void tick();
	//willing to cannibalize?
	bool desperate();
	int color(); //changes color with age
	void eat(SugarPacket *sugar);
	void eat(Creature *victim); //how sick
	void die(); //how sad
};