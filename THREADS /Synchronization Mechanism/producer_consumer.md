# Linux System Programming Handbook

# Chapter 5
# Producer – Consumer Problem (Masterclass)

---

# Table of Contents

1. What is the Producer-Consumer Problem?
2. Why is it Important?
3. Real Life Examples
4. Basic Architecture
5. Problems Without Synchronization
6. Busy Waiting
7. Using Only Mutex
8. Why Mutex Alone Fails
9. Using Mutex + Condition Variable
10. Buffer Size = 1
11. Buffer Size = N (Circular Queue)
12. Multiple Producers and Consumers
13. Internal Working
14. Timeline Diagrams
15. Complete Example
16. Common Mistakes
17. Interview Questions

---

# 1. What is the Producer-Consumer Problem?

The Producer-Consumer problem is one of the oldest and most important synchronization problems in operating systems.

It consists of two types of threads.

```
Producer

↓

Creates Data
```

```
Consumer

↓

Uses Data
```

Between them is a shared buffer.

```
Producer

↓

+-----------------+
| Shared Buffer   |
+-----------------+

↓

Consumer
```

---

# 2. Real Life Examples

## Camera

```
Camera Sensor

↓

Captures Frame

↓

Buffer

↓

Image Processing Thread
```

---

## Bluetooth

```
Bluetooth Chip

↓

Receives Packet

↓

Packet Queue

↓

Bluetooth Stack
```

---

## Keyboard Driver

```
Keyboard Interrupt

↓

Stores Key

↓

Input Buffer

↓

Application Reads Key
```

---

## Network Driver

```
NIC

↓

Receives Packet

↓

RX Queue

↓

Network Stack
```

---

## Audio Player

```
Disk

↓

Reads Audio

↓

Audio Buffer

↓

Speaker
```

---

Every Linux system uses this design.

---

# 3. Basic Architecture

```
Producer

↓

Produces Data

↓

Stores into Buffer

↓

Consumer Reads

↓

Buffer Becomes Empty

↓

Producer Produces Again
```

---

# 4. Shared Variables

Suppose

```
Buffer Size = 1
```

We need

```c
int buffer;

int full = 0;
```

Meaning

```
full = 0

↓

Buffer Empty
```

```
full = 1

↓

Buffer Full
```

---

# 5. Problem Without Synchronization

Producer

```c
buffer = 100;
```

Consumer

```c
printf("%d",buffer);
```

Suppose

Consumer executes first.

Buffer

```
Not Written Yet
```

Consumer prints

```
Garbage
```

Wrong.

---

Another problem

Producer

writes

```
100
```

Immediately writes

```
200
```

Consumer reads only

```
200
```

Value

```
100

Lost Forever
```

---

# 6. Busy Waiting

Many beginners write

```c
while(full == 0)
{
}
```

Consumer keeps checking.

CPU

```
100%
```

Why?

Suppose

Producer takes

```
5 Seconds
```

Consumer executes

```
Millions

of

Loops
```

during those 5 seconds.

Complete waste of CPU.

This is called

```
Busy Waiting
```

---

# 7. Using Only Mutex

People think

```
Mutex

↓

Solved
```

Wrong.

Suppose

Consumer

```
Lock

↓

Buffer Empty

↓

Unlock

↓

Lock Again

↓

Still Empty

↓

Unlock

↓

Repeat
```

Consumer still keeps checking.

Mutex only protects the variable.

It DOES NOT tell Consumer

when Producer has produced data.

---

# 8. Why Condition Variable Is Needed

Condition Variable tells Consumer

```
Don't keep checking.

Go to sleep.

I'll wake you

when data arrives.
```

Huge difference.

CPU usage

```
Busy Waiting

↓

100%
```

Condition Variable

```
0%
```

until Producer wakes Consumer.

---

# 9. Producer Consumer (Buffer Size = 1)

Shared Variables

```c
int buffer;

int full = 0;

pthread_mutex_t mutex;

pthread_cond_t cond;
```

Producer

```
Lock

↓

Buffer Full?

↓

YES

↓

Wait

↓

NO

↓

Produce

↓

full = 1

↓

Signal

↓

Unlock
```

Consumer

```
Lock

↓

Buffer Empty?

↓

YES

↓

Wait

↓

NO

↓

Consume

↓

full = 0

↓

Signal

↓

Unlock
```

---

# 10. Timeline

Initially

