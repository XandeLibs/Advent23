CPPFLAGS = -std=c++20 -Wall -Werror -O2
CPPDBGFLAGS = -std=c++20 -Wall -Werror

build:
	$(CXX) $(CPPFLAGS) -o run "$$day/code.cpp"

clean:
	rm -f run test_result.txt result.txt

debug:
	$(CXX) $(CPPDBGFLAGS) -g -o run "$$day/code.cpp"
