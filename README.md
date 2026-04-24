# Bzip-Data-Compression
Bzip-Data-Compression

A C implementation of a simple Bzip-style data compression pipeline. This project demonstrates how block-based compression works by combining Run-Length Encoding (RLE), the Burrows-Wheeler Transform (BWT), and a custom binary archive format.

The project can encode a normal input file into a compressed binary file and decode that binary file back into the original data.

Table of Contents

* Overview￼
* Features￼
* How the Compression Pipeline Works￼
* Project Structure￼
* Requirements￼
* Configuration￼
* Build Instructions￼
* Usage￼
* Examples￼
* Testing￼
* Core Modules￼
* Archive Format￼
* Current Limitations￼
* Future Improvements￼
* License￼

Overview

Bzip-Data-Compression is an educational file compression project written in C.

It is inspired by the structure of the Bzip2 compression process, but it is not a full Bzip2-compatible implementation. Instead, it focuses on implementing the early stages of a compression system in a clear and modular way.

The program supports two main operations:

1. Encoding a file
2. Decoding a previously encoded file

During encoding, the program reads the input file, divides it into blocks, optionally applies Run-Length Encoding, applies the Burrows-Wheeler Transform, and writes the result to a custom binary archive.

During decoding, the program reads the archive, reverses the Burrows-Wheeler Transform, reverses Run-Length Encoding if it was used, and reconstructs the original file.

Features

* Command-line file encoding and decoding
* Block-based file processing
* Configurable block size
* Optional Run-Length Encoding stage
* Burrows-Wheeler Transform support
* Custom binary archive format
* File reconstruction after decoding
* Makefile-based build system
* Windows build target using MinGW
* Built-in round-trip test command
* Written in C

How the Compression Pipeline Works

Encoding pipeline:

Input File
↓
Divide file into blocks
↓
Apply Run-Length Encoding if enabled
↓
Apply Burrows-Wheeler Transform
↓
Write encoded data to custom binary archive

Decoding pipeline:

Encoded Archive
↓
Read archive headers and block data
↓
Apply inverse Burrows-Wheeler Transform
↓
Apply Run-Length Decoding if enabled
↓
Reassemble blocks into output file

Project Structure

.
├── include/
│   ├── archive.h
│   ├── block.h
│   ├── bwt.h
│   ├── config.h
│   └── rle.h
│
├── src/
│   ├── archive.c
│   ├── block.c
│   ├── bwt.c
│   ├── config.c
│   ├── main.c
│   └── rle.c
│
├── config.ini
├── Makefile
├── test_input.txt
├── LICENSE
└── README.md

Requirements

To build and run this project, you need:

* GCC
* Make
* A Unix-like terminal environment such as Linux, macOS, or WSL

For Windows builds, you need:

* MinGW
* x86_64-w64-mingw32-gcc

Configuration

The project uses a config.ini file to control compression settings.

Default configuration:

[General]
block_size = 100000
rle1_enabled = true
bwt_type = matrix

[Paths]
input_directory = ./benchmarks/
output_directory = ./results/

Configuration Options

block_size
The number of bytes processed per block.

rle1_enabled
Controls whether Run-Length Encoding is enabled before BWT.

Accepted values:

* true
* false
* 1
* 0

bwt_type
Specifies the BWT implementation type.

Current supported value:

* matrix

input_directory
Reserved path setting for organizing input files.

output_directory
Reserved path setting for organizing output files.

Build Instructions

Build the project using:

make

This creates the executable:

./bzip2_impl

To remove compiled files:

make clean

To build a Windows executable using MinGW:

make windows

This creates:

bzip2_impl.exe

Usage

The program uses this command format:

./bzip2_impl  <input_file> <output_file>

Available modes:

encode
Encodes a normal input file into a compressed binary file.

decode
Decodes a previously encoded binary file back into its original form.

Examples

Encode a File

./bzip2_impl encode test_input.txt encoded.bin

This reads test_input.txt and creates encoded.bin.

Decode a File

./bzip2_impl decode encoded.bin decoded.txt

