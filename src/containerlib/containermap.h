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
#include "container.h"

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

    // Add a container to the map
    void addContainer(const QString &id, Container* container);

    // Add a list of containers to the map
    void addContainers(const QVector<Container*> &containers);

    // Get a container by ID
    Container* getContainer(const QString &id);

    // Remove a container by ID
    void removeContainer(const QString &id);

    // Get the map of containers
    QMap<QString, Container*> containers() const;

    // Clear the map
    void clear();

    // Copy contents from another ContainerMap
    void copyFrom(ContainerMap &other);

    qsizetype size() const;

    QJsonObject toJson() const;

    // Get containers by the given next destination
    QVector<Container*> getContainersByNextDestination(const QString &destination);

    // Dequeues containers by the given next destination
    QVector<Container*> dequeueContainersByNextDestination(const QString &destination);



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
    QCache<QString, Container> m_cache;
    mutable QMutex m_mutex;

    bool m_useDatabase;

    // Helper function to deep copy containers
    void deepCopy(const ContainerMap &other);

    // Helper functions for database interaction
    bool openDatabase(const QString &dbLocation);
    void createTables();
    void loadContainerFromDB(const QString &id);
    void saveContainerToDB(const Container &container);
    void removeContainerFromDB(const QString &id);
    void clearDatabase();
};
}

#endif // CONTAINERMAP_H