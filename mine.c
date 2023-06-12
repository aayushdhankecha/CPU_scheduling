// 2CS403 Operating System

#include <stdio.h>
#include <stdlib.h>

struct chart {
    int data;
    struct chart* next;
};
struct chart* gc;

struct chart* create(int d) {
    struct chart* new;
    new=(struct chart*)malloc(sizeof(struct chart));
    new->data=d;
    new->next=NULL;
    return new;
}

void Display_gantt_chart() {
    struct chart* ptr=gc;
    while (ptr!=NULL) {
        printf("P%d ", ptr->data);
        ptr=ptr->next;
    }
    printf("\n");
}

typedef struct process {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int remaining_time;
    int turnaround_time;
    int waiting_time;
} P;

void Sort_by_arrival_time(P *p,int n) {
     P temp;
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (p[i].arrival_time>p[j].arrival_time) {
                temp=p[i];
                p[i]=p[j];
                p[j]=temp;
            }
        }
    }
}

void print_all(P *p, int n) {
    int i;
    float avg_tat = 0, avg_wt = 0;
    float total_tat = 0, total_wt = 0;
    for (i = 0; i < n; i++) {
        // printf("%d -> %d %d\n",i+1,p[i].turnaround_time,p[i].waiting_time);
        total_tat += p[i].turnaround_time;
        total_wt += p[i].waiting_time;
    }
    avg_tat = total_tat / n;
    avg_wt = total_wt / n;
    printf("Avg. turnaround time: %f\n", avg_tat);
    printf("Avg. waiting time: %f\n", avg_wt);
    printf("Gantt Chart :\n");
    Display_gantt_chart();
}   

void FCFS(P *p, int n) {
    int i, j;
    int current_time = 0;
    struct chart* ptr=NULL;
    for (i = 0; i < n; i++) {
        if (p[i].arrival_time > current_time) {
            current_time = p[i].arrival_time;
        }
        for (j=0; j<p[i].burst_time; j++) {
            if (ptr==NULL) {
                gc=create(p[i].pid);
                ptr=gc;
            } else {
                ptr->next=create(p[i].pid);
                ptr=ptr->next;
            }
        }
        p[i].waiting_time = current_time - p[i].arrival_time;
        current_time += p[i].burst_time;
        p[i].turnaround_time = current_time - p[i].arrival_time;
    }
    printf("----------------------------------------------\n");
    printf("First-Come First-Served Scheduling:\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n");
}

void SJF(P *p, int n) {
    int i,j,current_time=0,mn=0,completed=0,sel=0,first=0;
    int taken[n];
    for (i=0; i<n; i++) taken[i]=0;
    struct chart* ptr=NULL;
    while (completed<n) {
        sel=-1;
        mn=1e9;
        first=1e9;
        for(i=0; i<n; i++) {
            if (taken[i]==1) continue;
            if (p[i].arrival_time<=current_time && p[i].burst_time<mn) {
                sel=i;
                mn=p[i].burst_time;
            } else if (p[i].arrival_time<first) {
                first=p[i].arrival_time;
            }
        }
        if (sel==-1) {
            current_time=first;
        } else {
            for (j = 0; j < p[sel].burst_time; j++) {            
                if (ptr==NULL) {
                    gc=create(p[sel].pid);
                    ptr=gc;
                } else {
                    ptr->next=create(p[sel].pid);
                    ptr=ptr->next;
                }
            }
            taken[sel]=1;
            completed++;
            p[sel].waiting_time = current_time - p[sel].arrival_time;
            current_time+=p[sel].burst_time;
            p[sel].turnaround_time = current_time - p[sel].arrival_time;
        }
    }
    printf("----------------------------------------------\n");
    printf("Shortest Job First:\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n");
}

void RR(P *p,int  n) {
    int i,j,sz,current_time=0,mn=0,completed=0,sel=0,first=0;
    printf("Enter quantum size: ");
    scanf("%d",&sz);
    int queue[n],taken[n];
    int front=-1,rear=-1;
    struct chart* ptr=NULL;
    for (i=0; i<n; i++) taken[i]=0;
    for (i=0; i<n; i++) {
       p[i].remaining_time = p[i].burst_time;
    }    
    for (i=0; i<n; i++) {
        if (p[i].arrival_time==0) {
            if (front==-1) front=0;
            taken[i]=1;
            rear=(rear+1)%n;
            queue[rear]=i;
        }
    }

    while (completed<n) {
        // printf("cur: %d %d %d\n",current_time,front,rear);
        // Display_gantt_chart();
        int check=1,count=0;
        if (front!=-1) {
            sel=queue[front];
            if (front==rear) front=rear=-1;
            else front=(front+1)%n;
            
            if (p[sel].remaining_time<=sz) {
                current_time+=p[sel].remaining_time;
                count=p[sel].remaining_time;
                p[sel].remaining_time=0;
                completed++;
                check=0;
                // printf("%d -> %d",sel,current_time);
                p[sel].turnaround_time = current_time - p[sel].arrival_time;
                p[sel].waiting_time = p[sel].turnaround_time - p[sel].burst_time;
            } else {
                current_time+=sz;
                count=sz;
                p[sel].remaining_time -= sz;    
            }
            while (count--) {
                if (ptr==NULL) {
                    gc=create(p[sel].pid);
                    ptr=gc;
                } else {
                    ptr->next=create(p[sel].pid);
                    ptr=ptr->next;
                }
            }
        }
        else current_time+=sz;

        for (i=0; i<n; i++) {
            if (taken[i]==1) continue;
            if (p[i].arrival_time<=current_time) {
                if (front==-1) front=0;
                taken[i]=1;
                rear=(rear+1)%n;
                queue[rear]=i;
            }
        }
        if (check) {
            rear=(rear+1)%n;
            queue[rear]=sel;
        }
    }
    printf("----------------------------------------------\n");
    printf("Round Robin Scheduling:\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n"); 
}

