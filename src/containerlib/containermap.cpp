#include "containermap.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QDebug>
#include <QFile>

namespace ContainerCore {

#define CONTAINER_CORE_CACHE_SIZE 200

ContainerMap::ContainerMap(QObject *parent)
    : QObject(parent),
    m_cache(CONTAINER_CORE_CACHE_SIZE), // Set cache size to 200 containers
    m_useDatabase(false)
{
}

ContainerMap::ContainerMap(const QString &dbLocation, QObject *parent)
    : QObject(parent), m_cache(CONTAINER_CORE_CACHE_SIZE), m_useDatabase(true)
{
    if (!openDatabase(dbLocation)) {
        emit databaseErrorOccurred("Failed to open or create database.");
        m_useDatabase = false;
    } else {
        createTables();
    }
}

ContainerMap::ContainerMap(const QJsonObject &json, QObject *parent)
    : QObject(parent), m_cache(CONTAINER_CORE_CACHE_SIZE)
{
    // Check if databaseLocation exists in JSON
    if (json.contains("databaseLocation") &&
        json["databaseLocation"].isString()) {
        m_useDatabase = true;
        QString dbLocation = json["databaseLocation"].toString();
        if (!openDatabase(dbLocation)) {
            emit databaseErrorOccurred("Failed to open or create database.");
            m_useDatabase = false;
        }
    } else {
        // Initialize using QMap
        m_useDatabase = false;
        if (json.contains("containers") && json["containers"].isArray()) {
            QJsonArray containersArray = json["containers"].toArray();
            for (const QJsonValue &value : containersArray) {
                if (value.isObject()) {
                    try {
                        QJsonObject containerObject = value.toObject();
                        Container *container =
                            new Container(containerObject, this);
                        addContainer(container->getContainerID(), container);
                    } catch (const std::exception &e) {
                        qDebug() << "Error initializing container from JSON: "
                                 << e.what();
                    }
                }
            }
        }
    }
}


ContainerMap::~ContainerMap()
{
    clear();
    if (m_useDatabase) {
        m_db.close();
    }
}

// Copy constructor
ContainerMap::ContainerMap(const ContainerMap &other)
    : QObject(other.parent())
{
    QMutexLocker locker(&other.m_mutex);
    deepCopy(other);
}

// Assignment operator
ContainerMap& ContainerMap::operator=(const ContainerMap &other)
{
    if (this != &other) {
        QMutexLocker locker(&other.m_mutex);
        clear();
        deepCopy(other);
    }
    return *this;
}

void ContainerMap::addContainer(const QString &id, Container* container)
{
    QMutexLocker locker(&m_mutex);

    if (m_useDatabase) {
        saveContainerToDB(*container);
        m_cache.insert(id, container, 1);
    } else {
        m_containers.insert(id, container);
    }
    emit containersChanged();
}

void ContainerMap::addContainers(const QVector<Container*> &containers)
{
    for (Container* container : containers) {
        if (container) {
            addContainer(container->getContainerID(), container);
        }
    }
}

Container* ContainerMap::getContainer(const QString &id)
{
    QMutexLocker locker(&m_mutex);

    if (m_useDatabase) {
        if (!m_cache.contains(id)) {
            loadContainerFromDB(id);
        }
        return m_cache.object(id);
    } else {
        return m_containers.value(id, nullptr);
    }
}

void ContainerMap::removeContainer(const QString &id)
{
    QMutexLocker locker(&m_mutex);

    if (m_useDatabase) {
        removeContainerFromDB(id);
        m_cache.remove(id);
    } else {
        delete m_containers.take(id);
    }
    emit containersChanged();
}

QMap<QString, Container*> ContainerMap::containers() const
{
    QMutexLocker locker(&m_mutex);

    if (m_useDatabase) {
        QMap<QString, Container*> result;
        for (auto &id : m_cache.keys()) {
            result.insert(id, m_cache.object(id));
        }
        return result;
    } else {
        return m_containers;
    }
}

void ContainerMap::clear()
{
    QMutexLocker locker(&m_mutex);

    if (m_useDatabase) {
        clearDatabase();
        m_cache.clear();
    } else {
        qDeleteAll(m_containers);
        m_containers.clear();
    }
    emit containersChanged();
}

void ContainerMap::copyFrom(ContainerMap &other)
{
    QMutexLocker locker(&m_mutex);
    QMutexLocker otherLocker(&other.m_mutex);

    if (other.m_useDatabase) {
        // If the source ContainerMap is using a database,
        // iterate over all container IDs in the database
        QSqlQuery query(other.m_db);
        query.prepare("SELECT id FROM Containers");

        if (query.exec()) {
            while (query.next()) {
                QString containerID = query.value(0).toString();
                Container* container = other.getContainer(containerID);
                if (container) {
                    // Deep copy of the container
                    addContainer(containerID,
                                 new Container(*container));
                }
            }
        } else {
            emit databaseErrorOccurred("Failed to query containers "
                                       "from the source database.");
        }
    } else {
        // If the source ContainerMap is using in-memory storage,
        // iterate over the keys in m_containers
        for (const auto &containerID : other.m_containers.keys()) {
            Container* container = other.getContainer(containerID);
            if (container) {
                // Deep copy of the container
                addContainer(containerID,
                             new Container(*container));
            }
        }
    }
}

qsizetype ContainerMap::size() const
{
    QMutexLocker locker(&m_mutex);
    qsizetype count = 0;

    if (m_useDatabase) {
        // Query the database to count the number of containers
        QSqlQuery query;
        query.prepare("SELECT COUNT(*) FROM Containers");

        if (query.exec() && query.next()) {
            count = static_cast<qsizetype>(query.value(0).toLongLong());
        } else {
            emit databaseErrorOccurred("Failed to count containers "
                                       "in the database.");
        }
    } else {
        // Return the size of m_containers using qsizetype
        count = static_cast<qsizetype>(m_containers.size());
    }

    return count;
}

QJsonObject ContainerMap::toJson() const
{
    QMutexLocker locker(&m_mutex);
    QJsonObject jsonObject;

    if (m_useDatabase) {
        // Add the database location to the JSON object
        jsonObject["databaseLocation"] = m_db.databaseName();
    } else {
        // Add containers to the JSON object
        QJsonArray containerArray;

        for (auto it = m_containers.constBegin();
             it != m_containers.constEnd(); ++it)
        {
            if (it.value())
            {
                containerArray.append(it.value()->toJson());
            }
        }

        jsonObject["containers"] = containerArray;
    }

    return jsonObject;
}

QVector<Container *> ContainerMap::
    getContainersByNextDestination(const QString &destination)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety
    QVector<Container*> result;

