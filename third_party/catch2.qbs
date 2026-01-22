import qbs.FileInfo
import online

StaticLibrary {
  name: "catch2-with-main"

  readonly property path sourcePath: source.sourceDirectory

  condition: source.found
  files: [
    "catch2/catch_user_config.hpp",
  ]

  cpp.combineCxxSources: true
  cpp.includePaths: [
    FileInfo.joinPaths(sourcePath, "src"),
    ".",
  ]

  online.Source {
    id: source

    name: "catch2"
    uri: "gh:catchorg/catch2@3.12.0"
  }
  Group {
    name: "src/catch2"
    prefix: FileInfo.joinPaths(sourcePath, "src", "catch2") + "/"
    files: [
      "**/*.cpp",
      "**/*.hpp"
    ]
  }
  Depends { name: "cpp" }
  Depends { name: project.configProductName }
  Export {
    cpp.includePaths: [
      FileInfo.joinPaths(exportingProduct.sourcePath, "src"),
      exportingProduct.sourceDirectory,
    ]

    Depends { name: "cpp" }
    Depends { name: project.configProductName }
  }
}
