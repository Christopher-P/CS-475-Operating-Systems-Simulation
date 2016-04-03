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

const int NUM_FILES = 100;
const int MAX_PRIO = 10;
const int MIN_CONTEXT = 0;
const int MAX_CONTEXT = 10;

int dataSet = 0;

class PCB;

void runSimulate(list<PCB>);
void runRR(list<PCB>, long long, int);
void runFCFS(list<PCB>, int);
void tabulate(vector<PCB>, double);

void createFiles();
list<PCB> readFile(int);
void writeUp(vector<PCB>, int, int);

int main()
{
	list<PCB> myTable;
	//createFiles();
	for (int i = 0; i < NUM_FILES; i++) {
		dataSet = i;
		myTable = readFile(i);
		runSimulate(myTable);
		cout << "Finished Data Set: " << i << endl;
	}	
}

enum state
{
	ready, stopped, terminating
};
class PCB
{
private:
	int waitTime;
	int totalTime;
	int arrival_time;
	int finishTime;
	int nextTime;
	int firstTime;
	
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
		firstTime = -1;
		waitTime = totalTime = 0;
		current_state = ready;
		PID = os_pid;
		burst = os_burst;
		arrival_time = os_arrival_time;
		this->priority = rand() % MAX_PRIO;
	}
	void setFinalTime(int time) {
		finishTime = time;
	}
	void advanceWait(int time) {
		waitTime += time;
		totalTime += time;
	}
	void advanceIO(int globalTime) {
		setNextTime(getNextTime(), burst.front());		//Add IO time needed to current time needed
		burst.pop_front();
		if (burst.size() > 0) {
			//setPriority(rand() % MAX_PRIO);			//Uncomment to add random priorities between cpu calls
			//do nothing cause switching is handled in processor
		}
		else {
			setFinalTime(getNextTime());
			setCurrentState(stopped);
		}
	}
	void advanceCPU(int time, int globalTime) {
		if (time == burst.front()) {
			setNextTime(globalTime, time);
			burst.pop_front();
			if (burst.size() > 0) {
				advanceIO(globalTime);
			}
			else {
				setFinalTime(getNextTime());
				setCurrentState(stopped);
			}
		}
		else {
			temp = burst.front();
			temp = temp - time;
			burst.pop_front();
			burst.push_front(temp);
			setCurrentState(ready);
		}
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
	int getArrivalTime() {
		return arrival_time;
	}
	int getTotalTime() {
		return finishTime - arrival_time;
	}
	int getWaitTime() {
		return waitTime;
	}
	int getNextTime() {
		return nextTime;
	}
	void setNextTime(int currentTime, int advanceTime) {
		nextTime = currentTime + advanceTime;
	}
	void setFirstTime(int globalTime) {
		if (firstTime == -1) {
			firstTime = globalTime;
		}
	}
	int getFirstTime() {
		return firstTime;
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

	int CPUS = 0;

	for (int i = 0; i < 5; i++) {
		CPUS = pow(2, i);		//run with 1, 2, 4, 8, 16 Processors

		runFCFS(myTable, CPUS);
		cout << "Finished FCFS" << endl;
		runRR(myTable, 10, CPUS);
		cout << "Finished RR, 10" << endl;
		runRR(myTable, 20, CPUS);
		cout << "Finished RR, 20" << endl;
		runRR(myTable, 30, CPUS);
		cout << "Finished RR, 30" << endl;
	}
}

int contextSwitch() {
	return rand() % (MAX_CONTEXT - MIN_CONTEXT + 1) + MIN_CONTEXT;	//+1 because it is non inclusive
}

void runFCFS(list<PCB> myTable, int CPUS) {
	runRR(myTable, LLONG_MAX, CPUS);
}

bool cmd(PCB& s1, PCB& s2)
{
	return s1.getPID() < s2.getPID();
}

void runRR(list<PCB> myTable, long long timeQuantum, int CPUS) {

		vector<PCB> pq;				//simulate priority queue
		vector<PCB> io;				//Place to hold processes that are working in IO
		vector<PCB> finished;		//Holds finished processes

		vector<PCB>::iterator it;	//Used to manipulate vector positions

		vector<PCB> Current;		//Holds the current processes in their processors

		int totalProcesses = myTable.size();	//Used to test if all the processes are done running

		int globalTime = 0;						//simulated time

		int advanceTime = 0;					//time used to advance each process

		int temp = 0;							//temp variable used randomly to hold things

		int wastedTime = 0;						//Time Processor spent context switching

		double CScounter = 0;					//Number of context switches
		double CStotal = 0;						//Total time spent context switching

		
		vector<int> nextSwitch;

		timeQuantum--; //Because we start at 0

		//Fill the next switch vector to size of CPUSs
		for (int i = 0; i < CPUS; i++) {
			nextSwitch.push_back(0);
			Current.push_back(PCB());
		}

		//EVENT BASED
		while (true) {

			/*************SECTION 1 -- ADD ANY COMPLETED IO PROCESSES  OR NEW PROCESSES   TO THE WAITING QUEUE*********/
			//add any completed IO process to mytable so it can be sorted into the pq (code reuse!)
			for (int i = 0; i < io.size(); i++) {
				if (io[i].getCurrentState() == stopped) {
					finished.push_back(io[i]);
					it = io.begin() + i;
					io.erase(it);
				}
				if (globalTime >= io[i].getNextTime()) {
					myTable.push_front(io[i]);
					it = io.begin() + i;
					io.erase(it);
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
			/********************SECTION 2 -- COMPLETE CONTEXT SWITCH && advance next process ************************************************************/
			for (int i = 0; i < CPUS; i++) {
				if (globalTime >= nextSwitch[i]) {
					if (pq.size() > 0) {
						//Load next process into processor
						Current[i] = pq.front();

						//Set first time
						Current[i].setFirstTime(globalTime);

						//Remove next process from front of queue
						it = pq.begin();
						pq.erase(it);
						//Check if burst is smaller than timeQuantum
						if (Current[i].getBurst() > timeQuantum) {
							advanceTime = timeQuantum;
						}
						else {
							advanceTime = Current[i].getBurst();
						}
						//Do work on the process
						Current[i].advanceCPU(advanceTime, globalTime);

						//Check to see if the process is done or if it needs to do IO next
						if (Current[i].getCurrentState() == stopped) {
							finished.push_back(Current[i]);
						}
						else if (Current[i].getCurrentState() == ready) {
							myTable.push_front(Current[i]);
						}
						else {
							io.push_back(Current[i]);
						}
						//get new context switch time
						temp = contextSwitch();

						//Increase wasted time
						wastedTime += temp;

						//Since context switch time is bundled into this we add it to the time needed for the proccess
						nextSwitch[i] = globalTime + temp + advanceTime;

						CScounter++;
						CStotal += temp;
					}
				}
				else {
					//either context switching or working on process
				}
			}
			/**********************SECTION 3 ADVANCE WAIT TIME FOR EACH PROCESS THAT IS WAITING**************************************************************/

			for (int i = 0; i < pq.size(); i++) {
				pq[i].advanceWait(1);
			}

			/**************SECTION 4  INCREMENT  GLOBAL COUNTER AND CHECK IF IT IS DONE**************88*/
			if (finished.size() == totalProcesses) {
				break;
			}
			globalTime++;
		}
		sort(finished.begin(), finished.end(), cmd);
		if (timeQuantum > 10000) {
			writeUp(finished, CPUS, -1);
		}
		else {
			timeQuantum++; //since it was decremented before
			writeUp(finished, CPUS, timeQuantum);
		}
		CStotal = (CStotal / CScounter);	//put average in CStotal
		tabulate(finished, CStotal);
}
void writeUp(vector<PCB> list, int CPUs, int timeQuantum) {
	FILE * pFile;
	char filename[NUM_FILES];								//charArray for name

	
		sprintf(filename, "Data_%i_%iCPU_%iquantum.csv", dataSet + 1, CPUs, timeQuantum);				//Put created filename into the charArray 
		pFile = fopen(filename, "w");						//Create File with name and open it

		fprintf(pFile, "PID, Wait Time, Turnaround, Response");
		fprintf(pFile, "\n");		
		for (int i = 0; i < list.size(); i++)						// for each process in list add a new entry
		{
			fprintf(pFile, "%i, ", list[i].getPID());						
			fprintf(pFile, "%i, ", list[i].getWaitTime());
			fprintf(pFile, "%i, ", list[i].getTotalTime());
			fprintf(pFile, "%i", list[i].getFirstTime() - list[i].getArrivalTime());
			fprintf(pFile, "\n");							//Create newline at end of process
		}
		fclose(pFile);										//Close the file when done
		printf("File generated: \"%s\"\n", filename);		//Print out the filename to console saying it is created
	
}
void tabulate(vector<PCB> processes, double avgCS) {
	double throughputTime = 0;
	double averageThroughputTime = 0;
	double T = 0;
	double avgT = 0;
	double W = 0;
	double avgW = 0;
	double R = 0;
	double avgR = 0;
	for (int i = 0; i < processes.size(); i++) {
		T = T + (processes[i].getTotalTime());
		W += processes[i].getWaitTime();
		R = R + (processes[i].getFirstTime() - processes[i].getArrivalTime());
	}
	avgW = W / processes.size();
	cout << "Average Wait Time: " << avgW << endl;
	avgT = T / processes.size();
	cout << "Average Turnaround time: " << avgT << endl;
	avgR = R / processes.size();
	cout << "Average Response time: " << avgR << endl;
	cout << "Average Context Switch time: " << avgCS<< endl;
}