#include <postgres.h>
#include <fmgr.h>
#include <c.h>
#include <executor/executor.h>
#include <nodes/nodeFuncs.h>

PG_MODULE_MAGIC;

ExecutorRun_hook_type old_ExecutorRun_hook = NULL;

// We want to augment every `PlanState` with this structure.
// If we were forking Postgres, that would be easy: just add it to the
// `PlanState` structure and reference it from there: `pstate->augmentation`
typedef struct PlanStateAugmentation {
  int value;
  char *str;
  ExecProcNodeMtd original_callback;
} PlanStateAugmentation;

// This is our "wrapper" executor for all nodes
static TupleTableSlot* exec_proc(struct PlanState *pstate) {
  PlanStateAugmentation *augmentation = /* TODO: get the augmentation */;
  Assert(augmentation);
  // Log information stored in the augmentation
  ereport(LOG, errmsg("value: %d str: %s", augmentation->value, augmentation->str));
  // Call the original `ExecProcNodeReal`:
  return augmentation->original_callback(pstate);
}

static bool walker(struct PlanState *pstate, void *context) {
  PlanStateAugmentation *augmentation;

  augmentation = /* TODO: where do we get the pointer from? */;
  Assert(augmentation);
  augmentation->value = rand();
  augmentation->str = "augmentation";
  augmentation->original_callback = pstate->ExecProcNodeReal;

  // Replace the original callback
  pstate->ExecProcNodeReal = exec_proc;

  // TODO: how do we pass the augmentation?

  return false;
}


static void planstatech_executor_run_hook(QueryDesc *queryDesc,
    ScanDirection direction, uint64 count, bool execute_once) {
  if (queryDesc->planstate != NULL) {
    planstate_tree_walker(queryDesc->planstate, walker, NULL);
  }
  if (old_ExecutorRun_hook != NULL) {
    old_ExecutorRun_hook(queryDesc, direction, count, execute_once);
  } else {
    standard_ExecutorRun(queryDesc, direction, count, execute_once);
  }
}

void _PG_init(void) {
  if (ExecutorRun_hook != NULL) {
    old_ExecutorRun_hook = ExecutorRun_hook;
  }
  ExecutorRun_hook = planstatech_executor_run_hook;
}
