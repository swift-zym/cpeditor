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

#ifndef STRESSTESTING_HPP
#define STRESSTESTING_HPP

#include "Widgets/TestCase.hpp"
#include <QMainWindow>

class PathItem;
class QLabel;
class MainWindow;
class QLineEdit;
class QPushButton;
class MessageLogger;
class QTemporaryDir;

namespace Core
{
class Runner;
class Compiler;
class Checker;
} // namespace Core

namespace Widgets
{
class StressTesting : public QMainWindow
{
    Q_OBJECT

  public:
    explicit StressTesting(QWidget *parent = nullptr);
    void onCheckFinished(TestCase::Verdict verdict);

  private:
    MainWindow *mainWindow;
    PathItem *generatorPath = nullptr, *stdPath = nullptr;
    QLabel *generatorLable = nullptr, *stdLabel = nullptr, *argumentsPatternLabel = nullptr;
    QLineEdit *argumentsPattern = nullptr;
    QPushButton *startButton = nullptr, *stopButton = nullptr;
    QVector<QString> tests;
    Core::Runner *generatorRunner = nullptr;
    Core::Runner *userRunner = nullptr;
    Core::Runner *stdRunner = nullptr;
    Core::Compiler *generatorCompiler = nullptr;
    Core::Compiler *userCompiler = nullptr;
    Core::Compiler *stdCompiler = nullptr;
    Core::Checker *checker = nullptr;
    MessageLogger *log = nullptr;
    QTemporaryDir *tmpDir = nullptr;
    QString generatorTmpPath;
    QString userTmpPath;
    QString stdTmpPath;
    QString userOut;
    QString stdOut;
    QString in;
    int compiledCount;
    int runFinishedCount;

  signals:
    void compilationErrorOccurred(const QString &error);
    void compilationFailed(const QString &reason);
    void compilationKilled();

  private slots:
    void start();
    void stop();
    void nextTest();
    void onCompilationErrorOccurred(const QString &error);
    void onCompilationFailed(const QString &reason);
    void onCompilationKilled();
    void onGeneratorCompilationStarted();
    void onGeneratorCompilationFinished();
    void onStdCompilationStarted();
    void onStdCompilationFinished();
    void onUserCompilationStarted();
    void onUserCompilationFinished();
    void onRunFinished(int index, const QString &out, const QString &err, int exitCode, qint64 timeUsed, bool tle);
    void onRunOutputLimitExceeded(int index);
    void onRunKilled(int index);
};
} // namespace Widgets

#endif // STRESSTESTING_HPP
