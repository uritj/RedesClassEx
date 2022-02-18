#include <iostream>
#include <SFML/Network.hpp>
#include <vector>
#include <thread>
#include <chrono>
#include <mutex>

std::mutex mtx;

void Reception(sf::TcpSocket* _sock)
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
			//eliminar del vector de threads (?)
			break;
		default:
			break;
		}

	} while (true);
}

void AceptarConexiones(std::vector<sf::TcpSocket*>* _clientes, bool* _end)
{
	//Listener
	sf::TcpListener escuchador;
	sf::Socket::Status status = escuchador.listen(50000);
	if (status != sf::Socket::Status::Done)
	{
		return;
	}

	while (!(*_end))
	{
		sf::TcpSocket* sock = new sf::TcpSocket();

		//Accept
		escuchador.accept(*sock);
		if (status != sf::Socket::Status::Done)
		{
			delete sock;
			continue;
		}

		//Guardarla en algun "lao" -> _clientes

		std::thread tReceive(Reception, sock);
		tReceive.detach();

		mtx.lock();
		_clientes->push_back(sock);
		mtx.unlock();
	}
}

void EnvioPeriodico(std::vector<sf::TcpSocket*>* _clientes, bool* _end)
{
	sf::Packet pack;
	std::string str = "Mensaje desde servidor Uris \n";
	pack << str;

	while (!(*_end))
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
		mtx.lock();
		for (size_t i = 0; i < _clientes->size(); i++)
		{
			sf::Socket::Status status = (_clientes->at(i))->send(pack);
			switch (status)
			{
			case sf::Socket::Done: //mensaje ha llegado al destino

				continue;
			case sf::Socket::Disconnected: //cliente ya no esta ahí

				(_clientes->at(i))->disconnect();
				delete _clientes->at(i);
				_clientes->erase(_clientes->begin() + i);

				break;
			default:
				break;
			}
		}
		mtx.unlock();
	}
}


int main()
{
	std::vector<sf::TcpSocket*> clientes;
	char opc;
	bool end = false;

	std::thread tAccepts(AceptarConexiones, &clientes, &end);
	tAccepts.detach();

	std::thread tSends(EnvioPeriodico, &clientes, &end);
	tSends.detach();

	do
	{
		std::cin >> opc;
	} while (opc != 'e');
	end = true;
	return 0;
}