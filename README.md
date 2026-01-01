
# SourceMap-Cpp

**C++23 implementation of SourceMaps for C++*

Source Maps became popular for Javascript and Stylesheet generator languages like CoffeeScript, LESS and SCSS/SASS.
This implementations conforms to the [proposal of Mozilla and Google](https://docs.google.com/document/d/1U1RGAehQwRypUTovF1KRlpiOFze0b-_2gc6fAH0KY0k/edit?pli=1#heading=h.1ce2c87bpj24) and the reference implementation of Mozilla:
https://github.com/mozilla/source-map

## Features

* fully C++ typed
* load and save the common JSON format
* searchable index to find mappings of generated source locations
* extendable template component design
* Interpolation extension (see below)
* Caller extension (see below)

## Interpolation extension

Normal source maps can only reference source blocks.
Sometimes the original and generated string are the same. For example embedded strings or code.
The interpolation extensions marks these mappings as interpolatable.
This allows for character accurate mapping where possible.

The main use case is tool support. The user requests the origin of a position.
With this extension, we send him to the original file and the exact position.

The implementation of Interpolation is fully transparent.
If you never define a mapping with interpolation, nothing is stored in the JSON output.

## Caller extension

Minifications have a one-to-one mapping from source to target.
Meaning every "feature" of the source is represented by exactly one position in the target.
When you source-map more templates like systems, you may include a position twice or call a function from anywhere.
That's where the Caller Extension becomes useful.

Each Source Map entry may be annotated with a Caller and each Caller may have a caller as well, given a CallerStack.
For each Caller a source location is stored.

The implementation of Caller is fully transparent.
If you never define a mapping with a caller, nothing is stored in the JSON output.

## Requirements

* C++23 compiler
  * Tested with GCC 15.2 / Clang 21
  * Tested with Visual Studio 2026
* Conan
  * `conan profile detect`
  * `conan install . -g=QbsDeps --output-folder=R:\sourceMap-debug --build missing --settings=build_type=Debug`
  * `conan install . -g=QbsDeps --output-folder=R:\sourceMap-release --build missing --settings=build_type=Release`
  * VsCode: `"moduleProviders.conan.installDirectory": "R:\\SourceMap-debug"` to `.vscode/qbs-cconfiguration.json` and same for release
  * QtCreator: `moduleProviders.conan.installDirectory:R:\SourceMap-debug` to qbs build arguments and same for release

## How to use?

`Entry` is the structure defining a SourceMap entry.
If you create a SourceMap, you create a list of these.

The `Mapping` class implements the indexing from file location (line, column) in the generated file to source file and location.

## Roadmap

* Implement the mapping from a source location to generated file location.
* Implement the nested source map version

## License

Apache License Version 2.0
See LICENSE file for more details

## Contribute

If you like this project give it a star.
If you don't like it or found a bug, please write an issue.
