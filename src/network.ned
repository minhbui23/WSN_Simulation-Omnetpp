package test;

import ned.IdealChannel;


network NetworkSimulation
{
    @display("bgb=1697.605,820.82");
    submodules:
        BaseStation: BaseStation {
            @display("p=1324.505,363.77252;is=vl");
        }
        SensorNode1: SensorNode {
            @display("p=253.175,153.2375");
        }
        SensorNode2: SensorNode {
            @display("p=506.35,266.5");
        }
        SensorNode3: SensorNode {
            @display("p=207.87001,393.0875");
        }
        SensorNode4: SensorNode {
            @display("p=430.39752,458.38");
        }
        SensorNode5: SensorNode {
            @display("p=796.835,287.82");
        }
        SensorNode6: SensorNode {
            @display("p=676.91003,540.995");
        }
        SensorNode8: SensorNode {
            @display("p=1046.0126,542.3275");
        }
        SensorNode7: SensorNode {
            @display("p=1015.365,174.55751");
        }
    connections:
        SensorNode1.out++ --> IdealChannel --> SensorNode3.in++;
        SensorNode2.out++ --> IdealChannel --> SensorNode1.in++;
        SensorNode1.out++ --> IdealChannel --> SensorNode2.in++;
        SensorNode3.out++ --> IdealChannel --> SensorNode4.in++;
        SensorNode3.out++ --> IdealChannel --> SensorNode2.in++;
        SensorNode2.out++ --> IdealChannel --> SensorNode3.in++;
        SensorNode3.out++ --> IdealChannel --> SensorNode1.in++;
        SensorNode4.out++ --> IdealChannel --> SensorNode3.in++;

        SensorNode2.out++ --> IdealChannel --> SensorNode5.in++;
        SensorNode5.out++ --> IdealChannel --> SensorNode2.in++;
        SensorNode5.out++ --> IdealChannel --> SensorNode6.in++;
        SensorNode6.out++ --> IdealChannel --> SensorNode5.in++;
        SensorNode4.out++ --> IdealChannel --> SensorNode6.in++;
        SensorNode6.out++ --> IdealChannel --> SensorNode4.in++;
        SensorNode5.out++ --> IdealChannel --> SensorNode7.in++;
        SensorNode7.out++ --> IdealChannel --> SensorNode5.in++;
        SensorNode5.out++ --> IdealChannel --> SensorNode8.in++;
        SensorNode8.out++ --> IdealChannel --> SensorNode5.in++;
        SensorNode8.out++ --> IdealChannel --> BaseStation.in++;
        BaseStation.out++ --> IdealChannel --> SensorNode8.in++;
        BaseStation.out++ --> IdealChannel --> SensorNode7.in++;
        SensorNode7.out++ --> IdealChannel --> BaseStation.in++;
}
