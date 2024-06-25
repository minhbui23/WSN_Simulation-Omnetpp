package test;

simple SensorNode
{
    parameters:
        @display("i=device/sensor3");
        int nodeId;
   	gates:
   	    input in[]; 
   	    output out[];      
}