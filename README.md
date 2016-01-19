# guerillamail-client

Client application for interaction with guerrillamail.com - 1 hour email.
This repo will contain only console version of app.

## Requirements

To successfull compile we need:

	1. libjson
	2. libcurl
	3. pthread

## Compiling

CMAKE OPTIONS:
```sh
	#generate makefiles for debug
	cmake . -DCMAKE_BUILD_TYPE=Debug
	#compile and install to ./debug/ directory
	make && make install
	#run
	cd debug && ./guerillamail
```

## Console mode

Type `h` key to get help.