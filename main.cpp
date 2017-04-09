#include "event_loop.h"
#include "server.h"
#include "base/log.h"

int main (int argc, const char * argv[])
{
	getlog()->init("./testlog.log", LOG_LEVEL_DEBUG);
	getlog()->startBg();
	
	EventLoop loop;
	Server server(&loop, 11250);
	getlog()->LogWrite(LOG_LEVEL_INDISPENSABLE, "SERVER START");
	server.start();

	loop.loop();
	return 0;
}