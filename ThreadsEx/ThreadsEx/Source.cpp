#include <thread>
#include <vector>
#include <iostream>
#include <mutex>
#include <chrono>

std::vector<std::thread> threads;
std::mutex mtx;

void FuncThread(int num)
{


	for (int i = 0; i < 10; i++)
	{
		std::cout << "Soy el thread " << std::this_thread::get_id() << " y estoy en la posicion " << num << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	mtx.lock();

	mtx.unlock();
}

int main() 
{
	char input;
	int counter;

	while (true)
	{
		std::cin >> input;

		if (input == 'n') 
		{
			
			std::thread t(FuncThread, counter);
			t.detach();

			mtx.lock();
			threads.push_back(std::move(t));
			mtx.unlock();

			counter++;
		}
	}

	return 0;
}