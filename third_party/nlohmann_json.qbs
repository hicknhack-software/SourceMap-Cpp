import qbs.FileInfo
import online

Product {
  name: "nlohmann_json"
  condition: source.found

  online.Source {
    id: source

    name: "nlohmann_json"
    uri: "gh:nlohmann/json@3.12.0"
  }
  Export {
    cpp.includePaths: FileInfo.joinPaths(source.sourceDirectory, "include")

    Depends { name: "cpp" }
  }
}
