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
	this->pos = start;  //ok in c++?
}

Creature::Creature(const int lifespan, ofVec3f *startPos, ofVec3f *startVel)
{
	this->ttl = this->lifespan = lifespan;
	pos = startPos;
	vel = startVel;
	numCreatures = 1; //regular creature
}

//create composite creature (basically a group)
//(how many creatures in this LOC?)
Creature::Creature(Creature *creature1, Creature *creature2)
{
	this->numCreatures = creature1->numCreatures + creature2->numCreatures;
	this->ttl = creature1->ttl + creature2->ttl;
	this->lifespan = creature1->lifespan + creature2->lifespan;

	//TODO: average of 2 vectors
	this->pos = creature1->pos;
	this->vel = creature2->vel;
	
	creature1->die();
	creature2->die();
}

void Creature::tick() {
	*pos += *vel;
	ttl -= 1;
}

int Creature::color()
{
	//how dead are we?
	//start turning red at middle-age...
	float t = 2 * (float)ttl/(float)lifespan;
	ofLerp(BLUE, RED, t);
}

bool Creature::desperate()
{
	ttl <= lifespan/3;
}

//sugar packets pull on creature
void Creature::goTowards(SugarPacket *sugar)
{
	ofVec3f f = this->pos - sugar->pos;
	f.normalize();
	*(this->vel) += f;
}

//sugar packets pull on creature
void Creature::goTowards(Creature *creature)
{
	ofVec3f f = this->pos - creature->pos;
	f.normalize();
	*(this->vel) += f;
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