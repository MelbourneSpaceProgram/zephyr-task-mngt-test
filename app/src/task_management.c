//Dependencies
#include <stdio.h>
#include <stdlib.h>
#include "task_management.h"

//Default task parameters
const ZTaskParameters Z_TASK_DEFAULT_PARAMS =
{
   NULL,                             //Task control block
   NULL,                             //Stack
   0,                                //Size of the stack
   CONFIG_NUM_PREEMPT_PRIORITIES - 1 //Task priority
};

/**
 * @brief Create a task
 * @param[in] name NULL-terminated string identifying the task
 * @param[in] taskCode Pointer to the task entry function
 * @param[in] arg Argument passed to the task function
 * @param[in] params Task parameters
 * @return Task identifier referencing the newly created task
 **/

ZTaskId zCreateTask(const char_t *name, ZTaskCode taskCode, void *arg,
   const ZTaskParameters *params)
{
   k_tid_t tid;

   //Check parameters
   if(params->tcb != NULL && params->stack != NULL)
   {
      //Create a new thread
      tid = k_thread_create(params->tcb, params->stack, params->stackSize,
         (k_thread_entry_t) taskCode, arg, NULL, NULL, params->priority, 0,
         K_NO_WAIT);

      //Check whether the thread was successfully created
      if(tid != Z_INVALID_TASK_ID)
      {
         //Set current thread name
         k_thread_name_set(tid, name);
      }
   }
   else
   {
      //Invalid parameters
      tid = Z_INVALID_TASK_ID;
   }

   //Return the handle referencing the newly created thread
   return (ZTaskId) tid;
}


/**
 * @brief Delete a task
 * @param[in] taskId Task identifier referencing the task to be deleted
 **/

void zDeleteTask(ZTaskId taskId)
{
   k_tid_t tid;

   //Delete the currently running thread?
   if(taskId == Z_SELF_TASK_ID)
   {
      tid = k_current_get();
   }
   else
   {
      tid = (k_tid_t) taskId;
   }

   //Abort the thread
   k_thread_abort(tid);
}


/**
 * @brief Delay routine
 * @param[in] delay Amount of time for which the calling task should block
 **/

void zDelayTask(systime_t delay)
{
   //Put the current thread to sleep
   k_sleep(K_MSEC(delay));
}


/**
 * @brief Yield control to the next task
 **/

void zSwitchTask(void)
{
   //Wake up a sleeping thread
   k_yield();
}


/**
 * @brief Suspend scheduler activity
 **/

void zSuspendAllTasks(void)
{
   //Lock the scheduler
   k_sched_lock();
}


/**
 * @brief Resume scheduler activity
 **/

void zResumeAllTasks(void)
{
   //Unlock the scheduler
   k_sched_unlock();
}


/**
 * @brief Create an event object
 * @param[in] event Pointer to the event object
 * @return The function returns TRUE if the event object was successfully
 *   created. Otherwise, FALSE is returned
 **/

bool_t zCreateEvent(ZEvent *event)
{
   int err;

   //Create a binary semaphore
   err = k_sem_init(event, 0, 1);

   //Check whether the semaphore was successfully created
   if(err)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


/**
 * @brief Delete an event object
 * @param[in] event Pointer to the event object
 **/

void zDeleteEvent(ZEvent *event)
{
   //No resource to release
}


/**
 * @brief Set the specified event object to the signaled state
 * @param[in] event Pointer to the event object
 **/

void zSetEvent(ZEvent *event)
{
   //Set the specified event to the signaled state
   k_sem_give(event);
}


/**
 * @brief Set the specified event object to the nonsignaled state
 * @param[in] event Pointer to the event object
 **/

void zResetEvent(ZEvent *event)
{
   //Force the specified event to the nonsignaled state
   k_sem_reset(event);
}


/**
 * @brief Wait until the specified event is in the signaled state
 * @param[in] event Pointer to the event object
 * @param[in] timeout Timeout interval
 * @return The function returns TRUE if the state of the specified object is
 *   signaled. FALSE is returned if the timeout interval elapsed
 **/

bool_t zWaitForEvent(ZEvent *event, systime_t timeout)
{
   int err;

   //Wait until the specified event is in the signaled state or the timeout
   //interval elapses
   if(timeout == 0)
   {
      //Non-blocking call
      err = k_sem_take(event, K_NO_WAIT);
   }
   else if(timeout == INFINITE_DELAY)
   {
      //Infinite timeout period
      err = k_sem_take(event, K_FOREVER);
   }
   else
   {
      //Wait until the specified event becomes set
      err = k_sem_take(event, K_MSEC(timeout));
   }

   //Check whether the specified event is set
   if(err)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
}


/**
 * @brief Retrieve system time
 * @return Number of milliseconds elapsed since the system was last started
 **/

systime_t zGetSystemTime(void)
{
   //Get the elapsed time since the system booted, in milliseconds
   return (systime_t) k_uptime_get();
}


/**
 * @brief Retrieve 64-bit system time
 * @return Number of milliseconds elapsed since the system was last started
 **/

uint64_t zGetSystemTime64(void)
{
   //Get the elapsed time since the system booted, in milliseconds
   return (uint64_t) k_uptime_get();
}


/**
 * @brief Allocate a memory block
 * @param[in] size Bytes to allocate
 * @return A pointer to the allocated memory block or NULL if
 *   there is insufficient memory available
 **/

__weak_func void *zAllocMem(size_t size)
{
   void *p;

   //Enter critical section
   zSuspendAllTasks();
   //Allocate a memory block
   p = k_malloc(size);
   //Leave critical section
   zResumeAllTasks();

   //Return a pointer to the newly allocated memory block
   return p;
}


/**
 * @brief Release a previously allocated memory block
 * @param[in] p Previously allocated memory block to be freed
 **/

__weak_func void zFreeMem(void *p)
{
   //Make sure the pointer is valid
   if(p != NULL)
   {
      //Enter critical section
      zSuspendAllTasks();
      //Free memory block
      k_free(p);
      //Leave critical section
      zResumeAllTasks();
   }
}