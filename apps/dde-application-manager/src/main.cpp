// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "global.h"
#include <QDBusConnection>
#include <QCoreApplication>
#include <QDir>
#include "dbus/applicationmanager1service.h"
#include "cgroupsidentifier.h"
#include "global.h"

static void registerComplexDbusType()
{
    qDBusRegisterMetaType<QMap<QString, QDBusUnixFileDescriptor>>();
    qDBusRegisterMetaType<QMap<uint, QMap<QString, QDBusUnixFileDescriptor>>>();
    qDBusRegisterMetaType<IconMap>();
}

int main(int argc, char *argv[])
{
    QCoreApplication app{argc, argv};
    auto &bus = ApplicationManager1DBus::instance();
    bus.initGlobalServerBus(DBusType::Session);
    bus.setDestBus("");
    auto &AMBus = bus.globalServerBus();

    registerComplexDbusType();
    ApplicationManager1Service AMService{std::make_unique<CGroupsIdentifier>(), AMBus};
    QList<DesktopFile> fileList{};
    QByteArray XDGDataDirs;
    XDGDataDirs = qgetenv("XDG_DATA_DIRS");
    if (XDGDataDirs.isEmpty()) {
        XDGDataDirs.append("/usr/local/share/:/usr/share/");
    }
    auto desktopFileDirs = XDGDataDirs.split(':');

    for (const auto &dir : desktopFileDirs) {
        auto dirPath = QDir{QDir::cleanPath(dir) + "/applications"};
        if (!dirPath.exists()) {
            continue;
        }
        QDirIterator it{dirPath.absolutePath(),
                        {"*.desktop"},
                        QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable,
                        QDirIterator::Subdirectories};
        while (it.hasNext()) {
            auto file = it.next();
            ParseError err;
            auto ret = DesktopFile::searchDesktopFile(file, err);
            if (!ret.has_value()) {
                continue;
            }
            if (!AMService.addApplication(std::move(ret).value())) {
                break;
            }
        }
    }

    return app.exec();
}
