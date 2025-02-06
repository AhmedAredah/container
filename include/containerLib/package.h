#ifndef PACKAGE_H
#define PACKAGE_H

#include "Container_global.h"
#include <QObject>
#include <QDataStream>
#include <QJsonObject>

namespace ContainerCore {

class CONTAINER_EXPORT Package : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString packageID READ packageID WRITE setPackageID NOTIFY packageIDChanged FINAL)

public:
    explicit Package(QObject *parent = nullptr);
    virtual ~Package();


    Package(const Package &other);  // Copy constructor
    Package(const QString id, QObject *parent = nullptr);
    Package(const QJsonObject &json, QObject *parent = nullptr);
    Package &operator=(const Package &other);  // Copy assignment operator

    QString packageID() const;
    void setPackageID(const QString &id);

    QJsonObject toJson() const;

    friend QDataStream &operator<<(QDataStream &out, const Package &package);
    friend QDataStream &operator>>(QDataStream &in, Package &package);

signals:
    void packageIDChanged();

private:
    QString m_packageID;

    void deepCopy(const Package &other);  // Helper function to perform deep copy
};
}

#endif // PACKAGE_H
