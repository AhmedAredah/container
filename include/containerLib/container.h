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
    Q_PROPERTY(double addedTime READ getContainerAddedTime WRITE setContainerAddedTime NOTIFY containerAddedTimeChanged FINAL)
    Q_PROPERTY(double leavingTime READ getContainerLeavingTime WRITE setContainerLeavingTime NOTIFY containerLeavingTimeChanged FINAL)
    Q_PROPERTY(ContainerSize containerSize READ getContainerSize WRITE setContainerSize NOTIFY containerSizeChanged FINAL)
    Q_PROPERTY(QVector<Package*> packages READ getPackages WRITE setPackages NOTIFY packagesChanged FINAL)
    Q_PROPERTY(QMap<HaulerType, QVariantMap> customVariables READ getCustomVariables WRITE setCustomVariables NOTIFY customVariablesChanged FINAL)
    Q_PROPERTY(QString containerCurrentLocation READ getContainerCurrentLocation WRITE setContainerCurrentLocation NOTIFY containerCurrentLocationChanged FINAL)
    Q_PROPERTY(QVector<QString> containerNextDestinations READ getContainerNextDestinations WRITE setContainerNextDestinations NOTIFY containerNextDestinationsChanged FINAL)
    Q_PROPERTY(QVector<QString> containerMovementHistory READ getContainerMovementHistory WRITE setContainerMovementHistory NOTIFY containerMovementHistoryChanged FINAL)

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
        airTransport,
        noHauler
    };
    Q_ENUM(HaulerType)

    explicit Container(QObject *parent = nullptr);
    Container(const QString &id, ContainerSize size, QObject *parent = nullptr);
    Container(const QJsonObject &json, QObject *parent = nullptr);
    Container(const Container &other);  // Copy constructor
    Container &operator=(const Container &other);  // Copy assignment operator
    void setIsRunningThroughPython(bool isRunningThroughPython);

    virtual ~Container();

    // Getter and Setter for containerID
    QString getContainerID() const;
    void setContainerID(const QString &id);

    double getContainerAddedTime() const;
    void setContainerAddedTime(const double &time);

    double getContainerLeavingTime() const;
    void setContainerLeavingTime(const double &time);

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

    // Getters and Setters for current location
    QString getContainerCurrentLocation() const;
    void setContainerCurrentLocation(const QString &location);

    // Getters and Setters for next destinations
    QVector<QString> getContainerNextDestinations() const;
    void setContainerNextDestinations(const QVector<QString> &destinations);
    void addDestination(const QString &destination);
    bool removeDestination(const QString &destination);

    // Getters and Setters for history
    QVector<QString> getContainerMovementHistory() const;
    void setContainerMovementHistory(const QVector<QString> &history);
    void addMovementHistory(const QString &history);
    bool removeMovementHistory(const QString &history);

    void clear();

    QJsonObject toJson() const;

    friend QDataStream &operator<<(QDataStream &out, const Container &container);
    friend QDataStream &operator>>(QDataStream &in, Container &container);

signals:
    void containerIDChanged();
    void containerAddedTimeChanged();
    void containerLeavingTimeChanged();
    void containerSizeChanged();
    void packagesChanged();
    void customVariablesChanged();
    void containerCurrentLocationChanged();
    void containerNextDestinationsChanged();
    void containerMovementHistoryChanged();

private:
    QString m_containerID;
    double m_addedTime;
    double m_leavingTime;
    ContainerSize m_containerSize;
    QVector<Package*> m_packages;
    QMap<HaulerType, QVariantMap> m_customVariables;
    QString m_containerCurrentLocation;
    QVector<QString> m_containerNextDestinations;
    QVector<QString> m_containerMovementHistory;
    bool m_isRunningThroughPython = false;

    void deepCopy(const Container &other);  // Helper function to perform deep copy
};

}

Q_DECLARE_METATYPE(ContainerCore::Container)
Q_DECLARE_METATYPE(ContainerCore::Container*)


#endif // CONTAINER_H
