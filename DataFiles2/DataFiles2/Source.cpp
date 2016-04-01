#include <random>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <string>
#include <queue>
#include <vector>
#include <fstream>
using namespace std;

void createFiles();
struct PCB;
vector<PCB> readFiles();
const int NUM_FILES = 100;


int main()
{
	createFiles();
	//readFiles();
	
}

void createFiles() {
	FILE * pFile;
	srand(time(NULL));
	char filename[NUM_FILES];								//charArray for name
	int arrival_time = 0;									//Used to be incrementally random
	int j = 0;												//Number to hold bursts  in each process

	for (int l = 0; l < 100; l++)							//Create a hundred files
	{
		sprintf(filename, "Data_%i.csv", l + 1);				//Put created filename into the charArray 
		pFile = fopen(filename, "w");						//Create File with name and open it

		for (int i = 0; i < 100; i++)						//Create a hundred processes in the file
		{
			j = rand() % 10;								//Create number of bursts in each process
			fprintf(pFile, "%i, ", i++);					//Send PID to each process
			fprintf(pFile, "%i, ", arrival_time);			//Incrementally random arrival time
			for (int n = -1; n < j; n++)					//Set to -1 so it runs atleast once
			{
				fprintf(pFile, "%i, ", rand() % 50 + 1);	//Random burst for each process burst time
			}
			fprintf(pFile, "\n");							//Create newline at end of process
			arrival_time += rand() % 20 + 1;				//Increment random arrival time
		}
		fclose(pFile);										//Close the file when done
		printf("File generated: \"%s\"\n", filename);		//Print out the filename to console saying it is created
	}
}

vector<PCB> readFiles() {
	vector<PCB> myVec;
	ifstream currentFile;
	char filename[NUM_FILES];
	string line;
	for (int i = 0; i < NUM_FILES; i++)
	{
		sprintf(filename, "Data_%i.csv", i+1);
		currentFile.open(filename);
		while (!currentFile.eof())
		{
			getline(currentFile, line);
			char * section;
			//section = strtok(line.c_str, ",");
			if (section != NULL)
			{

			}
		}
	}
	
	return  myVec;
}

enum state 
{
	ready, stopped, waiting, terminating, created
};
struct PCB 
{
	int T, W, R, priority, PID, arrival_time;
	queue<int> burst;
	state current_state;
	PCB(int os_pid, queue<int> os_burst, int os_arrival_time)
	{
		T = W = R = 0;
		current_state = created;
		PID = os_pid;
		burst = os_burst;
		arrival_time = os_arrival_time;
	}
	void increment_T()
	{
		T++;
	}
	int getBurst() 
	{
		return burst.front();
	}
	int getNextBurst() 
	{
		burst.pop();
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
};