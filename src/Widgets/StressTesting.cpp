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
#include <QVBoxLayout>

namespace Widgets
{
StressTesting::StressTesting(QWidget *parent) : QMainWindow(parent), mainWindow(qobject_cast<MainWindow *>(parent))
{

    auto *widget = new QWidget(this);
    auto *layout = new QHBoxLayout();
    widget->setLayout(layout);
    setCentralWidget(widget);
    setWindowTitle(tr("Stress Testing"));
    resize(720, 480);

    auto *generatorLayout = new QVBoxLayout();
    generatorLable = new QLabel(tr("Generator Path"), widget);
    generatorLayout->addWidget(generatorLable);
    generatorPath = new PathItem(PathItem::AnyFile, widget);
    generatorLayout->addWidget(generatorPath);

    layout->addLayout(generatorLayout);

    auto *stdLayout = new QVBoxLayout();
    stdLabel = new QLabel(tr("Standard Program Path"), widget);
    stdLayout->addWidget(stdLabel);
    stdPath = new PathItem(PathItem::AnyFile, widget);
    stdLayout->addWidget(stdPath);

    layout->addLayout(stdLayout);
}
} // namespace Widgets