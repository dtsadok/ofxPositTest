/*
 *  Species.cpp
 *  kitchenSink
 *
 *  Created by Daniel Tsadok on 4/17/11.
 *  Copyright 2011. All rights reserved.
 *
 */

#include "Species.h"

SugarPacket::SugarPacket(int energy, ofVec3f *start)
{
	this->energy = energy;
	this->pos = start;
}

Creature::Creature(const int lifespan, ofVec3f *startPos, ofVec3f *startVel)
{
	alive = true; //helps

	this->ttl = this->lifespan = lifespan;
	pos = startPos;
	vel = startVel;
	numCreatures = 1; //regular creature
}

//create composite creature (basically a group)
//(how many creatures in this LOC?)
Creature::Creature(Creature *creature1, Creature *creature2)
{
	alive = true;

	this->numCreatures = creature1->numCreatures + creature2->numCreatures;
	this->ttl = creature1->ttl + creature2->ttl;
	this->lifespan = creature1->lifespan + creature2->lifespan;

	//TODO: average of 2 vectors
	this->pos = creature1->pos;
	this->vel = creature2->vel;
	
	//don't need original creatures - we have one mega-creature
	creature1->die();
	creature2->die();
}

int Creature::color()
{
	//how dead are we?
	//start turning red at middle-age...
	float t = (float)ttl/(float)lifespan;
	return ofLerp(RED, BLUE, t);
}

bool Creature::desperate()
{
	return ttl <= lifespan/3;
}

//sugar packets pull on creature
void Creature::goTowards(SugarPacket *sugar)
{
	ofVec3f f = *(this->pos) - *(sugar->pos);
	float len = f.length();
	f.normalize();

	//distance effect reduces pull of vector
	//so closer sugar packets have more effect
	f/(len*len);

	//but more sugar means more pull
	//f *= sugar->energy;

	//force of sugar now has effect on velocity of creature
	*(this->pos) += f;
}

//other creatures pull on creature (if creature is sufficiently hungry)
void Creature::goTowards(Creature *creature)
{
	ofVec3f f = *(this->pos) - *(creature->pos);
	float len = f.length();
	f.normalize();
	f/(len*len);
	//f *= creature->ttl;

	*(this->pos) += f;
}


//TODO: show death animation
//Also need to GC creature
void Creature::die()
{
	alive = false;
}

void Creature::eat(SugarPacket *sugar)
{
	ttl += sugar->energy;
	delete sugar;
}

//will only eat another creature when desperate
void Creature::eat(Creature *victim)
{
	ttl += victim->ttl / 2; //can't get all the energy...
	victim->die(); //so sad
}
