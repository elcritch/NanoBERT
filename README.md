# arduno based nano-bert rpc
This Arduino library provides a light weight serializer and parser for messagepack.

## Install
Download the zip, and import it with your Arduino IDE: *Sketch>Include Library>Add .zip library*

## Usage
See the either the `.h` file, or the examples ().


## Limitations
Currently the library does **not** support:
* 8 bytes float (Only 4 bytes floats are supported by default on every Arduino and floats are anyway not recommended on Arduino)
* 2^32 char long (or longer) strings
* 2^32 byte long (or longer) bins
* extention types.
