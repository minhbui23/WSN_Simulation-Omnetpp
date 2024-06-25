#include "SensorNode.h"

Define_Module(SensorNode);

void SensorNode::initialize()
{
    // Initialize node ID from parameter
    nodeId = par("nodeId").intValue();
    maxHopCount = 3;
    packetIdCounter = 0;
    // Create and schedule the first message
    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime() + uniform(0.1, 1.0), sendMessageEvent);
}

cPacket* SensorNode::createPacket(int destinationNodeId, int hopCount, int version)
{
    // Create a new packet
    std::string packetName = "Packet from " + std::to_string(nodeId);
    cPacket *pkt = new cPacket(packetName.c_str());

    // Set packet parameters
    pkt->addPar("sourceNodeId") = nodeId;
    pkt->addPar("destinationNodeId") = destinationNodeId;
    pkt->addPar("packetVersion") = version;
    pkt->addPar("hopCount") = hopCount;
    pkt->setTimestamp();

    return pkt;
}

int SensorNode::getDestinationNodeId(cGate *outGate)
{
    // Retrieve the destination node ID from the given output gate
    cModule *destNode = outGate->getPathEndGate()->getOwnerModule();
    return destNode->par("nodeId").intValue();
}

bool SensorNode::isPacketInQueue(int nodeId, int packetVersion)
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

void SensorNode::addPacketToQueue(int nodeId, int packetVersion)
{
    if (packetQueue.size() >= queueSize) {
        packetQueue.pop_front(); // Remove the oldest entry if the queue is full
    }
    packetQueue.push_back(std::make_pair(nodeId, packetVersion));
}

void SensorNode::handleMessage(cMessage *msg)
{
    if (msg == sendMessageEvent) {
        // Sending messages
        int numOutGates = gateSize("out");

        for (int i = 0; i < numOutGates; ++i) {
            cGate *outGate = gate("out", i);
            int destinationNodeId = getDestinationNodeId(outGate);

            cPacket *pkt = createPacket(destinationNodeId, 0,packetIdCounter); // Initial hop count is 0

            EV << "Node " << nodeId << " sending packet to node " << destinationNodeId << " through gate " << i << endl;
            send(pkt, "out", i);
        }
        packetIdCounter++;

        // Reschedule the next message
        scheduleAt(simTime() + uniform(0.1, 1.0), sendMessageEvent);
    } else {
        // Process received packet
        cPacket *receivedPkt = check_and_cast<cPacket*>(msg);
        if (receivedPkt) {
            int arrivalNodeId = receivedPkt->par("sourceNodeId").longValue();
            int packetVersion = receivedPkt->par("packetVersion").longValue();
            int hopCount = receivedPkt->par("hopCount").longValue();

            EV << "Node " << nodeId << " received packet from node " << arrivalNodeId << " with version " << packetVersion << " and hop count " << hopCount << endl;

            if (hopCount >= maxHopCount) {
                EV << "Node " << nodeId << " dropping packet due to max hop count reached." << endl;
                delete receivedPkt;
                return;
            }

            if (isPacketInQueue(arrivalNodeId, packetVersion)) {
                EV << "Node " << nodeId << " already received this packet version, dropping packet." << endl;
                delete receivedPkt;
                return;
            }

            addPacketToQueue(arrivalNodeId, packetVersion);
            receivedPkt->par("hopCount") = hopCount + 1;

            int numOutGates = gateSize("out");
            for (int i = 0; i < numOutGates; ++i) {
                cGate *outGate = gate("out", i);
                int destinationNodeId = getDestinationNodeId(outGate);

                if (arrivalNodeId != destinationNodeId) {
                    cPacket *pktCopy = receivedPkt->dup();
                    EV << "Node " << nodeId << " forwarding packet to node " << destinationNodeId << " through gate " << i << endl;
                    send(pktCopy, "out", i);
                }
            }
        }
        delete receivedPkt;
    }
}

void SensorNode::finish()
{
    // Clean up
    cancelAndDelete(sendMessageEvent);
}
