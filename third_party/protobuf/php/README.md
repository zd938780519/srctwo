This directory contains the Protocol Buffers runtime implementation via both a
pure PHP package and a native c extension. The pure PHP package is intended to
provide usability to wider range of PHP platforms, while the c extension is
intended to provide higher performance. Both implementations provide the same
runtime APIs and share the same generated code. Users don’t need to re-generate
code for the same proto definition when they want to switch the implementation
later.

Both implementations make use of generated PHP code that defines message and
enum types in PHP. We strongly recommend using protoc's PHP generation support
with .proto files. The build process in this directory only installs the
extension/package; you need to install protoc as well to have PHP code
generation functionality.

## Requirements

To use PHP runtime library requires:

- C extension: PHP 5.5.x or 5.6.x.
- PHP package: PHP 5.5, 5.6 or 7.

## Installation

### C Extension

#### Prerequirements

To install the c extension, the following tools are needed:
* autoconf
* automake
* libtool
* make
* gcc
* pear
* pecl

On Ubuntu, you can install them with:
```
sudo apt-get install php-pear php5-dev autoconf automake libtool make gcc
```
On other platforms, please use the corresponding package managing tool to
install them before proceeding.

#### Installation from Source (Building extension)

To build the c extension, run the following command:
```
cd ext/google/protobuf
pear package
sudo pecl install protobuf-{VERSION}.tgz
```

#### Installation from PECL

When we release a version of Protocol Buffers, we will upload the extension to
[PECL](https://pecl.php.net/). To use this pre-packaged extension, simply
install it as you would any other extension:

```
sudo pecl install protobuf-{VERSION}
```

### PHP Package

#### Installation from composer

Simply add "google/protobuf" to the 'require' section of composer.json in your
project.

### Protoc

Once the extension or package is installed, if you wish to generate PHP code
from a `.proto` file, you will also want to install the Protocol Buffers
compiler (protoc), as described in this repository's main `README` file.  The
version of `protoc` included in the latest release supports the `--php_out`
option to generate PHP code:
```
protoc --php_out=out_dir test.proto
```

## Usage

For generated code:
  https://developers.google.com/protocol-buffers/docs/reference/php-generated

Known Issues
------------

* Missing native support for well known types.
* Missing support for proto2.
* No API provided for clear/copy messages.
* No API provided for encoding/decoding with stream.
* Map fields may not be garbage-collected if there is cycle reference.
* No debug information for messages in c extension.
* HHVM not tested.
* C extension not tested on windows, mac, php 7.0.
* Message name cannot be Empty.
