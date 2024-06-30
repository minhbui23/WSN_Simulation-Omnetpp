
package test;

simple BaseStation
{
    parameters:
    	@display("i=device/receiverdish;bgb=296,194");
    	int nodeId = 0;
    	double connectionRange @unit(m); 
    gates:
        input in @directIn;  
}