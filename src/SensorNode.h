#ifndef SENSORNODE_H_
#define SENSORNODE_H_

#include <omnetpp.h>
#include<string.h>
#include <queue>
#include <deque>

using namespace omnetpp;
class SensorNode : public cSimpleModule {
    private:
      cMessage *sendMessageEvent; // self-message to schedule events
      int nodeId;
      int maxHopCount;
      int packetIdCounter;
      std::deque<std::pair<int, int>> packetQueue; // Queue to store (NodeId, PacketVersion) pairs
      const int queueSize = 10;

    protected:
      virtual void initialize() override;
      virtual void handleMessage(cMessage *msg) override;
      virtual void finish() override;
      cPacket* createPacket(int destinationNodeId,int hopCount, int version);
      int getDestinationNodeId(cGate *outGate);
      bool isPacketInQueue(int nodeId, int packetVersion);
      void addPacketToQueue(int nodeId, int packetVersion);

};

#endif /* SENSORNODE_H_ */
