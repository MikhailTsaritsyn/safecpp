# safecpp

Implementing Rust-inspired mechanisms for memory- and thread-safety described
in [this blog post](https://safecpp.org/P3390R0.html#the-call-for-memory-safety).

# Access Manager

A reference-counting mechanism is implemented based on the following rules:

1. There can be any number of immutable references to an object
2. There can be only one mutable reference to the object
3. But not both at once

`AccessManager` class provides APIs for mutable and immutable reference borrowing.
Each of those has three options:

1. **Failing**. If the attempt violates the rules, `null` is returned instead of the actual reference.
2. **Throwing**. If a borrowing attempt violates the rules preceding, an exception is thrown.
3. **Waiting**. If the rules are violated, another attempt is made after a retry timeout. It is repeated until the
   borrowing is successful or until a timeout is reached.

Each option has its own desired usage:

1. **Failing** is the safest one suitable for any usage.
2. **Throwing** is for the context where the user can ensure that the rules aren't violated.
3. **Waiting** is for synchronization in multithreaded environment.

Examples of those can be found in `test/AccessManager.cpp`.