    if (m_useDatabase) {
        // If using a database, query for containers with the
        // specified next destination
        QSqlQuery query;
        query.prepare("SELECT container_id FROM NextDestinations "
                      "WHERE destination = :destination");
        query.bindValue(":destination", destination);

        if (query.exec()) {
            while (query.next()) {
                QString containerId = query.value(0).toString();
                Container* container = getContainer(containerId);
                if (container) {
                    result.append(container);
                }
            }
        } else {
            emit databaseErrorOccurred("Failed to query containers "
                                       "by next destination.");
        }
    } else {
        // If not using a database, search in-memory containers
        for (auto it = m_containers.begin(); it != m_containers.end(); ++it) {
            Container* container = it.value();
            if (container &&
                container->getContainerNextDestinations().
                             contains(destination)) {
                result.append(container);
            }
        }
    }

    return result;
}

QVector<Container *> ContainerMap::dequeueContainersByNextDestination(const QString &destination)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety
    QVector<Container*> matchingContainers;

    if (m_useDatabase) {
        // Retrieve containers from the database
        QSqlQuery query;
        query.prepare("SELECT id FROM Containers WHERE id IN ("
                      "SELECT container_id FROM NextDestinations WHERE "
                      "destination = :destination)");
        query.bindValue(":destination", destination);

        if (query.exec()) {
            while (query.next()) {
                QString containerID = query.value(0).toString();
                Container* container = getContainer(containerID);
                if (container) {
                    matchingContainers.append(container);
                    // Remove the container from the database and cache
                    removeContainer(containerID);
                }
            }
        } else {
            emit databaseErrorOccurred("Failed to dequeue containers "
                                       "by next destination.");
        }
    } else {
        // Retrieve containers from the in-memory map
        for (auto it = m_containers.begin(); it != m_containers.end();) {
            Container* container = it.value();
            if (container->getContainerNextDestinations().
                contains(destination)) {
                matchingContainers.append(container);
                delete it.value(); // Ensure memory is cleaned
                it = m_containers.erase(it);
            } else {
                ++it;
            }
        }
    }

    emit containersChanged();
    return matchingContainers;
}

