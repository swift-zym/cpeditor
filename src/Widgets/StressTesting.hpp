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
} // namespace Core

namespace Widgets
{
class StressTesting : public QMainWindow
{
    Q_OBJECT

  public:
    explicit StressTesting(QWidget *parent = nullptr);

  private:
    MainWindow *mainWindow;
    PathItem *generatorPath = nullptr, *stdPath = nullptr;
    QLabel *generatorLable = nullptr, *stdLabel = nullptr, *argumentsPatternLabel = nullptr;
    QLineEdit *argumentsPattern = nullptr;
    QPushButton *startButton = nullptr, *stopButton = nullptr;
    QVector<QString> tests;
    Core::Runner *runner = nullptr;
    Core::Compiler *generatorCompiler = nullptr;
    Core::Compiler *userCompiler = nullptr;
    Core::Compiler *stdCompiler = nullptr;
    MessageLogger *logger = nullptr;
    QTemporaryDir *tmpDir = nullptr;
    QString generatorTmpPath;
    QString userTmpPath;
    QString stdTmpPath;
    int compiledCount;

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
};
} // namespace Widgets

#endif // STRESSTESTING_HPP
