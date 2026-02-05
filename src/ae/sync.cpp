#include <ae/sync.hpp>
#include <mutex>

using namespace ae;

template<typename T>
void Channel<T>::send(T msg)
{
	std::lock_guard<std::mutex> lock(this->m);
	this->line.push(std::move(msg));
	this->cv.notify_one();
}

template<typename T>
std::optional<T> Channel<T>::recv()
{
	std::unique_lock<std::mutex> lock(this->m);
	cv.wait(lock, [this](){ return !this->line.empty(); });
	auto p = std::move(this->line.front());
	this->line.pop();
	return p;
}