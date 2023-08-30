// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <QCoreApplication>
#include <gtest/gtest.h>
#include <QTimer>
#include <QDBusMetaType>

namespace {
void registerComplexDbusType()
{
    qDBusRegisterMetaType<QDBusObjectPath>();
}
}  // namespace

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    registerComplexDbusType();

    ::testing::InitGoogleTest(&argc, argv);
    int ret{0};
    QTimer::singleShot(0, &app, [&ret] {
        ret = RUN_ALL_TESTS();
        QCoreApplication::quit();
    });
    QCoreApplication::exec();
    return ret;
}
