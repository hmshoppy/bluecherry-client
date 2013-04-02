/*
 * Copyright 2010-2013 Bluecherry
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DVRSERVER_H
#define DVRSERVER_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include "core/ServerRequestManager.h"
#include "core/DVRCamera.h"

class DVRServerConfiguration;
class QNetworkRequest;
class QUrl;
class QSslCertificate;

class DVRServer : public QObject
{
    Q_OBJECT

public:
    ServerRequestManager *api;
    explicit DVRServer(int id, QObject *parent = 0);

    bool isOnline() const;

    DVRServerConfiguration * const configuration() const;

    QUrl url() const;
    int serverPort() const;
    int rtspPort() const;

    QList<DVRCamera> cameras() const { return m_cameras; }
    DVRCamera findCamera(int id) { return DVRCamera::getCamera(this, id); }

    QString statusAlertMessage() const { return m_statusAlertMessage; }

    /* SSL */
    bool isKnownCertificate(const QSslCertificate &certificate) const;
    void setKnownCertificate(const QSslCertificate &certificate);

    void setError(const QString &error);

public slots:
    /* Permanently remove from config and delete */
    void removeServer();

    void login();
    void toggleOnline();
    void updateCameras();

signals:
    void changed();
    void devicesReady();

    void serverRemoved(DVRServer *server);

    void cameraAdded(const DVRCamera &camera);
    void cameraRemoved(const DVRCamera &camera);

    void statusAlertMessageChanged(const QString &message);

    void loginRequestStarted();
    void loginSuccessful();
    void serverError(const QString &message);
    void loginError(const QString &message);
    void disconnected();
    void statusChanged(int status);
    void onlineChanged(bool online);

private slots:
    void updateCamerasReply();
    void updateStatsReply();
    void disconnectedSlot();

private:
    DVRServerConfiguration *m_configuration;

    QList<DVRCamera> m_cameras;
    QString m_statusAlertMessage;
    QTimer m_refreshTimer;
    bool m_devicesLoaded;

};

Q_DECLARE_METATYPE(DVRServer*)

#endif // DVRSERVER_H
