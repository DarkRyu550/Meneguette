#pragma once

#include <stdbool.h>
#include <threads.h>

/**
 * Helper structure for joining threads concurrently with the rest of the
 * application, so that their resources can be released. Usually, the main
 * thread would be responsible for this, but it's busy in an accept() loop.
 *
 * Threads can register themselves to be cleaned up with join_list_notify_exit.
 *
 * This structure aims to minimize the time spent in join calls, such that threads
 * are joined as soon as possible. For this to be achieved, threads should notify
 * their exit as late as they can, so the joiner thread doesn't spend too much time
 * waiting on a thrd_join call.
 *
 * The intended usage is:
 *
 * Main Thread:
 * ```
 * join_list* list = join_list_new();
 * thrd_t joiner = spawn_joiner_thread(list);
 *
 * //can be repeated as needed
 * join_list_notify_creation(list);
 * spawn_worker_thread(...);
 *
 * join_list_start_termination(list);
 * thrd_join(joiner, NULL);
 * join_list_free(list);
 * ```
 *
 * Joiner Thread:
 * ```
 * int joiner(...) {
 *   thrd_t thread;
 *   while(join_list_next(list, &thread)) thrd_join(thread, NULL);
 *   return 0;
 * }
 * ```
 *
 * Worker Threads:
 * ```
 * int worker(...) {
 *   do_work();
 *
 *   join_list_notify_exit(list, thrd_current());
 *   return 0;
 * }
 * ```
 */
typedef struct join_list_internal join_list;

/**
 * Allocates a new empty join list.
 *
 * @return a new join list.
 */
join_list* join_list_new();

/**
 * Releases the resources used by a join list.
 *
 * @param list List to release.
 */
void join_list_free(join_list* list);

/**
 * Blocks until either a thread gets added to the list to be joined or termination
 * is started and all created threads have been processed.
 *
 * @param list   List to poll.
 * @param thread Where to store the thread to be joined, if any.
 *
 * @return Whether or not there's a thread to be joined. Once this returns false, it'll
 *         never return true again.
 */
bool join_list_next(join_list* list, thrd_t* thread);

/**
 * Starts termination of this join list. Once termination starts, attempts to
 * notify creation of new threads will panic.
 *
 * @param list List to start terminating.
 */
void join_list_start_termination(join_list* list);

/**
 * Notifies a list that a new thread that will be added to this list for joining
 * was started.
 *
 * This does not yet modify the list, but allows it to know how many threads will
 * be added to it (for termination detection).
 *
 * Calling this function after termination is started will trigger a panic.
 *
 * @param list List to notify.
 */
void join_list_notify_creation(join_list* list);

/**
 * Notifies a list that a thread has terminated and should be joined.
 *
 * @param list   List to notify.
 * @param thread Thread that's terminating.
 */
void join_list_notify_exit(join_list* list, thrd_t thread);