void ContainerMap::deepCopy(const ContainerMap &other)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety for this object
    QMutexLocker otherLocker(&other.m_mutex); // Ensure thread safety for the source object


    m_useDatabase = other.m_useDatabase;
    m_cache.setMaxCost(other.m_cache.maxCost());

    if (m_useDatabase) {
        // Copy database reference
        m_db = other.m_db;
        // Copy cached containers
        for (auto &id : other.m_cache.keys()) {
            Container* originalContainer = other.m_cache.object(id);
            Container* containerCopy = new Container(*originalContainer);

            // Ensure to copy next destinations and movement history
            containerCopy->setContainerNextDestinations(
                originalContainer->getContainerNextDestinations());
            containerCopy->setContainerMovementHistory(
                originalContainer->getContainerMovementHistory());


            m_cache.insert(id, containerCopy);
        }
    } else {
        for (auto it = other.m_containers.cbegin();
             it != other.m_containers.cend(); ++it) {
            Container* originalContainer = it.value();
            Container* containerCopy = new Container(*originalContainer);

            // Ensure to copy next destinations and movement history
            containerCopy->setContainerNextDestinations(
                originalContainer->getContainerNextDestinations());
            containerCopy->setContainerMovementHistory(
                originalContainer->getContainerMovementHistory());

            m_containers.insert(it.key(), containerCopy);
        }
    }
}

// Serialization
QDataStream &operator<<(QDataStream &out, const ContainerMap &containerMap)
{
    QMutexLocker locker(&containerMap.m_mutex);

    out << containerMap.m_containers.size();
    for (auto it = containerMap.m_containers.cbegin();
         it != containerMap.m_containers.cend(); ++it) {
        out << it.key() << *it.value();
    }
    return out;
}

// Deserialization
QDataStream &operator>>(QDataStream &in, ContainerMap &containerMap)
{
    QMutexLocker locker(&containerMap.m_mutex);

    int size;
    in >> size;
    for (int i = 0; i < size; ++i) {
        QString id;
        Container *container = new Container();
        in >> id >> *container;
        containerMap.addContainer(id, container);
    }
    return in;
}

// Convert to QVariant
QVariant ContainerMap::toVariant() const
{
    QMutexLocker locker(&m_mutex);

    QVariantMap variantMap;
    for (auto it = m_containers.cbegin(); it != m_containers.cend(); ++it) {
        variantMap.insert(it.key(), QVariant::fromValue(*it.value()));
    }
    return variantMap;
}

// Convert from QVariant
ContainerMap ContainerMap::fromVariant(const QVariant &variant)
{
    ContainerMap containerMap;
    QVariantMap variantMap = variant.toMap();
    for (auto it = variantMap.cbegin(); it != variantMap.cend(); ++it) {
        Container *container = new Container();
        *container = it.value().value<Container>();
        containerMap.addContainer(it.key(), container);
    }
    return containerMap;
}

// Helper function to open SQLite database
bool ContainerMap::openDatabase(const QString &dbLocation)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbLocation);

    if (!m_db.open()) {
        qDebug() << "Database Error: " << m_db.lastError().text();
        emit databaseErrorOccurred("Database Error: " +
                                   m_db.lastError().text());
        return false;
    }

    // Check if the database file exists. If it doesn't, create it.
    QFile dbFile(dbLocation);
    if (!dbFile.exists()) {
        qDebug() << "Database file does not exist. Creating new database.";
        // Creating new database and tables
        createTables();
    }

    return true;
}

// Helper function to create necessary tables in SQLite
void ContainerMap::createTables()
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Containers ("
               "id TEXT PRIMARY KEY, "
               "size INTEGER, "
               "currentLocation TEXT);");

    query.exec("CREATE TABLE IF NOT EXISTS NextDestinations ("
               "container_id TEXT, "
               "destination TEXT, "
               "FOREIGN KEY(container_id) REFERENCES Containers(id));");

    query.exec("CREATE TABLE IF NOT EXISTS MovementHistory ("
               "container_id TEXT, "
               "history TEXT, "
               "FOREIGN KEY(container_id) REFERENCES Containers(id));");

    query.exec("CREATE TABLE IF NOT EXISTS Packages ("
               "id TEXT PRIMARY KEY, "
               "container_id TEXT, "
               "FOREIGN KEY(container_id) REFERENCES Containers(id))");

    query.exec("CREATE TABLE IF NOT EXISTS CustomVariables ("
               "hauler_type INTEGER, "
               "container_id TEXT, "
               "key TEXT, "
               "value BLOB, "
               "PRIMARY KEY(hauler_type, container_id, key), "
               "FOREIGN KEY(container_id) REFERENCES Containers(id))");
}

