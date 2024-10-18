#ifndef CONTAINER_H
#define CONTAINER_H

#include "Container_global.h"
#include <QObject>
#include <QVector>
#include <QVariant>
#include <QDataStream>
#include <QJsonObject>
#include <QJsonArray>
#include "package.h"

namespace ContainerCore{

class CONTAINER_EXPORT Container : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString containerID READ getContainerID WRITE setContainerID NOTIFY containerIDChanged FINAL)
    Q_PROPERTY(ContainerSize containerSize READ getContainerSize WRITE setContainerSize NOTIFY containerSizeChanged FINAL)
    Q_PROPERTY(QVector<Package*> packages READ getPackages WRITE setPackages NOTIFY packagesChanged FINAL)
    Q_PROPERTY(QMap<HaulerType, QVariantMap> customVariables READ getCustomVariables WRITE setCustomVariables NOTIFY customVariablesChanged FINAL)

public:
    // Enum for container sizes
    enum ContainerSize {
        twentyFT,
        twentyFT_HighCube,
        fourtyFT,
        fourtyFT_HighCube,
        fortyFiveFT,
        fortyFiveFT_HighCube,
        tenFT,
        thirtyFT,
        fortyEightFT,
        fiftyThreeFT,
        sixtyFT
    };
    Q_ENUM(ContainerSize)

    enum HaulerType {
        truck,
        train,
        waterTransport,
        airTransport
    };
    Q_ENUM(HaulerType)

    explicit Container(QObject *parent = nullptr);
    Container(const QString &id, ContainerSize size, QObject *parent = nullptr);
    Container(const QJsonObject &json, QObject *parent = nullptr);
    Container(const Container &other);  // Copy constructor
    Container &operator=(const Container &other);  // Copy assignment operator
    ~Container();

    // Getter and Setter for containerID
    QString getContainerID() const;
    void setContainerID(const QString &id);

    // Getter and Setter for containerSize
    ContainerSize getContainerSize() const;
    void setContainerSize(ContainerSize size);

    // Getter and Setter for packages
    QVector<Package*> getPackages() const;
    void setPackages(const QVector<Package*> &packages);
    void addPackage(Package *package);

    // Getter and Setter for customVariables
    QMap<HaulerType, QVariantMap> getCustomVariables() const;
    void setCustomVariables(const QMap<HaulerType, QVariantMap> &variables);
    void addCustomVariable(HaulerType hauler, const QString &key, const QVariant &value);
    void removeCustomVariable(HaulerType hauler, const QString &key);
    QVariant getCustomVariable(HaulerType hauler, const QString &key) const;
    QVariantMap getCustomVariablesForHauler(HaulerType hauler) const;

    void clear();

    QJsonObject toJson() const;

    friend QDataStream &operator<<(QDataStream &out, const Container &container);
    friend QDataStream &operator>>(QDataStream &in, Container &container);

signals:
    void containerIDChanged();
    void containerSizeChanged();
    void packagesChanged();
    void customVariablesChanged();

private:
    QString m_containerID;
    ContainerSize m_containerSize;
    QVector<Package*> m_packages;
    QMap<HaulerType, QVariantMap> m_customVariables;


    void deepCopy(const Container &other);  // Helper function to perform deep copy
};

}

Q_DECLARE_METATYPE(ContainerCore::Container)
Q_DECLARE_METATYPE(ContainerCore::Container*)


#endif // CONTAINER_H
