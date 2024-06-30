package test;

import ned.IdealChannel;


network NetworkSimulation
{
    parameters:
        int numNodes;
        @display("bgb=3321.8188,1536.6626");
    submodules:
        BaseStation: BaseStation {
            @display("p=2105.0564,599.8125;is=vl;r=600");
        }
        SensorNode1: SensorNode {
            @display("p=419.86877,328.46875;r=200");
        }
        SensorNode2: SensorNode {
            @display("p=785.46875,259.91876;r=200");
        }
        SensorNode3: SensorNode {
            @display("p=211.3625,565.53754;r=200");
        }
        SensorNode4: SensorNode {
            @display("p=479.85,739.7688;r=200");
        }
        SensorNode5: SensorNode {
            @display("p=1068.2375,511.26877;r=200");
        }
        SensorNode6: SensorNode {
            @display("p=834.025,745.48126;r=200");
        }

        SensorNode7: SensorNode {
            @display("p=1382.425,351.31876;r=200");
        }
        SensorNode8: SensorNode {
            @display("p=1348.15,771.1875;r=200");
        }

}
