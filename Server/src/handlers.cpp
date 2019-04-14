#include "SockerServer.hpp"
auto public_handler = [](SocketServer &socket_server, wstring& message, const wstring &speaker) {
	cout << "public_handler" << endl;
	socket_server.public_message(speaker + wstring(L" says:") + message.substr(8));

};

auto private_handler = [](SocketServer &socket_server, wstring& message, const wstring &speaker) {
	cout << "private_handler" << endl;
	socket_server.private_message(message, speaker);

};

auto ignore_handler = [](SocketServer &socket_server, wstring& message, const wstring &speaker)
{cout << "ignore" << endl; };

auto private_confirm_handler = [](SocketServer &socket_server, wstring& message, const wstring &speaker) {
	cout << "private_confirm_handler" << endl;
	socket_server.private_confirm(message.substr(17), speaker);

};

unordered_map<wstring, handler> SocketServer::prefix2handler{
	{ L"public",public_handler },{ L"private",private_handler },
	{ L"private confirm",private_confirm_handler },
	{ L"ignore",ignore_handler }

};