
#ifndef BASESTATION_H_
#define BASESTATION_H_

#include <omnetpp.h>
#include <deque>
using namespace omnetpp;

class BaseStation : public cSimpleModule
{
  private:
    std::deque<std::pair<int, int>> packetQueue;
    int queueSize = 10;
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    bool isPacketInQueue(int nodeId, int packetVersion);
    void addPacketToQueue(int nodeId, int packetVersion);
};

#endif /* BASESTATION_H_ */
