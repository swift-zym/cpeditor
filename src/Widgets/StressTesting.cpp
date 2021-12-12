/*
 * Copyright (C) 2019-2021 Ashar Khan <ashar786khan@gmail.com>
 *
 * This file is part of CP Editor.
 *
 * CP Editor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * I will not be responsible if CP Editor behaves in unexpected way and
 * causes your ratings to go down and or lose any important contest.
 *
 * Believe Software is "Software" and it isn't immune to bugs.
 *
 */

#include "Widgets/StressTesting.hpp"
#include "Core/Checker.hpp"
#include "Core/MessageLogger.hpp"
#include "Settings/PathItem.hpp"
#include "mainwindow.hpp"
#include <QCodeEditor>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

namespace Widgets
{
StressTesting::StressTesting(QWidget *parent) : QMainWindow(parent), mainWindow(qobject_cast<MainWindow *>(parent))
{

    auto *widget = new QWidget(this);
    auto *layout = new QVBoxLayout();
    widget->setLayout(layout);
    setCentralWidget(widget);
    setWindowTitle(tr("Stress Testing"));
    resize(400, 360);

    auto *generatorLayout = new QHBoxLayout();
    generatorLable = new QLabel(tr("Generator Path:"), widget);
    generatorLayout->addWidget(generatorLable);
    generatorPath = new PathItem(PathItem::AnyFile, widget);
    generatorLayout->addWidget(generatorPath);

    layout->addLayout(generatorLayout);

    auto *argumentsPatternLayout = new QHBoxLayout();
    argumentsPatternLabel = new QLabel(tr("Generator Arguments Pattern:"));
    argumentsPatternLayout->addWidget(argumentsPatternLabel);
    argumentsPattern = new QLineEdit(widget);
    argumentsPattern->setPlaceholderText(tr("Example: \"g 10 [5..100] abacaba\""));
    argumentsPatternLayout->addWidget(argumentsPattern);

    layout->addLayout(argumentsPatternLayout);

    auto *stdLayout = new QHBoxLayout();
    stdLabel = new QLabel(tr("Standard Program Path:"), widget);
    stdLayout->addWidget(stdLabel);
    stdPath = new PathItem(PathItem::AnyFile, widget);
    stdLayout->addWidget(stdPath);

    layout->addLayout(stdLayout);

    auto *controlLayout = new QHBoxLayout();
    startButton = new QPushButton(tr("Start"));
    controlLayout->addWidget(startButton);
    stopButton = new QPushButton(tr("Stop"));
    stopButton->setDisabled(true);
    controlLayout->addWidget(stopButton);

    layout->addLayout(controlLayout);
}
} // namespace Widgets