// Helper function to load container from database
void ContainerMap::loadContainerFromDB(const QString &id)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety

    QSqlQuery query;
    query.prepare("SELECT size, currentLocation FROM Containers "
                  "WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        Container::ContainerSize size =
            static_cast<Container::ContainerSize>(query.value(0).toInt());
        QString currentLocation = query.value(1).toString();

        Container* container = new Container(id, size);
        container->setContainerCurrentLocation(currentLocation);

        bool loadSuccessful = true;

        // Load packages
        QSqlQuery packageQuery;
        packageQuery.prepare("SELECT id FROM Packages WHERE "
                             "container_id = :id");
        packageQuery.bindValue(":id", id);
        if (packageQuery.exec()) {
            QVector<Package*> packages;
            while (packageQuery.next()) {
                Package* package = new Package();
                package->setPackageID(packageQuery.value(0).toString());
                packages.append(package);
            }
            container->setPackages(packages);
        } else {
            loadSuccessful = false;
            qDebug() << "Failed to load packages for container:"
                     << id << packageQuery.lastError().text();
        }

        // Load custom variables
        QSqlQuery customVarQuery;
        customVarQuery.prepare("SELECT hauler_type, key, value FROM "
                               "CustomVariables WHERE container_id = :id");
        customVarQuery.bindValue(":id", id);
        if (customVarQuery.exec()) {
            QMap<Container::HaulerType, QVariantMap> customVariables;
            while (customVarQuery.next()) {
                Container::HaulerType hauler =
                    static_cast<Container::HaulerType>(
                    customVarQuery.value(0).toInt());
                QString key = customVarQuery.value(1).toString();
                QVariant value = customVarQuery.value(2);
                customVariables[hauler].insert(key, value);
            }
            container->setCustomVariables(customVariables);
        } else {
            loadSuccessful = false;
            qDebug() << "Failed to load custom variables for container:"
                     << id << customVarQuery.lastError().text();
        }

        // Load next destinations
        QSqlQuery nextDestQuery;
        nextDestQuery.prepare("SELECT destination FROM NextDestinations "
                              "WHERE container_id = :id");
        nextDestQuery.bindValue(":id", id);
        QVector<QString> destinations;
        if (nextDestQuery.exec()) {
            while (nextDestQuery.next()) {
                destinations.append(nextDestQuery.value(0).toString());
            }
            container->setContainerNextDestinations(destinations);
        } else {
            loadSuccessful = false;
            qDebug() << "Failed to load next destinations for container:"
                     << id << nextDestQuery.lastError().text();
        }

        // Load movement history
        QSqlQuery historyQuery;
        historyQuery.prepare("SELECT history FROM MovementHistory WHERE "
                             "container_id = :id");
        historyQuery.bindValue(":id", id);
        QVector<QString> histories;
        if (historyQuery.exec()) {
            while (historyQuery.next()) {
                histories.append(historyQuery.value(0).toString());
            }
            container->setContainerMovementHistory(histories);
        } else {
            loadSuccessful = false;
            qDebug() << "Failed to load movement history for container:"
                     << id << historyQuery.lastError().text();
        }

        if (loadSuccessful) {
            // Proceed to insert container if all sub-loads are successful
            m_cache.insert(id, container);
        } else {
            emit databaseErrorOccurred("Failed to load complete "
                                       "data for container.");
            delete container; // Clean up to prevent memory leaks
        }
    } else {
        emit databaseErrorOccurred("Failed to load container from database.");
        qDebug() << "Failed to execute query to load container:"
                 << id << query.lastError().text();
    }
}


