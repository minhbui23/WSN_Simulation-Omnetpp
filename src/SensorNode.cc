#include "SensorNode.h"

// Register this module with OMNeT++'s simulation kernel
Define_Module(SensorNode);

// Initialization function called once at the start of the simulation for each node
void SensorNode::initialize()
{

    nodeId = par("nodeId").intValue();
    maxHopCount = 3;
    packetIdCounter = 0;
    queueSize = 10;
    sendMessageEvent = new cMessage("sendMessageEvent");

    scheduleAt(simTime() + uniform(0.1, 1.0), sendMessageEvent);
}


cPacket* SensorNode::createPacket(int destinationNodeId, int hopCount, int version)
{
    // Construct a unique packet name based on the source node ID
    std::string packetName = "Packet from " + std::to_string(nodeId);
    cPacket *pkt = new cPacket(packetName.c_str());

    // Attach important information to the packet as parameters
    pkt->addPar("sourceNodeId") = nodeId;
    pkt->addPar("destinationNodeId") = destinationNodeId;
    pkt->addPar("packetVersion") = version;
    pkt->addPar("hopCount") = hopCount;
    pkt->setTimestamp();

    return pkt;
}

int SensorNode::getDestinationNodeId(cGate *outGate)
{
    cModule *destNode = outGate->getPathEndGate()->getOwnerModule();
    return destNode->par("nodeId").intValue();
}

bool SensorNode::isPacketInQueue(int nodeId, int packetVersion){
    std::deque<std::pair<int,int>>::iterator it = packetQueue.begin();

    while(it != packetQueue.end()){
        if(it->first == nodeId && it->second == packetVersion){
            return true;
        }
        ++it;
    }
    return false;
}

void SensorNode::addPacketToQueue(int nodeId, int packetVersion){
    if(packetQueue.size() >= queueSize){
        packetQueue.pop_front();
    }
    packetQueue.push_back(std::make_pair(nodeId,packetVersion));
}

// Main message handling function
void SensorNode::handleMessage(cMessage *msg)
{
    // Handle the self-message to trigger packet sending
    if (msg == sendMessageEvent) {


        int numOutGates = gateSize("out");
        for (int i = 0; i < numOutGates; ++i) {
            cGate *outGate = gate("out", i);
            int destinationNodeId = getDestinationNodeId(outGate);

            cPacket *pkt = createPacket(destinationNodeId, 0, packetIdCounter);


            EV << "Node " << nodeId << " sending packet to node " << destinationNodeId << " through gate " << i << endl;
            send(pkt, "out", i);
        }

        packetIdCounter++;

        scheduleAt(simTime() + uniform(0.1, 1.0), sendMessageEvent);

    }
    else {
        // Handle received packets from other nodes
        cPacket *receivedPkt = check_and_cast<cPacket*>(msg);
        if (receivedPkt) {

            int arrivalNodeId = receivedPkt->par("sourceNodeId").longValue();
            int packetVersion = receivedPkt->par("packetVersion").longValue();
            int hopCount = receivedPkt->par("hopCount").longValue();

            EV << "Node " << nodeId << " received packet from node " << arrivalNodeId << " with version " << packetVersion << " and hop count " << hopCount << endl;

            // Drop packet if it has reached the maximum hop count
            if (hopCount >= maxHopCount) {
                EV << "Node " << nodeId << " dropping packet due to max hop count reached." << endl;
                delete receivedPkt;
                return;
            }

            // Drop the packet if the packet with the particular version from the node is already received.
            if(isPacketInQueue(arrivalNodeId,packetVersion)){
                EV << "Node " << nodeId << " already received this packet version, dropping packet." << endl;
                delete receivedPkt;
                return;
            }


            addPacketToQueue(arrivalNodeId,packetVersion);

            receivedPkt->par("hopCount") = hopCount + 1;
            int numOutGates = gateSize("out");
            // Forward the packet to all outgoing gates except the one it came from
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
    cancelAndDelete(sendMessageEvent);
}
