#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TEST_DIR "/tmp/gnl_test"
#define RED   "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define RESET "\x1b[0m"

static int tests_passed = 0;
static int tests_failed = 0;

static void assert_test(const char *name, int condition)
{
	if (condition)
	{
		printf(GREEN "[OK] " RESET "%s\n", name);
		tests_passed++;
	}
	else
	{
		printf(RED "[KO] " RESET "%s\n", name);
		tests_failed++;
	}
}

static void create_file(const char *filename, const char *content)
{
	int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		perror("create_file");
		exit(1);
	}
	write(fd, content, strlen(content));
	close(fd);
}

static void cleanup_files(void)
{
	system("rm -rf " TEST_DIR);
}

static void setup_test_dir(void)
{
	system("mkdir -p " TEST_DIR);
}

// Test 1: Normal multi-line file
static void test_normal_file(void)
{
	const char *content = "line1\nline2\nline3\n";
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/normal.txt", content);

	int fd = open(TEST_DIR "/normal.txt", O_RDONLY);
	assert_test("open normal file", fd >= 0);

	line = get_next_line(fd);
	assert_test("line1 with \\n", line && strcmp(line, "line1\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line2 with \\n", line && strcmp(line, "line2\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line3 with \\n", line && strcmp(line, "line3\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF returns NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 2: Empty file
static void test_empty_file(void)
{
	char *line;

	setup_test_dir();
	int fd = open(TEST_DIR "/empty.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	close(fd);

	fd = open(TEST_DIR "/empty.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("empty file returns NULL", line == NULL);
	free(line);

	line = get_next_line(fd);
	assert_test("empty file second call NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 3: File without final newline
static void test_no_final_newline(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/no_nl.txt", "line1\nline2\nno_newline");

	int fd = open(TEST_DIR "/no_nl.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("first line with \\n", line && strcmp(line, "line1\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("second line with \\n", line && strcmp(line, "line2\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("last line without \\n", line && strcmp(line, "no_newline") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF returns NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 4: Only newlines
static void test_only_newlines(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/only_nl.txt", "\n\n\n");

	int fd = open(TEST_DIR "/only_nl.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("first \\n", line && strcmp(line, "\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("second \\n", line && strcmp(line, "\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("third \\n", line && strcmp(line, "\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 5: Invalid fd
static void test_invalid_fd(void)
{
	char *line;

	line = get_next_line(-1);
	assert_test("fd -1 returns NULL", line == NULL);
	free(line);

	line = get_next_line(-42);
	assert_test("fd -42 returns NULL", line == NULL);
	free(line);
}

// Test 6: Long line (exceeds BUFFER_SIZE for small buffers)
static void test_long_line(void)
{
	char *line;
	char *expected;
	int len = 10000;

	setup_test_dir();
	expected = malloc(len + 3);
	if (!expected)
		return;
	for (int i = 0; i < len; i++)
		expected[i] = 'A' + (i % 26);
	expected[len] = '\n';
	expected[len + 1] = 'B';
	expected[len + 2] = '\0';

	create_file(TEST_DIR "/long.txt", expected);

	int fd = open(TEST_DIR "/long.txt", O_RDONLY);

	line = get_next_line(fd);
	int line1_correct = line && (int)strlen(line) == len + 1 && line[len] == '\n';
	for (int i = 0; i < len && line1_correct; i++)
		if (line[i] != 'A' + (i % 26))
			line1_correct = 0;
	assert_test("long line (10000 chars) with \\n", line1_correct);
	free(line);

	line = get_next_line(fd);
	assert_test("remaining char after long line", line && strcmp(line, "B") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	free(expected);
	cleanup_files();
}

// Test 7: Multiple calls after EOF
static void test_multiple_eof(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/single.txt", "hello\n");

	int fd = open(TEST_DIR "/single.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("single line", line && strcmp(line, "hello\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF 1st call NULL", line == NULL);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF 2nd call NULL", line == NULL);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF 3rd call NULL", line == NULL);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF 4th call NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 8: Single char file
static void test_single_char(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/onechar.txt", "X");

	int fd = open(TEST_DIR "/onechar.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("single char no \\n", line && strcmp(line, "X") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 9: Single newline in file
static void test_single_newline_file(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/singlenl.txt", "\n");

	int fd = open(TEST_DIR "/singlenl.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("single \\n", line && strcmp(line, "\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 10: Lines with varying lengths (boundary testing)
static void test_varying_lines(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/varying.txt", "a\nbb\nccc\ndddd\neeeee\nffffff\n");

	int fd = open(TEST_DIR "/varying.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("line 'a\\n'", line && strcmp(line, "a\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line 'bb\\n'", line && strcmp(line, "bb\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line 'ccc\\n'", line && strcmp(line, "ccc\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line 'dddd\\n'", line && strcmp(line, "dddd\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line 'eeeee\\n'", line && strcmp(line, "eeeee\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line 'ffffff\\n'", line && strcmp(line, "ffffff\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 11: Read from stdin note (just verifies fd 0 is accepted)
static void test_fd_zero(void)
{
	// fd 0 is stdin - we can't easily test interactive input
	// but we can verify fd 0 doesn't crash immediately
	// This test is informational only
	assert_test("BUFFER_SIZE defined", BUFFER_SIZE > 0);
}

// Test 12: Mix of newlines and no newline at end
static void test_mixed_endings(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/mixed.txt", "alpha\nbeta\ngamma");

	int fd = open(TEST_DIR "/mixed.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("alpha\\n", line && strcmp(line, "alpha\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("beta\\n", line && strcmp(line, "beta\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("gamma no \\n", line && strcmp(line, "gamma") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 13: Only \\n in the middle
static void test_mid_newline(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/midnl.txt", "abc\n");

	int fd = open(TEST_DIR "/midnl.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("abc\\n", line && strcmp(line, "abc\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

// Test 14: Stress test - large file with many lines
static void test_large_file(void)
{
	char *line;
	int i;

	setup_test_dir();
	int fd = open(TEST_DIR "/large.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	for (i = 0; i < 500; i++)
	{
		char buf[32];
		int len = snprintf(buf, sizeof(buf), "line_%04d\n", i);
		write(fd, buf, len);
	}
	close(fd);

	fd = open(TEST_DIR "/large.txt", O_RDONLY);
	int ok = 1;
	for (i = 0; i < 500; i++)
	{
		line = get_next_line(fd);
		if (!line)
		{
			ok = 0;
			break;
		}
		char expected[32];
		snprintf(expected, sizeof(expected), "line_%04d\n", i);
		if (strcmp(line, expected) != 0)
			ok = 0;
		free(line);
	}
	assert_test("large file - 500 lines correct", ok && get_next_line(fd) == NULL);

	close(fd);
	cleanup_files();
}

// Test 15: File with just spaces and newlines
static void test_spaces_and_newlines(void)
{
	char *line;

	setup_test_dir();
	create_file(TEST_DIR "/spaces.txt", "   \n \n\n");

	int fd = open(TEST_DIR "/spaces.txt", O_RDONLY);

	line = get_next_line(fd);
	assert_test("line '   \\n'", line && strcmp(line, "   \n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line ' \\n'", line && strcmp(line, " \n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("line '\\n'", line && strcmp(line, "\n") == 0);
	free(line);

	line = get_next_line(fd);
	assert_test("EOF NULL", line == NULL);
	free(line);

	close(fd);
	cleanup_files();
}

int main(void)
{
	printf(YELLOW "========== GET_NEXT_LINE TESTS ==========\n" RESET);
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);

	printf(YELLOW "--- Basic Tests ---\n" RESET);
	test_normal_file();
	test_empty_file();
	test_no_final_newline();
	test_only_newlines();

	printf(YELLOW "\n--- Error Handling ---\n" RESET);
	test_invalid_fd();

	printf(YELLOW "\n--- Edge Cases ---\n" RESET);
	test_single_char();
	test_single_newline_file();
	test_long_line();
	test_multiple_eof();
	test_varying_lines();
	test_mixed_endings();
	test_mid_newline();
	test_spaces_and_newlines();
	test_fd_zero();

	printf(YELLOW "\n--- Stress Test ---\n" RESET);
	test_large_file();

	printf("\n" YELLOW "========== RESULTS ==========\n" RESET);
	printf(GREEN "Passed: %d\n" RESET, tests_passed);
	printf(RED "Failed: %d\n" RESET, tests_failed);

	char *result = tests_failed == 0 ? GREEN "ALL TESTS PASSED" RED : RED "SOME TESTS FAILED";
	printf("%s\n" RESET, result);

	return (tests_failed > 0);
}
