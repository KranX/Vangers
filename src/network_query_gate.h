#ifndef NETWORK_QUERY_GATE_H
#define NETWORK_QUERY_GATE_H

// Runs the response parser only when the blocking wait actually returned the
// expected event. A cancelled or timed-out wait resets the input buffer read
// offset, so parsing it would consume stale or partial data.
template<typename ParseFn>
bool run_network_query_parser(int received_event, int expected_event, ParseFn parse) {
	if (received_event != expected_event)
		return false;
	parse();
	return true;
}

#endif
