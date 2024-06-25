#include "BaseStation.h"

Define_Module(BaseStation);

void BaseStation::initialize()
{
    // Initialization if necessary
}

bool BaseStation::isPacketInQueue(int nodeId, int packetVersion)
{
    std ::deque<std::pair<int,int>>::iterator it = packetQueue.begin();

    while(it != packetQueue.end()){
        if(it->first == nodeId && it->second == packetVersion){
            return true;
        }
        ++it;
    }

    return false;
}

void BaseStation::addPacketToQueue(int nodeId, int packetVersion)
{
    if (packetQueue.size() >= queueSize) {
        packetQueue.pop_front(); // Remove the oldest entry if the queue is full
    }
    packetQueue.push_back(std::make_pair(nodeId, packetVersion));
}

void BaseStation::handleMessage(cMessage *msg)
{
    cPacket *receivedPkt = dynamic_cast<cPacket*>(msg);
    if (receivedPkt) {
        int nodeId = receivedPkt->par("sourceNodeId").longValue();
        int packetVersion = receivedPkt->par("packetVersion").longValue();

        if (isPacketInQueue(nodeId, packetVersion)) {
            EV << "BaseStation: Dropping duplicate packet from node " << nodeId << " (version " << packetVersion << ")" << endl;
        } else {
            EV << "BaseStation: Received packet from node " << nodeId << " (version " << packetVersion << ")" << endl;

            addPacketToQueue(nodeId, packetVersion);
        }
    }

    delete msg;
}
