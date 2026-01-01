Project {
    name: "SourceMap-Cpp"

    property string version: "2.0.0"
    property bool enableTests: (sourceDirectory === path)

    minimumQbsVersion: "3.1"
    qbsModuleProviders: "conan"

    StaticLibrary {
        name: "SourceMapLibrary"
        version: project.version

        cpp.cxxLanguageVersion: "c++23"
        cpp.includePaths: "src"

        Depends { name: "cpp" }
        Depends { name: "nlohmann_json" }
        Export {
            cpp.cxxLanguageVersion: "c++23"
            cpp.includePaths: exportingProduct.sourceDirectory + "/src"

            Depends { name: "cpp" }
            Depends { name: "nlohmann_json" }
        }
        Group {
            name: "sources"
            prefix: "src/SourceMap/"
            files: [
                "Data.h",
                "Data_impl.h",
                "Entry.h",
                "Extension/Base.h",
                "Extension/Caller.cpp",
                "Extension/Caller.h",
                "Extension/Caller_impl.h",
                "Extension/Interpolation.cpp",
                "Extension/Interpolation.h",
                "Extension/Interpolation_impl.h",
                "Extensions.h",
                "FilePosition.h",
                "FilePosition.ostream.h",
                "Position.ostream.h",
                "intern/Base64VLQ.cpp",
                "intern/Base64VLQ.h",
                "Mapping.h",
                "Mapping_impl.h",
                "meta/tuple_without_void.h",
                "Position.h",
                "RevisionThree.cpp",
                "RevisionThree.h",
                "RevisionThree_impl.h",
            ]
        }
    }
    SubProject {
        condition: parent.enableTests
        filePath: "test/test.qbs"
    }
    Product {
        name: "[sourcemap extra files]"
        files: [
            ".clang-format",
            "CHANGES",
            "conanfile.txt",
            "LICENSE",
            "NOTICE",
            "README.md",
        ]
    }
}