```
Buffer Empty
```

Consumer

```
Lock

↓

Empty

↓

Wait
```

Consumer Sleeps.

Producer

```
Lock

↓

Produce 10

↓

full = 1

↓

Signal

↓

Unlock
```

Consumer wakes

```
Automatically

Locks Mutex

↓

Consumes

↓

Unlock
```

Perfect synchronization.

---

# 11. What Actually Happens?

Suppose

Consumer calls

```c
pthread_cond_wait();
```

Internally

```
Unlock Mutex

↓

Move Thread

to Wait Queue

↓

Scheduler Runs Another Thread
```

Producer executes

```
Produce

↓

Signal

↓

Kernel Moves Consumer

to Ready Queue
```

Consumer

```
Locks Mutex Again

↓

Returns

from pthread_cond_wait()
```

Notice

Consumer did NOT execute

from the beginning.

It resumes

immediately after

```
pthread_cond_wait()
```

This is very important.

---

# 12. Continuous Producer

Suppose

Producer

```
1

2

3

4

5

...

100
```

Consumer

Instead of

```
Wake

↓

Consume

↓

Sleep

↓

Wake

↓

Consume
```

Better

```
Wake Once

↓

Consume

Until Queue Empty

↓

Sleep Again
```

Why?

Less

```
Context Switching
```

Better Performance.

Linux

uses this technique extensively.

---

# 13. Circular Buffer

Real systems

don't use

Buffer Size = 1.

Instead

```
+--------------------------------+

10

20

30

40

50

+--------------------------------+

Head

Tail
```

Producer

writes

at

```
Tail
```

Consumer

reads

from

```
Head
```

When

Tail

reaches end

```
↓

Wrap

↓

Beginning
```

Hence

```
Circular Buffer
```

Also called

```
Ring Buffer
```

Linux kernel uses ring buffers extensively.

---

# 14. Why Circular Buffer?

Suppose

Producer

is much faster.

Instead of waiting

after every value

it fills

```
100 Buffers
```

Consumer

processes

at its own speed.

Huge performance improvement.

---

# 15. Multiple Producers

Suppose

```
Producer1

↓

Queue

↓

Consumer
```

Now add

```
Producer2
```

Both producers

must

NOT

write simultaneously.

Therefore

Producer also needs

```
Mutex
```

---

# 16. Multiple Consumers

Suppose

```
Producer

↓

Queue

↓

Consumer1

Consumer2

Consumer3
```

All consumers

must NOT

read

the same item.

Mutex protects

queue operations.

Condition Variables

wake

waiting consumers.

---

# 17. Typical Linux Design

Linux Servers

```
Network Thread

↓

Packet Queue

↓

Worker Threads
```

Bluetooth

```
ISR

↓

Packet Queue

↓

Bluetooth Thread
```

Camera

```
Sensor

↓

Frame Queue

↓

ISP Thread
```

USB Driver

```
Interrupt

↓

Queue

↓

Worker Thread
```

Everywhere

Producer

↓

Queue

↓

Consumer

---

# 18. Common Mistakes

Mistake

```
Using sleep()

Instead

of

Condition Variable
```

Wrong.

---

Mistake

```
Using if()

instead of while()
```

Wrong.

---

Mistake

```
Not locking mutex

before accessing queue
```

Wrong.

---

Mistake

```
Signaling

before changing

shared variable.
```

Wrong.

Correct order

```
Change State

↓

Signal
```

---

# 19. Interview Questions

Q.

Why can't mutex solve Producer Consumer?

Answer

Mutex protects shared data.

It does NOT notify waiting threads.

---

Q.

Why is Busy Waiting bad?

Answer

It wastes CPU cycles continuously checking a condition instead of sleeping until work is available.

---

Q.

Why is a Circular Buffer preferred?

Answer

It allows producers and consumers to operate independently, reduces waiting, avoids unnecessary memory movement, and improves throughput.

---

Q.

Why do Linux drivers use Producer Consumer?

Answer

Hardware events and software processing happen at different speeds. A queue decouples the producer from the consumer.

---

# Summary

Producer

```
Creates Data
```

Consumer

```
Uses Data
```

Mutex

```
Protects Queue
```

Condition Variable

```
Sleeps

and

Wakes

Threads
```

Circular Buffer

```
Improves Performance
```

Busy Waiting

```
Bad
```

Condition Variable

```
Good
```

---

End of Chapter 5
