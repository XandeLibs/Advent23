CPPFLAGS = -Wall -Werror -O2

build:
	$(CXX) $(CPPFLAGS) -o run "Day $$day/code.cpp"

clean:
	rm -f run test_result.txt result.txt
