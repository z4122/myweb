#include"Inc/stdafx.hpp"



int main (int argc, char *argv[])
{
	Log *log = new Log("test.log");
	*log<<"testlog";
	
	Eventloop eventloop(50000,log);
	eventloop.start();

	return 0;
}
