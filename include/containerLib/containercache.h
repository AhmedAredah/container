/**
* @file containercache.h
* @brief LRU cache implementation for container management
* @author Ahmed Aredah
* @date 2024
* 
* This file provides a templated Least Recently Used (LRU) cache implementation
* specifically designed for container objects. It manages memory efficiently
* by maintaining a fixed-size cache with automatic eviction of least recently
* used items.
*/

#ifndef CONTAINERCACHE_H
#define CONTAINERCACHE_H

#include <QMap>
#include <QList>
#include <QString>

namespace ContainerCore{

/**
* @class ContainerCache
* @brief Implements an LRU cache for container objects
* @tparam T The type of objects to be cached (must be pointer type)
* 
* This class provides a fixed-size LRU cache implementation with:
* - Automatic eviction of least recently used items
* - Optional memory management of cached objects
* - Thread-unsafe operations (external synchronization required)
* 
* Requirements for type T:
* - Must be a pointer type
* - Must support proper cleanup in destructor
* - Should support copy construction if cache entries need duplication
*/
template <typename T>
class ContainerCache {
public:

   /**
    * @brief Constructs a new cache
    * @param maxSize Maximum number of objects to store (default: 200)
    * @param deletePntrsWhileDestructing Whether to delete cached objects on destruction
    */
    explicit ContainerCache(int maxSize = 200, bool deletePntrsWhileDestructing = true);

   /**
    * @brief Destructor that handles cleanup of cached objects
    * 
    * If deletePntrsWhileDestructing is true, deletes all cached objects.
    */
    ~ContainerCache();

   /**
    * @brief Inserts an object into the cache
    * @param key The key to associate with the object
    * @param object Pointer to the object to cache
    * 
    * If the cache is full, the least recently used item is evicted.
    * If the key already exists, the old object is replaced.
    */
    void insert(const QString &key, T *object);
    
   /**
    * @brief Retrieves an object from the cache (non-const version)
    * @param key The key of the object to retrieve
    * @return Pointer to the cached object, or nullptr if not found
    * 
    * Updates the object's position in the LRU order.
    */
    T* object(const QString &key);               // Non-const version
    
   /**
    * @brief Retrieves an object from the cache (const version)
    * @param key The key of the object to retrieve
    * @return Pointer to the cached object, or nullptr if not found
    * 
    * Does not modify the LRU order.
    */
    T* object(const QString &key) const;         // Const version, but returns non-const pointer
    
   /**
    * @brief Removes an object from the cache
    * @param key The key of the object to remove
    * @param deleteObject Whether to delete the removed object
    */
    void remove(const QString &key, bool deleteObject = false);
    
   /**
    * @brief Clears all objects from the cache
    * @param deleteObjects Whether to delete the cached objects
    */
    void clear(bool deleteObjects = false);
    
   /**
    * @brief Checks if a key exists in the cache
    * @param key The key to check
    * @return true if the key exists, false otherwise
    */
    bool contains(const QString &key) const;

   /**
    * @brief Returns the number of objects in the cache
    * @return Current cache size
    */
    int size() const;

   /**
    * @brief Returns all keys in the cache
    * @return List of cache keys
    */
    QList<QString> keys() const;

   /**
    * @brief Sets whether to delete objects during destruction
    * @param dlt true to delete objects, false to leave them intact
    */
    void setDeleteWhileDestructing(bool dlt);

private:

   /** @brief Maximum number of objects the cache can hold */
    int m_maxSize;

   /** @brief Map storing the cached objects */
    QMap<QString, T *> m_cache;

   /** @brief List maintaining the order of object access (LRU order) */
    QList<QString> m_accessOrder;

   /** @brief Whether to delete cached objects during destruction */
    bool m_deletePointerWhenDesctructing = true;
};

// Implementation of the template class
template <typename T>
ContainerCache<T>::ContainerCache(int maxSize, bool deletePntrsWhileDestructing)
    : m_maxSize(maxSize), m_deletePointerWhenDesctructing(deletePntrsWhileDestructing)
{}

template <typename T>
ContainerCache<T>::~ContainerCache() {
    clear(m_deletePointerWhenDesctructing);
}

template <typename T>
void ContainerCache<T>::insert(const QString &key, T *object) {
    if (m_cache.contains(key)) {
        m_cache.remove(key);
    } else if (m_cache.size() >= m_maxSize) {
        QString lruKey = m_accessOrder.takeLast();
        remove(lruKey, true);
    }
    m_cache.insert(key, object);
    m_accessOrder.prepend(key);
}

template <typename T>
T *ContainerCache<T>::object(const QString &key) {
    if (m_cache.contains(key)) {
        m_accessOrder.removeOne(key);
        m_accessOrder.prepend(key);
        return m_cache.value(key);
    }
    return nullptr;
}

template<typename T>
T* ContainerCache<T>::object(const QString &key) const {
    auto it = m_cache.constFind(key);
    return (it != m_cache.cend()) ? const_cast<T*>(it.value()) : nullptr;
}

template <typename T>
void ContainerCache<T>::remove(const QString &key, bool deleteObject) {
    if (m_cache.contains(key)) {
        if (deleteObject) {
            delete m_cache.value(key);
        }
        m_cache.remove(key);
        m_accessOrder.removeOne(key);
    }
}

template <typename T>
void ContainerCache<T>::clear(bool deleteObjects) {
    if (deleteObjects) {
        qDeleteAll(m_cache);
    }
    m_cache.clear();
    m_accessOrder.clear();
}

template <typename T>
bool ContainerCache<T>::contains(const QString &key) const {
    return m_cache.contains(key);
}

template <typename T>
int ContainerCache<T>::size() const {
    return m_cache.size();
}

template <typename T>
QList<QString> ContainerCache<T>::keys() const {
    return m_cache.keys();
}

template<typename T>
void ContainerCache<T>::setDeleteWhileDestructing(bool dlt)
{
    m_deletePointerWhenDesctructing = dlt;
}
}
#endif // CONTAINERCACHE_H
