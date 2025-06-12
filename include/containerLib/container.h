/**
* @file container.h
* @brief Container class definition for shipping container management system
* @author Ahmed Aredah
* @date 2024
* 
* This file defines the Container class, which represents a shipping container
* with support for package storage, location tracking, custom variables, and
* movement history.
*/

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

/**
* @class Container
* @brief Represents a shipping container with package storage and tracking capabilities
* 
* The Container class provides comprehensive functionality for managing shipping containers:
* - Package storage and management
* - Location tracking and movement history
* - Custom variable storage for different hauler types
* - Size classification
* - JSON serialization/deserialization
* - Signal notifications for property changes
*/
class CONTAINER_EXPORT Container : public QObject
{
    Q_OBJECT

   /**
    * @property containerID
    * @brief Unique identifier for the container
    */
    Q_PROPERTY(QString containerID READ getContainerID WRITE setContainerID NOTIFY containerIDChanged FINAL)

   /**
    * @property addedTime
    * @brief Time when the container was added to the system
    */
    Q_PROPERTY(double addedTime READ getContainerAddedTime WRITE setContainerAddedTime NOTIFY containerAddedTimeChanged FINAL)


    Q_PROPERTY(double leavingTime READ getContainerLeavingTime WRITE setContainerLeavingTime NOTIFY containerLeavingTimeChanged FINAL)
    Q_PROPERTY(ContainerSize containerSize READ getContainerSize WRITE setContainerSize NOTIFY containerSizeChanged FINAL)
    Q_PROPERTY(QVector<Package*> packages READ getPackages WRITE setPackages NOTIFY packagesChanged FINAL)
    Q_PROPERTY(QMap<HaulerType, QVariantMap> customVariables READ getCustomVariables WRITE setCustomVariables NOTIFY customVariablesChanged FINAL)
    Q_PROPERTY(QString containerCurrentLocation READ getContainerCurrentLocation WRITE setContainerCurrentLocation NOTIFY containerCurrentLocationChanged FINAL)
    Q_PROPERTY(QVector<QString> containerNextDestinations READ getContainerNextDestinations WRITE setContainerNextDestinations NOTIFY containerNextDestinationsChanged FINAL)
    Q_PROPERTY(QVector<QString> containerMovementHistory READ getContainerMovementHistory WRITE setContainerMovementHistory NOTIFY containerMovementHistoryChanged FINAL)

public:
   /**
    * @enum ContainerSize
    * @brief Standard shipping container sizes
    */
    enum ContainerSize {
        twentyFT,             /**< Standard 20-foot container */
        twentyFT_HighCube,    /**< High cube 20-foot container */
        fourtyFT,            /**< Standard 40-foot container */
        fourtyFT_HighCube,   /**< High cube 40-foot container */
        fortyFiveFT,         /**< Standard 45-foot container */
        fortyFiveFT_HighCube,/**< High cube 45-foot container */
        tenFT,               /**< Standard 10-foot container */
        thirtyFT,            /**< Standard 30-foot container */
        fortyEightFT,        /**< Standard 48-foot container */
        fiftyThreeFT,        /**< Standard 53-foot container */
        sixtyFT              /**< Standard 60-foot container */
    };
    Q_ENUM(ContainerSize)

   /**
    * @enum HaulerType
    * @brief Types of transportation modes for containers
    */
    enum HaulerType {
        truck,           /**< Road transport vehicle */
        train,           /**< Rail transport vehicle */
        waterTransport,  /**< Water-based transport vessel */
        airTransport,    /**< Aircraft transport */
        noHauler        /**< No transport mode specified */
    };
    Q_ENUM(HaulerType)

   /**
    * @brief Default constructor
    * @param parent Optional parent QObject for memory management
    */
    explicit Container(QObject *parent = nullptr);

   /**
    * @brief Constructor with ID and size
    * @param id Unique identifier for the container
    * @param size Size classification of the container
    * @param parent Optional parent QObject for memory management
    */
    Container(const QString &id, ContainerSize size, QObject *parent = nullptr);

   /**
    * @brief Constructor from JSON
    * @param json JSON object containing container data
    * @param parent Optional parent QObject for memory management
    * @throw std::invalid_argument if required JSON fields are missing
    */
    Container(const QJsonObject &json, QObject *parent = nullptr);

   /**
    * @brief Copy constructor
    * @param other Source Container to copy from
    */
    Container(const Container &other);  // Copy constructor

   /**
    * @brief Assignment operator
    * @param other Source Container to copy from
    * @return Reference to this Container
    */
    Container &operator=(const Container &other);  // Copy assignment operator

   /**
    * @brief Sets whether the Container is being used through Python bindings
    * @param isRunningThroughPython True if used through Python
    */
    void setIsRunningThroughPython(bool isRunningThroughPython);

   /**
    * @brief Virtual destructor
    */
    virtual ~Container();

