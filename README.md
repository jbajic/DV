# Digital Video Techniques Project

Project consists of three main modules:
- init_configuration
- player_tuner
- table_parser

## Module init_configuration
This module initializes TV application by reading a meta data about the data transfer such as: frequency, bandwidth, module and others.
Initial data is then parse from input stream and parses the data into appropriate structure.
This module also contains a functionality for setting and triggering reminders in TV application.
## Table parser module
Table parser module constantly parses the input stream from PAT(Program Association Table) and PMT(Program Map Table).
## Player tuner module
This module acts as an controller module, and calles all others. Its main job is to take input from user(signal from remote) parse it and act 
accordingly. It start a thread for taking input from user, graphics thread that displays all TV application graphics, and
thread for parsing input stream.

This project is built via MAKEFILE.
