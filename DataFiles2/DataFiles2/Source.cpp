#include <random>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <list>

using namespace std;

void createFiles();
class PCB;
list<PCB> readFile(int);
void runSimulate(list<PCB>);
const int NUM_FILES = 100;
const int MAX_PRIO = 10;
struct pCompare;
void runRR(list<PCB>, long long);
void runFCFS(list<PCB>);
void tabulate();

int main()
{
	list<PCB> myTable;
	//createFiles();
	for (int i = 0; i < NUM_FILES; i++) {
		myTable = readFile(i);
		runSimulate(myTable);
		cout << "Finished Data Set: " << i << endl;
	}	
}

enum state
{
	//Created useless?
	ready, stopped, waiting, terminating, created, working_io, working_cpu
};
class PCB
{
private:
	int waitTime;
	int workTime;
	int totalTime;
	int arrival_time;
	int finishTime;
	
	int temp;

	int priority;
	int PID;

	int R; //Implement this

	list<int> burst;
	state current_state;
public:
	PCB() {
		list<int> temp;
		PCB(0, temp, 0);
	}

	PCB(int os_pid, list<int> os_burst, int os_arrival_time)
	{
		waitTime = workTime = totalTime = 0;
		current_state = ready;
		PID = os_pid;
		burst = os_burst;
		arrival_time = os_arrival_time;
		this->priority = rand() % MAX_PRIO;
	}
	void setFinalTime(int time) {
		finishTime = time;
	}
	void tick()
	{
		if (current_state == stopped) {
			totalTime--; //Do this to stop totalTime from incrementing when it is done
		}
		else if (current_state == ready){
			waitTime++;
		}
		else if (current_state == working_io) {
			temp = burst.front();
			temp--;
			burst.pop_front();
			burst.push_front(temp);
			if (burst.front() == 0) {
				if (burst.size() > 1) {
					burst.pop_front();
					this->setCurrentState(ready);
					//priority = rand() % MAX_PRIO;				//Uncomment to allow for changing PRIO between cpu calls
				}
				else {
					this->setCurrentState(stopped);
				}
			}
			workTime++;
		}
		else if (current_state == working_cpu) {
			temp = burst.front();
			temp--;
			burst.pop_front();
			burst.push_front(temp);
			if (burst.front() == 0) {
				if (burst.size() > 1) {
					this->setCurrentState(working_io);
					burst.pop_front();
				}
				else {
					this->setCurrentState(stopped);
				}
			}
			workTime++;
		}
		
		totalTime++;
	}
	
	int getBurst()
	{
		return burst.front();
	}
	int getPID()
	{
		return PID;
	}
	state getCurrentState()
	{
		return current_state;
	}
	void setCurrentState(state newState)
	{
		workTime = 0;
		this->current_state = newState;
	}
	int getPriority()
	{
		return priority;
	}
	void setPriority(int priority)
	{
		this->priority = priority;
	}
	int getWorkTime() {
		return workTime;
	}
	int getArrivalTime() {
		return arrival_time;
	}
	int getTotalTime() {
		return totalTime;
	}
	int getWaitTime() {
		return waitTime;
	}
	void setWorkTime(int t) {
		workTime = t;
	}
};

void createFiles() {
	FILE * pFile;
	srand(time(NULL));
	char filename[NUM_FILES];								//charArray for name
	int arrival_time = 0;									//Used to be incrementally random
	int j = 0;												//Number to hold bursts  in each process

	for (int l = 1; l < 101; l++)							//Create a hundred files
	{
		sprintf(filename, "Data_%i.csv", l);				//Put created filename into the charArray 
		pFile = fopen(filename, "w");						//Create File with name and open it

		for (int i = 1; i < 101; i++)						//Create a hundred processes in the file
		{
			j = rand() % 10;								//Create number of bursts in each process
			fprintf(pFile, "%i, ", i);						//Send PID to each process
			fprintf(pFile, "%i, ", arrival_time);			//Incrementally random arrival time
			for (int n = -1; n < j - 1; n++)					//Set to -1 so it runs atleast once, TOTAL -1 because we want to change the last one
			{
				fprintf(pFile, "%i, ", rand() % 50 + 1);	//Random burst for each process burst time
			}
			fprintf(pFile, "%i", rand() % 50 + 1);			//Last one does not have a comma and space after it
			fprintf(pFile, "\n");							//Create newline at end of process
			arrival_time += rand() % 20;				//Increment random arrival time
		}
		fclose(pFile);										//Close the file when done
		printf("File generated: \"%s\"\n", filename);		//Print out the filename to console saying it is created
	}
}

