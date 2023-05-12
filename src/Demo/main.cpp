#include "DemoApplication.h"

int main(void)
{
	DemoApplication* app = new DemoApplication;
	app->run();
	delete app;
}
