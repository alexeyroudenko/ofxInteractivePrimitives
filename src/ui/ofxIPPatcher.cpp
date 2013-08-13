#include "ofxIPPatcher.h"

using namespace ofxInteractivePrimitives;

// PatchCord

PatchCord::PatchCord(Port *upstream_port, Port *downstream_port) : upstream(upstream_port), downstream(downstream_port)
{
	getUpstream()->addCord(this);
	getDownstream()->addCord(this);
	
	setParent(upstream_port->getPatchObject()->getUIElement());
}

void PatchCord::disconnect()
{
	getUpstream()->removeCord(this);
	getDownstream()->removeCord(this);
	
	dispose();
	delayedDelete();
}

void PatchCord::draw()
{
	if (!isValid()) return;
	
	const ofVec3f p0 = getUpstream()->getPos();
	const ofVec3f p1 = getUpstream()->getPatchObject()->globalToLocalPos(getDownstream()->getGlobalPos());

	ofPushStyle();
	
	if (this->isHover())
		ofSetLineWidth(2);
	else
		ofSetLineWidth(1);
	
	if (this->isFocus())
		ofSetColor(ofColor::fromHex(0xCCFF77), 127);

	ofLine(p0, p1);
	
	ofPopStyle();
}

void PatchCord::hittest()
{
	if (!isValid()) return;
	
	const ofVec3f p0 = getUpstream()->getPos();
	const ofVec3f p1 = getUpstream()->getPatchObject()->globalToLocalPos(getDownstream()->getGlobalPos());
	
	ofSetLineWidth(3);
	ofLine(p0, p1);
}

void PatchCord::keyPressed(int key)
{
	if (key == OF_KEY_DEL || key == OF_KEY_BACKSPACE)
	{
		disconnect();
		delayedDelete();
	}
}

// Port

Port::Port(IPatchObject *patcher, size_t index, PortIdentifer::Direction direction, const string &desc) : patcher(patcher), index(index), direction(direction), desc(desc)
{
}

MessageRef& Port::requestUpdate()
{
	if (direction == PortIdentifer::INPUT)
	{
		CordContainerType::iterator it = cords.begin();
		while (it != cords.end())
		{
			PatchCord *cord = *it;
			Port *port = cord->getUpstream();
			if (port)
			{
				port->requestUpdate();
				
				// TODO: data multiplexing
				data = port->data;
			}

			it++;
		}
	}
	else if (direction == PortIdentifer::OUTPUT)
	{
		patcher->executeUpstream();
	}
	
	return data;
}

ofVec3f Port::getGlobalPos() const
{
	return patcher->localToGlobalPos(getPos());
}

bool Port::hasConnectTo(Port *port)
{
	CordContainerType::iterator it = cords.begin();
	while (it != cords.end())
	{
		PatchCord *cord = *it;
		Port *p = cord->getDownstream();
		if (p == port) return true;
		
		it++;
	}
	
	return false;
}