list<PCB> readFile(int fileNumber) {
	list<PCB> myVec;
	list<int> burst;
	int PID;
	int arrivalTime;
	ifstream currentFile;
	char filename[NUM_FILES];
	char cNum[10];
	int count = 0;
	string line;

	sprintf(filename, "Data_%i.csv", fileNumber+1);
	currentFile.open(filename);

	if (currentFile.is_open())
	{
		while (getline(currentFile, line))
		{
			burst.clear();
			stringstream  lineStream(line);
			string        cell;
			getline(lineStream, cell, ',');
			PID = stoi(cell);
			getline(lineStream, cell, ',');
			arrivalTime = stoi(cell);
			while (getline(lineStream, cell, ','))
			{
				burst.push_back(stoi(cell));
			}
			PCB *tempPCB = new PCB(PID, burst, arrivalTime);
			myVec.push_back(*tempPCB);
		}
		currentFile.close();
	}
	else
	{
		cout << "Error opening file";
	}
	
	return myVec;
}

void runSimulate(list<PCB> myTable) {
	runFCFS(myTable);
	cout << "Finished FCFS" << endl;
	runRR(myTable, 10);
	cout << "Finished RR, 100" << endl;
	runRR(myTable, 20);
	cout << "Finished RR, 20" << endl;
	runRR(myTable, 30);
	cout << "Finished RR, 30" << endl;
}

void runFCFS(list<PCB> myTable) {
	runRR(myTable, LLONG_MAX);
}

void runRR(list<PCB> myTable, long long timeQuantum) {
	
	vector<PCB> pq;
	vector<PCB> io;
	vector<PCB> finished;

	vector<PCB>::iterator it;

	PCB Current;

	state processor = ready;

	int totalProcesses = myTable.size();
	int finishedProcesses = 0;

	int globalTime = 0;

	int temp = 0;

	timeQuantum--; //Because we start at 0

	//Do while processes still need to run
	while (finishedProcesses < totalProcesses) {

		//	Add any processes that have finished IO (add it to the front of myTable because we can resuse the sorting code
		//	below and its arrivale time will definatly be lower than globaltime
		for (int i = 0; i < io.size(); i++) {
			if (io[i].getCurrentState() == ready) {
				myTable.push_front(io[i]);
				it = io.begin() + i;
				io.erase(it);
			}
			else if (io[i].getCurrentState() == stopped){
				finished.push_back(io[i]);
				it = io.begin() + i;
				io.erase(it);
				finishedProcesses++;
			}
			else {
				//Do nothing
			}
		}

		//Add any processes that have arrived!   
		while (myTable.size() > 0) {
			if (globalTime >= myTable.front().getArrivalTime()) {
				if (pq.size() == 0) {
					pq.push_back(myTable.front());
				}
				else {
					temp = 0;
					while (myTable.front().getPriority() + 1 > pq[temp].getPriority()) {		//We use +1 here so it gets puts at the bac of the number segments
						if (temp >= pq.size() - 1) {
							break;
						}
						temp++;
					}
					vector<PCB>::iterator it = pq.begin() + temp;
					pq.insert(it, myTable.front());
				}
				myTable.pop_front();
			}
			else {
				break;
			}
		}

		//Move things in/out from cpu if needed
		if (processor == working_cpu) {
			//Move front to IO if it is now working in IO
			if (Current.getCurrentState() == working_io) {
				io.push_back(Current);
				processor = ready;
			}
			//Move front to Finished if it is done
			else if (Current.getCurrentState() == stopped) {
				finished.push_back(Current);
				processor = ready;
				finishedProcesses++;
			}
			//Move front if time is up (use >  in case something wierd happened)
			else if (Current.getWorkTime() >= timeQuantum) {
				Current.setWorkTime(0);
				myTable.push_front(Current);
				processor = ready;
			}
			else {
				//PCB keeps running in processor
			}
		}
		else {
			//Move front of queue into current and start running current
			if (pq.size() > 0) {
				Current = pq.front();
				Current.setCurrentState(working_cpu);
				it = pq.begin();
				pq.erase(it);

				processor = working_cpu;
			}
			else {
				//Nothing needs to run in the processor?!?!?!
			}
		}
		//Tick current
		Current.tick();
		//Tick everything waiting for CPU
		for (int i = 0; i < pq.size(); i++) {
			pq[i].tick();
		}
		//Tick everything working in IO
		for (int i = 0; i < io.size(); i++) {
			io[i].tick();
		}

		globalTime++;
	}
	tabulate();
}
void tabulate() {

}