#include "SensorNode.h"
#include "BaseStation.h"
// Register this module with OMNeT++'s simulation kernel
Define_Module(SensorNode);

// Initialization function called once at the start of the simulation for each node
void SensorNode::initialize()
{

    nodeId = par("nodeId").intValue();
    connectionRange = par("connectionRange").doubleValue();
    maxHopCount = 3;
    packetIdCounter = 0;
    sendMessageEvent = new cMessage("sendMessageEvent");

    // Initialize the vector nodeNames
    cModule *network = getParentModule(); // Assuming this is the network module
    int numNodes = network->par("numNodes").intValue();

    nodeList.push_back("BaseStation");

    for (int i = 1; i < numNodes; i++) {
        // Construct the submodule name dynamically
        std::string nodeName = "SensorNode" + std::to_string(i);
        cModule *node = network->getSubmodule(nodeName.c_str());
        if (node) {
            nodeList.push_back(node->getFullName());
        }
    }

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

std::pair<double, double> SensorNode::getNodePosition(cModule *node)
{
    const char *displayString = node->getDisplayString();
    std::string inputStr(displayString);
    size_t pStart = inputStr.find("p=");
    pStart +=2;
    size_t pEnd = inputStr.find_first_of(";", pStart);

    std::string xyString = inputStr.substr(pStart, pEnd - pStart);

    std::istringstream iss(xyString);
    double x, y;
    char comma; // Để đọc dấu phẩy
    if (!(iss >> x >> comma >> y)) {
        throw std::runtime_error("Lỗi khi phân tích giá trị x và y");
    }

    return std::make_pair(x, y);

}

double SensorNode::calculateDistance(cModule *otherNode)
{
    std::pair<double, double> selfPos = getNodePosition(this);
    std::pair<double, double> otherPos;

    SensorNode *otherSensorNode = dynamic_cast<SensorNode *>(otherNode);
    if (otherSensorNode)
    {
        otherPos = getNodePosition(otherSensorNode);
    }
    else
    {
        BaseStation *baseStation = dynamic_cast<BaseStation *>(otherNode);
        if (baseStation)
        {
            otherPos = getNodePosition(baseStation);
        }
        else
        {
            throw cRuntimeError("Unsupported node type for distance calculation");
        }
    }

    double dx = selfPos.first - otherPos.first;
    double dy = selfPos.second - otherPos.second;
    return sqrt(dx * dx + dy * dy);
}

std::vector<const char*> SensorNode::Scan()
{
    std::vector<const char*> nearbyNodes;
    cModule *network = getParentModule(); // Assuming this is the network module

    for (const auto& nodeName : nodeList)
    {
        cModule *otherNode = network->getSubmodule(nodeName);
        if (otherNode && otherNode != this)
        {
            double distance = calculateDistance(otherNode);
            if (distance <= (this->connectionRange + otherNode->par("connectionRange").doubleValue()))
            {
                nearbyNodes.push_back(otherNode->getFullName());
            }
        }
    }
    return nearbyNodes;
}



// Main message handling function
void SensorNode::handleMessage(cMessage *msg)
{
    std::vector<const char *> nearbyNodes = Scan();

    if (msg == sendMessageEvent)
    {

        for (const char *nodeName : nearbyNodes)
        {
            cModule *destNode = getModuleByPath(nodeName);

            if (destNode)
            {
                cPacket *pkt = createPacket(destNode->par("nodeId").intValue(), 0, packetIdCounter);
                EV << "Node " << nodeId << " sending packet to node " << destNode->par("nodeId").intValue() << endl;
                sendDirect(pkt, destNode, "in"); // Gửi trực tiếp qua cổng radioIn của node đích
            }
        }
        packetIdCounter++;
        scheduleAt(simTime() + uniform(0.1, 1.0), sendMessageEvent);
    }
    else
    {
        cPacket *receivedPkt = check_and_cast<cPacket*>(msg);
        cModule *arrivalNode = receivedPkt->getSenderModule();
        int arrivalNodeId = arrivalNode->par("nodeId").intValue();
        int packetVersion = receivedPkt->par("packetVersion").longValue();
        int hopCount = receivedPkt->par("hopCount").longValue();

        EV << "Node " << nodeId << " received packet from node " << arrivalNodeId << " with version " << packetVersion << " and hop count " << hopCount << endl;

        if (hopCount >= maxHopCount)
        {
            EV << "Node " << nodeId << " dropping packet due to max hop count reached." << endl;
            delete receivedPkt;
            return;
        }

        if (isPacketInQueue(arrivalNodeId, packetVersion))
        {
            EV << "Node " << nodeId << " already received this packet version, dropping packet." << endl;
            delete receivedPkt;
            return;
        }

        addPacketToQueue(arrivalNodeId, packetVersion);

        receivedPkt->par("hopCount") = hopCount + 1;
        for (const char *nodeName : nearbyNodes) {
            cModule *destNode = getModuleByPath(nodeName);
            if (destNode && destNode != this && destNode->par("nodeId").intValue()!= arrivalNodeId) {
                cPacket *pktCopy = receivedPkt->dup();
                sendDirect(pktCopy, destNode, "in");
            }
        }
        delete receivedPkt;
    }
}



void SensorNode::finish()
{
    cancelAndDelete(sendMessageEvent);
}
