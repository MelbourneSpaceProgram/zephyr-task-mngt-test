//Dependencies
#include <zephyr/kernel.h>

//Invalid task identifier
#define Z_INVALID_TASK_ID NULL
//Self task identifier
#define Z_SELF_TASK_ID NULL

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

#define __weak_func __attribute__((weak))

typedef char char_t;
typedef signed int int_t;
typedef unsigned int uint_t;
typedef int bool_t;
#define FALSE 0
#define TRUE 1
//Infinite delay
#define INFINITE_DELAY ((uint_t) -1)
//Maximum delay
#define MAX_DELAY (INFINITE_DELAY / 2)

typedef enum
{
   NO_ERROR = 0,                    ///<Success
   ERROR_FAILURE = 1,               ///<Generic error code
   ERROR_OUT_OF_RESOURCES
} error_e;

/**
 * @brief System time
 **/

typedef uint32_t systime_t;


/**
 * @brief Task identifier
 **/

typedef k_tid_t ZTaskId;


/**
 * @brief Task parameters
 **/

typedef struct
{
   struct k_thread *tcb;
   k_thread_stack_t *stack;
   size_t stackSize;
   uint_t priority;
} ZTaskParameters;


/**
 * @brief Event object
 **/

typedef struct k_sem ZEvent;

typedef struct
{
   ZEvent event;                                ///<Event object to receive notifications
   bool_t running;                               ///<The stack is currently running
   ZTaskParameters taskParams;                  ///<Task parameters
   ZTaskId taskId;                              ///<Task identifier
} ZContext;


/**
 * @brief Task routine
 **/

typedef void (*ZTaskCode)(void *arg);


//Default task parameters
extern const ZTaskParameters Z_TASK_DEFAULT_PARAMS;

//Task management
ZTaskId zCreateTask(const char_t *name, ZTaskCode taskCode, void *arg,
   const ZTaskParameters *params);

void zDeleteTask(ZTaskId taskId);
void zDelayTask(systime_t delay);
void zSwitchTask(void);
void zSuspendAllTasks(void);
void zResumeAllTasks(void);

//Event management
bool_t zCreateEvent(ZEvent *event);
void zDeleteEvent(ZEvent *event);
void zSetEvent(ZEvent *event);
void zResetEvent(ZEvent *event);
bool_t zWaitForEvent(ZEvent *event, systime_t timeout);
bool_t zSetEventFromIsr(ZEvent *event);

//System time
systime_t zGetSystemTime(void);
uint64_t zGetSystemTime64(void);

//Memory management
void *zAllocMem(size_t size);
void zFreeMem(void *p);

//C++ guard
#ifdef __cplusplus
}
#endif