#include <list>
#include <mutex>
#include <vector>
#include <iostream>
#include <chrono>
#include <queue>
#include <thread>

using namespace std;

mutex outputLock;

template<typename T>
class MessagingQueue {
private:
	queue<T> data;
	mutex mu;
	bool closed = false;
public:
	T getMessage() {
		while (true) {
			if (!data.empty()) {
				mu.lock();
				T output = data.front();
				data.pop();
				mu.unlock();
				return output;
			}
			else {
				if (closed) {
					return NULL;
				}
			}
			this_thread::yield();
		}
	}

	void sendMessage(T Message) {
		lock_guard<mutex> guard(mu);
		data.push(Message);
	}

	void close() {
		closed = true;
	}

};

void filterProcess(MessagingQueue<int>*);

int main() {

	MessagingQueue<int> *output = new MessagingQueue<int>();
	thread t1(filterProcess,output);
	for (int i = 2; i < 100000; i++) {
		output->sendMessage(i);
	}
	output->close();
	t1.join();
	return 0;
}

void filterProcess(MessagingQueue<int> *input) {

	int prime = input->getMessage();
	if (!prime) {
		return;
	}
	MessagingQueue<int> *output = new MessagingQueue<int>();
	thread t1(filterProcess,output);
	int nextNumber = input->getMessage();
	while (nextNumber) {
		if (nextNumber % prime) {
			output->sendMessage(nextNumber);
		}
		nextNumber = input->getMessage();
	}
	cout << prime << endl;
	output->close();
	t1.join();
	return;
}

