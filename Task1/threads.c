#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

float countA = 0, countB = 0;
int lastRun = 3;
int winner = 0;
int a=1, b=1, seed1=2, seed2=99;
int map[] = {1, 1, 1};
int arr[] = {1, 2, 3};
int pointsToWin = 0;

/* 
Binary search for randomly picked number r. Returns the index randomly choosen number maps to(0, 1 or 2) in arr{1, 2, 3}
*/
int findCeil(int arr[], int r, int l, int h)
{
    int mid;
    while (l < h)
    {
         mid = l + ((h - l) >> 1);  // Same as mid = (l+h)/2
        (r > arr[mid]) ? (l = mid + 1) : (h = mid);
    }
    
    return (arr[l] >= r) ? l : -1;
}
 
// The main function that returns a random number from arr[] according to
// distribution array defined by freq[]. n is size of arrays.
int myRand(int arr[], int n, int seed)
{
    // Create and fill prefix array
    int prefix[n], i;
    prefix[0] = map[0];
    for (i = 1; i < n; ++i)
        prefix[i] = prefix[i - 1] + map[i];

    // prefix[n-1] is sum of all frequencies. Generate a random number
    // with value from 1 to this sum
    int a = prefix[n-1];
    
    int r = rand_r(&seed)%prefix[n-1] + 1;
    
    // Find index of ceiling of r in prefix arrat
    int indexc = findCeil(prefix, r, 0, n - 1);
    
    /*Returns paper, scissors or rock based on randomly choosen value and howmany times each one won a round*/
    return arr[indexc];
}

int whoWon(int a, int b) {
// input
// 1 - paper
// 2 - scissors
// 3 - rock

  if(a == 3 && b == 2 || a == 1 && b == 3 || a == 2 && b == 1) {
    return 1;
  } else if(a == 1 && b == 2 || a == 2 && b == 3 || a == 3 && b == 1) {
    return 2;
  } else
    return 3;

}

void *master2(void *ptr) {	
    while(countA < pointsToWin && countB < pointsToWin) {
        if(lastRun == 3) {
           // printf("Running Master\n");
            if(whoWon(a, b) == 1){
                countA++;
                // if thread C has won, increase count of (rock, paper or scissors) picked by C 
                map[a-1]++;
        }else if(whoWon(a, b) == 2){
                countB++;
                // if thread D has won, increase count of (rock, paper or scissors) picked by D 
                map[b-1]++;
        }
            else {
                countA += 0.5;
                countB += 0.5;
            }
            printf("C choose %d, scored %f\n", a, countA);
            printf("D choose %d, scored %f\n", b, countB);
            lastRun = 1;
        }
    }

    if(countA == countB) {
        if(rand() > rand()) {
            printf("C won!\n");
        } else
            printf("D won!\n");
    } else if(countA > countB) {
        printf("C won!\n");
        winner = 1;
    } else {
        printf("D won!\n");
        winner = 2;
    }
    printf("Paper: %d, Scissors: %d, Rock: %d", map[0], map[1], map[2]);

    pthread_exit(0);
}

void *func_C(void *ptr) {
    while(countA < pointsToWin && countB < pointsToWin) {
        if(lastRun == 1) {
            seed1++;
            a = myRand(arr, 3, seed1);
            lastRun = 2;
        }
    }
    pthread_exit(0);
}

void *func_D(void *ptr) {
    while(countA < pointsToWin && countB < pointsToWin) {
        if(lastRun == 2) {
            seed2++;
            b = myRand(arr, 3, seed2);
            lastRun = 3;
        }
    }
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    if(argc > 1)
        pointsToWin = atoi(argv[1]);
    else   
        pointsToWin = 100;
    if(pointsToWin > 500) {
      pointsToWin = 500;
      printf("Max limit set to 500\n");
    }
    srand(time(NULL));
	seed1 = rand();
    seed2 = rand();
	pthread_t master, C, D;
    
    pthread_create(&master, NULL, master2, NULL);
    pthread_create(&C, (const pthread_t*)NULL, func_C, NULL);
    pthread_create(&D, (const pthread_t*)NULL, func_D, NULL);
	

	pthread_join(C, NULL);
	pthread_join(D, NULL);
	pthread_join(master, NULL);
	return winner;
}