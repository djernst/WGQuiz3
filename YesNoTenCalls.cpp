//============================================================================
// Name        : YesNoTenCalls.cpp
// Author      : Dan Ernst
// Version     :
// Copyright   : Ernstco
// Description : YesNoTenCalls in C++, Ansi-style
//============================================================================
//
// Theory of operation:
// Every time callEventTask() is called by a thread, the function will first use a mutex to synchronize access to the shared
// list and other operations.  The function will then query the os for the current time and insert it into the shared list.  The function then
// will iterate over all the elements in the list and compute the elapsed time between the current time and the timestamp.  If the
// difference is great or equal to 60 seconds, the function will the purge them from the list.
// Then if the list has remaining exactly 10 elements in the list "yes" (plus some addition info) will be printed out.  Otherwise "no"
// (plus the additional info) will be printed out.

#include <iostream>
#include <ctime>

using namespace std;

#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <vector>

std::mutex myMutex;
bool loopEnabled = true;
struct eventItem{
	time_t time;
};

std::vector<eventItem> eventList;


 void callEventTask(){
	 eventItem myItem;
	 std::vector<eventItem>::iterator eventListItr;

	 myMutex.lock();
	 myItem.time = time(0);
	 eventList.insert(eventList.begin(), myItem);

	 for (eventListItr = eventList.begin(); eventListItr != eventList.end(); eventListItr++){
		 //cout << "timeDelta: " << (myItem.time-(*eventListItr).time ) << endl;

		 if ((myItem.time - (*eventListItr).time ) >= 60){
			 break;
		 }
	 }

	 if (eventListItr != eventList.end()){
		 cout << "removing "  << (eventList.end() - eventListItr) <<" expired elements" << endl;
		 eventList.erase(eventListItr, eventList.end());
	 }

	 if (eventList.size()==10) {
		 cout << "yes " << "From thread " << std::this_thread::get_id() << " " << eventList.size() << " Calls in last 60 Secs" << endl;
	 } else {
		 cout << "no " << "From thread " << std::this_thread::get_id() << " " << eventList.size() << " Calls in last 60 Secs"<< endl;
	 }
	 myMutex.unlock();
 }

 void testThread(int moduleVal){

	 while (loopEnabled){
		 callEventTask();
		 std::this_thread::sleep_for(std::chrono::milliseconds(rand() % moduleVal));
	 }
 }

int main() {

	srand (time(0));

    std::thread th1 (testThread, 22345);
    std::thread th2 (testThread, 54456);

	std::this_thread::sleep_for(240s);
	loopEnabled= false;


    th1.join();
    th2.join();
	cout << "Done with Test" << endl;
	return 0;
}