    // Getter and Setter for containerID

   /**
    * @brief Gets the container's unique identifier
    * @return The container ID
    */
    QString getContainerID() const;

   /**
    * @brief Sets the container's unique identifier
    * @param id The new container ID
    */
    void setContainerID(const QString &id);

    /**
    * @brief Gets the time when the container was added to the system
    * @return The added time as a double (NaN if not set)
    */
    double getContainerAddedTime() const;

    /**
    * @brief Sets the time when the container was added to the system
    * @param time The added time value (NaN for undefined)
    * emits containerAddedTimeChanged() if the time changes
    */
    void setContainerAddedTime(const double &time);

    /**
    * @brief Gets the scheduled leaving time for the container
    * @return The leaving time as a double (NaN if not set)
    */
    double getContainerLeavingTime() const;

    /**
    * @brief Sets the scheduled leaving time for the container
    * @param time The leaving time value (NaN for undefined)
    * emits containerLeavingTimeChanged() if the time changes
    */
    void setContainerLeavingTime(const double &time);

    // Getter and Setter for containerSize

    /**
    * @brief Gets the size classification of the container
    * @return The container's size enumeration value
    */
    ContainerSize getContainerSize() const;

    /**
    * @brief Sets the size classification of the container
    * @param size The new container size
    * emits containerSizeChanged() if the size changes
    */
    void setContainerSize(ContainerSize size);

    // Getter and Setter for packages

    /**
    * @brief Gets the list of packages stored in the container
    * @return Vector of pointers to Package objects
    */
    QVector<Package*> getPackages() const;

    /**
    * @brief Replaces the entire package collection
    * @param packages Vector of pointers to new Package objects
    * emits packagesChanged()
    * @note Performs deep copy of provided packages
    */
    void setPackages(const QVector<Package*> &packages);

    /**
    * @brief Adds a single package to the container
    * @param package Pointer to the Package to add
    * emits packagesChanged()
    */
    void addPackage(Package *package);

    // Getter and Setter for customVariables

    /**
    * @brief Gets all custom variables for all hauler types
    * @return Map of hauler types to their respective variable maps
    */
    QMap<HaulerType, QVariantMap> getCustomVariables() const;

    /**
    * @brief Sets all custom variables for all hauler types
    * @param variables Map of hauler types to their respective variable maps
    * emits customVariablesChanged()
    */
    void setCustomVariables(const QMap<HaulerType, QVariantMap> &variables);

    /**
    * @brief Adds or updates a custom variable for a specific hauler
    * @param hauler The hauler type
    * @param key The variable key
    * @param value The variable value
    * emits customVariablesChanged()
    */
    void addCustomVariable(HaulerType hauler, const QString &key, const QVariant &value);

    /**
    * @brief Removes a custom variable for a specific hauler
    * @param hauler The hauler type
    * @param key The variable key to remove
    * emits customVariablesChanged() if the variable existed
    */
    void removeCustomVariable(HaulerType hauler, const QString &key);

    /**
    * @brief Gets a specific custom variable value
    * @param hauler The hauler type
    * @param key The variable key
    * @return The variable value, or invalid QVariant if not found
    */
    QVariant getCustomVariable(HaulerType hauler, const QString &key) const;

    /**
    * @brief Gets all custom variables for a specific hauler
    * @param hauler The hauler type
    * @return Map of variable keys to values for the specified hauler
    */
    QVariantMap getCustomVariablesForHauler(HaulerType hauler) const;

    // Getters and Setters for current location

    /**
    * @brief Gets the container's current location
    * @return The current location string
    */
    QString getContainerCurrentLocation() const;

    /**
    * @brief Sets the container's current location
    * @param location The new location
    * emits containerCurrentLocationChanged()
    * @note Also adds location to movement history
    */
    void setContainerCurrentLocation(const QString &location);

    // Getters and Setters for next destinations

    /**
    * @brief Gets the list of planned destinations
    * @return Vector of destination strings
    */
    QVector<QString> getContainerNextDestinations() const;

    /**
    * @brief Sets the list of planned destinations
    * @param destinations Vector of destination strings
    * emits containerNextDestinationsChanged()
    */
    void setContainerNextDestinations(const QVector<QString> &destinations);

    /**
    * @brief Adds a destination to the planned route
    * @param destination The destination to add
    * emits containerNextDestinationsChanged() if not already present
    */
    void addDestination(const QString &destination);

    /**
    * @brief Removes a destination from the planned route
    * @param destination The destination to remove
    * @return true if destination was found and removed, false otherwise
    * emits containerNextDestinationsChanged() if found
    */
    bool removeDestination(const QString &destination);

    // Getters and Setters for history

    /**
    * @brief Gets the container's movement history
    * @return Vector of historical location strings
    */
    QVector<QString> getContainerMovementHistory() const;

