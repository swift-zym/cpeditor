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
#include <QCodeEditor>

namespace Widgets
{
StressTesting::StressTesting(MessageLogger *logger, QCodeEditor *editor, Core::Checker *Checker, const QString &lang,
                             QWidget *parent = nullptr)
    : QMainWindow(parent)
{
}
} // namespace Widgets