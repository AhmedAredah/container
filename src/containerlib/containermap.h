#ifndef CONTAINERMAP_H
#define CONTAINERMAP_H

#include "Container_global.h"
#include <QObject>
#include <QMap>
#include <QVariant>
#include <QDataStream>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCache>
#include <QJsonObject>
#include <QJsonArray>
#include "containercache.h"
#include "container.h"
#include <QCoreApplication>


namespace ContainerCore {
class CONTAINER_EXPORT ContainerMap : public QObject
{
    Q_OBJECT
public:
    // Constructor without SQL
    explicit ContainerMap(QObject *parent = nullptr);

    // Constructor with SQL support
    ContainerMap(const QString &dbLocation, QObject *parent = nullptr);

    // Constructor to initialize from QJsonObject
    ContainerMap(const QJsonObject &json, QObject *parent = nullptr);


    ~ContainerMap();

    // Copy constructor
    ContainerMap(const ContainerMap &other);

    // Assignment operator
    ContainerMap& operator=(const ContainerMap &other);

    void setIsRunningThroughPython(bool isRunningThroughPython);

    // Add a container to the map
    void addContainer(const QString &id, Container* container, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    // Add a list of containers to the map
    void addContainers(const QVector<Container*> &containers, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    void addContainers(const QJsonObject &json, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    // Get a container by ID
    Container* getContainerByID(const QString &id);

    // Remove a container by ID
    void removeContainerByID(const QString &id);

    // Get the map of containers
    QMap<QString, Container*> getAllContainers() const;

    QMap<QString, Container*> getLatestContainers();

    // Clear the map
    void clear();

    // Copy contents from another ContainerMap
    void copyFrom(ContainerMap &other);

    qsizetype size() const;

    QJsonObject toJson() const;

    // Get containers by the given added time and a condition.
    QVector<Container *> getContainersByAddedTime(const QString &condition, double referenceTime);

    // Dequeues containers by the given added time and a condition.
    QVector<Container *> dequeueContainersByAddedTime(const QString &condition, double referenceTime);

    // count containers by the given added time and a condition
    qsizetype countContainersByAddedTime(const QString &condition, double referenceTime);

    // Get containers by the given leaving time and a condition
    QVector<Container *> getContainersByLeavingTime(const QString &condition, double referenceTime);

    // Dequeues containers by the given leaving time and a condition
    QVector<Container *> dequeueContainersByLeavingTime(const QString &condition, double referenceTime);

    // count containers by the given leaving time and a condition
    qsizetype countContainersByLeavingTime(const QString &condition, double referenceTime);

    // Get containers by the given next destination
    QVector<Container*> getContainersByNextDestination(const QString &destination);

    // Dequeues containers by the given next destination
    QVector<Container*> dequeueContainersByNextDestination(const QString &destination);

    // Count containers by the given next destination
    qsizetype countContainersByNextDestination(const QString &destination);

    // Load containers from json object to a QVector
    static QVector<Container*> loadContainersFromJson(const QJsonObject &json, QObject *parent = nullptr);


    // Serialization and deserialization
    friend QDataStream &operator<<(QDataStream &out, const ContainerMap &containerMap);
    friend QDataStream &operator>>(QDataStream &in, ContainerMap &containerMap);

    // Convert to QVariant
    QVariant toVariant() const;

    // Convert from QVariant
    static ContainerMap fromVariant(const QVariant &variant);

signals:
    void containersChanged();
    void databaseErrorOccurred(const QString &error) const;

private:
    QMap<QString, Container*> m_containers;
    QSqlDatabase m_db;
    ContainerCore::ContainerCache<Container> m_cache;
    mutable QMutex m_mutex;

    bool m_useDatabase;
    bool m_isRunningThroughPython = false;

    // Helper function to deep copy containers
    void deepCopy(const ContainerMap &other);

    // Helper functions for database interaction
    bool openDatabase(const QString &dbLocation);
    void createTables();
    void loadContainerFromDB(const QString &id);
    void saveContainerToDB(const Container &container);
    void removeContainerFromDB(const QString &id);
    void clearDatabase();
    void loadAdditionalContainerData(Container &container) const;
    Container* getContainer(const QString &id);
    void removeContainer(const QString &id);
    void addContainerUtil(const QString &id, Container* container, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));
    void clearUtil(bool enableClearDatabase = false, bool enableEmit = true);
    void initializeQtCoreIfNeeded();
};
}

#endif // CONTAINERMAP_H
