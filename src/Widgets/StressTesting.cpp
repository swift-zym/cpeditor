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
#include "Core/Compiler.hpp"
#include "Core/EventLogger.hpp"
#include "Core/MessageLogger.hpp"
#include "Core/Runner.hpp"
#include "Settings/PathItem.hpp"
#include "Util/FileUtil.hpp"
#include "generated/SettingsHelper.hpp"
#include "mainwindow.hpp"
#include <QCodeEditor>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTemporaryDir>
#include <QVBoxLayout>
#include <functional>

namespace Widgets
{
StressTesting::StressTesting(QWidget *parent)
    : QMainWindow(parent), mainWindow(qobject_cast<MainWindow *>(parent)), compiledCount(0)
{
    logger = mainWindow->getLogger();

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
    connect(startButton, &QPushButton::clicked, this, &StressTesting::start);
    controlLayout->addWidget(startButton);
    stopButton = new QPushButton(tr("Stop"));
    connect(stopButton, &QPushButton::clicked, this, &StressTesting::stop);
    stopButton->setDisabled(true);
    controlLayout->addWidget(stopButton);

    layout->addLayout(controlLayout);
}

void StressTesting::start()
{
    stop();
    QString pattern = argumentsPattern->text();
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
            realArgumentsString += "%";
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

    ok &= (leftBracketPos == -1);

    if (!ok)
    {
        logger->error(tr("Stress Testing"), tr("Invalid arguments pattern"));
        return;
    }
    std::function<void(QString, int)> add = [&](const QString &current, int index) {
        if (index == argumentsRange.length())
        {
            LOG_INFO(INFO_OF(current));
            tests.append(current);
            return;
        }
        for (unsigned long long i = argumentsRange[index].first; i <= argumentsRange[index].second; i++)
        {
            add(current.arg(QString::number(i)), index + 1);
        }
    };
    add(realArgumentsString, 0);

    QString generatorCode = Util::readFile(generatorPath->getLineEdit()->text(), tr("Read Generator"), logger);
    QString userCode = mainWindow->getEditor()->toPlainText();
    QString stdCode = Util::readFile(stdPath->getLineEdit()->text(), tr("Read Standard Program"), logger);

    if (generatorCode.isNull())
    {
        logger->error(tr("Stress Testing"), tr("Failed to open generator"));
        return;
    }

    if (stdCode.isNull())
    {
        logger->error(tr("Stress Testing"), tr("Failed to open standard program"));
        return;
    }

    tmpDir = new QTemporaryDir();

    if (!tmpDir->isValid())
    {
        logger->error(tr("Stress Testing"), tr("Failed to create temporary directory"));
        return;
    }

    generatorTmpPath = tmpDir->filePath("gen.cpp");
    userTmpPath = tmpDir->filePath("user.cpp");
    stdTmpPath = tmpDir->filePath("std.cpp");

    if (!Util::saveFile(generatorTmpPath, generatorCode, tr("Stress Testing"), false, logger))
        return;
    if (!Util::saveFile(userTmpPath, userCode, tr("Stress Testing"), false, logger))
        return;
    if (!Util::saveFile(stdTmpPath, stdCode, tr("Stress Testing"), false, logger))
        return;

    auto testlib_h = Util::readFile(":/testlib/testlib.h", tr("Read testlib.h"), logger);
    if (testlib_h.isNull())
        return;
    if (!Util::saveFile(tmpDir->filePath("testlib.h"), testlib_h, tr("Save testlib.h"), false, logger))
        return;

    compiledCount = 0;

    generatorCompiler = new Core::Compiler();

    connect(generatorCompiler, &Core::Compiler::compilationStarted, this,
            &StressTesting::onGeneratorCompilationStarted);
    connect(generatorCompiler, &Core::Compiler::compilationFinished, this,
            &StressTesting::onGeneratorCompilationFinished);
    connect(generatorCompiler, &Core::Compiler::compilationErrorOccurred, this,
            &StressTesting::onCompilationErrorOccurred);
    connect(generatorCompiler, &Core::Compiler::compilationFailed, this, &StressTesting::onCompilationFailed);
    connect(generatorCompiler, &Core::Compiler::compilationKilled, this, &StressTesting::onCompilationKilled);
    generatorCompiler->start(generatorTmpPath, "", SettingsHelper::getCppCompileCommand(), "C++");

    userCompiler = new Core::Compiler();

    connect(userCompiler, &Core::Compiler::compilationStarted, this, &StressTesting::onUserCompilationStarted);
    connect(userCompiler, &Core::Compiler::compilationFinished, this, &StressTesting::onUserCompilationFinished);
    connect(userCompiler, &Core::Compiler::compilationErrorOccurred, this, &StressTesting::onCompilationErrorOccurred);
    connect(userCompiler, &Core::Compiler::compilationFailed, this, &StressTesting::onCompilationFailed);
    connect(userCompiler, &Core::Compiler::compilationKilled, this, &StressTesting::onCompilationKilled);
    userCompiler->start(userTmpPath, "", mainWindow->compileCommand(), mainWindow->getLanguage());

    stdCompiler = new Core::Compiler();

    connect(stdCompiler, &Core::Compiler::compilationStarted, this, &StressTesting::onStdCompilationStarted);
    connect(stdCompiler, &Core::Compiler::compilationFinished, this, &StressTesting::onStdCompilationFinished);
    connect(stdCompiler, &Core::Compiler::compilationErrorOccurred, this, &StressTesting::onCompilationErrorOccurred);
    connect(stdCompiler, &Core::Compiler::compilationFailed, this, &StressTesting::onCompilationFailed);
    connect(stdCompiler, &Core::Compiler::compilationKilled, this, &StressTesting::onCompilationKilled);
    stdCompiler->start(stdTmpPath, "", mainWindow->compileCommand(), mainWindow->getLanguage());
}

void StressTesting::onGeneratorCompilationStarted()
{
    logger->info(tr("Compiler"), tr("Generator compilation has started"));
}

void StressTesting::onGeneratorCompilationFinished()
{
    logger->info(tr("Compiler"), tr("Generator compilation has finished"));
    compiledCount++;
    if (compiledCount == 3)
    {
        nextTest();
    }
}

void StressTesting::onUserCompilationStarted()
{
    logger->info(tr("Compiler"), tr("User program compilation has started"));
}

void StressTesting::onUserCompilationFinished()
{
    logger->info(tr("Compiler"), tr("User program compilation has finished"));
    compiledCount++;
    if (compiledCount == 3)
    {
        nextTest();
    }
}

void StressTesting::onStdCompilationStarted()
{
    logger->info(tr("Compiler"), tr("Standard program compilation has started"));
}

void StressTesting::onStdCompilationFinished()
{
    logger->info(tr("Compiler"), tr("Standard program compilation has finished"));
    compiledCount++;
    if (compiledCount == 3)
    {
        nextTest();
    }
}

void StressTesting::stop()
{
    delete runner;
    delete generatorCompiler;
    delete userCompiler;
    delete stdCompiler;

    tests.clear();
    startButton->setDisabled(false);
    stopButton->setDisabled(true);
}

void StressTesting::nextTest()
{
    if (tests.empty())
    {
        stop();
        logger->info(tr("Stress Testing"), tr("All tests finished, no countertest found"));
        return;
    }
    QString arguments = tests.front();
    tests.pop_front();
    logger->info(tr("Stress Testing"), tr("Running test with arguments \"%1\"").arg(arguments));
}

void StressTesting::onCompilationErrorOccurred(const QString &error)
{
    emit compilationErrorOccurred(error);
}

void StressTesting::onCompilationFailed(const QString &reason)
{
    emit compilationFailed(reason);
}

void StressTesting::onCompilationKilled()
{
    emit compilationKilled();
}

} // namespace Widgets