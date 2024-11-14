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
    int wait_time;
    bool started;
    string out;

    Process(string n, int b, int a) {
        name = n;
        burst = b;
        arrival = a;
        started = false;
    }
};

bool byBurstPointer(Process* a, Process* b) {return a->burst < b->burst;}
bool byArrivalPointer(Process* a, Process* b) {return a->arrival < b->arrival;}
bool byBurst (Process& a, Process& b) { return a.burst < b.burst; }
bool byArrival (Process& a, Process& b) { return a.arrival < b.arrival; }

void print(vector<Process> p, int awt, int art, int t){
    for (size_t i = 0; i < p.size(); i++)
    {
         cout << p[i].name << "\t" << p[i].out << endl;
    }
    cout << "Average wait time: " << awt << endl;
    cout << "Average response time: " << art << endl;
    cout << "Throughput over 10 cycles: " << t << endl;
    
}
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
    double avgwaitime = 0;
    double avgresptime = 0;
    int thr = 0;
    cout << "\nFIFO Schedule:" << endl;
    for (size_t i = 0; i < p.size(); i++)
    {
        for (size_t j = 0; j < time; j++)
        {
            if(j>= p[i].arrival){
                p[i].wait_time++;
                p[i].out.append("_");
            }
            else{
                p[i].out.append(" ");
            }
        }
        p[i].start_time = time;
        while (p[i].burst>0)
        {
            p[i].out.append("#");
            p[i].burst--;
            if(time == 10){
                thr = i;
            }
            time++;
        }
    }
    for (size_t i = 0; i < p.size(); ++i) {
        avgresptime += p[i].start_time;
        avgwaitime += p[i].wait_time;
    }
    print(p, avgwaitime/p.size(), avgresptime/p.size(), thr);

}

void sjf(vector<Process> p) {
    int time = 0;
    double avgwaitime = 0, avgresptime = 0;
    int completed = 0, thr = 0;
    vector<Process*> ready_queue;

    cout << "\nSJF Schedule:\n";

    sort(p.begin(), p.end(), byArrival);

    while (completed < p.size()) {
        // Add newly arrived processes to the ready queue
        for (size_t i = 0; i < p.size(); ++i) {
            Process& process = p[i];
            if (process.arrival <= time && process.burst > 0 && find(ready_queue.begin(), ready_queue.end(), &process) == ready_queue.end()) {
                ready_queue.push_back(&process);
            }
        }

        sort(ready_queue.begin(), ready_queue.end(), byBurstPointer);

        if (!ready_queue.empty()) {
            Process* first = ready_queue.front();
            if (!first->started) {
                first->start_time = time;
                first->started = true;
            }

            while (first->out.size() < first->arrival) {
                first->out += " ";
            }
            while (first->out.size() < time) {
                first->out += "_";
            }

            // Execute one unit of burst time
            first->out += "#";
            first->burst--;
            time++;
            if (first->burst == 0) {
                first->end_time = time;
                avgwaitime += (first->end_time - first->arrival - (first->end_time - first->start_time));
                avgresptime += (first->start_time - first->arrival);
                completed++;
                ready_queue.erase(ready_queue.begin());
                if (time <= 10) thr++;
            }
        } else {
            for (size_t i = 0; i < p.size(); ++i) {
                p[i].out += " ";
            }
            time++;
        }
    }
    print(p, avgwaitime/p.size(), avgresptime/p.size(), thr);
}

void round_robin(vector<Process> p, int quantum = 1) {
    int time = 0;
    double avgwaitime = 0, avgresptime = 0;
    int completed = 0, thr = 0;
    queue<Process*> ready_queue;

    cout << "\nRound Robin Schedule:\n";

    vector<Process*> sorted_processes;
    for (size_t i = 0; i < p.size(); ++i) {
        sorted_processes.push_back(&p[i]);
    }
    sort(sorted_processes.begin(), sorted_processes.end(), byArrivalPointer);

    int next_process_index = 0;
    int n = sorted_processes.size();

    while (completed < n) {
        while (next_process_index < n && sorted_processes[next_process_index]->arrival <= time) {
            ready_queue.push(sorted_processes[next_process_index]);
            next_process_index++;
        }

        if (!ready_queue.empty()) {
            Process* first = ready_queue.front();
            ready_queue.pop();

            if (!first->started) {
                first->start_time = time;
                first->started = true;
            }

            while (first->out.length() < (size_t)first->arrival) {
                first->out += " ";
            }
            while (first->out.length() < (size_t)time) {
                first->out += "_";
            }

            int time_slice = min(quantum, first->burst);
            for (int i = 0; i < time_slice; ++i) {
                first->out += "#";
            }
            time += time_slice;
            first->burst -= time_slice;

            while (next_process_index < n && sorted_processes[next_process_index]->arrival <= time) {
                ready_queue.push(sorted_processes[next_process_index]);
                next_process_index++;
            }

            if (first->burst == 0) {
                first->end_time = time;
                int turnaround_time = first->end_time - first->arrival;
                int wait_time = turnaround_time - (first->burst + time_slice);
                avgwaitime += wait_time;
                avgresptime += (first->start_time - first->arrival);
                completed++;
                if (time <= 10) thr++;
            } else {
                ready_queue.push(first);
            }
        } else {
            time++;
        }
    }

    for (size_t i = 0; i < p.size(); ++i) {
        while (p[i].out.length() < (size_t)time) {
            p[i].out += " ";
        }
    }
    print(p, avgwaitime/p.size(), avgresptime/p.size(), thr);

}


int main(){
    vector<Process> processes = readFile();
    fifo(processes);
    sjf(processes);
    round_robin(processes);
    return 0;
}