#include <ae/network.hpp>

int main()
{
	ae::net::init();
	
	ae::net::TcpListener listener(26225);
	auto s = listener.accept();

	ae::net::shutdown();
	return 0;
}