#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Represents a process with its attributes.
 */
typedef struct {
    int id;          ///< Unique identifier for the process.
    int arrival;     ///< Arrival time of the process.
    int burst;       ///< Total burst time required by the process.
    int remaining;   ///< Remaining burst time of the process.
    int start_time;  ///< Start time of the process execution. 
    int finish_time; ///< Finish time of the process execution.
} Process;

/**
 * @brief Reads process information from a file.
 *
 * @param filename The name of the file containing process data.
 * @param processes An array of Process structures to store the read data.
 * @return The number of processes read from the file.
 *
 * @details
 * The function opens the specified file, skips the header line, and reads process data
 * in the format "id arrival burst" from each subsequent line.  It populates the
 * processes array with the read data.  Error handling is included for file opening.
 * The remaining field is initialized to the burst time, and start_time and
 * finish_time are initialized to -1.
 */
int readProcesses(const char *filename, Process processes[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    fgets(line, sizeof(line), file); // Skip header
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        int id, arrival, burst;
        if (sscanf(line, "%d %d %d", &id, &arrival, &burst) == 3) {
            processes[count].id = id;
            processes[count].arrival = arrival;
            processes[count].burst = burst;
            processes[count].remaining = burst;
            processes[count].start_time;
            processes[count].finish_time;
            count++;
        }
    }

    fclose(file);
    return count;
}


/**
 * @brief Computes the First-Come, First-Served (FCFS) scheduling algorithm.
 *
 * @param processes An array of Process structures.
 * @param n The number of processes in the array.
 *
 * @details
 * This function implements the FCFS scheduling algorithm. It iterates through the
 * processes in the order they appear in the array and calculates the start and finish
 * times for each process based on the current time.
 */
void computeFCFS(Process processes[], int n) {
    int current_time = 0;
    for (int i = 0; i < n; i++) {
        if (processes[i].arrival > current_time)
            current_time = processes[i].arrival;
        processes[i].start_time = current_time;
        processes[i].finish_time = current_time + processes[i].burst;
        current_time = processes[i].finish_time;
    }
}

/**
 * @brief Computes the Round Robin (RR) scheduling algorithm.
 *
 * @param processes An array of Process structures.
 * @param n The number of processes in the array.
 * @param quantum The time quantum for the RR algorithm.
 *
 * @details
 * This function implements the RR scheduling algorithm. It uses a queue to manage
 * the processes and simulates the execution of each process for a time quantum.
 * It calculates the start and finish times for each process.
 */
void computeRR(Process processes[], int n, int quantum) {
    int *remaining = malloc(n * sizeof(int));
    int *start_time = malloc(n * sizeof(int));
    int *finish_time = malloc(n * sizeof(int));
    
    for (int i = 0; i < n; i++) {
        remaining[i] = processes[i].burst;
        start_time[i] = -1;
        finish_time[i] = -1;
    }

    int queue[1000], front = 0, rear = -1, size = 0;
    int current_time = 0, idx = 0, completed = 0;

    while (idx < n && processes[idx].arrival <= current_time) {
        queue[++rear] = idx++;
        size++;
    }

    while (completed < n) {
        if (size == 0) {
            current_time++;
            while (idx < n && processes[idx].arrival <= current_time) {
                queue[++rear] = idx++;
                size++;
            }
            continue;
        }

        int proc_idx = queue[front++];
        size--;

        if (start_time[proc_idx] == -1)
            start_time[proc_idx] = current_time;

        int exec_time = (remaining[proc_idx] < quantum) ? remaining[proc_idx] : quantum;
        remaining[proc_idx] -= exec_time;
        current_time += exec_time;

        while (idx < n && processes[idx].arrival <= current_time) {
            queue[++rear] = idx++;
            size++;
        }

        if (remaining[proc_idx] > 0) {
            queue[++rear] = proc_idx;
            size++;
        } else {
            finish_time[proc_idx] = current_time;
            completed++;
        }
    }

    for (int i = 0; i < n; i++) {
        processes[i].start_time = start_time[i];
        processes[i].finish_time = finish_time[i];
    }


    free(remaining);
    free(start_time);
    free(finish_time);
}

/**
 * @brief Calculates performance metrics for the scheduling algorithms.
 *
 * @param processes An array of Process structures.
 * @param n The number of processes in the array.
 * @param avg_tat A pointer to a float variable to store the average turnaround time.
 * @param avg_rt A pointer to a float variable to store the average response time.
 * @param throughput A pointer to a float variable to store the throughput.
 *f
 * @details
 * This function calculates the average turnaround time, average response time, and
 * throughput based on the start and finish times of the processes.
 */
void calculateMetrics(Process processes[], int n, float *avg_tat, float *avg_rt, float *throughput) {
    float total_tat = 0, total_rt = 0;
    int max_finish = 0;

    for (int i = 0; i < n; i++) {
        int tat = processes[i].finish_time - processes[i].arrival; //Turnaround Time
        int rt = processes[i].start_time - processes[i].arrival; //Response Time
        total_tat += tat;
        total_rt += rt;
        if (processes[i].finish_time > max_finish)
            max_finish = processes[i].finish_time;
    }

    *avg_tat = total_tat / n; // Total TAT and RT are divided by the number of processes (n) to get the averages.
    *avg_rt = total_rt / n;
    *throughput = (float)n / max_finish;
}

/**
 * @brief Main function of the program.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of strings representing the command-line arguments.
 * @return An integer indicating the exit status of the program.
 *
 * @details
 * This function reads process data from a file specified as a command-line argument,
 * computes the FCFS and RR scheduling algorithms, and prints the performance metrics
 * for each algorithm.
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <process_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    Process processes[100];
    int n = readProcesses(argv[1], processes);

    // FCFS
    computeFCFS(processes, n);
    float fcfs_tat, fcfs_rt, fcfs_throughput;
    calculateMetrics(processes, n, &fcfs_tat, &fcfs_rt, &fcfs_throughput);

    // Reset for RR
    for (int i = 0; i < n; i++) {
        processes[i].start_time;
        processes[i].finish_time;
        processes[i].remaining = processes[i].burst;
    }

    // RR
    computeRR(processes, n, 1);
    float rr_tat, rr_rt, rr_throughput;
    calculateMetrics(processes, n, &rr_tat, &rr_rt, &rr_throughput);

    printf("FCFS Scheduling:\n");
    printf("Average Turnaround Time: %.2f\n", fcfs_tat);
    printf("Average Response Time: %.2f\n", fcfs_rt);
    printf("Throughput: %.2f processes/ut\n\n", fcfs_throughput);

    printf("Round Robin Scheduling (Quantum=1):\n");
    printf("Average Turnaround Time: %.2f\n", rr_tat);
    printf("Average Response Time: %.2f\n", rr_rt);
    printf("Throughput: %.2f processes/ut\n", rr_throughput);

    return 0;
}