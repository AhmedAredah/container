#include "cache.h"

template <typename T>
Cache<T>::Cache(int maxSize, bool deletePntrsWhileDestructing)
    : m_maxSize(maxSize), m_deletePointerWhenDesctructing(deletePntrsWhileDestructing)
{}

template <typename T>
Cache<T>::~Cache() {
    clear(m_deletePointerWhenDesctructing);
}

template <typename T>
void Cache<T>::insert(const QString &key, T *object) {
    // If the cache already contains this key, update and move it to the front
    if (m_cache.contains(key)) {
        m_cache.remove(key);
    } else if (m_cache.size() >= m_maxSize) {
        // If the cache is full, remove the least recently used item
        QString lruKey = m_accessOrder.takeLast(); // Get the last element (LRU)
        remove(lruKey, true); // Remove the LRU item without deleting the pointer
    }

    m_cache.insert(key, object);
    m_accessOrder.prepend(key); // Mark this key as most recently used
}

template <typename T>
T *Cache<T>::object(const QString &key) {
    if (m_cache.contains(key)) {
        // Move key to the front (most recently used)
        m_accessOrder.removeOne(key);
        m_accessOrder.prepend(key);
        return m_cache.value(key);
    }
    return nullptr;
}

template<typename T>
T* Cache<T>::object(const QString &key) const {
    auto it = m_cache.constFind(key);
    return (it != m_cache.cend()) ? const_cast<T*>(it.value()) : nullptr;
}

template <typename T>
void Cache<T>::remove(const QString &key, bool deleteObject) {
    if (m_cache.contains(key)) {
        if (deleteObject) {
            delete m_cache.value(key); // Delete the object if requested
        }
        m_cache.remove(key);
        m_accessOrder.removeOne(key); // Remove from access order
    }
}

template <typename T>
void Cache<T>::clear(bool deleteObjects) {
    if (deleteObjects) {
        qDeleteAll(m_cache);
    }
    m_cache.clear();
    m_accessOrder.clear();
}

template <typename T>
bool Cache<T>::contains(const QString &key) const {
    return m_cache.contains(key);
}

template <typename T>
int Cache<T>::size() const {
    return m_cache.size();
}

template <typename T>
QList<QString> Cache<T>::keys() const {
    return m_cache.keys();
}

template<typename T>
void Cache<T>::setDeleteWhileDestructing(bool dlt)
{
    m_deletePointerWhenDesctructing = dlt;
}

