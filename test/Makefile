all : app accepted_tests rejected_tests

y_tests := $(wildcard ../../JSONTestSuite/test_parsing/y_*.json)
n_tests := $(wildcard ../../JSONTestSuite/test_parsing/n_*.json)

.PHONY: accepted_tests rejected_tests

accepted_tests : app
	@for f in $(y_tests); do \
		./app "$$f" || echo Failed $$f; \
	done

rejected_tests : app
	@for f in $(n_tests); do \
		./app "$$f" && echo ">>>>" Should have rejected $$f; \
	done

app : test.cpp ../mini_json/json.cpp ../mini_json/json_file.cpp
	CC -o app -I.. -lstdc++ -std=c++17 $+

