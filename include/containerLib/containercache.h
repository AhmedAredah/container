#ifndef CONTAINERCACHE_H
#define CONTAINERCACHE_H

#include <QMap>
#include <QList>
#include <QString>

namespace ContainerCore{

template <typename T>
class ContainerCache {
public:
    explicit ContainerCache(int maxSize = 200, bool deletePntrsWhileDestructing = true);
    ~ContainerCache();

    void insert(const QString &key, T *object);
    T* object(const QString &key);               // Non-const version
    T* object(const QString &key) const;         // Const version, but returns non-const pointer
    void remove(const QString &key, bool deleteObject = false);
    void clear(bool deleteObjects = false);
    bool contains(const QString &key) const;
    int size() const;
    QList<QString> keys() const;
    void setDeleteWhileDestructing(bool dlt);

private:
    int m_maxSize;
    QMap<QString, T *> m_cache;
    QList<QString> m_accessOrder;
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
