#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int pointsToWin;
int winner = 0;
int CPipe[2];
int DPipe[2];
//Map stores number of times (paper, scissors and rock) won in that order.
int *map;
int thread_safe_random, thread_safe_random2;

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
int myRand(int arr[], int n, int thread_safe_random)
{
    // Create and fill prefix array
    int prefix[n], i;
    prefix[0] = map[0];
    for (i = 1; i < n; ++i)
        prefix[i] = prefix[i - 1] + map[i];

    // prefix[n-1] is sum of no of wins of each rock paper and scissors
    int a = prefix[n-1];
    int r = rand_r(&thread_safe_random)%prefix[n-1] + 1;

    int indexc = findCeil(prefix, r, 0, n - 1);
    
    /*Returns paper, scissors or rock based on randomly choosen value and howmany times each one won a round*/
    return arr[indexc];
}

void sig_handler1(int signo) {

  //thread_safe_random variable used as seed to random generator, incremented each time it's used.
  thread_safe_random++;
  //arr: 1 - paper, 2 -scissors, 3 - rock
  int arr[] = {1, 2, 3};
  int i = myRand(arr, 3, thread_safe_random);
  while(i == 0)
    i = rand_r(&thread_safe_random)%4;
  //printf("C giving %d\n", i);
  write(CPipe[1], &i, sizeof(i));
}

void sig_handler2(int signo) {
  //thread_safe_random variable used as seed to random generator, incremented each time it's used.
  thread_safe_random2++;
  
  //arr: 1 - paper, 2 -scissors, 3 - rock
  int arr[] = {1, 2, 3};
  int i = myRand(arr, 3, thread_safe_random2);
  while(i == 0)
    i = rand_r(&thread_safe_random2)%4;
  //printf("D giving %d\n", i);
  write(DPipe[1], &i, sizeof(i));
}

void sig_handler3(int signo) {
  //Child process that won gets SIGINT as prize. Then exits
  if(signo == SIGINT) {
    printf("%d won!", getpid());
    exit(0);
  }
}


/* 
  Returns who won, out of two players C and D; C choose a, D choose b
*/
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


int main(int argc, char **argv)
{
  //Reading second argument to CLI to be the points to be won
    if(argc > 1) {
      pointsToWin = atoi(argv[1]);
    } else
      pointsToWin = 100;
    if(pointsToWin > 400) {
      pointsToWin = 400;
      printf("Max limit set to 400\n");
    }
    srand(time(NULL));
    thread_safe_random = rand();
    thread_safe_random2 = rand();
    signal(SIGUSR1, sig_handler1);
    signal(SIGUSR2, sig_handler2);
    signal(SIGINT, sig_handler3);
    int fd = open("/tmp/mmap_vars", O_RDWR | O_CREAT | O_TRUNC, (mode_t)0777);
    if (fd == -1) {
	    perror("Error opening file for writing");
	  }
  
    /* Stretch the file size to the size of the (mmapped) array of ints
     */
    int result = lseek(fd, sizeof(int)*3, SEEK_SET);
    if (result == -1) {
      close(fd);
      perror("Error calling lseek() to 'stretch' the file");
    }
    
    result = write(fd, "", 1);
    if (result != 1) {
      close(fd);
      perror("Error writing last byte of the file");
    }
  
    /* Now the file is ready to be mmapped.
     */
    map = (int*)mmap(0, sizeof(int)*3, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    
    if (map == MAP_FAILED) {
      close(fd);
      perror("Error mmapping the file");
    }
  
  // 0 - read, 1-write
  if(pipe(CPipe) == -1)
    printf("Error creating pipe C\n");
  if(pipe(DPipe) == -1)
    printf("Error creating pipe D\n");
  pid_t C = fork();
  if( C < 0)
    printf("Failed creating process C\n");
  if( C == 0 ) {
    close(CPipe[0]);
    srand(sig_handler1);
    while(1)
    pause();
  } else {
    close(CPipe[1]);
  }
  pid_t D;
  //Parent P
  if(C > 0) {
    
    D = fork();
    if (D < 0)
      printf("Failed creating process D\n");
    if(D == 0) {
      close(DPipe[0]);
      srand(sig_handler2);
      while(1)
      pause();
    } else {
      close(DPipe[1]);
    }
  
    if( D > 0) {
      float countA = 0, countB = 0;
      map[0] = 1; map[1] = 1; map[2] = 1;
      while(countA < pointsToWin && countB < pointsToWin) {
        kill(C, SIGUSR1);
        kill(D, SIGUSR2);
        int a, b;
        int nbytes = read(CPipe[0], &a, sizeof(a));
        nbytes = read(DPipe[0], &b, sizeof(b));
        if(whoWon(a, b) == 1) {
          map[a-1]++;
          // if thread C has won, increase count of (rock, paper or scissors) picked by C 
          countA++;
        } else if(whoWon(a, b) == 2) {
          countB++;
          // if thread D has won, increase count of (rock, paper or scissors) picked by D 
          map[b-1]++;
        }
        else {
          countA += 0.5;
          countB += 0.5;
        }
        printf("%d did %d and recieved: %f\n", C, a, countA);
        printf("%d did %d and recieved: %f\n", D, b, countB);
      }

      if(countA == countB) {
        if(rand() > rand()) {
          printf("C won!\n");
          kill(C, SIGINT);
          kill(D, SIGKILL);
        } else {
        printf("D won!\n");
        kill(D, SIGINT);
        kill(C, SIGKILL);
        }
      } else if(countA > countB) {
        printf("C won!\n");
        winner = 1; //For MySQL Db storage
        kill(C, SIGINT); //Letting C know it won
        kill(D, SIGKILL); // D can die
      }
      else {
        printf("D won!\n");
        winner = 2;
        kill(D, SIGINT); //Letting D know it won
        kill(C, SIGKILL); //C can die
      }
    }
  }
  printf("Paper:%d Scissor:%d Rock:%d\n", map[0], map[1], map[2]);
  if (munmap(map, sizeof(int)*3) == -1) {
	    perror("Error un-mmapping the file");
  }
  close(fd);
  printf("\nParent exiting..\n");
  return winner;
}