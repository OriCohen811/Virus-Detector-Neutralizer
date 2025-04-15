# Virus Detector & Neutralizer (C, Linux)

## Overview
This project is a simple anti-virus simulator developed in C for Linux environments. It allows the user to:

- Load virus signatures from a binary file.
- Print loaded signatures.
- Scan a suspected binary file for viruses based on the loaded signatures.
- Neutralize detected viruses by patching their code with a `RET` instruction (`0xC3`).

The neutralization assumes viruses are self-contained functions and can be safely skipped by forcing a return.

## How It Works
1. **Signature File**: A binary file containing virus records. Each virus includes:
   - Signature size (2 bytes)
   - Virus name (16 bytes)
   - Signature data (N bytes)

2. **Infected File**: A suspected binary file to scan for known virus signatures.

3. **Detection**: The program scans a 10KB buffer from the infected file and compares each byte window to known signatures.

4. **Neutralization**: If a match is found, the first byte of the virus in the file is replaced with `0xC3` (RET in x86), making the virus return immediately.

## Usage
Compile and run:
```terminal
make
./virusDetector [signatures-file (optional now or in '1' option as below)]
```

### Menu Options
1. **Load signatures**: Manually load a signatures file.
2. **Print signatures**: Display loaded virus signatures.
3. **Detect viruses**: Scan a file for known viruses.
4. **Fix file**: Automatically neutralize detected viruses in the suspected file.
5. **Quit**: Exit the program.

> When using "Fix file", make sure the program was launched with the infected file as a command-line argument.

## Dependencies
- `gcc` (C compiler)
- Linux environment
- `hexedit` (optional, for manual testing)
