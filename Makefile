all: init_build
	cmake --build build --target all

test: init_build
	cmake --build build --target test

test_verbose: init_build
	cmake --build build --target test_verbose

build_test: init_build
	cmake --build build --target build_test

build_test_verbose: init_build
	cmake --build build --target build_test_verbose

init_build:
	cmake -E make_directory build
	cmake -E chdir build cmake $(CURDIR)

clean:
	cmake -E chdir build make clean
	cmake -E remove_directory build
