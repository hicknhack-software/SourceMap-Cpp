Project {
    name: "Tests"
    references: [
        "TestExtensionCaller/TestExtensionCaller.qbs",
        "TestExtensionCombo/TestExtensionCombo.qbs",
        "TestExtensionInterpolation/TestExtensionInterpolation.qbs",
        "TestMapping/TestMapping.qbs",
        "TestRevisionThree/TestRevisionThree.qbs",
        "TestStructs/TestStructs.qbs",
    ]

    AutotestRunner {}
}
