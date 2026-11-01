// SPDX-FileCopyrightText: 2026 HicknHack Software GmbH
// SPDX-License-Identifier: Apache-2.0

Project {
    name: "Tests"
    references: [
        "TestExtensionCaller/TestExtensionCaller.qbs",
        "TestExtensionCombo/TestExtensionCombo.qbs",
        "TestExtensionInterpolation/TestExtensionInterpolation.qbs",
        "TestMapping/TestMapping.qbs",
        "TestRevisionThree/TestRevisionThree.qbs",
        "TestStructs/TestStructs.qbs",
        "TestSourcePaths/TestSourcePaths.qbs",
    ]

    AutotestRunner {}
}
