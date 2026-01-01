CppApplication {
   name: "TestExtensionCaller"

   consoleApplication: true
   type: base.concat(["autotest"])

   Depends { name: "SourceMapLibrary" }
   Depends { name: "catch2-with-main" }

   files: [
       "TestExtensionCaller.cpp"
   ]
}
