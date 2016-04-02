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
void runRR(list<PCB>, int);
void runFCFS(list<PCB>);

int main()
{
	list<PCB> myTable;
	createFiles();
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
			//Do nothing cause it is don
		}
		else if (current_state == waiting){
			waitTime++;
		}
		else if (current_state == working_io) {
			burst.front()--;
			if (burst.front() < 0) {
				if (burst.size() > 0) {
					burst.pop_front();
					current_state = ready;
					//priority = rand() % MAX_PRIO;				//Uncomment to allow for changing PRIO between cpu calls
				}
				else {
					current_state = stopped;
				}
			}
			workTime++;
		}
		else if (current_state == working_cpu) {
			burst.front()--;
			if (burst.front() < 0) {
				if (burst.size() > 0) {
					current_state = working_io;
					burst.pop_front();
				}
				else {
					current_state = stopped;
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
	runRR(myTable, 10);
	runRR(myTable, 20);
	runRR(myTable, 30);
}

void runFCFS(list<PCB> myTable) {
	runRR(myTable, INT_MAX);
}

void runRR(list<PCB> myTable, int timeQuantum) {
	
	list<PCB> pq;
	state processor = ready;

	int totalProcesses = myTable.size();
	int finishedProcesses = 0;

	int globalTime = 0;

	while (totalProcesses <= finishedProcesses) {

		//Add any processes that have arrived!   ">" incase someting goes wrong
		if (myTable.size() > 0) {
			if (myTable.front().getArrivalTime() >= globalTime) {
				insert(myTable.front, pq);
				myTable.pop_front();
			}
		}
		if (processor == working_cpu) {
			
		}
		else {

		}
		globalTime++;
	}
	
}

//Sorted by priority
list<PCB> insert(PCB pcb, list<PCB> lpcb) {
	list<PCB>::iterator it = std::next(lpcb.begin(), 0);
	int count = 0;
	while (pcb.getPriority() <= it->getPriority()) {
		it = next(it, 1);
		count++;
	}
	lpcb.insert(it, pcb);
}
void tabulate() {

}