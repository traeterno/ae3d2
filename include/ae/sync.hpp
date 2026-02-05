#ifndef aeSync
#define aeSync

#include <condition_variable>
#include <queue>
#include <optional>

namespace ae
{

template<typename T>
class Channel
{
public:
	Channel() = default;
	~Channel() = default;
	void send(T msg);
	std::optional<T> recv();
private:
	std::queue<T> line;
	std::mutex m;
	std::condition_variable cv;
};

};

#endif