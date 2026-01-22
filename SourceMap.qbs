import qbs.FileInfo

Project {
    name: "SourceMap-Cpp"

    property string version: "2.0.0"
    property string configProductName: "SourceMapConfig"
    property bool enableQbsImports: (sourceDirectory === path)
    property bool enableTests: (sourceDirectory === path)
    property bool enableThirdParty: true

    minimumQbsVersion: "3.1"
    qbsSearchPaths: enableQbsImports ? ["qbs"] : []

    Product {
        name: "SourceMapConfig"
        condition: configProductName === "SourceMapConfig"

        Export {
            cpp.cxxLanguageVersion: "c++23"

            Depends { name: "cpp" }
        }
    }
    StaticLibrary {
        name: "SourceMapLibrary"
        version: parent.version

        cpp.includePaths: "src"

        Depends { name: "cpp" }
        Depends { name: "nlohmann_json" }
        Depends { name: parent.configProductName }
        Export {
            cpp.includePaths: FileInfo.joinPaths(exportingProduct.sourceDirectory, "src")

            Depends { name: "cpp" }
            Depends { name: "nlohmann_json" }
            Depends { name: exportingProduct.parent.configProductName }
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
    SubProject {
        condition: parent.enableThirdParty
        filePath: "third_party/third_party.qbs"
    }
    Product {
        name: "[sourcemap extra files]"
        files: [
            ".clang-format",
            "CHANGES",
            "LICENSE",
            "NOTICE",
            "README.md",
        ]
    }
}
