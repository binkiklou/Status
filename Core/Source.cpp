#include "httplib.h"

int main()
{
	httplib::Server server;

	server.Get("/", [](const httplib::Request&, httplib::Response& res) {
		res.set_content("hi", "text/plain");
		});

	server.listen("127.0.0.1",1234);
	
	return 0;
}