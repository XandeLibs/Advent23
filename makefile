CPPFLAGS = -Wall

build:
	$(CXX) $(CPPFLAGS) -o run "Day $$day/code.cpp"

clean:
	rm -f run test_result.txt result.txt
