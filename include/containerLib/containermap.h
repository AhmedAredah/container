/**
 * @file containermap.h
 * @brief Container management and storage system with database support
 * @author Ahmed Aredah
 * @date 2024
 *
 * This file defines the ContainerMap class, which provides a comprehensive
 * container management system with both in-memory and database storage options.
 * It supports CRUD operations, queries based on various criteria, and JSON
 * serialization/deserialization.
 */


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

/**
 * @class ContainerMap
 * @brief Manages a collection of containers with optional database persistence
 *
 * ContainerMap provides a comprehensive system for managing shipping containers,
 * supporting both in-memory storage and SQL database persistence. It includes
 * functionality for:
 * - Adding and removing containers
 * - Querying containers based on various criteria
 * - Managing container lifecycles
 * - JSON serialization/deserialization
 * - Database operations
 *
 * Thread safety is ensured through mutex protection of critical operations.
 */
class CONTAINER_EXPORT ContainerMap : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs an empty ContainerMap using in-memory storage
     * @param parent Optional parent QObject for memory management
     */
    explicit ContainerMap(QObject *parent = nullptr);

    /**
     * @brief Constructs a ContainerMap with database storage
     * @param dbLocation Path to the SQLite database file
     * @param parent Optional parent QObject for memory management
     */
    ContainerMap(const QString &dbLocation, QObject *parent = nullptr);

    /**
     * @brief Constructs a ContainerMap from a JSON object
     * @param json JSON object containing container data
     * @param parent Optional parent QObject for memory management
     */
    ContainerMap(const QJsonObject &json, QObject *parent = nullptr);

    /**
     * @brief Destructor that ensures proper cleanup of resources
     */
    virtual ~ContainerMap();

    /**
     * @brief Copy constructor
     * @param other Source ContainerMap to copy from
     */
    ContainerMap(const ContainerMap &other);

    /**
     * @brief Assignment operator
     * @param other Source ContainerMap to copy from
     * @return Reference to this ContainerMap
     */
    ContainerMap& operator=(const ContainerMap &other);

    /**
     * @brief Sets whether the ContainerMap is being used through Python bindings
     * @param isRunningThroughPython True if used through Python
     */
    void setIsRunningThroughPython(bool isRunningThroughPython);

    /**
     * @brief Adds a container to the map
     * @param id Unique identifier for the container
     * @param container Pointer to the container to add
     * @param addingTime Time when the container was added (NaN for unspecified)
     * @param leavingTime Time when the container should leave (NaN for unspecified)
     */
    void addContainer(const QString &id, Container* container, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    /**
    * @brief Adds multiple containers to the map
    * @param containers Vector of container pointers to add
    * @param addingTime Time when the containers were added (NaN for unspecified)
    * @param leavingTime Time when the containers should leave (NaN for unspecified)
    * 
    * Each container is added to either the in-memory map or database storage,
    * depending on the current configuration.
    */
    void addContainers(const QVector<Container*> &containers, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    /**
    * @brief Adds containers from a JSON object representation
    * @param json JSON object containing container data
    * @param addingTime Time when the containers were added (NaN for unspecified)
    * @param leavingTime Time when the containers should leave (NaN for unspecified)
    * 
    * The JSON object should contain a "containers" array with individual container objects.
    * Each container object must include required fields like containerID and containerSize.
    */
    void addContainers(const QJsonObject &json, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));

    /**
    * @brief Retrieves a container by its unique identifier
    * @param id The container's unique identifier
    * @return Pointer to the container if found, nullptr otherwise
    * 
    * For database storage, the container is loaded from the database if not in cache.
    */
    Container* getContainerByID(const QString &id);

    /**
    * @brief Removes a container from the map by its ID
    * @param id The container's unique identifier
    * 
    * Removes the container from either in-memory storage or database,
    * and handles cleanup of associated resources.
    */
    void removeContainerByID(const QString &id);

    /**
    * @brief Retrieves all containers in the map
    * @return Map of container IDs to container pointers
    * @note For database storage, this loads all containers into memory
    */
    QMap<QString, Container*> getAllContainers() const;

    /**
    * @brief Retrieves the most recently added containers
    * @return Map of container IDs to container pointers
    * @note Returns cached containers for database storage
    */
    QMap<QString, Container*> getLatestContainers();

    /**
    * @brief Removes all containers from the map
    * 
    * Clears both in-memory and database storage if applicable.
    * Handles proper cleanup of container objects.
    */
    void clear();

    /**
    * @brief Copies all containers from another ContainerMap
    * @param other Source ContainerMap to copy from
    * 
    * Performs a deep copy of all containers and their associated data.
    */
    void copyFrom(ContainerMap &other);

    /**
    * @brief Returns the number of containers in the map
    * @return Number of containers
    */
    qsizetype size() const;

    /**
    * @brief Converts the ContainerMap to a JSON object
    * @return JSON object containing all container data
    */
    QJsonObject toJson() const;

    /**
    * @brief Retrieves containers based on their added time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Vector of containers meeting the condition
    */
    QVector<Container *> getContainersByAddedTime(const QString &condition, double referenceTime);

    /**
    * @brief Removes and returns containers based on their added time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Vector of removed containers meeting the condition
    */
    QVector<Container *> dequeueContainersByAddedTime(const QString &condition, double referenceTime);

    /**
    * @brief Counts containers based on their added time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Number of containers meeting the condition
    */
    qsizetype countContainersByAddedTime(const QString &condition, double referenceTime);

    /**
    * @brief Retrieves containers based on their leaving time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Vector of containers meeting the condition
    */
    QVector<Container *> getContainersByLeavingTime(const QString &condition, double referenceTime);

    /**
    * @brief Removes and returns containers based on their leaving time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Vector of removed containers meeting the condition
    */
    QVector<Container *> dequeueContainersByLeavingTime(const QString &condition, double referenceTime);

    /**
    * @brief Counts containers based on their leaving time
    * @param condition Comparison operator (">", ">=", "<", "<=", "=", "!=")
    * @param referenceTime Time to compare against
    * @return Number of containers meeting the condition
    */
    qsizetype countContainersByLeavingTime(const QString &condition, double referenceTime);

    /**
    * @brief Retrieves containers with a specific next destination
    * @param destination The destination to search for
    * @return Vector of containers with matching destination
    */
    QVector<Container*> getContainersByNextDestination(const QString &destination);

    /**
    * @brief Removes and returns containers with a specific next destination
    * @param destination The destination to search for
    * @return Vector of removed containers with matching destination
    */
    QVector<Container*> dequeueContainersByNextDestination(const QString &destination);

    /**
    * @brief Counts containers with a specific next destination
    * @param destination The destination to count
    * @return Number of containers with matching destination
    */
    qsizetype countContainersByNextDestination(const QString &destination);

    /**
    * @brief Creates containers from a JSON object
    * @param json JSON object containing container data
    * @return Vector of created containers
    * @note The caller is responsible for memory management of returned containers
    */
    static QVector<Container*> loadContainersFromJson(const QJsonObject &json);


    /**
    * @brief Serialization operator for ContainerMap
    * @param out Output stream
    * @param containerMap ContainerMap to serialize
    * @return Reference to the output stream
    */
    friend QDataStream &operator<<(QDataStream &out, const ContainerMap &containerMap);

    /**
    * @brief Deserialization operator for ContainerMap
    * @param in Input stream
    * @param containerMap ContainerMap to deserialize into
    * @return Reference to the input stream
    */
    friend QDataStream &operator>>(QDataStream &in, ContainerMap &containerMap);

    /**
    * @brief Converts the ContainerMap to a QVariant
    * @return QVariant containing the ContainerMap data
    */
    QVariant toVariant() const;

    /**
    * @brief Creates a ContainerMap from a QVariant
    * @param variant QVariant containing ContainerMap data
    * @return Newly created ContainerMap
    */
    static ContainerMap fromVariant(const QVariant &variant);

signals:
    /**
     * @brief Emitted when the container collection changes
     */
    void containersChanged();

    /**
     * @brief Emitted when a database error occurs
     * @param error Description of the error
     */
    void databaseErrorOccurred(const QString &error) const;

private:

    /** @brief Map storing containers in memory */
    QMap<QString, Container*> m_containers;

    /** @brief Database connection for persistent storage */
    QSqlDatabase m_db;

    /** @brief Cache for frequently accessed containers */
    ContainerCore::ContainerCache<Container> m_cache;

    /** @brief Mutex for thread synchronization */
    mutable QMutex m_mutex;

    /** @brief Flag indicating whether database storage is enabled */
    bool m_useDatabase;

    /** @brief Flag indicating whether running through Python bindings */
    bool m_isRunningThroughPython = false;

    /**
     * @brief Performs deep copy of container data
     * @param other Source ContainerMap to copy from
     */
    void deepCopy(const ContainerMap &other);

    // Helper functions for database interaction

    /**
    * @brief Opens or creates a SQLite database connection
    * @param dbLocation Path to the SQLite database file
    * @return true if connection successful, false otherwise
    * 
    * Creates a unique connection name based on object address to avoid conflicts.
    * If the database file doesn't exist, it will be created.
    * Emits databaseErrorOccurred signal on failure.
    */
    bool openDatabase(const QString &dbLocation);

    /**
    * @brief Creates all required database tables if they don't exist
    * 
    * Creates the following tables:
    * - Containers: Main container information (id, size, location, times)
    * - NextDestinations: Container routing information
    * - MovementHistory: Container location history
    * - Packages: Associated package data 
    * - CustomVariables: Container-specific variable storage
    */
    void createTables();

    /**
    * @brief Loads a container and all its related data from the database
    * @param id The container's unique identifier
    * 
    * Loads:
    * - Basic container information
    * - Associated packages
    * - Custom variables
    * - Next destinations
    * - Movement history
    * Adds loaded container to cache on success.
    */
    void loadContainerFromDB(const QString &id);

    /**
    * @brief Saves a container and all its related data to the database
    * @param container The container to save
    * 
    * Uses a transaction to ensure data consistency.
    * Saves all container properties and related data.
    * Rolls back transaction if any operation fails.
    * Emits databaseErrorOccurred signal on failure.
    */
    void saveContainerToDB(const Container &container);

    /**
    * @brief Removes a container and all its related data from the database
    * @param id The container's unique identifier
    * 
    * Removes container and cascades deletion to:
    * - Associated packages
    * - Custom variables
    * - Next destinations
    * - Movement history
    * Emits databaseErrorOccurred signal on failure.
    */
    void removeContainerFromDB(const QString &id);

    /**
    * @brief Removes all data from all container-related database tables
    * 
    * Clears the following tables:
    * - Containers
    * - Packages
    * - CustomVariables
    * - NextDestinations
    * - MovementHistory
    */
    void clearDatabase();

    /**
    * @brief Loads related data for an existing container from the database
    * @param container Container to load additional data for
    * 
    * Loads:
    * - Associated packages
    * - Custom variables
    * - Next destinations
    * - Movement history
    */
    void loadAdditionalContainerData(Container &container) const;

    /**
    * @brief Retrieves a container by ID from either cache or database
    * @param id The container's unique identifier
    * @return Pointer to the container if found, nullptr otherwise
    * 
    * For database storage:
    * - Checks cache first
    * - Loads from database if not in cache
    */
    Container* getContainer(const QString &id);

    /**
    * @brief Removes a container from storage
    * @param id The container's unique identifier
    * 
    * For in-memory storage:
    * - Removes from map and deletes object
    * For database storage:
    * - Removes from database and cache
    */
    void removeContainer(const QString &id);

    /**
    * @brief Utility function for adding a container to storage
    * @param id Container's unique identifier
    * @param container Pointer to container to add
    * @param addingTime Time when container was added (NaN for unspecified)
    * @param leavingTime Time when container should leave (NaN for unspecified)
    * 
    * For database storage:
    * - Saves to database
    * - Adds to cache
    * For in-memory storage:
    * - Adds to container map
    * Emits containersChanged signal.
    */
    void addContainerUtil(const QString &id, Container* container, double addingTime = std::nan("notDefined"), double leavingTime = std::nan("notDefined"));
    
    /**
    * @brief Utility function for clearing container storage
    * @param enableClearDatabase Whether to clear database if using database storage
    * @param enableEmit Whether to emit containersChanged signal
    * 
    * For in-memory storage:
    * - Deletes all containers if not running through Python
    * - Clears container map
    * For database storage:
    * - Optionally clears database
    * - Clears cache
    */
    void clearUtil(bool enableClearDatabase = false, bool enableEmit = true);

    /**
    * @brief Initializes QCoreApplication if needed for database operations
    * 
    * Creates a QCoreApplication instance if one doesn't exist.
    * Required for proper Qt database functionality.
    * Registers cleanup handler for application exit.
    */
    void initializeQtCoreIfNeeded();
};
}

#endif // CONTAINERMAP_H