// Helper function to save container to database
void ContainerMap::saveContainerToDB(const Container &container)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety


    QSqlDatabase::database().transaction(); // Start transaction

    QSqlQuery query;
    query.prepare("REPLACE INTO Containers (id, size, currentLocation) "
                  "VALUES (:id, :size, :currentLocation)");
    query.bindValue(":id", container.getContainerID());
    query.bindValue(":size", static_cast<int>(container.getContainerSize()));
    query.bindValue(":currentLocation", container.getContainerCurrentLocation());

    bool allSuccessful = query.exec();

    // Save packages
    for (auto package : container.getPackages()) {
        if (!allSuccessful) break; // Stop if there's already a failure
        QSqlQuery packageQuery;
        packageQuery.prepare("REPLACE INTO Packages (id, container_id) "
                             "VALUES (:id, :container_id)");
        packageQuery.bindValue(":id", package->packageID());
        packageQuery.bindValue(":container_id", container.getContainerID());

        allSuccessful = allSuccessful && packageQuery.exec();
    }

    // Save custom variables
    for (auto it = container.getCustomVariables().constBegin();
         it != container.getCustomVariables().constEnd(); ++it) {
        for (auto varIt = it.value().constBegin();
             varIt != it.value().constEnd(); ++varIt) {
            QSqlQuery customVarQuery;
            customVarQuery.prepare("REPLACE INTO CustomVariables "
                                   "(hauler_type, container_id, key, value) "
                                   "VALUES (:hauler_type, "
                                   ":container_id, :key, :value)");
            customVarQuery.bindValue(":hauler_type",
                                     static_cast<int>(it.key()));
            customVarQuery.bindValue(":container_id",
                                     container.getContainerID());
            customVarQuery.bindValue(":key", varIt.key());
            customVarQuery.bindValue(":value", varIt.value());

            allSuccessful = allSuccessful && customVarQuery.exec();
        }
    }

    // Save next destinations
    // First, remove existing destinations to avoid duplicates
    QSqlQuery deleteDestinationsQuery;
    deleteDestinationsQuery.prepare("DELETE FROM NextDestinations "
                                    "WHERE container_id = :id");
    deleteDestinationsQuery.bindValue(":id", container.getContainerID());

    allSuccessful = allSuccessful && deleteDestinationsQuery.exec();


    for (const auto &destination : container.getContainerNextDestinations()) {
        if (!allSuccessful) break;
        QSqlQuery nextDestQuery;
        nextDestQuery.prepare("INSERT INTO NextDestinations "
                              "(container_id, destination) "
                              "VALUES (:id, :destination)");
        nextDestQuery.bindValue(":id", container.getContainerID());
        nextDestQuery.bindValue(":destination", destination);

        allSuccessful = allSuccessful && nextDestQuery.exec();
    }

    // Save movement history
    // First, remove existing history to avoid duplicates
    QSqlQuery deleteHistoryQuery;
    deleteHistoryQuery.prepare("DELETE FROM MovementHistory WHERE "
                               "container_id = :id");
    deleteHistoryQuery.bindValue(":id", container.getContainerID());
    allSuccessful = allSuccessful && deleteHistoryQuery.exec();


    for (const auto &history : container.getContainerMovementHistory()) {
        if (!allSuccessful) break;
        QSqlQuery historyQuery;
        historyQuery.prepare("INSERT INTO MovementHistory "
                             "(container_id, history) "
                             "VALUES (:id, :history)");
        historyQuery.bindValue(":id", container.getContainerID());
        historyQuery.bindValue(":history", history);

        allSuccessful = allSuccessful && historyQuery.exec();
    }

    if (allSuccessful) {
        QSqlDatabase::database().commit(); // Commit if all operations succeed
    } else {
        QSqlDatabase::database().rollback(); // Rollback if any operation fails
        emit databaseErrorOccurred("Failed to save container "
                                   "and related data to database.");
    }
}

// Helper function to remove container from database
void ContainerMap::removeContainerFromDB(const QString &id)
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety

    QSqlQuery query;
    query.prepare("DELETE FROM Containers WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        emit databaseErrorOccurred("Failed to remove container from database.");
    }

    QSqlQuery packageQuery;
    packageQuery.prepare("DELETE FROM Packages WHERE container_id = :id");
    packageQuery.bindValue(":id", id);

    if (!packageQuery.exec()) {
        emit databaseErrorOccurred("Failed to remove packages from database.");
    }

    QSqlQuery customVarQuery;
    customVarQuery.prepare("DELETE FROM CustomVariables "
                           "WHERE container_id = :id");
    customVarQuery.bindValue(":id", id);

    if (!customVarQuery.exec()) {
        emit databaseErrorOccurred("Failed to remove custom "
                                   "variables from database.");
    }

    // Remove next destinations associated with the container
    QSqlQuery nextDestQuery;
    nextDestQuery.prepare("DELETE FROM NextDestinations WHERE "
                          "container_id = :id");
    nextDestQuery.bindValue(":id", id);

    if (!nextDestQuery.exec()) {
        emit databaseErrorOccurred("Failed to remove next destinations "
                                   "from database.");
    }

    // Remove movement history associated with the container
    QSqlQuery historyQuery;
    historyQuery.prepare("DELETE FROM MovementHistory WHERE "
                         "container_id = :id");
    historyQuery.bindValue(":id", id);

    if (!historyQuery.exec()) {
        emit databaseErrorOccurred("Failed to remove movement "
                                   "history from database.");
    }
}

// Helper function to clear the database
void ContainerMap::clearDatabase()
{
    QMutexLocker locker(&m_mutex); // Ensure thread safety

    QSqlQuery query;
    query.exec("DELETE FROM Containers");
    query.exec("DELETE FROM Packages");
    query.exec("DELETE FROM CustomVariables");
    query.exec("DELETE FROM NextDestinations");
    query.exec("DELETE FROM MovementHistory");
}

}