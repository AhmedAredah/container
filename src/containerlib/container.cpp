#include "container.h"
#include <QDataStream>
#include <QDebug>

namespace ContainerCore {

Container::Container(QObject *parent)
    : QObject(parent),
    m_containerSize(twentyFT) {} // Default to a common container size

Container::Container(const QString &id, ContainerSize size, QObject *parent)
    : QObject(parent), m_containerID(id), m_containerSize(size) {
    qDebug() << "Constructor parameter id:" << id;
    qDebug() << "Container constructed with ID:" << m_containerID;
    m_containerCurrentLocation = "Unknown"; // Default location if not provided
}

Container::Container(const QJsonObject &json, QObject *parent)
    : QObject(parent)
{
    // Initialize containerID
    // Check for the existence of keys and validity of JSON before accessing them
    if (!json.contains("containerID") || !json["containerID"].isString()) {
        throw std::invalid_argument("Invalid or missing 'containerID'");
    }
    m_containerID = json["containerID"].toString();

    // Initialize containerSize
    if (!json.contains("containerSize") || !json["containerSize"].isDouble()) {
        throw std::invalid_argument("Invalid or missing 'containerSize'");
    }
    m_containerSize = static_cast<ContainerSize>(json["containerSize"].toInt());

    if (json.contains("containerCurrentLocation") &&
        json["containerCurrentLocation"].isString()) {
        m_containerCurrentLocation =
            json["containerCurrentLocation"].toString();
    } else {
        m_containerCurrentLocation =
            "Unknown"; // Default if not provided or invalid
    }

    // Check and initialize containerNextDestinations from JSON array
    if (json.contains("containerNextDestinations") &&
        json["containerNextDestinations"].isArray()) {
        QJsonArray nextDestinationsArray =
            json["containerNextDestinations"].toArray();
        for (const QJsonValue &value : nextDestinationsArray) {
            if (value.isString()) {
                m_containerNextDestinations.append(value.toString());
            }
        }
    }

    // Initialize containerMovementHistory from JSON array
    // if available and valid
    if (json.contains("containerMovementHistory") &&
        json["containerMovementHistory"].isArray()) {
        QJsonArray movementHistoryArray =
            json["containerMovementHistory"].toArray();
        for (const QJsonValue &value : movementHistoryArray) {
            if (value.isString()) {
                m_containerMovementHistory.append(value.toString());
            }
        }
    }

    // Initialize packages
    if (json.contains("packages") && json["packages"].isArray()) {
        QJsonArray packagesArray = json["packages"].toArray();
        for (const QJsonValue &value : packagesArray) {
            if (value.isObject()) {
                Package *package = new Package(value.toObject(), this);
                m_packages.append(package);
            }
        }
    }

    // Initialize customVariables
    if (json.contains("customVariables") &&
        json["customVariables"].isObject()) {
        QJsonObject customVariablesObject = json["customVariables"].toObject();
        for (const QString &key : customVariablesObject.keys()) {
            HaulerType hauler = static_cast<HaulerType>(key.toInt());
            QJsonObject haulerObject = customVariablesObject[key].toObject();

            QVariantMap variables;
            for (const QString &varKey : haulerObject.keys()) {
                variables.insert(varKey, haulerObject[varKey].toVariant());
            }
            m_customVariables[hauler] = variables;
        }
    }
}

// Copy constructor
Container::Container(const Container &other)
    : QObject(other.parent())
{
    deepCopy(other);
}

// Copy assignment operator
Container &Container::operator=(const Container &other)
{
    if (this != &other) {
        clear();  // Clear existing packages
        deepCopy(other);  // Perform deep copy
    }
    return *this;
}

// Destructor
Container::~Container() {
    clear();
}

QString Container::getContainerID() const {
    return m_containerID;
}

void Container::setContainerID(const QString &id) {
    if (id.isEmpty()) {
        qDebug() << "Warning: Attempting to set an empty ID.";
        return; // Prevent setting an empty ID
    }
    if (id != m_containerID) {
        m_containerID = id;
        emit containerIDChanged();
    }
}

Container::ContainerSize Container::getContainerSize() const {
    return m_containerSize;
}

void Container::setContainerSize(ContainerSize size) {
    if (size != m_containerSize) {
        m_containerSize = size;
        emit containerSizeChanged();
    }
}

QVector<Package*> Container::getPackages() const {
    return m_packages;
}

void Container::setPackages(const QVector<Package*> &packages) {
    clear();
    for (Package* package : packages) {
        Package* packageCopy = new Package(*package);  // Deep copy of each package
        m_packages.append(packageCopy);
    }
    emit packagesChanged();
}

void Container::addPackage(Package *package) {
    m_packages.append(package);
    emit packagesChanged();
}


QMap<Container::HaulerType, QVariantMap> Container::getCustomVariables() const {
    return m_customVariables;
}

void Container::setCustomVariables(const QMap<HaulerType, QVariantMap> &variables) {
    m_customVariables = variables;
    emit customVariablesChanged();
}

void Container::addCustomVariable(HaulerType hauler, const QString &key, const QVariant &value) {
    m_customVariables[hauler].insert(key, value);
    emit customVariablesChanged();
}

void Container::removeCustomVariable(HaulerType hauler, const QString &key) {
    if (m_customVariables.contains(hauler)) {
        m_customVariables[hauler].remove(key);
        emit customVariablesChanged();
    }
}

QVariant Container::getCustomVariable(HaulerType hauler, const QString &key) const {
    if (m_customVariables.contains(hauler) && m_customVariables[hauler].contains(key)) {
        return m_customVariables[hauler][key];
    }
    return QVariant();
}

QVariantMap Container::getCustomVariablesForHauler(HaulerType hauler) const {
    return m_customVariables.value(hauler);
}

QString Container::getContainerCurrentLocation() const {
    return m_containerCurrentLocation;
}

void Container::setContainerCurrentLocation(const QString &location) {
    if (location != m_containerCurrentLocation) {
        m_containerCurrentLocation = location;
        emit containerCurrentLocationChanged();
    }
}

QVector<QString> Container::getContainerNextDestinations() const {
    return m_containerNextDestinations;
}

void Container::setContainerNextDestinations(const QVector<QString> &destinations) {
    if (destinations != m_containerNextDestinations) {
        m_containerNextDestinations = destinations;
        emit containerNextDestinationsChanged();
    }
}

void Container::addDestination(const QString &destination) {
    if (!m_containerNextDestinations.contains(destination)) {
        m_containerNextDestinations.append(destination);
        emit containerNextDestinationsChanged();
    }
}

bool Container::removeDestination(const QString &destination) {
    int index = m_containerNextDestinations.indexOf(destination);
    if (index != -1) {
        m_containerNextDestinations.removeAt(index);
        emit containerNextDestinationsChanged();
        return true;
    }
    return false;
}

QVector<QString> Container::getContainerMovementHistory() const {
    return m_containerMovementHistory;
}

void Container::setContainerMovementHistory(const QVector<QString> &history) {
    if (history != m_containerMovementHistory) {
        m_containerMovementHistory = history;
        emit containerMovementHistoryChanged();
    }
}

void Container::addMovementHistory(const QString &history) {
    if (!m_containerMovementHistory.contains(history)) {
        m_containerMovementHistory.append(history);
        emit containerMovementHistoryChanged();
    }
}

bool Container::removeMovementHistory(const QString &history) {
    int index = m_containerMovementHistory.indexOf(history);
    if (index != -1) {
        m_containerMovementHistory.removeAt(index);
        emit containerMovementHistoryChanged();
        return true;
    }
    return false;
}

// Helper function to perform deep copy
void Container::deepCopy(const Container &other)
{
    m_containerID = other.m_containerID;
    m_containerSize = other.m_containerSize;
    m_containerCurrentLocation = other.m_containerCurrentLocation;
    m_containerNextDestinations = other.m_containerNextDestinations;
    m_containerMovementHistory = other.m_containerMovementHistory;

    // Delete existing packages before copying new ones
    qDeleteAll(m_packages);
    m_packages.clear();
    for (Package* package : other.m_packages) {
        Package* packageCopy = new Package(*package);  // Deep copy of each package
        m_packages.append(packageCopy);
    }

    m_customVariables = other.m_customVariables; // Deep copy of custom variables
}

// Clear the package list and delete all packages
void Container::clear() {
    qDeleteAll(m_packages);
    m_packages.clear();
    m_customVariables.clear();
    m_containerNextDestinations.clear();
    m_containerMovementHistory.clear();
}

QJsonObject Container::toJson() const
{
    QJsonObject jsonObject;
    jsonObject["containerID"] = m_containerID;
    jsonObject["containerSize"] = static_cast<int>(m_containerSize);
    jsonObject["containerCurrentLocation"] = m_containerCurrentLocation;

    // Convert next destinations to QJsonArray
    QJsonArray nextDestinationsArray;
    for (const QString &destination : m_containerNextDestinations) {
        nextDestinationsArray.append(QJsonValue(destination));
    }
    jsonObject["containerNextDestinations"] = nextDestinationsArray;

    // Convert movement history to QJsonArray
    QJsonArray movementHistoryArray;
    for (const QString &history : m_containerMovementHistory) {
        movementHistoryArray.append(QJsonValue(history));
    }
    jsonObject["containerMovementHistory"] = movementHistoryArray;

    // Convert packages to QJsonArray
    QJsonArray packagesArray;
    for (const Package* package : m_packages) {
        if (package) {
            packagesArray.append(package->toJson());
        }
    }
    jsonObject["packages"] = packagesArray;

    // Convert customVariables to QJsonObject
    QJsonObject customVariablesObject;
    for (auto it = m_customVariables.constBegin(); it != m_customVariables.constEnd(); ++it) {
        QJsonObject haulerObject;
        for (auto varIt = it.value().constBegin(); varIt != it.value().constEnd(); ++varIt) {
            haulerObject[varIt.key()] = QJsonValue::fromVariant(varIt.value());
        }
        customVariablesObject[QString::number(static_cast<int>(it.key()))] = haulerObject;
    }
    jsonObject["customVariables"] = customVariablesObject;

    return jsonObject;
}

// Serialization
QDataStream &operator<<(QDataStream &out, const Container &container) {
    out << container.m_containerID;
    out << static_cast<int>(container.m_containerSize);
    out << container.m_containerCurrentLocation;
    out << container.m_containerNextDestinations;
    out << container.m_containerMovementHistory;

    out << container.m_packages.size();
    for (auto package : container.m_packages) {
        out << *package;
    }

    // Serialize custom variables
    out << container.m_customVariables.size();
    for (auto it = container.m_customVariables.cbegin();
         it != container.m_customVariables.cend(); ++it) {
        out << static_cast<int>(it.key()); // Serialize HaulerType
        out << it.value();  // Serialize the QVariantMap
    }

    return out;
}

// Deserialization
QDataStream &operator>>(QDataStream &in, Container &container) {
    QString id, currentLocation;
    QVector<QString> nextDestinations, movementHistory;
    int size;
    in >> id >> size >> currentLocation >> nextDestinations >> movementHistory;
    container.setContainerID(id);
    container.setContainerSize(static_cast<Container::ContainerSize>(size));
    container.setContainerCurrentLocation(currentLocation);
    container.setContainerNextDestinations(nextDestinations);
    container.setContainerMovementHistory(movementHistory);

    int packageCount;
    in >> packageCount;
    QVector<Package*> packages;
    for (int i = 0; i < packageCount; ++i) {
        Package* package = new Package();
        in >> *package;
        packages.append(package);
    }
    container.setPackages(packages);

    // Deserialize custom variables
    int customVarCount;
    in >> customVarCount;
    QMap<Container::HaulerType, QVariantMap> customVariables;
    for (int i = 0; i < customVarCount; ++i) {
        int haulerTypeInt;
        in >> haulerTypeInt;
        Container::HaulerType haulerType =
            static_cast<Container::HaulerType>(haulerTypeInt);

        QVariantMap variables;
        in >> variables;
        customVariables[haulerType] = variables;
    }
    container.setCustomVariables(customVariables);

    return in;
}

}