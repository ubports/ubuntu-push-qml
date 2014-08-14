/*
Copyright 2014 Canonical Ltd.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License, version 3
as published by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.
*/

#include "plugin.h"
#include "pushclient.h"

// Qt
#include <QtCore/QDir>
#include <QtCore/QStandardPaths>
#include <QtCore/QtGlobal>
#include <QtGui/QGuiApplication>
#include <QtQml>

void UbuntuBrowserPlugin::initializeEngine(QQmlEngine* engine, const char* uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
}

void UbuntuBrowserPlugin::registerTypes(const char* uri)
{
    Q_ASSERT(uri == QLatin1String("Ubuntu.PushNotifications"));

    qmlRegisterType<PushClient>(uri, 0, 1, "PushClient");
}
