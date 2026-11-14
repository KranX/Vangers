#include "xerrhand.h"

#include <SDL3/SDL.h>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iterator>

namespace {

namespace fs = std::filesystem;
fs::path original_directory;
fs::path test_directory;
fs::path expected_log;
bool missing_working_directory = false;

void cleanup() {
	std::error_code error;
	fs::current_path(original_directory, error);
	fs::remove_all(test_directory, error);
}

void require(bool condition, const char *message) {
	if (!condition) {
		std::cerr << "FAIL: " << message << '\n';
		std::exit(1);
	}
}

} // namespace

// This test target renames only the dialog call, so it can exercise the real
// Abort/Log implementations without opening a blocking native message box.
extern "C" bool SDLCALL VangersTest_ShowSimpleMessageBox(
	SDL_MessageBoxFlags flags,
	const char *title,
	const char *message,
	SDL_Window *window
) {
	require(flags == SDL_MESSAGEBOX_ERROR, "not an error dialog");
	require(std::string(title) == "Vangers error" && !window, "unexpected dialog arguments");
	const std::string text = message;
	const std::string marker = " - logfile from ";
	const auto start = text.find(marker);
	require(start != std::string::npos, "dialog has no log path");
	const auto end = text.find(",\n", start);
	require(end != std::string::npos, "dialog has no log path terminator");
	const auto reported_path = text.substr(start + marker.size(), end - start - marker.size());
	if (missing_working_directory) {
		require(reported_path == "logfile.txt", "cannot report the configured fallback path");
	} else {
		const auto path = fs::u8path(reported_path);
		std::error_code error;
		require(
			path.is_absolute() && fs::equivalent(path, expected_log, error) && !error,
			"dialog does not report the actual log path"
		);
		std::ifstream input(expected_log);
		require(input.is_open(), "log was not written at the reported path");
		const std::string contents{std::istreambuf_iterator<char>(input), {}};
		require(contents == "before abort\n" + text, "Abort did not append the dialog text");
	}
	return true;
}

int main(int argc, char **argv) {
	require(argc == 2, "expected a path test case");
	const std::string mode = argv[1];
	original_directory = fs::current_path();
	test_directory =
		fs::temp_directory_path() /
		("vangers error log " + mode + " " +
			std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
	// A non-ASCII working directory also checks the UTF-8 dialog path on Windows.
	const auto working_directory = test_directory / fs::u8path(u8"рабочая папка");
	fs::create_directories(working_directory / "logs");
	fs::create_directories(test_directory / "absolute logs");
	std::atexit(cleanup);
	fs::current_path(working_directory);

	if (mode == "relative") {
		ErrH.log_name = "logfile.txt";
		expected_log = working_directory / "logfile.txt";
	} else if (mode == "nested") {
		ErrH.log_name = (fs::path("logs") / "logfile.txt").string();
		expected_log = working_directory / "logs" / "logfile.txt";
	} else if (mode == "absolute") {
		expected_log = test_directory / "absolute logs" / "logfile.txt";
		ErrH.log_name = expected_log.string();
	} else if (mode == "missing-cwd") {
		missing_working_directory = true;
		ErrH.log_name = "logfile.txt";
		fs::remove(working_directory / "logs");
		fs::remove(working_directory);
	} else {
		require(false, "unknown path test case");
	}

	ErrH.Log("before abort");
	ErrH.Abort("log path regression test", 0);
	return 1;
}
