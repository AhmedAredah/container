/**
 * @file package.h
 * @brief Package class definition for container management system
 * @author Ahmed Aredah
 * @date 2024
 * 
 * This file defines the Package class, which represents a package or item
 * that can be stored in a shipping container. It provides functionality for
 * package identification, serialization, and JSON conversion.
 */

#ifndef PACKAGE_H
#define PACKAGE_H

#include "Container_global.h"
#include <QObject>
#include <QDataStream>
#include <QJsonObject>

namespace ContainerCore {

/**
 * @class Package
 * @brief Represents a package that can be stored in a container
 * 
 * The Package class provides functionality for:
 * - Package identification and tracking
 * - JSON serialization/deserialization
 * - Data stream serialization
 * - Signal notifications for property changes
 * 
 * It inherits from QObject to support Qt's property system and
 * signal/slot mechanism.
 */
class CONTAINER_EXPORT Package : public QObject
{
    Q_OBJECT

    /**
     * @property packageID
     * @brief The unique identifier for the package
     * 
     * This property provides read/write access to the package's ID
     * and notifies when the ID changes.
     */
    Q_PROPERTY(QString packageID READ packageID WRITE setPackageID NOTIFY packageIDChanged FINAL)

public:

    /**
     * @brief Default constructor
     * @param parent Optional parent QObject for memory management
     */
    explicit Package(QObject *parent = nullptr);

    /**
     * @brief Virtual destructor
     */
    virtual ~Package();

    /**
     * @brief Copy constructor
     * @param other Source Package to copy from
     * 
     * Creates a deep copy of the package, including all properties
     * and maintaining proper Qt parent relationships.
     */
    Package(const Package &other);  // Copy constructor

    /**
     * @brief Constructor with ID
     * @param id The unique identifier for the package
     * @param parent Optional parent QObject for memory management
     */
    Package(const QString id, QObject *parent = nullptr);

    /**
     * @brief Constructor from JSON
     * @param json JSON object containing package data
     * @param parent Optional parent QObject for memory management
     * @throw std::invalid_argument if required JSON fields are missing
     */
    Package(const QJsonObject &json, QObject *parent = nullptr);

    /**
     * @brief Assignment operator
     * @param other Source Package to copy from
     * @return Reference to this Package
     * 
     * Performs a deep copy of all package data while maintaining
     * proper Qt parent relationships.
     */
    Package &operator=(const Package &other);  // Copy assignment operator

    /**
     * @brief Gets the package ID
     * @return The package's unique identifier
     */
    QString packageID() const;

    /**
     * @brief Sets the package ID
     * @param id The new unique identifier
     * 
     * Emits packageIDChanged signal if the ID changes.
     */
    void setPackageID(const QString &id);

    /**
     * @brief Converts the package to a JSON object
     * @return JSON object containing all package data
     * 
     * The JSON object includes:
     * - packageID: The package's unique identifier
     */
    QJsonObject toJson() const;

    /**
     * @brief Creates a deep copy of this package
     * @return Pointer to the new copied package
     * @note The caller is responsible for managing the memory of the
     * returned package
     */
    ContainerCore::Package* copy() const;

    /**
     * @brief Serialization operator
     * @param out Output stream
     * @param package Package to serialize
     * @return Reference to the output stream
     */
    friend QDataStream &operator<<(QDataStream &out, const Package &package);

    /**
     * @brief Deserialization operator
     * @param in Input stream
     * @param package Package to deserialize into
     * @return Reference to the input stream
     */
    friend QDataStream &operator>>(QDataStream &in, Package &package);

signals:

    /**
     * @brief Emitted when the package ID changes
     */
    void packageIDChanged();

private:

    /**
     * @brief The package's unique identifier
     */
    QString m_packageID;

    /**
     * @brief Helper function to perform deep copy
     * @param other Source Package to copy from
     * 
     * Copies all package data while maintaining proper Qt parent relationships.
     */
    void deepCopy(const Package &other);  // Helper function to perform deep copy
};  // class Package
} // namespace ContainerCore

#endif // PACKAGE_H
