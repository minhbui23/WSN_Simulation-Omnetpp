package test;

simple SensorNode
{
    parameters:
        @display("i=device/sensor3");
        int nodeId;
        double connectionRange @unit(m); 
   	gates:
		input in @directIn;   
}