This reads encoded.bin and reconstructs the original content into decoded.txt.

Verify Output

cmp test_input.txt decoded.txt

If there is no output, the original file and decoded file are identical.

You can also run:

cmp test_input.txt decoded.txt && echo “Success”

Testing

The Makefile includes a test command:

make run-test

This command:

1. Builds the project
2. Encodes test_input.txt into encoded.bin
3. Decodes encoded.bin into decoded.txt
4. Compares test_input.txt and decoded.txt
5. Prints a success message if both files match

Expected output:

Round-trip OK

Core Modules

main.c

This file contains the main program entry point and controls the encode/decode workflow.

It expects exactly three command-line arguments:

./bzip2_impl  <input_file> <output_file>

Supported modes are:

* encode
* decode

If an invalid mode is provided, the program prints an error message.

config.c and config.h

These files handle loading settings from config.ini.

The configuration controls:

* Block size
* Whether RLE is enabled
* BWT type
* Input and output path settings

block.c and block.h

These files manage file block operations.

Main responsibilities:

* Read an input file
* Divide it into fixed-size blocks
* Store each block in memory
* Track original block sizes
* Reassemble decoded blocks into the final output file
* Free allocated block memory

rle.c and rle.h

These files implement Run-Length Encoding.

RLE compresses repeated byte sequences by storing each sequence as:

count + value

For example:

AAAAA

can be represented as:

5A

In this project, each run stores:

* One byte for count
* One byte for value

The maximum run length is 255.

bwt.c and bwt.h

These files implement the Burrows-Wheeler Transform.

BWT rearranges the input data so that similar characters are grouped together. This usually makes the data easier to compress in later stages.

The encoder:

1. Builds references to all rotations of the input block
2. Sorts the rotations
3. Stores the last character of each sorted rotation
4. Saves the primary index needed for decoding

The decoder:

1. Reconstructs ordering information from the transformed block
2. Uses the primary index
3. Rebuilds the original block data

archive.c and archive.h

These files handle the custom encoded file format.

Main responsibilities:

* Write encoded file headers
* Write block headers
* Write transformed block data
* Read encoded files
* Validate archive format
* Restore compression settings from the archive header

Archive Format

The project uses a custom binary archive format.

File Header

Each encoded file begins with a file header containing:

magic
A format identifier.

version
Archive format version.

block_size
The block size used during encoding.

num_blocks
The total number of encoded blocks.

flags
Stores compression options. Bit 0 indicates whether RLE was enabled.

Block Header

Each block contains a block header with:

transformed_size
The size of the block after RLE and BWT.

original_size
The original size of the block before transformation.

primary_index
The BWT primary index needed for decoding.

Block Data

After each block header, the encoded block data is written directly to the archive.

Current Limitations

This project is not a full Bzip2 implementation.

Current limitations include:

* It does not generate standard .bz2 files
* It cannot decode standard .bz2 files
* Huffman coding is not implemented
* Move-To-Front encoding is not implemented
* CRC/error checking is not implemented
* Compression ratio may not always be smaller than the original file
* The current BWT implementation is simple and not optimized for very large files
* RLE can increase file size when the input does not contain repeated data

Future Improvements

Possible improvements include:

* Add Move-To-Front Transform
* Add Huffman Coding
* Add CRC validation
* Add better error handling
* Add compression statistics
* Add benchmark mode
* Add support for larger files
* Optimize BWT performance
* Add unit tests for each module
* Add GitHub Actions for automated builds and tests
* Improve the Makefile formatting
* Add support for multiple compression levels

Example Full Workflow

Build the project:

make

Encode the sample file:

./bzip2_impl encode test_input.txt encoded.bin

Decode the encoded file:

./bzip2_impl decode encoded.bin decoded.txt

Compare the result:

cmp test_input.txt decoded.txt && echo “Round-trip OK”

Clean generated build files:

make clean

License

This project is licensed under the MIT License.

Author

Talal Nadeem, Saim Naseer, Abdullah Yasir
