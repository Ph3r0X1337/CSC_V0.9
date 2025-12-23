Teaser release for the upcoming release of the CStandardContainers library.
The library's goal is to implement basic data containers of the C/C++ standard libraries in pure ANSI-C.
Architecture- or context-dependent dependencies like support for DMA (Dynamic Memory Allocation) are outsourced through interfaces, that have to be implemented for the corresponding context.
For example, the V0.9 comes with two native allocators for Windows-systems.
The library holds a strong focus towards safe transaction semantics enabling safe atomic operations, although currently not thread-safe.

The upcoming version will improve a lot on the implementation of polymorphism and object oriented principles, eliminating restrictions like the "one iterator per container" restriction right now.
Additionally, future versions will support Harvard-achitectures aswell, so the library will be usable on systems like microcontroller systems.
While performace is not the main priority, it is only not top priority when performance interferes with safe transactional semantics which possibly reduces determinism of operations.
