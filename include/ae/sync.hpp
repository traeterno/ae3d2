#ifndef aeSync
#define aeSync

#include <ae/types.hpp>
#include <nlohmann/json_fwd.hpp>

using nlohmann::json;

namespace ae::sync
{

#if defined(__WIN32__) or defined(_WIN32)
typedef unsigned long long Socket;
#else
typedef ae::i32 Socket;
#endif

Socket* buildPair();

void send(Socket s, json);
json recv(Socket s);

void setBlocking(ae::sync::Socket s, bool blocking);

};

#endif