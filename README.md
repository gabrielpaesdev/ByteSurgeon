# ByteSurgeon
ByteSurgeon is a 64-bit ELF binary editor that lets you locate and modify strings in the .rodata and .data sections. Precise, fast, and safe, it works like a digital scalpel for your code, allowing controlled and intuitive patching.

## Features

-Scan .rodata and .data sections for printable strings.
-Replace strings without changing binary size.
-Creates a patched copy, leaving the original intact.
-Simple command-line interface.

## Usage
./ByteSurgeon <binary_file>

Lists all detected strings with indexes.

Choose the index of the string to modify.

Enter the new string (must be same length or shorter).

A patched binary <original>_patched will be created.
