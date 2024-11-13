#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<queue>

using namespace std;

class Process {
public:
    string name;
    int burst;
    int arrival;
    int remaining; // To track remaining burst time
    std::string timeline; // For graph representation
    // int remaining; // To track remaining burst time for SJF and RR
    // int startTime = -1; // To track the first time the process starts
    Process(string n, int b, int a) {
        name = n;
        burst = b;
        arrival = a;
    }
    void print(){
        cout << name << " " << burst << " " << arrival << endl;
    }
};

class Compare {//the custom comparator for the priority queue
    public:
       bool operator()(Process& lhs, Process& rhs){//overloading the operator method
           if(lhs.arrival > rhs.arrival){//comparing
               return true;
           }
           return false;
      }
};

vector<Process> readFile(){
    ifstream processes("processes.txt");
    vector<string> tokens;
    string line;
    vector<Process> process;
    while (getline(processes, line, '\n'))
    {
        size_t pos = 0;
        std::string token;
        while ((pos = line.find(' ')) != string::npos) {
            token = line.substr(0, pos);
            tokens.push_back(token);
            line.erase(0, pos+1);
        }
        tokens.push_back(line);
        process.push_back(Process(tokens[0], stoi(tokens[1]), stoi(tokens[2])));
        tokens.clear();
    }
    priority_queue<Process, std::vector<Process>, Compare> pq;
    for(Process p: process){//pushing passengers to the priority queue
        pq.push(p);
    }
    process.clear();
    while(!pq.empty()){
        process.push_back(pq.top());
        pq.pop();
    }
    processes.close();
    return process;
}

void fifo(vector<Process> p){
    int time = p[0].arrival;
    //int timeline = p[0].arrival + p[0].burst;
    double avgtime = 0;
    double resptime = 0;
    int thr = 0;
    cout << "\nFIFO Schedule:\n" << "        0123456789\n";
    for (size_t i = 0; i < p.size(); i++)
    {
        if(time == 10){
            thr = i;
        }
        cout << p[i].name;
        avgtime += time;
        for (size_t j = 0; j < time; j++)
        {
            if(j>= p[i].arrival){
                resptime++;
                cout << "_";
            }
            else{
                cout << " ";
            }
        }
        while (p[i].burst>0)
        {
            cout << "#";
            p[i].burst--;
            time++;
        }
        cout << endl;
    }
    cout << "Average wait time: " << avgtime/p.size() << endl;
    cout << "Average response time: " << resptime/p.size() << endl;
    cout << "Throughput over 10 cycles: " << thr << endl;
}

// void sjf(std::vector<Process> processes) {
//     int time = 0;
//     int completed = 0;
//     int n = processes.size();

//     std::cout << "\nSJF with Preemption Schedule:\n";
    
//     while (completed < n) {
//         // Find the process with the shortest remaining time that has arrived
//         int minIndex = -1;
//         for (int i = 0; i < n; ++i) {
//             if (processes[i].arrival <= time && processes[i].remaining > 0) {
//                 if (minIndex == -1 || processes[i].remaining < processes[minIndex].remaining) {
//                     minIndex = i;
//                 }
//             }
//         }

//         if (minIndex != -1) {
//             // Run the chosen process for 1 time unit
//             processes[minIndex].timeline += "#";
//             processes[minIndex].remaining--;
            
//             // Check if process is completed
//             if (processes[minIndex].remaining == 0) {
//                 completed++;
//             }
//         } else {
//             // If no process is ready, advance time with idle
//             for (auto& p : processes) {
//                 if (p.arrival <= time && p.remaining > 0) p.timeline += "_";
//             }
//         }
//         time++;
//     }

//     // Print the timeline for each process
//     for (const auto& process : processes) {
//         std::cout << process.name << " " << process.timeline << std::endl;
//     }
// }

// void roundRobin(std::vector<Process> processes, int quantum) {
//     int time = 0;
//     int n = processes.size();
//     std::queue<int> readyQueue;
//     int completed = 0;

//     // Sort processes by arrival time
//     std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
//         return a.arrival < b.arrival;
//     });

//     std::cout << "\nRound Robin Schedule:\n";
    
//     int index = 0;
//     while (completed < n) {
//         // Add all processes that have arrived to the ready queue
//         while (index < n && processes[index].arrival <= time) {
//             readyQueue.push(index);
//             index++;
//         }

//         if (!readyQueue.empty()) {
//             int currIndex = readyQueue.front();
//             readyQueue.pop();

//             // Run process for one quantum
//             processes[currIndex].timeline += "#";
//             processes[currIndex].remaining--;
//             time++;

//             // Check if the process is completed
//             if (processes[currIndex].remaining == 0) {
//                 completed++;
//             } else {
//                 // If not completed, add it back to the queue if other processes are waiting
//                 readyQueue.push(currIndex);
//             }

//             // Add "_" for waiting processes at each time step
//             for (int i = 0; i < n; ++i) {
//                 if (i != currIndex && processes[i].arrival <= time && processes[i].remaining > 0) {
//                     processes[i].timeline += "_";
//                 }
//             }
//         } else {
//             // If no process is ready, increment time with idle
//             for (auto& process : processes) {
//                 if (process.arrival <= time && process.remaining > 0) process.timeline += "_";
//             }
//             time++;
//         }
//     }

//     // Print the timeline for each process
//     for (const auto& process : processes) {
//         std::cout << process.name << " " << process.timeline << std::endl;
//     }
// }

int main(){
    vector<Process> processes = readFile();
    for(Process p: processes){
        p.print();
    }
    fifo(processes);
    // sjf(processes);
    // roundRobin(processes, 1);
    return 0;
}