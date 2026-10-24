#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

#include "rle.h"

namespace {

bool check(bool condition, const char *message, std::size_t input_size) {
	if (!condition)
		std::cerr << message << " (input size: " << input_size << ")\n";
	return condition;
}

bool round_trip(const std::vector<uchar> &input, const std::vector<uchar> &expected = {}) {
	// No padding: ASan must catch even a one-byte overread/overwrite.
	std::unique_ptr<uchar[]> source(new uchar[input.size()]);
	std::copy(input.begin(), input.end(), source.get());
	uchar *encoded = nullptr;
	const int packed_size = RLE_ANALISE(source.get(), static_cast<int>(input.size()), encoded);
	std::unique_ptr<uchar[]> packed(encoded);
	if (input.empty())
		return check(packed_size == 0 && !packed, "Empty input produced data", 0);
	if (!check(
			packed && packed_size > 0 && static_cast<std::size_t>(packed_size) <= input.size() * 2,
			"Invalid encoded size",
			input.size()
		))
		return false;

	// Check packet boundaries before calling the length-unaware legacy decoder.
	std::size_t decoded_size = 0;
	std::size_t offset = 0;
	while (offset < static_cast<std::size_t>(packed_size)) {
		const uchar header = packed[offset++];
		const std::size_t count = (header & 127) + 1;
		const std::size_t payload = (header & 128) ? count : 1;
		if (!check(
				payload <= packed_size - offset && count <= input.size() - decoded_size,
				"Packet exceeds the encoded or decoded buffer",
				input.size()
			))
			return false;
		offset += payload;
		decoded_size += count;
	}
	if (!check(decoded_size == input.size(), "Encoded stream lost input bytes", input.size()))
		return false;
	if (!expected.empty() && !check(
								 std::vector<uchar>(encoded, encoded + packed_size) == expected,
								 "Unexpected legacy packet encoding",
								 input.size()
							 ))
		return false;

	std::unique_ptr<uchar[]> decoded(new uchar[input.size()]);
	RLE_UNCODE(decoded.get(), static_cast<int>(input.size()), encoded);
	return check(
			   std::equal(input.begin(), input.end(), decoded.get()),
			   "Round trip changed input bytes",
			   input.size()
		   ) &&
		   check(
			   std::equal(input.begin(), input.end(), source.get()),
			   "Encoder modified its input",
			   input.size()
		   );
}

bool invalid_input() {
	uchar byte = 0x41;
	for (uchar *input : {static_cast<uchar *>(nullptr), &byte}) {
		for (int length : {-1, 0, 1}) {
			if (input && length > 0)
				continue;
			uchar *output = &byte;
			if (!check(
					RLE_ANALISE(input, length, output) == 0 && output == nullptr,
					"Invalid input did not clear the output",
					0
				))
				return false;
		}
	}
	return true;
}

bool packet_boundaries() {
	for (int size : {1, 2, 3, 126, 127, 128, 129, 130, 254, 255, 256, 257, 4096}) {
		std::vector<uchar> repeated(size, 0x41);
		std::vector<uchar> literal(size);
		for (int i = 0; i < size; ++i)
			literal[i] = static_cast<uchar>(i);
		if (!round_trip(repeated) || !round_trip(literal))
			return false;
		repeated.push_back(0x42);
		if (!round_trip(repeated))
			return false;
		repeated.insert(repeated.begin(), 0x43);
		if (!round_trip(repeated))
			return false;
		literal.insert(literal.end(), repeated.begin(), repeated.end());
		if (!round_trip(literal))
			return false;
	}
	for (int size : {127, 128, 129}) {
		std::vector<uchar> literal(size);
		for (int i = 0; i < size; ++i)
			literal[i] = static_cast<uchar>(i);
		const int first_count = std::min(size, 128);
		std::vector<uchar> expected{static_cast<uchar>(128 + first_count - 1)};
		expected.insert(expected.end(), literal.begin(), literal.begin() + first_count);
		if (size > first_count) {
			expected.push_back(128);
			expected.push_back(literal.back());
		}
		if (!round_trip(literal, expected))
			return false;
	}
	return round_trip(std::vector<uchar>(127, 0x41), {126, 0x41}) &&
		   round_trip(std::vector<uchar>(128, 0x41), {127, 0x41}) &&
		   round_trip(std::vector<uchar>(129, 0x41), {127, 0x41, 128, 0x41});
}

bool varied_inputs() {
	for (int value = 0; value < 256; ++value) {
		if (!round_trip({static_cast<uchar>(value)}))
			return false;
		for (int next = 0; next < 256; ++next)
			if (!round_trip({static_cast<uchar>(value), static_cast<uchar>(next)}))
				return false;
	}
	for (int size = 1; size <= 10; ++size) {
		for (int bits = 0; bits < (1 << size); ++bits) {
			std::vector<uchar> input(size);
			for (int i = 0; i < size; ++i)
				input[i] = (bits & (1 << i)) ? 0xff : 0;
			if (!round_trip(input))
				return false;
		}
	}
	std::mt19937 rng(663);
	for (int trial = 0; trial < 1000; ++trial) {
		std::vector<uchar> input(rng() % 4097);
		for (std::size_t i = 0; i < input.size(); ++i)
			input[i] = (trial % 2 && i && rng() % 4) ? input[i - 1] : static_cast<uchar>(rng());
		if (!round_trip(input))
			return false;
	}
	return true;
}

} // namespace

int main() {
	return round_trip({0x41}, {0x80, 0x41}) && round_trip({0x41, 0x42}, {0x81, 0x41, 0x42}) &&
				   round_trip({0x41, 0x41}, {1, 0x41}) &&
				   round_trip({0x41, 0x41, 0x42}, {1, 0x41, 0x80, 0x42}) &&
				   round_trip({0x41, 0x42, 0x42}, {0x80, 0x41, 1, 0x42}) && round_trip({}) &&
				   invalid_input() && packet_boundaries() && varied_inputs()
			   ? 0
			   : 1;
}
