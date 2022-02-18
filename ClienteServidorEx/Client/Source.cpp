#include <iostream>
#include <SFML/Network.hpp>
#include <thread>


void Reception(sf::TcpSocket* _sock, bool* _end)
{
	sf::Socket::Status status;
	do
	{
		sf::Packet pack;
		status = _sock->receive(pack);
		std::string str;
		//controlar error
		switch (status)
		{
		case sf::Socket::Done:
			pack >> str;
			std::cout << str;
			break;
		case sf::Socket::Disconnected:
			*_end = true;
			break;
		default:
			break;
		}

	} while (!(*_end));
}

int main()
{

	sf::TcpSocket sock;

	sf::Socket::Status status;
	status = sock.connect("127.0.0.1", 50000);
	//controlar error

	bool end = false;

	std::thread tReceive(Reception, &sock, &end);
	tReceive.detach();

	std::string str;
	do
	{
		std::cin >> str;
		sf::Packet pack;
		pack << str;
		sock.send(pack);
		//TODO: Recoger status y si es disconnected -> cerrar
	} while (str != "e");
	end = true;

	sock.disconnect();



	return 0;
}