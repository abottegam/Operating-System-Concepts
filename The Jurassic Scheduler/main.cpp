#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<queue>
#include<algorithm>

using namespace std;

class Process {
public:
    string name;
    int burst;
    int arrival;
    int start_time;       // For calculating response time
    int end_time;         // For calculating waiting time
    bool started = false; // To check if process has started (for response time calculation)
    string out;

    Process(string n, int b, int a) {
        name = n;
        burst = b;
        arrival = a;
    }
    void print(){
        cout << name << " " << burst << " " << arrival << " " << out << endl;
    }
};

bool byBurstPointer(Process* a, Process* b) {
    return a->burst < b->burst;
}

bool byBurst (Process& a, Process& b) { return a.burst < b.burst; }
bool byArrival (Process& a, Process& b) { return a.arrival < b.arrival; }


// Static function to sort by arrival time

vector<Process> readFile(){
    ifstream input("processes.txt");
    string line;
    vector<string> tokens;
    vector<Process> processes;
    while (getline(input, line, '\n'))
    {
        int pos = 0;
        std::string token;
        while ((pos = line.find(' ')) != string::npos) {
            token = line.substr(0, pos);
            tokens.push_back(token);
            line.erase(0, pos+1);
        }
        tokens.push_back(line);
        processes.push_back(Process(tokens[0], stoi(tokens[1]), stoi(tokens[2])));
        tokens.clear();
    }
    sort(processes.begin(), processes.end(), byArrival);   
    return processes;
}



void fifo(vector<Process> p){
    int time = p[0].arrival;
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

void sjf(vector<Process> processes) {
    int time = 0;
    double total_wait_time = 0, total_response_time = 0;
    int completed = 0, thr = 0;
    vector<Process*> ready_queue;

    cout << "\nSJF Schedule:\n        01234567890123456789\n";

    sort(processes.begin(), processes.end(), byArrival);

    while (completed < processes.size()) {
        // Add newly arrived processes to the ready queue
        for (size_t i = 0; i < processes.size(); ++i) {
            Process& process = processes[i];
            if (process.arrival <= time && process.burst > 0 &&
                find(ready_queue.begin(), ready_queue.end(), &process) == ready_queue.end()) {
                ready_queue.push_back(&process);
            }
        }

        sort(ready_queue.begin(), ready_queue.end(), byBurstPointer);

        if (!ready_queue.empty()) {
            Process* current = ready_queue.front();
            if (!current->started) {
                current->start_time = time;
                current->started = true;
            }

            while (current->out.size() < current->arrival) {
                current->out += " ";
            }
            while (current->out.size() < time) {
                current->out += "_";
            }

            // Execute one unit of burst time
            current->out += "#";
            current->burst--;
            time++;
            if (current->burst == 0) {
                current->end_time = time;
                total_wait_time += (current->end_time - current->arrival - (current->end_time - current->start_time));
                total_response_time += (current->start_time - current->arrival);
                completed++;
                ready_queue.erase(ready_queue.begin());
                if (time <= 10) thr++;
            }
        } else {
            for (size_t i = 0; i < processes.size(); ++i) {
                processes[i].out += " ";
            }
            time++;
        }
    }
    for (size_t i = 0; i < processes.size(); ++i) {
        cout << processes[i].name << processes[i].out << endl;
    }

    cout << "Average wait time: " << total_wait_time / processes.size() << endl;
    cout << "Average response time: " << total_response_time / processes.size() << endl;
    cout << "Throughput over 10 cycles: " << thr << endl;
}


int main(){
    vector<Process> processes = readFile();
    cout << "Original processes:" << endl;
    fifo(processes);
    sjf(processes);
    return 0;
}