# 21114025 Assembler SIc Xe
Project Introduction
This project implements an assembler that supports SIC/XE instructions. The assembler includes all the instructions. It supports Control Sections.
### The instruction formats supported by SIC/XE :
  - Format 1
  - Format 2
  - Format 3
  - Format 4
### The Addressing modes supported by the Assembler are:
  - Base
  - PC relative
  - Direct
  - Immediate
  - Indirect
  - indexing 
  - extended
# Steps To Run
1.	Download the “assembler.cpp” attached with this document.
2.	Create a folder named “input.txt”. add your input to this folder. 
3.	Compile assembler.cpp (any c++ compiler) and run the .exe file.
4.	The program produces:
-	 Listing_File.txt containing the object codes.
-	 errors.txt contains the errors In pass1 and pass2 respectively.
-	 ”output.txt” contains the object program of the SIC/XE.
'''cpp
g++ assembler.cpp -o assembler
./assembler
'''

# Working of Assembler
-	The Input is preprocessed by preprocess.txt which removes comments, white spaces, and converts the input.txt to preprocess.txt.
-	Pass1 assigns address to all statements in the program and saves the values(addresses) assigned to all labels. Some assembler directives are processed.
-	Pass2 assembles instructions by generating Opcodes using tables created in pass1. The data values defined by BYTE, WORD, etc. are assigned values. The assembler directives that were not processed during pass1 are processed. The object program and the assembly listing are written into output.txt and Listing_File.txt.
# CONCLUSION
This SIC/XE assembler supports control sections. The outputs for sample inputs are given in the folder(sample io) attached to this document.
 
