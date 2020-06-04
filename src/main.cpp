#include <stdio.h>
#include <stdlib.h>


#include <core/threads/DisplayThread.hpp>




int main() // MAIN IS OUR CONTROL THREAD
{
  fprintf(stderr, "---START---\n");
  
 auto x = DisplayThread();
  x.print();
  x.start(NULL);
  x.wait_for_state(THREAD_STATES::T_RUNNING);
  x.wait_for_state(THREAD_STATES::T_STOPPED);
  
  
  exit(0);
  
}
