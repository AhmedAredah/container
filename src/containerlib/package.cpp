#include "containerLib/package.h"
#include <QDebug>

namespace ContainerCore {

Package::Package(QObject *parent)
    : QObject(parent) {}

Package::Package(const QString id, QObject *parent)
    : QObject(parent), m_packageID(id)
{

}

Package::~Package() = default;

Package::Package(const QJsonObject &json, QObject *parent)
    : QObject(parent)
{
    // Initialize packageID
    m_packageID = json[QStringLiteral("packageID")].toString();
}

// Copy constructor
Package::Package(const Package &other)
    : QObject(other.parent())
{
    deepCopy(other);
}


// Copy assignment operator
Package &Package::operator=(const Package &other)
{
    if (this != &other) {
        deepCopy(other);
    }
    return *this;
}

QString Package::packageID() const {
    return m_packageID;
}

void Package::setPackageID(const QString &id) {
    if (id != m_packageID) {
        m_packageID = id;
        emit packageIDChanged();
    }
}

QJsonObject Package::toJson() const
{
    QJsonObject jsonObject;
    jsonObject[QStringLiteral("packageID")] = m_packageID;
    return jsonObject;
}

ContainerCore::Package* Package::copy() const {
    Package* newPackage = new Package();
    newPackage->setPackageID(m_packageID);
    return newPackage;
}

QDataStream &operator<<(QDataStream &out, const Package &package) {
    out << package.m_packageID;
    return out;
}

QDataStream &operator>>(QDataStream &in, Package &package) {
    in >> package.m_packageID;
    return in;
}


// Helper function to perform deep copy
void Package::deepCopy(const Package &other)
{
    m_packageID = other.m_packageID;
}

}
