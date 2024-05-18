# FPGA-Based SHA-1 Hardware Accelerator

## Project Overview

This project involves the implementation of an FPGA-based SHA-1 hardware accelerator. The primary objective is to optimize the SHA-1 hashing algorithm for embedded systems by leveraging FPGA hardware and the Intel Nios II processor. The project is divided into two main tasks: implementing the SHA-1 algorithm in C on an x86 target, and then adapting it for execution on the Intel Nios II processor.

## Introduction

The Secure Hash Algorithm 1 (SHA-1) is a cryptographic hash function used to ensure data integrity. This project focuses on accelerating SHA-1 computations using FPGA technology, which can significantly enhance performance compared to software-only implementations.

## Features

- Implementation of SHA-1 in C on x86 architecture
- Adaptation of SHA-1 algorithm for Intel Nios II processor
- FPGA-based hardware acceleration
- Improved hashing performance for embedded applications

## Getting Started

### Prerequisites

To build and run this project, you will need:

- Intel Quartus Prime software
- Intel Nios II EDS (Embedded Design Suite)
- A compatible FPGA development board
- Basic knowledge of C programming and FPGA design

### Installation

1. Clone this repository:
   ```sh
   git clone https://github.com/yourusername/FPGA-SHA1-Accelerator.git
   cd FPGA-SHA1-Accelerator
