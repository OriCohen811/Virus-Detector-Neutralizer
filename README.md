# Virus-Detector-Neutralizer
A simple anti-virus simulation written in C for Linux. It detects known viruses in binary files using signature matching and can neutralize them by overwriting their first instruction with a RET (0xC3). The tool loads virus signatures, scans binary files, and optionally "fixes" infected files by disabling embedded virus code.
