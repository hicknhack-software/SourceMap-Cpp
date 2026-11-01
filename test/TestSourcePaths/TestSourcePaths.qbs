CppApplication {
    name: "TestSourcePaths"
    consoleApplication: true
    type: base.concat(["autotest"])

    Depends { name: "SourceMapLibrary" }
    Depends { name: "catch2-with-main" }

    files: ["TestSourcePaths.cpp", "TestTextPosition.cpp"]
}
