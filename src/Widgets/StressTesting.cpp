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
#include "Core/EventLogger.hpp"
#include "Core/MessageLogger.hpp"
#include "Settings/PathItem.hpp"
#include "mainwindow.hpp"
#include <QCodeEditor>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <functional>

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
    generatorPath = new PathItem(PathItem::CppSource, widget);
    generatorLayout->addWidget(generatorPath);

    layout->addLayout(generatorLayout);

    auto *argumentsPatternLayout = new QHBoxLayout();
    argumentsPatternLabel = new QLabel(tr("Generator Arguments Pattern:"));
    argumentsPatternLayout->addWidget(argumentsPatternLabel);
    argumentsPattern = new QLineEdit(widget);
    argumentsPattern->setPlaceholderText(tr("Example: \"10 [5..100] abacaba\""));
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

void StressTesting::start()
{
    stop();
    QString pattern = argumentsPatternLabel->text();
    QString realArgumentsString = "";
    QVector<QPair<unsigned long long, unsigned long long>> argumentsRange;
    int argumentsCount = 0;
    int leftBracketPos = -1;
    int currentPos = 0;
    bool ok = true;
    for (auto &&c : pattern)
    {
        if (c == '[')
        {
            if (leftBracketPos != -1)
            {
                ok = false;
                break;
            }
            leftBracketPos = currentPos;
        }
        else if (c == ']')
        {
            if (leftBracketPos == -1)
            {
                ok = false;
                break;
            }
            argumentsCount++;
            realArgumentsString += "@";
            realArgumentsString += QString::number(argumentsCount);
            QString range = pattern.mid(leftBracketPos + 1, currentPos - leftBracketPos - 1);
            QStringList tmp = range.split("..");
            if (tmp.length() != 2)
            {
                ok = false;
                break;
            }

            unsigned long long left = tmp[0].toULongLong(&ok);
            if (!ok)
                break;
            unsigned long long right = tmp[1].toULongLong(&ok);
            if (!ok)
                break;

            argumentsRange.append(qMakePair(left, right));

            leftBracketPos = -1;
        }
        else if (leftBracketPos == -1)
        {
            realArgumentsString += c;
        }
        currentPos++;
    }
    if (!ok)
    {
        mainWindow->getLogger()->error(tr("Stress Testing"), tr("Invalid arguments pattern"));
        return;
    }
    std::function<void(QString, int)> buildArguments = [&](const QString &current, int index) {
        if (index == argumentsRange.length())
        {
            LOG_INFO(INFO_OF(current));
            tasks.append(current);
            return;
        }
        for (unsigned long long i = argumentsRange[index].first; i <= argumentsRange[index].second; i++)
        {
            buildArguments(current.arg(QString::number(i)), index + 1);
        }
    };
    buildArguments(realArgumentsString, 0);
}

void StressTesting::stop()
{
    tasks.clear();
    startButton->setDisabled(false);
    stopButton->setDisabled(true);
}

} // namespace Widgets