# Microwave Programming Language

Microwave is a basic esoteric programming language inspired by whitespace and COW. It's extremely minimalistic, but it gets the job done.

## Introduction

This language is a result of two things: boredom and taking CSO last semester in college. The idea of the language is to show you can technically program a computer only using microwave sounds such as humming and beeps. In Microwave, the code is written using a set of simple commands that correspond to common actions performed on a microwave oven, such as heating, setting timers, and checking the status. Though you can technically do anything since Microwave is almost complete, we for sure would not recommend programming anything meaningful in it.

## Getting Started

Microwave programs are written in files with a `.mw` extension. Start writing Microwave code using your favorite text editor. You can write programs in the microwave language, and they can be either interpreted or compiled. These can be written across multiple platforms. Initially, this was done in Python but was found to be extremely slow. Therefore, we wrote it in C, C++, and then x86-64 Assembly. Currently, Rust support is being added.

## Language Basics

### Commands

Microwave has a pretty basic instruction set, which is largely inspired by COW. These are the complete set of instructions:

- `mmm`: Start cooking.
- `mMm`: Increase power level.
- `mmM`: Decrease power level.
- `mMM`: Pause cooking.
- `Mmm`: Open the door.
- `MMm`: Close the door.
- `MmM`: Set timer.
- `MMM`: Check timer status.
- `beep`: emit a beep sound.
- `Beep`: Emit multiple beep sounds.
- `beeP`: Pause cooking and prompt for user input.
- `BEEP`: Print the timer countdown.

### Memory Manipulation

To make this language as basic as possible, i.e., easy to write for but excruciating to write in, we try and achieve completeness using memory manipulation commands:

- `mMm`: Move the memory pointer to the left.
- `mmM`: Move the memory pointer to the right.
- `MMM`: Reset the memory block to 0.

### Input and Output

Microwave code interacts with the user with input and output commands:

- `Mmm`: Read a character from the input (door open) and store it in memory.
- `beeP`: prompt the user for input (pause cooking) and store it in memory.
- `BEEP`: Print the timer countdown to the output (display).

### Flow Control

These are the microwave's flow commands:

- `mmm` and `MMM`: Allow for conditional execution based on the timer status.
- `mMM`: Pause cooking and resume from the previous pause point.

## Examples

There are no examples; I haven't come up with any so far. Sorry!
