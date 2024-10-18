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
    if (json.contains("databaseLocation")) {
        m_useDatabase = true;
        QString dbLocation = json["databaseLocation"].toString();
        if (!openDatabase(dbLocation)) {
            emit databaseErrorOccurred("Failed to open or create database.");
            m_useDatabase = false;
        }
    } else {
        // Initialize using QMap
        m_useDatabase = false;
        QJsonArray containersArray = json["containers"].toArray();
        for (const QJsonValue &value : containersArray) {
            if (value.isObject()) {
                QJsonObject containerObject = value.toObject();
                Container *container = new Container(containerObject, this);
                addContainer(container->getContainerID(), container);
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

void ContainerMap::deepCopy(const ContainerMap &other)
{
    m_useDatabase = other.m_useDatabase;
    m_cache.setMaxCost(other.m_cache.maxCost());

    if (m_useDatabase) {
        // Copy database reference
        m_db = other.m_db;
        // Copy cached containers
        for (auto &id : other.m_cache.keys()) {
            Container* containerCopy =
                new Container(*other.m_cache.object(id));
            m_cache.insert(id, containerCopy);
        }
    } else {
        for (auto it = other.m_containers.cbegin();
             it != other.m_containers.cend(); ++it) {
            Container* containerCopy = new Container(*it.value());
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
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbLocation);

    if (!m_db.open()) {
        qDebug() << "Database Error: " << m_db.lastError().text();
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
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS Containers ("
               "id TEXT PRIMARY KEY, "
               "size INTEGER)");

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
    QSqlQuery query;
    query.prepare("SELECT size FROM Containers WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        Container::ContainerSize size =
            static_cast<Container::ContainerSize>(query.value(0).toInt());

        Container* container = new Container(id, size);
        m_cache.insert(id, container);

        // Load packages
        QSqlQuery packageQuery;
        packageQuery.prepare("SELECT id FROM Packages "
                             "WHERE container_id = :id");
        packageQuery.bindValue(":id", id);
        if (packageQuery.exec()) {
            QVector<Package*> packages;
            while (packageQuery.next()) {
                Package* package = new Package();
                package->setPackageID(packageQuery.value(0).toString());
                packages.append(package);
            }
            container->setPackages(packages);
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
        }
    } else {
        emit databaseErrorOccurred("Failed to load container from database.");
    }
}

// Helper function to save container to database
void ContainerMap::saveContainerToDB(const Container &container)
{
    QSqlQuery query;
    query.prepare("REPLACE INTO Containers (id, size) VALUES (:id, :size)");
    query.bindValue(":id", container.getContainerID());
    query.bindValue(":size", static_cast<int>(container.getContainerSize()));

    if (!query.exec()) {
        emit databaseErrorOccurred("Failed to save container to database.");
    }

    // Save packages
    for (auto package : container.getPackages()) {
        QSqlQuery packageQuery;
        packageQuery.prepare("REPLACE INTO Packages (id, container_id) "
                             "VALUES (:id, :container_id)");
        packageQuery.bindValue(":id", package->packageID());
        packageQuery.bindValue(":container_id", container.getContainerID());

        if (!packageQuery.exec()) {
            emit databaseErrorOccurred("Failed to save package to database.");
        }
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
            customVarQuery.bindValue(":container_id", container.getContainerID());
            customVarQuery.bindValue(":key", varIt.key());
            customVarQuery.bindValue(":value", varIt.value());

            if (!customVarQuery.exec()) {
                emit databaseErrorOccurred("Failed to save custom "
                                           "variable to database.");
            }
        }
    }
}

// Helper function to remove container from database
void ContainerMap::removeContainerFromDB(const QString &id)
{
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
}

// Helper function to clear the database
void ContainerMap::clearDatabase()
{
    QSqlQuery query;
    query.exec("DELETE FROM Containers");
    query.exec("DELETE FROM Packages");
    query.exec("DELETE FROM CustomVariables");
}

}
