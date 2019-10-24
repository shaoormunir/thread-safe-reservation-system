/**
 * HW4 Unit Tests.
 */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern void print_restaurant_status(void);
extern void *reserve_table(size_t, const char *);
extern const char get_guest_name(void *, size_t);
extern void release_table(void *);
extern size_t get_max_party_size(void);
extern unsigned int get_num_waiting(void);

#define TEST_PART_1

/* uncomment the next line when starting Part 2 */
// #define TEST_PART_2

/* uncomment the next line when starting Part 3 */
// #define TEST_PART_3

/* uncomment the next line when starting Part 4 */
// #define TEST_PART_4

static unsigned test_passed;
static unsigned test_failed;
static bool caught_fault;
static bool expecting_fault;

struct thread_arg {
	pthread_t pth;
	char names[1];
};

#define CHECK_IS_NOT_NULL(ptrA) \
	do { \
		if ((ptrA) != NULL) { \
			test_passed++; \
			printf("%d: PASS\n", __LINE__); \
		} else { \
			test_failed++; \
			printf("%d: FAIL\n", __LINE__); \
		} \
		fflush(stdout); \
	} while(0);

#define CHECK_IS_EQUAL(valA,valB) \
	do { \
		if ((valA) == (valB)) { \
			test_passed++; \
			printf("%d: PASS\n", __LINE__); \
		} else { \
			test_failed++; \
			printf("%d: FAIL\n", __LINE__); \
		} \
		fflush(stdout); \
	} while(0);

static void report_test_results(void) {
	printf("%u tests passed, %u tests failed.\n", test_passed, test_failed);
}

static void fault_handler(int signum, siginfo_t *siginfo __attribute__((unused)), void *context __attribute__((unused))) {
	printf("Caught signal %d: %s!\n", signum, strsignal(signum));
	if (!expecting_fault) {
		exit(EXIT_FAILURE);
	}
	caught_fault = true;
}

#ifdef TEST_PART_4
static sem_t sem_reserve;
static sem_t sem_release;

static void *test_threaded_reserve(void *data) {
	struct thread_arg *arg = data;
	void *h = reserve_table(1, arg->names);
	CHECK_IS_NOT_NULL(h);
	sem_post(&sem_reserve);
	while (sem_wait(&sem_release) != 0)
		;
	release_table(h);
	sem_post(&sem_release);
	return NULL;
}
#endif

/**
 * Unit test of your table management software. This will
 * reserve and release tables.
 */
void hw4_test(void) {
	if (atexit(report_test_results) < 0) {
		perror("atexit");
		exit(EXIT_FAILURE);
	}

	struct sigaction sa = {
		.sa_sigaction = fault_handler,
		.sa_flags = SA_SIGINFO,
	};
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGSEGV, &sa, &sa) < 0) {
		exit(EXIT_FAILURE);
	}

#ifdef TEST_PART_1
	printf("Test 1: Displaying restaurant status\n");
	print_restaurant_status();
	test_passed++;
#endif

#ifdef TEST_PART_2
	printf("Test 2: Reserving tables\n");
	void *party1 = reserve_table(4, "ABCD");
	CHECK_IS_NOT_NULL(party1);
	void *party2 = reserve_table(8, "efghijkl");
	CHECK_IS_NOT_NULL(party2);
	char party2_seat3 = get_guest_name(party2, 6);
	CHECK_IS_EQUAL(party2_seat3, 'k');
	print_restaurant_status();

	printf("Test 3: Error handling\n");
	errno = 0;
	void *party3 = reserve_table(100, NULL);
	CHECK_IS_EQUAL(errno, ENOMEM);
	CHECK_IS_EQUAL(party3, NULL);
	errno = 0;
	char party2_seat8 = get_guest_name(party2, 8);
	CHECK_IS_EQUAL(errno, EINVAL);
	CHECK_IS_EQUAL(party2_seat8, 0);
#endif

#ifdef TEST_PART_3
	printf("Test 4: Releasing tables\n");
	release_table(party1);
	expecting_fault = true;
	release_table(party1);
	CHECK_IS_EQUAL(caught_fault, true);
	print_restaurant_status();
	expecting_fault = false;
	release_table(NULL);
	test_passed++;
#endif

#ifdef TEST_PART_4
	printf("Test 5: Threading\n");
	size_t max_party = get_max_party_size();
	CHECK_IS_EQUAL(max_party, 14 * 6);

	if ((sem_init(&sem_reserve, 0, 0) < 0) ||
	    (sem_init(&sem_release, 0, 0) < 0)) {
		fprintf(stderr, "sem_init: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	struct thread_arg args[16];
	for (unsigned i = 0; i < 16; i++) {
		args[i].names[0] = 'M' + i;
		pthread_create(&args[i].pth, NULL, test_threaded_reserve, args + i);
	}
	for (unsigned i = 0; i < 14; i++) {
		if (sem_wait(&sem_reserve) < 0) {
			printf("errno = %d\n", errno);
		}
	}
	max_party = get_max_party_size();
	CHECK_IS_EQUAL(max_party, 0);
	print_restaurant_status();
	printf("^^^ above output should show 2 waiting parties ^^^\n");

	sem_post(&sem_release);
	for (unsigned i = 0; i < 16; i++) {
		pthread_join(args[i].pth, NULL);
	}
	unsigned num_waiting = get_num_waiting();
	CHECK_IS_EQUAL(num_waiting, 0);
	print_restaurant_status();
#endif

	exit(EXIT_SUCCESS);
}
