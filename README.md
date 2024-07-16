# PlanState Augmentation Challenge

Hey there! This is a hiring challenge project for our founding engineering
positions. If you are interested to join us at Omnigres in these early days,
we'd like you to take a stab at this project. It will help you see some of the
kinds of challenges we run into; and it will help us understand how you solve
them.

**Please keep your solution private**. This is to prevent spoilers and make it more
fun for others!

# The Project

Our approach to extending Postgres is that we don't fork it – this way, we
don't need to keep reintegrating changes from the upstream. This allows our
users to add Omnigres functionality to their vanilla Postgres installations.

This challenge is designed for both newcomers to Postgres extension building as
well as those who've done a lot of Postgres internals hacking.

In this project, we'll touch on [Postgres
executor](https://github.com/postgres/postgres/blob/master/src/backend/executor/README)
augmentation.

The executor has [multiple
hooks](https://github.com/taminomara/psql-hooks/blob/master/Detailed.md#executor-hooks),
allowing extensions to tap into the process. In our case, we need to go a bit further.

Every `PlanState` has the
[`ExecProcNodeReal`](https://doxygen.postgresql.org/execnodes_8h_source.html#l01123)
struct member, which is the pointer to a function that "executes" the particular
plan with the given state:

```c
typedef TupleTableSlot*(* ExecProcNodeMtd) (struct PlanState *pstate);
```

The problem with it is that it only gets the `PlanState` but there is no direct
way to pass anything else that we want to associate with every `PlanState`. Not
easily, anyway. Let's call this additional structure `PlanStateAugmentation` – you will
find it in the C file in this repository. Your job is to pass an individual augmentation
to every `PlanState`, log it there and call the original callback for the `PlanState`, without
too much of performance overhead.

Ultimately, in this exercise we want to provide the PlanState an additional
context. However, since we cannot change Postges' data structures or its internal
API, we need to implement a form of augmentation. The effect is we want is for
each `PlanState` execution to run as if it was executed with the signature of
`planstate->ExecProcNodeReal(PlanState *, additional_context)` instead of
`planstate->ExecProcNodeReal(PlanState *)`.

## Questions?

If anything is unclear, or you spotted an issue with the project itself, please
send an email to hiring@omnigres.com

## Running your code

You need some version of Postgres (let's say, 16) installed first.

Then, build the project:

```shell
make
```

Initialize the database:

```shell
# Linux
initdb -D test -c shared_preload_libraries=$(pwd)/planstatech.so -c port=55432
# macOS
initdb -D test -c shared_preload_libraries=$(pwd)/planstatech.dylib -c port=55432
```

You may already see log output of your extension! From there on, you can try starting the database
and playing with it.

# Submission

To submit your solution, please run the command below and send the patch(es) to
hiring@omnigres.com 

```shell
git format-patch origin/master
```
