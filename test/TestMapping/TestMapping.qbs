CppApplication {
   name: "TestMapping"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMapLibrary" }
   Depends { name: "catch2-with-main" }

   files: [
       "TestMapping.cpp"
   ]
}