    /**
    * @brief Sets the container's movement history
    * @param history Vector of historical location strings
    * emits containerMovementHistoryChanged()
    */
    void setContainerMovementHistory(const QVector<QString> &history);

    /**
    * @brief Adds a location to the movement history
    * @param history The location to add
    * emits containerMovementHistoryChanged() if not already present
    */
    void addMovementHistory(const QString &history);

    /**
    * @brief Removes a location from the movement history
    * @param history The location to remove
    * @return true if location was found and removed, false otherwise
    * emits containerMovementHistoryChanged() if found
    */
    bool removeMovementHistory(const QString &history);

    /**
    * @brief Clears all container data
    * 
    * Clears:
    * - Packages (with proper memory management)
    * - Custom variables
    * - Next destinations
    * - Movement history
    */
    void clear();

    /**
    * @brief Converts the container to a JSON object
    * @return JSON object containing all container data
    */
    QJsonObject toJson() const;

    /**
    * @brief Creates a deep copy of this container
    * @return Pointer to the new copied container
    * @note The caller is responsible for managing the memory of the
    * returned container
    */
    ContainerCore::Container* copy() const;

    /**
    * @brief Serialization operator for Container class
    * @param out Output stream
    * @param container Container to serialize
    * @return Reference to output stream
    */
    friend CONTAINER_EXPORT QDataStream &operator<<(QDataStream &out, const Container &container);

    /**
    * @brief Deserialization operator for Container class
    * @param in Input stream
    * @param container Container to deserialize into
    * @return Reference to input stream
    */
    friend CONTAINER_EXPORT QDataStream &operator>>(QDataStream &in, Container &container);

signals:

   /**
    * @brief Emitted when the container ID changes
    */
    void containerIDChanged();

   /**
    * @brief Emitted when the container's added time changes
    */
    void containerAddedTimeChanged();

    /**
     * @brief Emitted when the container's scheduled leaving time changes
     * 
     * This signal is emitted whenever setContainerLeavingTime() is called with a 
     * different value than the current leaving time.
     */
    void containerLeavingTimeChanged();

    /**
     * @brief Emitted when the container's size classification changes
     * 
     * This signal is emitted whenever setContainerSize() is called with a 
     * different ContainerSize value than the current size.
     */
    void containerSizeChanged();

    /**
     * @brief Emitted when the container's package collection changes
     * 
     * This signal is emitted when:
     * - A package is added via addPackage()
     * - The package collection is replaced via setPackages()
     * - The package collection is cleared
     */
    void packagesChanged();

    /**
     * @brief Emitted when the container's custom variables are modified
     * 
     * This signal is emitted when:
     * - A custom variable is added via addCustomVariable()
     * - A custom variable is removed via removeCustomVariable()
     * - The custom variables collection is replaced via setCustomVariables()
     */
    void customVariablesChanged();

    /**
     * @brief Emitted when the container's current location changes
     * 
     * This signal is emitted whenever setContainerCurrentLocation() is called with
     * a different location than the current location. The new location is also
     * automatically added to the movement history.
     */
    void containerCurrentLocationChanged();

    /**
     * @brief Emitted when the container's planned destinations change
     * 
     * This signal is emitted when:
     * - A destination is added via addDestination()
     * - A destination is removed via removeDestination()
     * - The destinations list is replaced via setContainerNextDestinations()
     */
    void containerNextDestinationsChanged();

    /**
     * @brief Emitted when the container's movement history is modified
     * 
     * This signal is emitted when:
     * - A location is added to history via addMovementHistory()
     * - A location is removed from history via removeMovementHistory()
     * - The movement history is replaced via setContainerMovementHistory()
     * - The current location changes, automatically adding to history
     */
    void containerMovementHistoryChanged();

private:

    /** Container's unique identifier */
    QString m_containerID;

    /** Time when container was added */
    double m_addedTime;

    /** Scheduled departure time */
    double m_leavingTime;

    /** Container size classification */
    ContainerSize m_containerSize;

    /** Stored packages */
    QVector<Package*> m_packages;

    /** Custom variables by hauler type */
    QMap<HaulerType, QVariantMap> m_customVariables;

    /** Current location */
    QString m_containerCurrentLocation;

    /** Planned destinations */
    QVector<QString> m_containerNextDestinations;

    /** Location history */
    QVector<QString> m_containerMovementHistory;

    /** Python binding flag */
    bool m_isRunningThroughPython = false;

    /**
    * @brief Performs deep copy of container data
    * @param other Source Container to copy from
    */
    void deepCopy(const Container &other);  // Helper function to perform deep copy
};

} // namespace ContainerCore

// Register Container types with Qt's meta-object system
Q_DECLARE_METATYPE(ContainerCore::Container)
Q_DECLARE_METATYPE(ContainerCore::Container*)


#endif // CONTAINER_H
