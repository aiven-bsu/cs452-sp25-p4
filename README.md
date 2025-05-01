# P4 - FIFO QUEUE FOR THREAD SYNCHRONIZATION

## Overview

The goal of this project is to implement a monitor-based First-In-First-Out (FIFO) queue to solve the classic bounded buffer (producer-consumer) problem. The queue will have a fixed capacity and employ thread blocking to manage full or empty states, ensuring safe concurrent access. This implementation will require core computer science concepts, including synchronization, concurrency control, and resource management to be applied. Additionally, the use of system tools and simple shell scripts to observe and analyze process behavior, helping to identify potential issues such as deadlocks, race conditions, and data loss will be needed. This project builds a foundation for understanding how lower-level system mechanisms support high-level abstractions in operating systems and modern software applications.

## Building

```bash
make
```

## Testing

```bash
make check
```

## Clean

```bash
make clean
```

## Install Dependencies

In order to use git send-mail you need to run the following command:

```bash
make install-deps
```
