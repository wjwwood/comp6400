all: homework3

install:
	cd build && make install

uninstall:
	cd build && make uninstall

homework3:
	@mkdir -p build
	-mkdir -p bin
	cd build && cmake $(CMAKE_FLAGS) ..
ifneq ($(MAKE),)
	cd build && $(MAKE)
else
	cd build && make
endif
	bin/homework3

clean:
	-cd build && make clean
	rm -rf build bin lib
