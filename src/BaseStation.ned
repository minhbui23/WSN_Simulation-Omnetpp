
package test;

simple BaseStation
{
    parameters:
    	@display("i=device/receiverdish;bgb=296,194");
    	int nodeId = -1;
    gates:
        input in[];
        output out[];
}