void PRIORITY(P *p,int  n) {
    int i,j,current_time=0,completed=0,sel;

    struct chart* ptr=NULL;
    for (i=0; i<n; i++) p[i].remaining_time = p[i].burst_time; 

    while (completed<n) {
        // printf("current: %d completed: %d\n",current_time,completed);
        int high=__INT32_MAX__;
        for(i=0; i<n; i++) {
            if (p[i].remaining_time==0) continue;
            if (p[i].arrival_time<=current_time && p[i].priority<high) {
                sel=i;
                high=p[i].priority;
            }
        }
        if (ptr==NULL) {
            gc=create(p[sel].pid);
            ptr=gc;
        } else {
            ptr->next=create(p[sel].pid);
            ptr=ptr->next;
        }
        current_time++;
        p[sel].remaining_time--;
        if (p[sel].remaining_time==0) {
            completed++;
            // printf("current: %d arrival_time: %d burst: %d\n",current_time,p[sel].arrival_time,p[sel].burst_time);
            p[sel].turnaround_time = current_time - p[sel].arrival_time;
            p[sel].waiting_time = p[sel].turnaround_time - p[sel].burst_time;
        }
    }
    
    printf("----------------------------------------------\n");
    printf("Priority Scheduling (Pre-emptive):\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n");

}

void SRF(P *p, int n) {
    int i,j,current_time=0,completed=0,sel;
    struct chart* ptr=NULL;
    for (i=0; i<n; i++) p[i].remaining_time = p[i].burst_time; 

    while (completed<n) {
        // printf("current: %d completed: %d\n",current_time,completed);
        int mn=__INT32_MAX__;
        for(i=0; i<n; i++) {
            if (p[i].remaining_time==0) continue;
            if (p[i].arrival_time<=current_time && p[i].remaining_time<mn) {
                sel=i;
                mn=p[i].remaining_time;
            }
        }
        if (ptr==NULL) {
            gc=create(p[sel].pid);
            ptr=gc;
        } else {
            ptr->next=create(p[sel].pid);
            ptr=ptr->next;
        }
        current_time++;
        p[sel].remaining_time--;
        if (p[sel].remaining_time==0) {
            completed++;
            // printf("current: %d arrival_time: %d burst: %d\n",current_time,p[sel].arrival_time,p[sel].burst_time);
            p[sel].turnaround_time = current_time - p[sel].arrival_time;
            p[sel].waiting_time = p[sel].turnaround_time - p[sel].burst_time;
        }
    }
    
    printf("----------------------------------------------\n");
    printf("Shortest Remaining Time First (Pre-emptive):\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n");
}

void LRF(P *p, int n) {
    int i,j,current_time=0,completed=0,sel;
    struct chart* ptr=NULL;
    for (i=0; i<n; i++) p[i].remaining_time = p[i].burst_time; 

    while (completed<n) {
        // printf("current: %d completed: %d\n",current_time,completed);
        int mn=__INT32_MAX__;
        for(i=0; i<n; i++) {
            if (p[i].remaining_time==0) continue;
            if (p[i].arrival_time<=current_time && p[i].remaining_time>mn) {
                sel=i;
                mn=p[i].remaining_time;
            }
        }
        if (ptr==NULL) {
            gc=create(p[sel].pid);
            ptr=gc;
        } else {
            ptr->next=create(p[sel].pid);
            ptr=ptr->next;
        }
        current_time++;
        p[sel].remaining_time--;
        if (p[sel].remaining_time==0) {
            completed++;
            // printf("current: %d arrival_time: %d burst: %d\n",current_time,p[sel].arrival_time,p[sel].burst_time);
            p[sel].turnaround_time = current_time - p[sel].arrival_time;
            p[sel].waiting_time = p[sel].turnaround_time - p[sel].burst_time;
        }
    }
    
    printf("----------------------------------------------\n");
    printf("Longest Remaining Time First (Pre-emptive):\n");
    printf("----------------------------------------------\n");
    print_all(p, n);
    printf("----------------------------------------------\n");
}

int main() {
    int choice, n, i;
    P *p;
    FILE *input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        printf("Error: Failed to open input file\n");
        exit(1);
    }
    fscanf(input_file, "%d", &n);
    p = (P *) malloc(n * sizeof(P));
    for (i = 0; i < n; i++) {
        fscanf(input_file, "%d %d %d %d", &p[i].pid, &p[i].arrival_time, &p[i].burst_time, &p[i].priority);
    }
    fclose(input_file);
    Sort_by_arrival_time(p, n);
    while (1) {
        printf("\nCPU Scheduling Algorithms\n");
        printf("------------------------\n");
        printf("1. First-Come First-Served (FCFS)\n");
        printf("2. Shortest Job First (SJF)\n");
        printf("3. Round Robin Scheduling\n");
        printf("4. Priority Scheduling\n");
        printf("5. Shortest Remaining Time First\n");
        printf("6. Longest remaining time first\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        gc=NULL;
        switch (choice) {
            case 1:
                FCFS(p, n);
                break;

            case 2:
                SJF(p, n);
                break;

            case 3:
                RR(p, n);
                break;

            case 4:
                PRIORITY(p, n);
                break;

            case 5:
                SRF(p, n);
                break;

            case 6:
                LRF(p, n);
                break;        

            case 7:
                printf("Exiting program...\n");
                exit(0);

            default:
                printf("Invalid choice. Please enter a valid choice.\n");
        }
    }
    free(p);
    return 0;
}
